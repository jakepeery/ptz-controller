#include "main.h"
#include "VISCA_Evaluators.h"
#include "display.h"


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


void Encoder1(bool direction) {
  if (direction){
    Serial.println("Encoder1 In");
    STATUS_TEXT = "Encoder1 In";
  } else {
    Serial.println("Encoder1 Out");
    STATUS_TEXT = "Encoder1 Out";
  }
}

void Encoder1Press(bool state){
  UDP_Info Testing;
  Testing.IP_Address = cameras[SELECTED_CAMERA].ipAddress;
  Testing.Port = cameras[SELECTED_CAMERA].visca_port;
  Testing.Message = "UDP Message To Send";

  xQueueSend(UDP_Queue, (void*)&Testing, (TickType_t) 0);
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
  const char* rx_chars = RX.c_str();
  // Serial.printf("Evaluating RX: %s\n", rx_chars);

  UDP_Info ToSend;
  ToSend.IP_Address = cameras[SELECTED_CAMERA].ipAddress;
  ToSend.Port = cameras[SELECTED_CAMERA].visca_port;

  String command, value;

  // split the string on the :
  String xy_response = ":";
  int split = RX.indexOf(xy_response);

  if (split > 0) {
    String command = RX.substring(0, split);
    String value = RX.substring(split + 1);

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
      }
    } else if (command == "key_long") {
      // STATUS_TEXT = command + " " + value;
    } else if (command == "key_held") {
      if (value.equals("A")) {
        SELECTED_CAMERA = 1;
      } else if (value.equals("B")) {
        SELECTED_CAMERA = 2;
      } else if (value.equals("C")) {
        SELECTED_CAMERA = 3;
      } else if (value.equals("D")) {
        SELECTED_CAMERA = 4;
      } else if (value.equals("*")) {
        STATUS_TEXT = "Key Held *";
      } else if (value.equals("#")) {
        STATUS_TEXT = "Key Held #";
      } else {
        StorePreset(value.toInt());
      }
    }
  } else {
    Serial.println("ERROR: BAD FORMATTING, NO ':'");
    return;
  }

  
  static char udpCommand[24];
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
  // to do initialize camera IPs from persistent storage
  cameras[1].ipAddress = IPAddress(192, 168, 1, 28);
  cameras[2].ipAddress = IPAddress(192, 168, 1, 29);
  cameras[3].ipAddress = IPAddress(192, 168, 1, 30);
  cameras[4].ipAddress = IPAddress(192, 168, 1, 101);
  cameras[1].visca_port = 1259;
  cameras[2].visca_port = 1259;
  cameras[3].visca_port = 1259;
  cameras[4].visca_port = 1259;

  //to do initialize max ptz values from persistent storage 
  //*******(maybe just initialize the entire cameras struct??)*******

  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  Wire.setPins(OLED_SDA, OLED_SCL);

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
}



unsigned long displayTimer;

void loop() {
  String RXString;
  String toPrint;
  if (Serial2.available()) {
    RXString = Serial2.readStringUntil('\n');
    // Serial.print("RX ");
    // Serial.println(RXString);
    EvaluateRXString(RXString);
  }

  if (RXString != toPrint && RXString != "") {
    toPrint = RXString;
  }


  if (ETH.linkUp()) {
    eth_connected = true;

  } else {
    eth_connected = false;
    udp_connected = false;
  }

  if (millis() > displayTimer) {
    displayTimer = millis() + 30;
    drawRX(toPrint);
  }
}
