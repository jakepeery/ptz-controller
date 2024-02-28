

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
    Serial2.printf("key_raw:%s\r\n", key);
}

void KeypadReleasedShort(String key, unsigned long time){
  Serial.printf("short key released: %s   time: %lu ticks\n", key,
                      time);
  Serial2.printf("key_short:%s\r\n", key);
}

void KeypadReleasedLong(String key, unsigned long time){
  Serial.printf("held key released: %s   time: %lu ticks\n", key,
                      time);
  Serial2.printf("key_long:%s\r\n", key);
}

void KeypadHeld(String key, unsigned long time){
  Serial.printf("held key event: %s   time: %lu ticks\n", key,
                      time);
  Serial2.printf("key_held:%s\r\n", key);
}

//---------------------------------------Keypad Events-----------------------------------------
//---------------------------------------------------------------------------------------------



//---------------------------------------------------------------------------------------------
//---------------------------------------Joystick Events-----------------------------------------
void JoystickPressed(String joystick){
    Serial.printf("\nraw joystick pressed: %s\n", joystick);
    Serial2.printf("joystick_raw:%s\r\n", joystick);
}

void JoystickReleasedShort(String joystick, unsigned long time){
  Serial.printf("short joystick released: %s   time: %lu ticks\n", joystick,
                      time);
  Serial2.printf("joystick_short_released:%s\r\n", joystick);
}

void JoystickReleasedLong(String joystick, unsigned long time){
  Serial.printf("held joystick released: %s   time: %lu ticks\n", joystick,
                      time);
  Serial2.printf("joystick_long_released:%s\r\n", joystick);
}

void JoystickHeld(String joystick, unsigned long time){
  Serial.printf("held joystick event: %s   time: %lu ticks\n", joystick,
                      time);
  Serial2.printf("joystick_held:%s\r\n", joystick);
}

//---------------------------------------Joystick Events-----------------------------------------
//---------------------------------------------------------------------------------------------



int EvaluateAnalog(int value){
  int rtnValue = 0;
  value = value - 1800;
  if (value > -300 && value < 300){
    rtnValue = 0;
  }  else if (value < -1800){
    rtnValue = -20;

  } else if (value > 2295){
    rtnValue = 20;
  } else if (value <= -200){
    rtnValue = value / 90;

  } else if (value >= 200){
    rtnValue = value / 114;
  }
  return rtnValue;
}



void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  delay(1000);
  printHardwareInfo();
  Serial2.println("Starting Controls ESP32");
  delay(1000);
  Serial2.println("Starting Controls ESP32");
  
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

unsigned long joystickPressedStart;
unsigned long joystickPressedTime;
bool joystickLastState = false;
bool joystickHeldEvent = false;

bool zoom_encoder_state = false;
int zoom_value = 128;
bool zoom_button_state;
bool zoom_toggle = false;

bool focus_encoder_state = false;
int focus_value = 128;
bool focus_button_state;
bool focus_toggle = false;

unsigned long joystickPacer;

int x_send, y_send, twist_send;

void loop() {
  //-------------------------------------------------------------------------
  //----------------------------------Keypad Events--------------------------
  String keyPressed;
  keyPressed = ScanKeypad();

  if (keyPressed != lastKeyPressed) {  // key state changes
    
    if (keyPressed == "") {  // key released
      currentKeyBeingPressed = false;
      keypadHeldEvent = true;
      keypadPressedTime = millis() - keypadPressedStart;
      if (keypadPressedTime > keypad_press_hold_time) {  // long release
        KeypadReleasedLong(lastKeyPressed, keypadPressedTime);
      } else if (keypadPressedTime > keypad_press_debounce_time) {  // short release
        KeypadReleasedShort(lastKeyPressed, keypadPressedTime);
      }

    } else {  // key pressed
      KeypadPressed(keyPressed);
      currentKeyBeingPressed = true;
      keypadHeldEvent = false;
      keypadPressedStart = millis();
    }
    lastKeyPressed = keyPressed;
  }

  if (millis() - keypadPressedStart > keypad_press_hold_time && !keypadHeldEvent) {
    KeypadHeld(keyPressed, millis() - keypadPressedStart);
    keypadHeldEvent = true;
  }
  //----------------------------------Keypad Events-------------------------
  //------------------------------------------------------------------------

  // Zoom encoder
  bool zoom_pressed = !zoom_press.buttonDebounce();
  if (zoom_pressed != zoom_button_state && zoom_pressed) {
    if (zoom_toggle) {
      zoom_toggle = false;
      Serial.printf("zoom_toggle = %d\n", zoom_toggle);
    } else {
      zoom_toggle = true;
      Serial.printf("zoom_toggle = %d\n", zoom_toggle);
    }
    Serial2.printf("zoom_toggle:%d\r\n", zoom_toggle);
  }
  zoom_button_state = zoom_pressed;


  bool zoom_up = !digitalRead(enc_zoom_up_pin);
  bool zoom_dn = !digitalRead(enc_zoom_dn_pin);
  if ((zoom_up != zoom_encoder_state) && zoom_up) {
    // Serial.printf("up: %d    down: %d\n", zoom_up, zoom_dn);
    if (zoom_dn != zoom_up) {
      Serial.printf("ZOOM UP. value start: %d\n", zoom_value);
      if (zoom_value < zoom_max) {
        zoom_value = zoom_value + 1 * zoom_rate_multiplier;
        
      } else {
        zoom_value = zoom_max;
      }
      Serial.printf("zoom up. value end: %d\n", zoom_value);
      //Serial2.printf("zoom_value:%d\r\n", zoom_value);
      Serial2.printf("zoom_up:1\r\n");

    } else {
      Serial.printf("ZOOM DOWN. value start: %d\n", zoom_value);
      if (zoom_value > zoom_min) {
        zoom_value = zoom_value - 1 * zoom_rate_multiplier;
      } else {
        zoom_value = zoom_min;
      }
      Serial.printf("zoom down. value end: %d\n", zoom_value);
      //Serial2.printf("zoom_value:%d\r\n", zoom_value);
      Serial2.printf("zoom_down:1\r\n");
    }
  }
  zoom_encoder_state = zoom_up;

  // Focus encoder
  bool focus_pressed = !focus_press.buttonDebounce();
  if (focus_pressed != focus_button_state && focus_pressed) {
    if (focus_toggle) {
      focus_toggle = false;
      Serial.printf("focus_toggle = %d\n", focus_toggle);
    } else {
      focus_toggle = true;
      Serial.printf("focus_toggle = %d\n", focus_toggle);
    }
    Serial2.printf("focus_toggle:%d\r\n", focus_toggle);
  }
  focus_button_state = focus_pressed;


  bool focus_up = !digitalRead(enc_focus_up_pin);
  bool focus_dn = !digitalRead(enc_focus_dn_pin);
  if ((focus_up != focus_encoder_state) && focus_up) {
    
    if (focus_dn != focus_up) {
      Serial.printf("FOCUS UP. value start: %d\n", focus_value);
      if (focus_value < focus_max) {
        focus_value = focus_value + 1 * focus_rate_multiplier;
      } else {
        focus_value = focus_max;
      }
      Serial.printf("focus up. value end: %d\n", focus_value);
      //Serial2.printf("focus_value:%d\r\n", focus_value);
      Serial2.printf("focus_up:1\r\n");
    } else {
      Serial.printf("FOCUS DOWN. value start: %d\n", focus_value);
      if (focus_value > focus_min) {
        focus_value = focus_value - 1 * focus_rate_multiplier;
      } else {
        focus_value = focus_min;
      }
      Serial.printf("focus down. value end: %d\n", focus_value);
      //Serial2.printf("focus_value:%d\r\n", focus_value);
      Serial2.printf("focus_down:1\r\n");
    }
  }
  focus_encoder_state = focus_up;

  // Joystick #1 - cheap style
  bool joystick_pressed = !joystick_press.buttonDebounce();


  if (joystick_pressed != joystickLastState) {  // key state changes
    joystickLastState = joystick_pressed;
    if (joystick_pressed == false) {  // key released
      currentKeyBeingPressed = false;
      joystickHeldEvent = true;
      joystickPressedTime = millis() - joystickPressedStart;
      if (joystickPressedTime > joystick_press_hold_time) {  // long release
        JoystickReleasedLong("1", joystickPressedTime);
      } else if (joystickPressedTime > joystick_press_debounce_time) {  // short release
        JoystickReleasedShort("1", joystickPressedTime);
      }

    } else {  // key pressed
      JoystickPressed("1");
      currentKeyBeingPressed = true;
      joystickHeldEvent = false;
      joystickPressedStart = millis();
    }
  }

  if (millis() - joystickPressedStart > keypad_press_hold_time && !joystickHeldEvent) {
    JoystickHeld("1", millis() - joystickPressedStart);
    joystickHeldEvent = true;
  }


  //slow down the joystick writes
  if (millis() > joystickPacer) {
    joystickPacer = millis() + joystickAnalogPaceTime;
 
    int joystick_x = analogRead(joystick_x_pin);
    int joystick_y = analogRead(joystick_y_pin);
    int joystick_twist = analogRead(joystick_twist_pin);

    // //shift the values
    // joystick_x = joystick_x - (4095/2);
    // joystick_y = joystick_y - (4095/2);
    // joystick_twist = joystick_twist - (4095/2);

    // //range the values
    // int high = 10;

    // joystick_x = (int)((float)joystick_x/(4095.0/2) * high);
    // joystick_y = (int)((float)joystick_y/(4095.0/2) * high);
    // joystick_twist = (int)((float)joystick_twist/(4095.0/2) * high);
    Serial.printf("X:%d, Y:%d, T:%d\n", joystick_x,  joystick_y, joystick_twist);


    joystick_x = EvaluateAnalog(joystick_x);
    joystick_y = -1 * EvaluateAnalog(joystick_y);
    joystick_twist = EvaluateAnalog(joystick_twist);


    if (x_send != joystick_x) {
      x_send = joystick_x;
      Serial.printf("Joystick X: %d\n", x_send);
      Serial2.printf("joystick_x:%d\r\n", x_send);
    }
    if (y_send != joystick_y) {
      y_send = joystick_y;
      Serial.printf("Joystick Y: %d\n", y_send);
      Serial2.printf("joystick_y:%d\r\n", y_send);
    }

    if (twist_send != joystick_twist) {
      twist_send = joystick_twist;
      Serial.printf("Joystick Twist: %d\n", twist_send);
      Serial2.printf("joystick_twist:%d\r\n", twist_send);
    }
  }
}