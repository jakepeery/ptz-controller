# PTZ-Controller
## ESP32 #1 - Ethernet
- Used for sending VISCA over IP commands to Ethernet
- Controls the OLED
- Receives UART from second ESP32
- Board Used: wireless-tag.com WT32-ETH01 V1.2
## ESP32 #2
- Used for getting GPIO from keypad, encoders, and joystick
- Communicates with ESP32 #1 via UART
- Board used: ESP32 DEVKITV1
