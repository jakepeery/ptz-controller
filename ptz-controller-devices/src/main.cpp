

#include "main.h"


void printHardwareInfo() {
  Serial.printf("\n\nChip Cores:         %d\n", ESP.getChipCores());
  Serial.printf("CPU frequency:      %d\n\n", ESP.getCpuFreqMHz());

  Serial.printf("Total heap:         %d\n", ESP.getHeapSize());
  Serial.printf("Free heap:          %d\n", ESP.getFreeHeap());
  Serial.printf("Used heap:          %d\n\n",
                ESP.getHeapSize() - ESP.getFreeHeap());

  Serial.printf("Total PSRAM:        %d\n", ESP.getPsramSize());
  Serial.printf("Free PSRAM:         %d\n\n", ESP.getFreePsram());

  Serial.printf("Total Flash:        %d\n", ESP.getFlashChipSize());
  Serial.printf("Free sketch space:  %d\n", ESP.getFreeSketchSpace());
  Serial.printf("Sketch Size:        %d\n", ESP.getSketchSize());
}
//---------------------------------------------------------------------------------------------
//---------------------------------------------------------Setup-------------------------------
DebouncedButton keypad_col1;
DebouncedButton keypad_col2;
DebouncedButton keypad_col3;
DebouncedButton keypad_col4;

DebouncedButton zoom_press;
DebouncedButton focus_press;
DebouncedButton joystick_press;

String keypadLayout[4][4] = {{"1", "2", "3", "A"},
                             {"4", "5", "6", "B"},
                             {"7", "8", "9", "C"},
                             {"*", "0", "#", "D"}};
String ScanKeypad(){
  bool col1, col2, col3, col4;
  int rows[4] = {keypad_row1_pin, keypad_row2_pin, keypad_row3_pin, keypad_row4_pin};
  String key = "";
  
  int colPressed = 0;
  for (int row=0; row<=3; row++){
    // Serial.println("\nRead");
    digitalWrite(rows[row], 0);

    col1 = digitalRead(keypad_col1_pin);
    col2 = digitalRead(keypad_col2_pin);
    col3 = digitalRead(keypad_col3_pin);
    col4 = digitalRead(keypad_col4_pin);

    if (!col1) {colPressed = 0;}
    if (!col2) {colPressed = 1;}
    if (!col3) {colPressed = 2;}
    if (!col4) {colPressed = 3;}

    digitalWrite(rows[row], 1);
    
    if (!col1 || !col2 || !col3 || !col4){
      // Serial.printf("Row: %d      Col: %d  col1:%d, col2:%d, col3:%d, col4:%d\n", row, colPressed, col1, col2, col3, col4);
      // Serial.println(keypadLayout[row][colPressed]);
      key = keypadLayout[row][colPressed];
    }
  }
  
  return key;
}


//---------------------------------------------------------------------------------------------
//---------------------------------------Keypad Events-----------------------------------------
void KeypadPressed(String key){
    Serial.printf("\nraw key pressed: %s\n", key);
}

void KeypadReleasedShort(String key, unsigned long time){
Serial.printf("short key released: %s   time: %lu seconds\n", key,
                      time / 1000);
}

void KeypadReleasedLong(String key, unsigned long time){
  Serial.printf("held key released: %s   time: %lu seconds\n", key,
                      time / 1000);
}

void KeypadHeld(String key, unsigned long time){
  Serial.printf("held key event: %s   time: %lu seconds\n", key,
                      time / 1000);
}

//---------------------------------------Keypad Events-----------------------------------------
//---------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(115200);
  delay(1000);
  printHardwareInfo();
  delay(1000);
  
  const int debaounce_delay = 50;

  //---------------------------------------------------------------------------------------------
  //---------------------------------------Keypad------------------------------------------------------

  Serial.println("Setting up keypad pins");
  pinMode(keypad_row1_pin, OUTPUT);
  pinMode(keypad_row2_pin, OUTPUT);
  pinMode(keypad_row3_pin, OUTPUT);
  pinMode(keypad_row4_pin, OUTPUT);

  pinMode(keypad_col1_pin, INPUT_PULLUP);
  keypad_col1.PIN = keypad_col1_pin;
  keypad_col1.Delay = debaounce_delay;

  pinMode(keypad_col2_pin, INPUT_PULLUP);
  keypad_col2.PIN = keypad_col2_pin;
  keypad_col2.Delay = debaounce_delay;

  pinMode(keypad_col3_pin, INPUT_PULLUP);
  keypad_col3.PIN = keypad_col3_pin;
  keypad_col3.Delay = debaounce_delay;

  pinMode(keypad_col4_pin, INPUT_PULLUP);
  keypad_col4.PIN = keypad_col4_pin;
  keypad_col4.Delay = debaounce_delay;
  //---------------------------------------Keypad------------------------------------------------------
  //---------------------------------------------------------------------------------------------







  //---------------------------------------------------------------------------------------------
  //---------------------------------------Zoom Encoder------------------------------------------------------
  Serial.println("Setting up zoom encoder pins");
  pinMode(enc_zoom_press_pin, INPUT_PULLUP);
  zoom_press.PIN = enc_zoom_press_pin;
  zoom_press.Delay = debaounce_delay;

  pinMode(enc_zoom_up_pin, INPUT_PULLUP);
  pinMode(enc_zoom_dn_pin, INPUT_PULLUP);

  //---------------------------------------Zoom Encoder------------------------------------------------------
  //---------------------------------------------------------------------------------------------






  //---------------------------------------------------------------------------------------------
  //---------------------------------------Focus Encoder------------------------------------------------------
  Serial.println("Setting up focus encoder pins");
  pinMode(enc_focus_press_pin, INPUT_PULLUP);
  focus_press.PIN = enc_focus_press_pin;
  focus_press.Delay = debaounce_delay;

  pinMode(enc_focus_up_pin, INPUT_PULLUP);
  pinMode(enc_focus_dn_pin, INPUT_PULLUP);

  //---------------------------------------Focus Encoder------------------------------------------------------
  //---------------------------------------------------------------------------------------------






  //---------------------------------------------------------------------------------------------
  //---------------------------------------Joystick1------------------------------------------------------
  Serial.println("Setting up joystick pins");
  pinMode(joystick_press_pin, INPUT_PULLUP);
  joystick_press.PIN = joystick_press_pin;
  joystick_press.Delay = debaounce_delay;

  //---------------------------------------Joystick1------------------------------------------------------
  //---------------------------------------------------------------------------------------------






  //---------------------------------------------------------------------------------------------
  //---------------------------------------Joystick2------------------------------------------------------


  //---------------------------------------Joystick2------------------------------------------------------
  //---------------------------------------------------------------------------------------------





}

bool BTN_PRESSED_STATE = false;
bool UP_ENCODER_STATE = false;
String lastKeyPressed;
unsigned long keypadPressedStart;
unsigned long keypadPressedTime;

bool currentKeyBeingPressed = false;
bool keypadHeldEvent = false;

void loop() {
  //-------------------------------------------------------------------------
  //----------------------------------Keypad Events--------------------------
  String keyPressed;
  keyPressed = ScanKeypad();

  if (keyPressed != lastKeyPressed) {  // key state changes
    lastKeyPressed = keyPressed;
    if (keyPressed == "") {  // key released
      currentKeyBeingPressed = false;
      keypadHeldEvent = true;
      keypadPressedTime = millis() - keypadPressedStart;
      if (keypadPressedTime > press_hold_time) {  // long release
        KeypadReleasedLong(keyPressed, keypadPressedTime);
      } else if (keypadPressedTime > press_debounce_time) {  // short release
        KeypadReleasedShort(keyPressed, keypadPressedTime);
      }

    } else {  // key pressed
      KeypadPressed(keyPressed);
      currentKeyBeingPressed = true;
      keypadHeldEvent = false;
      keypadPressedStart = millis();
    }
  }

  if (millis() - keypadPressedStart > press_hold_time && !keypadHeldEvent) {
    KeypadHeld(keyPressed, millis() - keypadPressedStart);
    keypadHeldEvent = true;
  }
  //----------------------------------Keypad Events-------------------------
  //------------------------------------------------------------------------

  // Zoom encoder
  bool zoom_pressed = zoom_press.buttonDebounce();
  bool zoom_up = digitalRead(enc_zoom_up_pin);
  bool zoom_dn = digitalRead(enc_zoom_dn_pin);
  if (!zoom_pressed || !zoom_up || !zoom_dn){
    Serial.printf("zoom pressed %d, zoom up: %d, zoom down: %d\n", zoom_pressed, zoom_up, zoom_dn);
  }


  // Focus encoder
  bool focus_pressed = focus_press.buttonDebounce();
  bool focus_up = digitalRead(enc_focus_up_pin);
  bool focus_dn = digitalRead(enc_focus_dn_pin);  
  if (!focus_pressed || !focus_up || !focus_dn){
    Serial.printf("focus pressed %d, focus up: %d, focus down: %d\n", focus_pressed, focus_up, focus_dn);
  }

  //Joystick #1 - cheap style
  bool joystick_pressed = joystick_press.buttonDebounce();
  int joystick_x = analogRead(joystick_x_pin);
  int joystick_y = analogRead(joystick_y_pin);

  if (!joystick_pressed) {
    //Serial.println("Joystick Pressed");
  }

  if (joystick_x < 1800 || joystick_y < 1800 || joystick_x > 2020 || joystick_y > 2020) {
    //Serial.printf("Joystick X, Y: %d, %d\n", joystick_x, joystick_y);
  }


// delay(100);
}