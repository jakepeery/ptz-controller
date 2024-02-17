

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
DebouncedButton keypad_row1;
DebouncedButton keypad_row2;
DebouncedButton keypad_row3;
DebouncedButton keypad_row4;

DebouncedButton keypad_col1;
DebouncedButton keypad_col2;
DebouncedButton keypad_col3;
DebouncedButton keypad_col4;

DebouncedButton zoom_press;
DebouncedButton focus_press;
DebouncedButton joystick1_press;

DebouncedButton joystick2_press;

void setup() {
  Serial.begin(115200);
  const int debaounce_delay = 50;

  //---------------------------------------------------------------------------------------------
  //---------------------------------------Keypad------------------------------------------------------


  pinMode(keypad_row1_pin, INPUT_PULLUP);
  keypad_row1.PIN = keypad_row1_pin;
  keypad_row1.Delay = debaounce_delay;

  pinMode(keypad_row2_pin, INPUT_PULLUP);
  keypad_row2.PIN = keypad_row2_pin;
  keypad_row2.Delay = debaounce_delay;

  pinMode(keypad_row3_pin, INPUT_PULLUP);
  keypad_row3.PIN = keypad_row3_pin;
  keypad_row3.Delay = debaounce_delay;

  pinMode(keypad_row4_pin, INPUT_PULLUP);
  keypad_row4.PIN = keypad_row4_pin;
  keypad_row4.Delay = debaounce_delay;



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
  pinMode(enc_zoom_press_pin, INPUT_PULLUP);
  zoom_press.PIN = enc_zoom_press_pin;
  zoom_press.Delay = debaounce_delay;

  pinMode(enc_zoom_up_pin, INPUT_PULLUP);
  pinMode(enc_zoom_dn_pin, INPUT_PULLUP);

  //---------------------------------------Zoom Encoder------------------------------------------------------
  //---------------------------------------------------------------------------------------------






  //---------------------------------------------------------------------------------------------
  //---------------------------------------Focus Encoder------------------------------------------------------
  pinMode(enc_focus_press_pin, INPUT_PULLUP);
  focus_press.PIN = enc_focus_press_pin;
  focus_press.Delay = debaounce_delay;

  pinMode(enc_focus_up_pin, INPUT_PULLUP);
  pinMode(enc_focus_dn_pin, INPUT_PULLUP);

  //---------------------------------------Focus Encoder------------------------------------------------------
  //---------------------------------------------------------------------------------------------






  //---------------------------------------------------------------------------------------------
  //---------------------------------------Joystick1------------------------------------------------------
  pinMode(joystick1_press_pin, INPUT_PULLUP);
  keypad_row1.PIN = joystick1_press_pin;
  keypad_row1.Delay = debaounce_delay;

  //---------------------------------------Joystick1------------------------------------------------------
  //---------------------------------------------------------------------------------------------






  //---------------------------------------------------------------------------------------------
  //---------------------------------------Joystick2------------------------------------------------------
  keypad_row1.PIN = joystick2_press_pin;
  keypad_row1.Delay = debaounce_delay;

  //---------------------------------------Joystick2------------------------------------------------------
  //---------------------------------------------------------------------------------------------





  //---------------------------------------------------------------------------------------------
  //---------------------------------------------------------------Loops--------------------
  xTaskCreate(
      GetTemps,                   /* Task function. */
      "GetTemps",                 /* String with name of task. */
      10000,                      /* Stack size in words. */
      (void *)&USER.SENSOR_TEMPS, /* Parameter passed as input of the task */
      2,                          /* Priority of the task. */
      &ReadTempsTask);            /* Task handle. */

  xTaskCreatePinnedToCore(
      MaintainWifi,                 /* Task function. */
      "MaintainWifi",               /* String with name of task. */
      10000,                        /* Stack size in words. */
      (void *)&USER,                /* Parameter passed as input of the task */
      6,                            /* Priority of the task. */
      &MaintainWifiTask,            /* Task handle. */
      CONFIG_ARDUINO_RUNNING_CORE); /* Core to run on. */

  storeTempsOneShotTimer =
      xTimerCreate("Store_Temp_Timer",  // Name of timer - not really used
                   2000 / portTICK_PERIOD_MS,  // Period of timer in ticks
                   pdFALSE,                    // pdTRUE to auto-reload timer
                   (void *)0,                  // Timer ID
                   SaveCurrentTempsCallback);  // the callback function

  // Give timer time to start if needed
  if (storeTempsOneShotTimer == NULL) {
    Serial.println("Could not create timer");
  } else {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    Serial.println("Starting timer");
  }
}

bool BTN_PRESSED_STATE = false;
bool UP_ENCODER_STATE = false;

void loop() {
  // Keypad Press
  bool row1 = keypad_row1.buttonDebounce();
  bool row2 = keypad_row2.buttonDebounce();
  bool row3 = keypad_row3.buttonDebounce();
  bool row4 = keypad_row4.buttonDebounce();

  bool col1 = keypad_col1.buttonDebounce();
  bool col2 = keypad_col2.buttonDebounce();
  bool col3 = keypad_col3.buttonDebounce();
  bool col4 = keypad_col4.buttonDebounce();

  if (row1 || row2 || row3 || row4 || col1 || col2 || col3 || col4){
    Serial.printf("rows: %s, %s, %s, %s\n cols: %s, %s, %s, %s\n", row1, row2, row3, row4, col1, col2, col3, col4)
  }


  // Zoom encoder
  bool zoom_pressed = zoom_press.buttonDebounce();
  bool zoom_up = digitalRead(enc_zoom_up_pin);      // UpEncoder.buttonDebounce(); //
  bool zoom_dn = digitalRead(enc_zoom_dn_pin);  // DownEncoder.buttonDebounce(); //
  if (zoom_pressed || zoom_up || zoom_dn){
    Serial.printf("zoom pressed %s, zoom up: %s, zoom down: %s\n", zoom_pressed, zoom_up, zoom_dn)
  }



}