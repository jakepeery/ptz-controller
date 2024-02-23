#include <Arduino.h>

// time in ms to reset the encoder press button selection to the default of the
// mode selection not temp selection
// aka wait 5 seconds to go back to selecting on/off/occupied
// Replace with your network credentials

//Keypad - rows = outputs, cols = pull up
#define keypad_row1_pin 13
#define keypad_row2_pin 12
#define keypad_row3_pin 14
#define keypad_row4_pin 27

#define keypad_col1_pin 26
#define keypad_col2_pin 25
#define keypad_col3_pin 33
#define keypad_col4_pin 32

#define keypad_press_hold_time 3000
#define keypad_press_debounce_time 20


//Zoom encoder - set to pull up
#define enc_zoom_press_pin 5
#define enc_zoom_up_pin 18
#define enc_zoom_dn_pin 19

#define zoom_rate_multiplier 1
#define zoom_max 255
#define zoom_min 0


//Focus encoder - set to pull up
#define enc_focus_press_pin 21
#define enc_focus_up_pin 22
#define enc_focus_dn_pin 23

#define focus_rate_multiplier 1
#define focus_max 255
#define focus_min 0


//voltage/analog
#define joystick_press_pin 4 //pull up
#define joystick_press_hold_time 3000
#define joystick_press_debounce_time 20

#define joystick_x_pin 34
#define joystick_y_pin 35
#define joystick_twist_pin 36

#define RXD2 16
#define TXD2 17








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