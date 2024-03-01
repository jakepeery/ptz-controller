#include <Arduino.h>

#include "SPIFFS.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Preferences.h>
#include "SPIFFS.h"
#include <SPI.h>


#include "ETH.h"
#include "udp.h"
#include "AsyncUDP.h"


#define RXD2 5
#define TXD2 17


// OLED Pins
#define OLED_SCL 4
#define OLED_SDA 14



#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET -1  // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS \
  0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32





QueueHandle_t UDP_Queue;

struct UDP_Info {
  IPAddress IP_Address;
  int Port;
  String Message;
};