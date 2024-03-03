#include <Arduino.h>
#include "SPIFFS.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Preferences.h>
#include "SPIFFS.h"
#include <SPI.h>

#include "ETH.h"
#include "WiFiUdp.h"
//#include "Udp.h"
//#include "AsyncUDP.h"

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

#define MAX_PAN 24
#define MAX_TILT 23
#define MAX_ZOOM 8 //range is 0-7 but I use 0 as stop

struct UDP_Info {
  IPAddress IP_Address;
  int Port;
  char* Message;
};

struct CameraProperties{
  int max_pan = MAX_PAN; //should not initialize to this with parameter recall
  int max_tilt = MAX_TILT;  //should not initialize to this with parameter recall
  int max_zoom = MAX_ZOOM;   //should not initialize to this with parameter recall
  IPAddress ipAddress;
  int visca_port;
  int focus_mode = 2; //initialize as auto mode
};

extern int JOYSTICK_X, JOYSTICK_Y, JOYSTICK_TWIST;
extern int SELECTED_CAMERA;

extern String STATUS_TEXT;

static TaskHandle_t SendUDP_Task;

extern bool eth_connected;
extern bool udp_connected;

extern CameraProperties cameras[5];
//extern AsyncUDP udp;
extern QueueHandle_t UDP_Queue;
extern int selected_max;

