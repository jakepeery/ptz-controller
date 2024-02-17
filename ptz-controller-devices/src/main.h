#include <Arduino.h>

// time in ms to reset the encoder press button selection to the default of the
// mode selection not temp selection
// aka wait 5 seconds to go back to selecting on/off/occupied
// Replace with your network credentials

//pull up
#define keypad_row1_pin 26
#define keypad_row2_pin 25
#define keypad_row3_pin 33
#define keypad_row4_pin 32

//require manual pull down resistors
#define keypad_col1_pin 35
#define keypad_col2_pin 34
#define keypad_col3_pin 39
#define keypad_col4_pin 36

//pull up
#define enc_zoom_press_pin 5
#define enc_zoom_up_pin 18
#define enc_zoom_dn_pin 18

//pull up
#define enc_focus_press_pin 15
#define enc_focus_up_pin 2
#define enc_focus_dn_pin 4

//voltage/analog
#define joystick1_press_pin 27
#define joystick1_x_pin 13
#define joystick1_y_pin 14

//potentiometer
#define jopystick2_x_pin 21
#define jopystick2_y_pin 22
#define jopystick2_twist_pin 23



class DebouncedButton {
 public:
  bool State = false;
  bool State_Last = false;
  unsigned long LAST_DEBOUNCE_TIME_PRESS = 0;
  unsigned long Delay = 10;
  int PIN;

  boolean buttonDebounce() {
    bool btnState = digitalRead(PIN);
    if (btnState != State_Last) {
      LAST_DEBOUNCE_TIME_PRESS = millis();
    }

    if ((millis() - LAST_DEBOUNCE_TIME_PRESS) > Delay) {
      if (btnState != State) {
        State = btnState;
        Serial.println("GPIO Pressed: " + String(PIN) +
                       " State: " + String(State));
      }
    }
    State_Last = btnState;
    return State;
  }
};


  //---------------------------------------------------------------------------------------------
  //---------------------------------------Keypad------------------------------------------------------


    //---------------------------------------Keypad------------------------------------------------------
  //---------------------------------------------------------------------------------------------