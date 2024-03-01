#include "main.h"



Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

static int JOYSTICK_X, JOYSTICK_Y, JOYSTICK_TWIST;
static int SELECTED_CAMERA = 1;


static String STATUS_TEXT;

static TaskHandle_t SendUDP_Task;

static bool eth_connected = false;
static bool udp_connected = false;

AsyncUDP udp;


struct CameraProperties{
  int max_pan = 24;
  int max_tilt = 23;
  int max_zoom = 7;
  IPAddress ipAddress;
  int visca_port;
};

CameraProperties cameras[5];

int selected_max;




#define up char(1)    //"\x01"
#define down char(2)  //"\x02"
#define left char(1)  //"\x01"
#define right char(2) //"\x02"
#define stop char(3)  //"\x03"
// expects a pan / x and tilt / y value for the intended speed\
// pan range = -24 to 24 decimal
// tilt range = -23 to 23 decimal 
void EvaluatePanTilt(int x, int y, String *command){
  char ptzPrefix[] ="\x81\x01\x06\x01";
  char x_direction, y_direction;
  char x_speed, y_speed;

  //limit incoming to available VISCA speed options
  if (x > 24) { x = 24;}
  if (x < -24) { x = -24;}
  if (y > 23) { y = 23;}
  if (y < -23) { y = -23;}
  if (x == 0){
      x_direction = stop;
      x_speed = char(1);
  } else if (x < 0){
      x_direction = left;
      x_speed = char(abs(x));
  } else if (x > 0){
      x_direction = right;
      x_speed = char(abs(x));
  }

    if (y == 0){
      y_direction = stop;
      y_speed = char(1); 
  } else if (y < 0){
      y_direction = left;
      y_speed = char(abs(y));
  } else if (y > 0){
      y_direction = right;
      y_speed = char(abs(y));
    }
  //Serial.printf("%s%c%c%c%c%c", ptzPrefix, x_speed, y_speed, x_direction, y_direction, char(255));
  char temp[24];
  sprintf(temp, "%s%c%c%c%c%c", ptzPrefix, x_speed, y_speed, x_direction, y_direction, char(255));

  *command = String(temp);
}



void StartupScreen() {
  char stat[64];
  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.setCursor(0, 0);              // Start at top-left corner
  display.cp437(true);  // Use full 256 char 'Code Page 437' font

  display.setTextSize(2);
  sprintf(stat, "%s", "255.255.255.0");
  display.println("BOOTING");
  display.println("Please");
  display.println("Wait..");
  display.display();
}

void IpAddressScreen() {
  char stat[64];
  char mode[64];
  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.setCursor(0, 0);              // Start at top-left corner
  display.cp437(true);  // Use full 256 char 'Code Page 437' font

  display.setTextSize(2);
  sprintf(stat, "%s", "255.255.255.0");
  sprintf(mode, "%s", "255.255.255.0");
  display.println("Wifi");
  display.setTextSize(1);
  display.print("IP Addr ");
  display.println(stat);
  display.println("");
  display.print("Wifi Mode ");
  display.println("Busted");
  display.display();
}


unsigned long status_clear_timer;
String Previous_Status_Text;

void drawRX(String received) {
  // const char* toPrint = received.c_str(); 
  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.setCursor(0, 0);              // Start at top-left corner
  display.cp437(true);  // Use full 256 char 'Code Page 437' font


  display.setTextSize(1);
  display.print(F("Selected Cam: "));
  char sel_cam[8];
  //Serial.println(SELECTED_CAMERA);
  if (SELECTED_CAMERA == 1){
    sprintf(sel_cam, "A");
  } else if (SELECTED_CAMERA == 2){
    sprintf(sel_cam, "B");
  } else if (SELECTED_CAMERA == 3){
    sprintf(sel_cam, "C");
  } else if (SELECTED_CAMERA == 4){
    sprintf(sel_cam, "D");
  } else {
    sprintf(sel_cam, "N/A");
  }
  display.print(sel_cam);


  //connected indicator
  char connection_indicator[8];
  if (eth_connected && udp_connected){
    sprintf(connection_indicator, "   + +");
  } else if (eth_connected){
    sprintf(connection_indicator, "   + -");
  }else if (udp_connected){
    sprintf(connection_indicator, "   - +");
  }else{
    sprintf(connection_indicator, "   - -");
  }
  display.print(connection_indicator);




  //status display clear timer
  if (STATUS_TEXT != Previous_Status_Text){
    Previous_Status_Text = STATUS_TEXT;
    status_clear_timer = millis() + 3000;
  }

  if (status_clear_timer <= millis()){
    STATUS_TEXT = "";
  }

  display.setCursor(0, 10);
  char buffer[256];
  sprintf(buffer, " %s", STATUS_TEXT.c_str());
  display.print(buffer);


  // max speed values
  display.setCursor(4, 37);
  char max_title[32];
  sprintf(max_title, "Max Speed Settings");
  display.print(max_title);

  display.setCursor(0, 46);
  char max_values_x[32];
  sprintf(max_values_x, "X:%d", cameras[SELECTED_CAMERA].max_pan);
  display.print(max_values_x);

  display.setCursor(35, 46);
  char max_values_y[32];
  sprintf(max_values_y, "Y:%d", cameras[SELECTED_CAMERA].max_tilt);
  display.print(max_values_y);

  display.setCursor(80, 46);
  char max_values_z[32];
  sprintf(max_values_z, "Zoom:%d", cameras[SELECTED_CAMERA].max_zoom);
  display.print(max_values_z);

  //underline selected max value - timeout after 5 seconds
  if (selected_max == 1){
    display.drawFastHLine(0, 54, 20, SSD1306_WHITE);
  } else if (selected_max == 2){
    display.drawFastHLine(35, 54, 20, SSD1306_WHITE);
  } else if (selected_max == 3){
    display.drawFastHLine(80, 54, 20, SSD1306_WHITE);
  }
    
  
  


  //joystick x, y, z values
  char joystick_x_print[32];
  sprintf(joystick_x_print, "X:%d", JOYSTICK_X);

  char joystick_y_print[32];
  sprintf(joystick_y_print, "Y:%d", JOYSTICK_Y);

  char joystick_twist_print[32];
  sprintf(joystick_twist_print, "Zoom:%d", JOYSTICK_TWIST);
  
  display.setCursor(0, 57);
  display.print(joystick_x_print);

  display.setCursor(35, 57);
  display.print(joystick_y_print);

  display.setCursor(80, 57);
  display.print(joystick_twist_print);


  display.display();
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

  if (cameras[SELECTED_CAMERA].max_pan > 24){
    cameras[SELECTED_CAMERA].max_pan = 24;
  }
  if (cameras[SELECTED_CAMERA].max_tilt > 23){
    cameras[SELECTED_CAMERA].max_tilt = 23;
  }
  if (cameras[SELECTED_CAMERA].max_zoom > 7){
    cameras[SELECTED_CAMERA].max_zoom = 7;
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
  const char* rx_chars = RX.c_str();
  // Serial.printf("Evaluating RX: %s\n", rx_chars);

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
      // newJoystickX = temp_x * int(float(cameras[SELECTED_CAMERA].max_pan) / 100.0);

    } else if (command == "joystick_y") {
      int temp_y = value.toInt();
      newJoystickY = (int)((float)temp_y *
                           (float)(cameras[SELECTED_CAMERA].max_tilt / 100.0));
      // newJoystickY = temp_y * int(float(cameras[SELECTED_CAMERA].max_tilt) / 100.0);

    } else if (command == "joystick_twist") {
      int temp_twist = value.toInt();
      JOYSTICK_TWIST =
          (int)((float)temp_twist *
                (float)(cameras[SELECTED_CAMERA].max_zoom / 100.0));

    } else if (command == "joystick_raw") {
      // STATUS_TEXT = command;
    } else if (command == "joystick_short_released") {
      STATUS_TEXT = "Joystick Short";
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


  if (JOYSTICK_X != newJoystickX || JOYSTICK_Y != newJoystickY){
    JOYSTICK_X = newJoystickX;
    JOYSTICK_Y = newJoystickY;
    // Serial.print("JoystickX: ");
    // Serial.println(JOYSTICK_X);
    // Serial.print("JoystickY: ");
    // Serial.println(JOYSTICK_Y);

    String pan_tilt_cmd;
    EvaluatePanTilt(JOYSTICK_X, JOYSTICK_Y, &pan_tilt_cmd);
    UDP_Info ToSend;
    ToSend.IP_Address = cameras[SELECTED_CAMERA].ipAddress;
    ToSend.Port = cameras[SELECTED_CAMERA].visca_port;
    ToSend.Message = pan_tilt_cmd;
      
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
      // if (udp.connect(IPAddress(192,168,2,75), 48630)) {
      //   udp_connected = true;
      // }else {
      //   udp_connected = false;
      // }
      
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





UDP_Info received_item;
int lastCamTracking;
void SendVISCACommands(void* pvParameters) {
  while (1) {
    // make sure to be connected to the proper camera
    if (lastCamTracking != SELECTED_CAMERA) {
      lastCamTracking = SELECTED_CAMERA;
      if (udp.connect(cameras[SELECTED_CAMERA].ipAddress,
                      cameras[SELECTED_CAMERA].visca_port)) {
        udp_connected = true;
        udp.printf("Connecting to camera: %d\nIP:", SELECTED_CAMERA);
        udp.println(cameras[SELECTED_CAMERA].ipAddress);

        Serial.printf("Connecting to camera: %d\nIP:", SELECTED_CAMERA);
        Serial.println(cameras[SELECTED_CAMERA].ipAddress);
      } else {
        udp_connected = false;
      }
    }
    if (xQueueReceive(UDP_Queue, &received_item, (TickType_t)20) == pdTRUE) {
      // if received something in the queue
      if (!ETH.linkUp()) {
        Serial.println("ethernet not connected");
        eth_connected = false;
        udp_connected = false;
        break;
      }

      // Serial.print(received_item.IP_Address);
      // Serial.print("  Message:");
      // Serial.println(received_item.Message);
      udp.print(received_item.Message);

    }
    vTaskDelay(2);
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

  //-----------------------------------------------------------OLED
  // Setup------------------------------------
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }
  delay(500);
  display.clearDisplay();
  Serial.println("Booting Yo");
  StartupScreen();


  IpAddressScreen();
  WiFi.onEvent(Net_Event);
  ETH.begin();
  delay(100);
  Serial.print("Local IP Address");
  Serial.println(ETH.localIP());
  delay(100);

  //---------------------------------------------------------------------
  //-------------------------- Create Integer Queue----------------------
  UDP_Queue = xQueueCreate(16, sizeof (UDP_Info));
  if (UDP_Queue == 0)  // Queue not created
  {
	  Serial.print("Unable to create Integer Queue\n");
  }
  else {
    //if queue is created
	  Serial.print("Integer Queue Created successfully\n");

    xTaskCreate(
      SendVISCACommands,                   /* Task function. */
      "GetTemps",                 /* String with name of task. */
      10000,                      /* Stack size in words. */
      (void*)NULL, /* Parameter passed as input of the task */
      2,                          /* Priority of the task. */
      &SendUDP_Task);            /* Task handle. */


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

  if (udp.connected()) {
    udp_connected = true;
  } else {
    udp_connected = false;
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
