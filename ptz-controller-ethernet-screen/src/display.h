
#include <Adafruit_SSD1306.h>
void StartupScreen();
void IpAddressScreen(IPAddress displayIP, IPAddress displaySubnet);
void drawRX();
void DisplayStartup();
void DrawCameraSetup(int oct1, int oct2, int oct3, int oct4,
                     int cameraVISCAPort, int selectedValue);
void DrawDeviceSetup(int oct1, int oct2, int oct3, int oct4, int sub1, int sub2,
                     int sub3, int sub4, bool DHCPMode, int selectedValue);