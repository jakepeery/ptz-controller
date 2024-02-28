#include "main.h"

#include "webserver.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int JOYSTICK_X, JOYSTICK_Y, JOYSTICK_TWIST;
int SELECTED_CAMERA = 1;

static String STATUS_TEXT;



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

void drawScreen(float tmp1, float tmp2, float tmp3, float highTemp,
                float lowTemp, int mode, int selected) {
  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.setCursor(0, 0);              // Start at top-left corner
  display.cp437(true);  // Use full 256 char 'Code Page 437' font

  char stat[64];
  switch (mode) {
    case 1:
      strcpy(stat, "UnOccupied");
      break;
    case 2:
      strcpy(stat, "Heat On");
      break;
    case 3:
      strcpy(stat, "Full Off");
      break;
    case 4:
      strcpy(stat, "Exhaust");
      break;
    case 5:
      strcpy(stat, "Force Fan");
      break;
    case 6:
      sprintf(stat, "%s", "255.255.255.0");
      break;
    default:
      strcpy(stat, "BROKEN");
  }

  display.setTextSize(2);
  if (selected == 0) {
    display.setTextSize(2);
    display.println(stat);

    display.setTextSize(1);
    char highTempPrint[64];
    if (mode == 2) {
      sprintf(highTempPrint, "High %.0f%cF active", highTemp, (char)9);
    } else {
      sprintf(highTempPrint, "High %.0f%cF", highTemp, (char)9);
    }
    display.println(highTempPrint);

    display.setTextSize(1);
    char lowTempPrint[64];
    if (mode == 1) {
      sprintf(lowTempPrint, "Low  %.0f%cF active", lowTemp, (char)9);
    } else {
      sprintf(lowTempPrint, "Low  %.0f%cF", lowTemp, (char)9);
    }
    display.println(lowTempPrint);
  } else if (selected == 1) {
    display.setTextSize(1);
    display.println(stat);

    display.setTextSize(2);
    char highTempPrint[64];
    if (mode == 2) {
      sprintf(highTempPrint, "High %.0f%cF", highTemp, (char)9);
    } else {
      sprintf(highTempPrint, "High %.0f%cF", highTemp, (char)9);
    }
    display.println(highTempPrint);

    display.setTextSize(1);
    char lowTempPrint[64];
    if (mode == 1) {
      sprintf(lowTempPrint, "Low  %.0f%cF active", lowTemp, (char)9);
    } else {
      sprintf(lowTempPrint, "Low  %.0f%cF", lowTemp, (char)9);
    }
    display.println(lowTempPrint);
  } else if (selected == 2) {
    display.setTextSize(1);
    display.println(stat);

    display.setTextSize(1);
    char highTempPrint[64];
    if (mode == 2) {
      sprintf(highTempPrint, "High %.0f%cF active", highTemp, (char)9);
    } else {
      sprintf(highTempPrint, "High %.0f%cF", highTemp, (char)9);
    }
    display.println(highTempPrint);

    display.setTextSize(2);
    char lowTempPrint[64];
    if (mode == 1) {
      sprintf(lowTempPrint, "Low  %.0f%cF", lowTemp, (char)9);
    } else {
      sprintf(lowTempPrint, "Low  %.0f%cF", lowTemp, (char)9);
    }
    display.println(lowTempPrint);
  }

  display.setTextSize(1);
  display.println(F("    Sensor Temps"));

  // handles two vs 3 digit temps for left alignment
  char buffer[256];
  if (tmp1 >= 100.0) {
    sprintf(buffer, " Room       %.1f%cF", tmp1, (char)9);
  } else {
    sprintf(buffer, " Room        %.1f%cF", tmp1, (char)9);
  }
  display.println(buffer);

  // handles two vs 3 digit temps for left alignment
  char buffer2[256];
  if (tmp2 >= 100.0) {
    sprintf(buffer2, " Lower Vent %.1f%cF", tmp2, (char)9);
  } else {
    sprintf(buffer2, " Lower Vent  %.1f%cF", tmp2, (char)9);
  }
  display.println(buffer2);

  // handles two vs 3 digit temps for left alignment
  char buffer3[256];
  if (tmp3 >= 100.0) {
    sprintf(buffer3, " Top Vent   %.1f%cF", tmp3, (char)9);
  } else {
    sprintf(buffer3, " Top Vent    %.1f%cF", tmp3, (char)9);
  }
  display.println(buffer3);

  // State of the system (heating, cooling, standby, etc)

  display.display();
}


unsigned long status_clear_timer;
String Previous_Status_Text;

void drawRX(String received) {
  const char* toPrint = received.c_str(); 
  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.setCursor(0, 0);              // Start at top-left corner
  display.cp437(true);  // Use full 256 char 'Code Page 437' font


  display.setTextSize(1);
  display.print(F("Selected Camera: "));
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


  //status display clear timer
  if (STATUS_TEXT != Previous_Status_Text){
    Previous_Status_Text = STATUS_TEXT;
    status_clear_timer = millis() + 3000;
  }

  if (status_clear_timer <= millis()){
    STATUS_TEXT = "";
  }

  display.setCursor(0, 48);
  char buffer[256];
  sprintf(buffer, " %s", STATUS_TEXT.c_str());

  display.print(buffer);

  

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
  STATUS_TEXT = "Rec Preset " + (String)preset;
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
  if (state){
    STATUS_TEXT = "Encoder1 Pressed";
  } else {
    STATUS_TEXT = "Encoder1 Not Pressed";
  }
}

void Encoder2(bool direction) {
  
  if (direction){
    Serial.println("Encoder2 In");
    STATUS_TEXT = "Encoder2 In";
  } else {
    Serial.println("Encoder2 Out");
    STATUS_TEXT = "Encoder2 Out";
  }
}

void Encoder2Press(bool state){
  if (state){
    STATUS_TEXT = "Encoder2 Pressed";
  } else {
    STATUS_TEXT = "Encoder2 Not Pressed";
  }
}



void EvaluateRXString(String RX) {
  const char* rx_chars = RX.c_str();
  Serial.printf("Evaluating RX: %s\n", rx_chars);

  String command, value;

  // split the string on the :
  String xy_response = ":";
  int split = RX.indexOf(xy_response);
  if (split > 0) {
    String command = RX.substring(0, split);
    String value = RX.substring(split + 1);

    split = value.indexOf(String("\r"));
    value = value.substring(0, split); //remove the \r\n
    Serial.print("X command: ");
    Serial.println(command);

    Serial.print("command == joystick_x ");
    Serial.println(command == "joystick_x");

    // event triggers
    if (command == "joystick_x") {
      JOYSTICK_X = value.toInt();
    } else if (command == "joystick_y") {
      JOYSTICK_Y = value.toInt();
    } else if (command == "joystick_twist") {
      JOYSTICK_TWIST = value.toInt();

    } else if (command == "joystick_raw") {
      // STATUS_TEXT = command;
    } else if (command == "joystick_short_released") {
      STATUS_TEXT = "Joystick Short";
    } else if (command == "joystick_long_released") {
      // STATUS_TEXT = command;
    } else if (command == "joystick_held") {
      STATUS_TEXT = "Joystick Held";
    }
      else if (command == "zoom_toggle") {
      Encoder1Press(value.toInt());
    } else if (command == "zoom_up") {
      Encoder1(true);
    } else if (command == "zoom_down") {
      Encoder1(false);

    } else if (command == "focus_toggle") {
      Encoder2Press(value.toInt());
    } else if (command == "focus_up") {
      Encoder2(true);
    } else if (command == "focus_down") {
      Encoder2(false);
    } else if (command == "key_raw") {
      // raw keypad here
    } else if (command == "key_short") {
        if (value.equals("A")){
          SELECTED_CAMERA = 1;
        } else if (value.equals("B")){
          SELECTED_CAMERA = 2;
        } else if (value.equals("C")){
          SELECTED_CAMERA = 3;
        } else if (value.equals("D")){
          SELECTED_CAMERA = 4;
        } else if (value.equals("*")){
          STATUS_TEXT = "Key Pressed *";
        } else if (value.equals("#")){
          STATUS_TEXT = "Key Pressed #";
        } else{
          RecallPreset(value.toInt());
        }
    } else if (command == "key_long") {
      // STATUS_TEXT = command + " " + value;
    } else if (command == "key_held") {
        if (value.equals("A")){
          SELECTED_CAMERA = 1;
        } else if (value.equals("B")){
          SELECTED_CAMERA = 2;
        } else if (value.equals("C")){
          SELECTED_CAMERA = 3;
        } else if (value.equals("D")){
          SELECTED_CAMERA = 4;
        } else if (value.equals("*")){
          STATUS_TEXT = "Key Held *";
        } else if (value.equals("#")){
          STATUS_TEXT = "Key Held #";
        } else{
          StorePreset(value.toInt());
        }
    }
  } else {
    Serial.println("ERROR: BAD FORMATTING, NO ':'");
    return;
  }
}





void setup() {
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
  delay(100);
  Serial.print("Here Yo");

}

unsigned long displayTimer;


void loop () {
  String RXString;
  String toPrint;
  if (Serial2.available()){
    RXString = Serial2.readStringUntil('\n');
    Serial.print("RX ");
    Serial.println(RXString);
    EvaluateRXString(RXString);
  }

  if (RXString != toPrint && RXString != "") {
    toPrint = RXString;
  }

  if (millis() > displayTimer){
    displayTimer = millis() + 30;
    drawRX(toPrint);
  }
}
