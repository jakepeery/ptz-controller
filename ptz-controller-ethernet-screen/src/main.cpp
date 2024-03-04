#include "main.h"
#include <Preferences.h>
#include "VISCA_Evaluators.h"
#include "display.h"

static Preferences storedValues;
CameraProperties cameras[5];
//AsyncUDP udp;
WiFiUDP udp;
//UDP udp;
int selected_max;
QueueHandle_t UDP_Queue;
int JOYSTICK_X, JOYSTICK_Y, JOYSTICK_TWIST;
int SELECTED_CAMERA = 1;
String STATUS_TEXT;
bool eth_connected = false;
bool udp_connected = false;
static char udpCommand[24];

static TimerHandle_t StopFocusCallbackTimer;
static TimerHandle_t SaveValuesCallbackTimer;
// xTimerStart(SaveValuesCallbackTimer, 30);

bool cameraIPSetupMode = false;
bool deviceIPSetupMode = false;




void SaveValuesCallback(TimerHandle_t xTimer) {
  String prefix;

  switch (SELECTED_CAMERA){
    case 1: prefix = "Cam1"; break;
    case 2: prefix = "Cam2"; break;
    case 3: prefix = "Cam3"; break;
    case 4: prefix = "Cam4"; break;
  }
  Serial.print("Storing camera values with prefix: ");
  Serial.println(prefix);

  storedValues.begin(prefix.c_str(), false);
  storedValues.putInt("ipAddress", cameras[SELECTED_CAMERA].ipAddress);
  storedValues.putInt("visca_port", cameras[SELECTED_CAMERA].visca_port);
  storedValues.putInt("max_pan", cameras[SELECTED_CAMERA].max_pan);
  storedValues.putInt("max_tilt", cameras[SELECTED_CAMERA].max_tilt);
  storedValues.putInt("max_zoom", cameras[SELECTED_CAMERA].max_zoom);
  storedValues.end();
  Serial.println("Stored Values to Flash Memory");
}

void StoreAllCameras(){
  TimerHandle_t xTimer;
  SELECTED_CAMERA = 1;
  SaveValuesCallback(xTimer);
  delay(200);

  SELECTED_CAMERA = 2;
  SaveValuesCallback(xTimer);
  delay(200);

  SELECTED_CAMERA = 3;
  SaveValuesCallback(xTimer);
  delay(200);

  SELECTED_CAMERA = 4;
  SaveValuesCallback(xTimer);
  delay(200);

  SELECTED_CAMERA = 1;
}

void RecallCameraValues(){
  String prefix;
  
  for (int i=1; i<5; i++){
    switch (i){
      case 1: prefix = "Cam1"; break;
      case 2: prefix = "Cam2"; break;
      case 3: prefix = "Cam3"; break;
      case 4: prefix = "Cam4"; break;
    }
  storedValues.begin(prefix.c_str(), true);
  cameras[i].ipAddress = storedValues.getInt("ipAddress", IPAddress(192, 168, 1, 28));
  cameras[i].visca_port = storedValues.getInt("visca_port", 1259);
  cameras[i].max_pan = storedValues.getInt("max_pan", 10);
  cameras[i].max_tilt = storedValues.getInt("max_tilt", 10);
  cameras[i].max_zoom = storedValues.getInt("max_zoom", 5);
  
  Serial.print(i);
  Serial.print(prefix);
  Serial.print(" Recalled Camera IP Address: ");
  Serial.println(cameras[i].ipAddress);

  storedValues.end();
  delay(100);
  }
}




void RecallPreset(int preset) {
  Serial.print("Recalling preset:");
  Serial.println(preset);
  STATUS_TEXT = "Recalled Preset " + (String)preset;
}

void StorePreset(int preset) {
  Serial.print("Storing preset:");
  Serial.println(preset);
  STATUS_TEXT = "Stored Preset " + (String)preset;
}

// focus in/out
void Encoder1(bool direction) {
  UDP_Info ToSend;
  if (direction){
    Serial.println("Encoder1 In");
    STATUS_TEXT = "Focus In";
    FocusNear(udpCommand);
  } else {
    Serial.println("Encoder1 Out");
    STATUS_TEXT = "Focus Out";
    FocusFar(udpCommand);
  }

  ToSend.IP_Address = cameras[SELECTED_CAMERA].ipAddress;
  ToSend.Port = cameras[SELECTED_CAMERA].visca_port;
  ToSend.Message = udpCommand;

  xQueueSend(UDP_Queue, (void*)&ToSend, (TickType_t) 0);
  xTimerStart(StopFocusCallbackTimer, 30);
}

void StopFocusCallback(TimerHandle_t xTimer) {
  Serial.println("Timer Callback");
  UDP_Info ToSend;
  FocusStop(udpCommand);
  ToSend.IP_Address = cameras[SELECTED_CAMERA].ipAddress;
  ToSend.Port = cameras[SELECTED_CAMERA].visca_port;
  ToSend.Message = udpCommand;
  xQueueSend(UDP_Queue, (void*)&ToSend, (TickType_t) 0);
}

// manual/automatic focus
void Encoder1Press(bool state){
  UDP_Info ToSend;
  // mode2=auto    mode3=manual
  if (cameras[SELECTED_CAMERA].focus_mode == 2) {
    cameras[SELECTED_CAMERA].focus_mode = 3;
    FocusManual(udpCommand);
    STATUS_TEXT = "Focus Manual";
  } else{
    cameras[SELECTED_CAMERA].focus_mode = 2;
    FocusAutomatic(udpCommand);
    STATUS_TEXT = "Focus Auto";
  }
  
  ToSend.IP_Address = cameras[SELECTED_CAMERA].ipAddress;
  ToSend.Port = cameras[SELECTED_CAMERA].visca_port;
  ToSend.Message = udpCommand;

  xQueueSend(UDP_Queue, (void*)&ToSend, (TickType_t) 0);
  
}

void Encoder2(bool direction) {
  if (selected_max == 1 && direction) {
    cameras[SELECTED_CAMERA].max_pan += 1;
  } else if (selected_max == 1 && !direction) {
    cameras[SELECTED_CAMERA].max_pan -= 1;
  } else if (selected_max == 2 && direction) {
    cameras[SELECTED_CAMERA].max_tilt += 1;
  } else if (selected_max == 2 && !direction) {
    cameras[SELECTED_CAMERA].max_tilt -= 1;
  } else if (selected_max == 3 && direction) {
    cameras[SELECTED_CAMERA].max_zoom += 1;
  } else if (selected_max == 3 && !direction) {
    cameras[SELECTED_CAMERA].max_zoom -= 1;
  }

  if (cameras[SELECTED_CAMERA].max_pan > MAX_PAN){
    cameras[SELECTED_CAMERA].max_pan = MAX_PAN;
  }
  if (cameras[SELECTED_CAMERA].max_tilt > MAX_TILT){
    cameras[SELECTED_CAMERA].max_tilt = MAX_TILT;
  }
  if (cameras[SELECTED_CAMERA].max_zoom > MAX_ZOOM){
    cameras[SELECTED_CAMERA].max_zoom = MAX_ZOOM;
  }

  if (cameras[SELECTED_CAMERA].max_pan < 1){
    cameras[SELECTED_CAMERA].max_pan = 1;
  }
  if (cameras[SELECTED_CAMERA].max_tilt < 1){
    cameras[SELECTED_CAMERA].max_tilt = 1;
  }
  if (cameras[SELECTED_CAMERA].max_zoom < 1){
    cameras[SELECTED_CAMERA].max_zoom = 1;
  }

  //to do add in storing to persistent storage
  xTimerStart(SaveValuesCallbackTimer, 30);
}

void Encoder2Press(bool state){
  if (selected_max == 1){
    selected_max = 2;
  } else if (selected_max == 2) {
    selected_max = 3;
  }
   else if (selected_max == 3) {
    selected_max = 1;
  } else {
    selected_max = 1;
  }
}



void EvaluateRXString(String RX) {
  static int newJoystickX;
  static int newJoystickY;
  static int newJoystickTwist;

  UDP_Info ToSend;
  ToSend.IP_Address = cameras[SELECTED_CAMERA].ipAddress;
  ToSend.Port = cameras[SELECTED_CAMERA].visca_port;

  String command, value;

  // split the string on the :
  String xy_response = ":";
  int split = RX.indexOf(xy_response);

  if (split > 0) {
    command = RX.substring(0, split);
    value = RX.substring(split + 1);

    split = value.indexOf(String("\r"));
    value = value.substring(0, split); //remove the \r\n

    // event triggers
    if (command == "joystick_x") {
      int temp_x = value.toInt();
      newJoystickX = (int)((float)temp_x *
                           (float)(cameras[SELECTED_CAMERA].max_pan / 100.0));

    } else if (command == "joystick_y") {
      int temp_y = value.toInt();
      newJoystickY = (int)((float)temp_y *
                           (float)(cameras[SELECTED_CAMERA].max_tilt / 100.0));

    } else if (command == "joystick_twist") {
      int temp_twist = value.toInt();
      newJoystickTwist =
          (int)((float)temp_twist *
                (float)(cameras[SELECTED_CAMERA].max_zoom / 100.0));

    } else if (command == "joystick_raw") {
      // STATUS_TEXT = command;
    } else if (command == "joystick_short_released") {
      STATUS_TEXT = "One Push Focus";
      char one_push_focus_cmd[256];
      OnePushFocus(one_push_focus_cmd);
      ToSend.Message = one_push_focus_cmd;
      xQueueSend(UDP_Queue, (void*)&ToSend, (TickType_t) 0);
    } else if (command == "joystick_long_released") {
      // STATUS_TEXT = command;
    } else if (command == "joystick_held") {
      STATUS_TEXT = "Joystick Held";
    } else if (command == "encoder1_press") {
      Encoder1Press(value.toInt());
    } else if (command == "encoder1_up") {
      Encoder1(true);
    } else if (command == "encoder1_down") {
      Encoder1(false);

    } else if (command == "encoder2_press") {
      Encoder2Press(value.toInt());
    } else if (command == "encoder2_up") {
      Encoder2(true);
    } else if (command == "encoder2_down") {
      Encoder2(false);

    } else if (command == "key_raw") {
      // raw keypad here
    } else if (command == "key_short") {
      if (value.equals("A")) {
        SELECTED_CAMERA = 1;
      } else if (value.equals("B")) {
        SELECTED_CAMERA = 2;
      } else if (value.equals("C")) {
        SELECTED_CAMERA = 3;
      } else if (value.equals("D")) {
        SELECTED_CAMERA = 4;
      } else if (value.equals("*")) {
        STATUS_TEXT = "Key Pressed *";
      } else if (value.equals("#")) {
        STATUS_TEXT = "Key Pressed #";
      } else {
        RecallPreset(value.toInt());
        EvaluatePresetRecall(value.toInt(), udpCommand);
        ToSend.Message = udpCommand;
        xQueueSend(UDP_Queue, (void*)&ToSend, (TickType_t) 0);
      }
    } else if (command == "key_long") {
      // STATUS_TEXT = command + " " + value;
    } else if (command == "key_held") {
      if (value.equals("A")) {
        SELECTED_CAMERA = 1;
        cameraIPSetupMode = true;
      } else if (value.equals("B")) {
        SELECTED_CAMERA = 2;
        cameraIPSetupMode = true;
      } else if (value.equals("C")) {
        SELECTED_CAMERA = 3;
        cameraIPSetupMode = true;
      } else if (value.equals("D")) {
        SELECTED_CAMERA = 4;
        cameraIPSetupMode = true;
      } else if (value.equals("*")) {
        STATUS_TEXT = "Key Held *";
        deviceIPSetupMode = true;
      } else if (value.equals("#")) {
        STATUS_TEXT = "Key Held #";
      } else {
        StorePreset(value.toInt());
        EvaluatePresetStore(value.toInt(), udpCommand);
        ToSend.Message = udpCommand;
        xQueueSend(UDP_Queue, (void*)&ToSend, (TickType_t) 0);
      }
    }
  } else {
    Serial.println("ERROR: BAD FORMATTING, NO ':'");
    return;
  }

  
  
  if (JOYSTICK_X != newJoystickX || JOYSTICK_Y != newJoystickY){
    JOYSTICK_X = newJoystickX;
    JOYSTICK_Y = newJoystickY;
    EvaluatePanTilt(JOYSTICK_X, JOYSTICK_Y, udpCommand);
    ToSend.Message = udpCommand;
    xQueueSend(UDP_Queue, (void*)&ToSend, (TickType_t) 0);
  }

  if (JOYSTICK_TWIST != newJoystickTwist){
    JOYSTICK_TWIST = newJoystickTwist;
    EvaluateZoom(JOYSTICK_TWIST, udpCommand);
    ToSend.Message = udpCommand;
    xQueueSend(UDP_Queue, (void*)&ToSend, (TickType_t) 0);
  }

}


void EvaluateSetupRXString(String RX) {
  String command, value;
   // split the string on the :
  String xy_response = ":";
  int split = RX.indexOf(xy_response);

  if (split > 0) {
    command = RX.substring(0, split);
    value = RX.substring(split + 1);

    split = value.indexOf(String("\r"));
    value = value.substring(0, split); //remove the \r\n

    // event triggers
    if (command == "encoder1_press") {
      Encoder1Press(value.toInt());
    } else if (command == "encoder1_up") {
      Encoder1(true);
    } else if (command == "encoder1_down") {
      Encoder1(false);

    } else if (command == "encoder2_press") {
      Encoder2Press(value.toInt());
    } else if (command == "encoder2_up") {
      Encoder2(true);
    } else if (command == "encoder2_down") {
      Encoder2(false);

    } else if (command == "key_raw") {
      // raw keypad here
    } else if (command == "key_short") {
      if (value.equals("A")) {
        SELECTED_CAMERA = 1;
        cameraIPSetupMode = false;
      } else if (value.equals("B")) {
        SELECTED_CAMERA = 2;
        cameraIPSetupMode = false;
      } else if (value.equals("C")) {
        SELECTED_CAMERA = 3;
        cameraIPSetupMode = false;
      } else if (value.equals("D")) {
        SELECTED_CAMERA = 4;
        cameraIPSetupMode = false;
      } else if (value.equals("*")) {
        STATUS_TEXT = "Key Pressed *";
      } else if (value.equals("#")) {
        STATUS_TEXT = "Key Pressed #";
        deviceIPSetupMode = false;
      } else {

      }
    } else if (command == "key_long") {
      // STATUS_TEXT = command + " " + value;
    } else if (command == "key_held") {
      if (value.equals("A")) {
        SELECTED_CAMERA = 1;
        cameraIPSetupMode = false;
      } else if (value.equals("B")) {
        SELECTED_CAMERA = 2;
        cameraIPSetupMode = false;
      } else if (value.equals("C")) {
        SELECTED_CAMERA = 3;
        cameraIPSetupMode = false;
      } else if (value.equals("D")) {
        SELECTED_CAMERA = 4;
        cameraIPSetupMode = false;
      } else if (value.equals("*")) {
        STATUS_TEXT = "Key Held *";
        deviceIPSetupMode = false;
      } else if (value.equals("#")) {
        STATUS_TEXT = "Key Held #";
      } else {

      }
    }
  } else {
    Serial.println("ERROR: BAD FORMATTING, NO ':'");
    return;
  }

}



void Net_Event(WiFiEvent_t event){
  switch (event) {
    case SYSTEM_EVENT_ETH_START:
      Serial.println("ETH Started");
      //set eth hostname here
      ETH.setHostname("Cool IR Shtuff");
      break;
    case SYSTEM_EVENT_ETH_CONNECTED:
      Serial.println("ETH Connected");
      break;
    case SYSTEM_EVENT_ETH_GOT_IP:
      Serial.print("ETH MAC: ");
      Serial.print(ETH.macAddress());
      Serial.print(", IPv4: ");
      Serial.print(ETH.localIP());
      if (ETH.fullDuplex()) {
        Serial.print(", FULL_DUPLEX");
      }
      Serial.print(", ");
      Serial.print(ETH.linkSpeed());
      Serial.println("Mbps");
      eth_connected = true;
      
      break;
    case SYSTEM_EVENT_ETH_DISCONNECTED:
      Serial.println("ETH Disconnected");
      eth_connected = false;
      udp_connected = false;
      break;
    case SYSTEM_EVENT_ETH_STOP:
      Serial.println("ETH Stopped");
      eth_connected = false;
      udp_connected = false;
      break;
    default:
      break;
  }
}






void SendVISCACommands(void* pvParameters) {
  UDP_Info received_item;
  int lastCamTracking;
  while (1) {
    if (xQueueReceive(UDP_Queue, &received_item, (TickType_t)20) == pdTRUE) {
      // if received something in the queue
      if (!ETH.linkUp()) {
        Serial.println("ethernet not connected");
        eth_connected = false;
        udp_connected = false;
        break;
      }
      //Serial.print("UDP Sending: ");
      //Serial.println(received_item.Message);

      // have to loop and send packets due to VISCA using \x00 in strings
      // and c using null as the termination char for char arrays and strings
      udp.beginPacket(cameras[SELECTED_CAMERA].ipAddress, cameras[SELECTED_CAMERA].visca_port);
      udp_connected = true;
      for (int i=0; i<24; i++){
        Serial.printf("$%x ",received_item.Message[i]);
        udp.printf("%c", received_item.Message[i]);
        if (received_item.Message[i] == char(255)){
          break;
        }
      }
      udp.endPacket();
      //Serial.println("\nEnd UDP TX");
    }
    vTaskDelay(20);
  }
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  Wire.setPins(OLED_SDA, OLED_SCL);

  //safety default values incase params dont exist
  // cameras[1].ipAddress = IPAddress(192, 168, 1, 28);
  // cameras[2].ipAddress = IPAddress(192, 168, 1, 29);
  // cameras[3].ipAddress = IPAddress(192, 168, 1, 30);
  // cameras[4].ipAddress = IPAddress(192, 168, 1, 101);
  // cameras[1].visca_port = 1259;
  // cameras[2].visca_port = 1259;
  // cameras[3].visca_port = 1259;
  // cameras[4].visca_port = 1259;


  //-------------------------------OLED----------------------------------
  DisplayStartup();
  Serial.println("Booting Yo");
  StartupScreen();
  
  WiFi.onEvent(Net_Event);
  ETH.begin();
  delay(100);
  IpAddressScreen(ETH.localIP(), ETH.subnetMask());
  Serial.print("Local IP Address");
  Serial.println(ETH.localIP());
  delay(2000);



  //---------------------------------------------------------------------
  //-------------------------- Create Integer Queue----------------------
  UDP_Queue = xQueueCreate(16, sizeof (UDP_Info));
  if (UDP_Queue == 0) {
    // Queue not created
	  Serial.print("Unable to create Integer Queue\n");
  } else {
    //if queue is created
	  Serial.print("Integer Queue Created successfully\n");

    xTaskCreate(
      SendVISCACommands,  /* Task function. */
      "GetTemps",         /* String with name of task. */
      10000,              /* Stack size in words. */
      (void*)NULL,        /* Parameter passed as input of the task */
      2,                  /* Priority of the task. */
      &SendUDP_Task);     /* Task handle. */
  }


  StopFocusCallbackTimer =
    xTimerCreate("Stop_Focus_Timer",          // Name of timer - not really used
                  30 / portTICK_PERIOD_MS,   // Period of timer in ticks
                  pdFALSE,                    // pdTRUE to auto-reload timer
                  (void *)0,                  // Timer ID
                  StopFocusCallback);         // the callback function

  // Give timer time to start if needed
  if (StopFocusCallbackTimer == NULL) {
    Serial.println("Could not create timer");
  } else {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    Serial.println("Starting timer");
  }

  SaveValuesCallbackTimer =
    xTimerCreate("Save_Prefs_Timer",          // Name of timer - not really used
                  30 / portTICK_PERIOD_MS,   // Period of timer in ticks
                  pdFALSE,                    // pdTRUE to auto-reload timer
                  (void *)0,                  // Timer ID
                  SaveValuesCallback);         // the callback function

  // Give timer time to start if needed
  if (SaveValuesCallbackTimer == NULL) {
    Serial.println("Could not create SaveValuesCallbackTimer timer");
  } else {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    Serial.println("Starting SaveValuesCallbackTimer timer");
  }


  

  // initialize camera IPs from persistent storage
  RecallCameraValues();
  delay(100);


  //initialize all cameras to auto focus mode
  //sorry, I'm too lazy to read the camera values
  UDP_Info ToSend;
  for (int i=1; i<5; i++){
    FocusAutomatic(udpCommand);
    cameras[SELECTED_CAMERA].focus_mode = 2;
    ToSend.IP_Address = cameras[i].ipAddress;
    ToSend.Port = cameras[i].visca_port;
    ToSend.Message = udpCommand;

    xQueueSend(UDP_Queue, (void*)&ToSend, (TickType_t) 0);
    delay(100);
  }

  
  // StoreAllCameras(); //just for testing
  // delay(1000); //just for testing
  // RecallCameraValues();



}



unsigned long displayTimer;

void loop() {
  String RXString;
  if (Serial2.available()) {
    RXString = Serial2.readStringUntil('\n');
    // Serial.print("RX ");
    // Serial.println(RXString);
    if (cameraIPSetupMode || deviceIPSetupMode){
      EvaluateSetupRXString(RXString);
    }else{
      EvaluateRXString(RXString);
    }
  }

  if (ETH.linkUp()) {
    eth_connected = true;

  } else {
    eth_connected = false;
    udp_connected = false;
  }

  if (millis() > displayTimer) {
    displayTimer = millis() + 30;
    IPAddress ip;
    if (cameraIPSetupMode){
      ip = cameras[SELECTED_CAMERA].ipAddress;
      int p = cameras[SELECTED_CAMERA].visca_port;
      DrawCameraSetup(ip[0], ip[1], ip[2], ip[3], p, 1);
    }else if (deviceIPSetupMode){
      ip = ETH.localIP();
      IPAddress s = ETH.subnetMask();
      DrawDeviceSetup(ip[0], ip[1], ip[2], ip[3], s[0], s[1], s[2], s[3], "DHCP", 1);
    }else{
      drawRX();
    }
  }
}
