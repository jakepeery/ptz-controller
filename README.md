# VISCA overIP PTZ Camera Controller
## ESP32 #1 - Ethernet
- Used for sending VISCA over IP commands to Ethernet
- Controls the OLED
- Receives UART from second ESP32
- Board Used: wireless-tag.com WT32-ETH01 V1.2
## ESP32 #2
- Used for getting GPIO from keypad, encoders, and joystick
- Communicates with ESP32 #1 via UART
- Board used: ESP32 DEVKITV1

I've included the .stl of the case I printed.  I did have to modify this after printing to include a hole for the network port.  I may update the drawing at some point in the future.

## Controls:
- Encoder 1
    - Press: Manual/Auto focus .
        - This can get out of sync if recalling presets.
        - Sets all cameras to auto on initial boot.
    - Rotate: only enabled in manual mode and controls focus.
- Encoder 2
    - Press: Selects the max speed setting.
    - Rotate: adjusts max speed setting to scale the Joystick appropriately.
- Joystick X/Y: pretty self explanatory.
- Joystick twist: Zoom in/out.
- Joystick pushbutton: One push focus (not entirely sure if this is actually doing anything in the cameras I'm using, but it's the correct command).
- Keypad 0-9:
    - Short press: Recall preset 0-9.
    - Long press (3 seconds): Store preset 0-9.
- Keypad A-D:
    - Short press: Select camera to control.
    - Long press (3 seconds): Change camera IP address and port.
        - Encoder 1/2 press to select values.
        - Encoder rotate: increment value up/down.
            - Encoder 1 will adjust +/- 200 for the port values.
        - Short press from settings page to store new values and return.
- Keypad * Long Press (3 seconds): Enter device IP settings.
    - Keypad # short press: exits the device settings page.
    - Keypad # and * long press: exits the device settings page.


## Parts
Part          | Link
------------- | -------------
Ethernet ESP-32| https://www.amazon.com/gp/product/B09V2VMMMX
Normal ESP-32  | https://www.amazon.com/gp/product/B08D5ZD528
Joystick       | https://www.amazon.com/gp/product/B0BRJBJ1GC
Encoders       | https://www.amazon.com/gp/product/B08BFJ4F5C
Keypad         | https://www.amazon.com/gp/product/B07THCLGCZ
Display        |https://www.amazon.com/gp/product/B0B2R57SCJ
- I had to solder some resistors on the display to convert it for I2C
