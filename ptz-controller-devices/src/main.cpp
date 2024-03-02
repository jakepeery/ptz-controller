

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

DebouncedButton encoder1_press;
DebouncedButton encoder2_press;
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
  value = value - joystickCalibrationCenter;
  //Serial.println(value);
  if (value > -1 * joystickCenterRange && value < joystickCenterRange){ //at home/center"
    rtnValue = 0;
    //Serial.println("At Home/Center");
  } else if (value < (-1 * joystickCalibrationCenter) + 1) { //all the way down
    rtnValue = -1 * joystickMaxValue;
    //Serial.println("At 0");
  } else if (value > 4094 - joystickCalibrationCenter){ //all the way up
    rtnValue = joystickMaxValue;
    //Serial.println("At full");
  } else if (value <= -1 * joystickCenterRange){ //below home
    rtnValue = value / (joystickCalibrationCenter / joystickMaxValue);
    //Serial.println("Below Home");
  } else if (value >= joystickCenterRange){ //above home
    rtnValue = value / ((4094 - joystickCalibrationCenter) / joystickMaxValue);
    //Serial.println("Above Home");
  }
  return rtnValue;
}



void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  delay(1000);
  printHardwareInfo();
  //Serial2.println("Starting Controls ESP32");
  delay(1000);
  //Serial2.println("Starting Controls ESP32");
  
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
  //---------------------------------------encoder1 Encoder------------------------------------------------------
  Serial.println("Setting up encoder1 encoder pins");
  pinMode(enc_encoder1_press_pin, INPUT_PULLUP);
  encoder1_press.PIN = enc_encoder1_press_pin;
  encoder1_press.Delay = debaounce_delay;

  pinMode(enc_encoder1_up_pin, INPUT_PULLUP);
  pinMode(enc_encoder1_dn_pin, INPUT_PULLUP);

  //---------------------------------------encoder1 Encoder------------------------------------------------------
  //---------------------------------------------------------------------------------------------






  //---------------------------------------------------------------------------------------------
  //---------------------------------------encoder2 Encoder------------------------------------------------------
  Serial.println("Setting up encoder2 encoder pins");
  pinMode(enc_encoder2_press_pin, INPUT_PULLUP);
  encoder2_press.PIN = enc_encoder2_press_pin;
  encoder2_press.Delay = debaounce_delay;

  pinMode(enc_encoder2_up_pin, INPUT_PULLUP);
  pinMode(enc_encoder2_dn_pin, INPUT_PULLUP);

  //---------------------------------------encoder2 Encoder------------------------------------------------------
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

bool encoder1_encoder_state = false;
int encoder1_value = 128;
bool encoder1_button_state;
bool encoder1_toggle = false;

bool encoder2_encoder_state = false;
int encoder2_value = 128;
bool encoder2_button_state;
bool encoder2_toggle = false;

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

  // encoder1 encoder
  bool encoder1_pressed = !encoder1_press.buttonDebounce();
  if (encoder1_pressed != encoder1_button_state && encoder1_pressed) {
    Serial2.printf("encoder1_press:1\r\n");
    if (encoder1_toggle) {
      encoder1_toggle = false;
      Serial.printf("encoder1_toggle = %d\n", encoder1_toggle);
    } else {
      encoder1_toggle = true;
      Serial.printf("encoder1_toggle = %d\n", encoder1_toggle);
    }

  }
  encoder1_button_state = encoder1_pressed;


  bool encoder1_up = !digitalRead(enc_encoder1_up_pin);
  bool encoder1_dn = !digitalRead(enc_encoder1_dn_pin);
  if ((encoder1_up != encoder1_encoder_state) && encoder1_up) {
    // Serial.printf("up: %d    down: %d\n", encoder1_up, encoder1_dn);
    if (encoder1_dn != encoder1_up) {
      Serial.printf("encoder1 UP. value start: %d\n", encoder1_value);
      if (encoder1_value < encoder1_max) {
        encoder1_value = encoder1_value + 1 * encoder1_rate_multiplier;
        
      } else {
        encoder1_value = encoder1_max;
      }
      Serial.printf("encoder1 up. value end: %d\n", encoder1_value);
      //Serial2.printf("encoder1_value:%d\r\n", encoder1_value);
      Serial2.printf("encoder1_up:1\r\n");

    } else {
      Serial.printf("encoder1 DOWN. value start: %d\n", encoder1_value);
      if (encoder1_value > encoder1_min) {
        encoder1_value = encoder1_value - 1 * encoder1_rate_multiplier;
      } else {
        encoder1_value = encoder1_min;
      }
      Serial.printf("encoder1 down. value end: %d\n", encoder1_value);
      //Serial2.printf("encoder1_value:%d\r\n", encoder1_value);
      Serial2.printf("encoder1_down:1\r\n");
    }
  }
  encoder1_encoder_state = encoder1_up;

  // encoder2 encoder
  bool encoder2_pressed = !encoder2_press.buttonDebounce();
  if (encoder2_pressed != encoder2_button_state && encoder2_pressed) {
    Serial2.printf("encoder2_press:1\r\n");
    if (encoder2_toggle) {
      encoder2_toggle = false;
      Serial.printf("encoder2_toggle = %d\n", encoder2_toggle);
    } else {
      encoder2_toggle = true;
      Serial.printf("encoder2_toggle = %d\n", encoder2_toggle);
    }
    
  }
  encoder2_button_state = encoder2_pressed;


  bool encoder2_up = !digitalRead(enc_encoder2_up_pin);
  bool encoder2_dn = !digitalRead(enc_encoder2_dn_pin);
  if ((encoder2_up != encoder2_encoder_state) && encoder2_up) {
    
    if (encoder2_dn != encoder2_up) {
      Serial.printf("encoder2 UP. value start: %d\n", encoder2_value);
      if (encoder2_value < encoder2_max) {
        encoder2_value = encoder2_value + 1 * encoder2_rate_multiplier;
      } else {
        encoder2_value = encoder2_max;
      }
      Serial.printf("encoder2 up. value end: %d\n", encoder2_value);
      //Serial2.printf("encoder2_value:%d\r\n", encoder2_value);
      Serial2.printf("encoder2_up:1\r\n");
    } else {
      Serial.printf("encoder2 DOWN. value start: %d\n", encoder2_value);
      if (encoder2_value > encoder2_min) {
        encoder2_value = encoder2_value - 1 * encoder2_rate_multiplier;
      } else {
        encoder2_value = encoder2_min;
      }
      Serial.printf("encoder2 down. value end: %d\n", encoder2_value);
      //Serial2.printf("encoder2_value:%d\r\n", encoder2_value);
      Serial2.printf("encoder2_down:1\r\n");
    }
  }
  encoder2_encoder_state = encoder2_up;

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

    //Serial.printf("X:%d, Y:%d, T:%d\n", joystick_x,  joystick_y, joystick_twist);


    joystick_x = EvaluateAnalog(joystick_x);
    joystick_y = EvaluateAnalog(joystick_y);
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