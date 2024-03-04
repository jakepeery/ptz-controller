#include "main.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void DisplayStartup() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }
  delay(500);
  display.clearDisplay();
}

void StartupScreen() {
  char stat[64];
  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.setCursor(0, 0);              // Start at top-left corner
  display.cp437(true);  // Use full 256 char 'Code Page 437' font

  display.setTextSize(2);
  display.println("BOOTING");
  display.println("Please");
  display.println("Wait..");
  display.display();
}

String IpAddress2String(const IPAddress& ipAddress) {
  return String(ipAddress[0]) + String(".") +\
  String(ipAddress[1]) + String(".") +\
  String(ipAddress[2]) + String(".") +\
  String(ipAddress[3])  ; 
}



void IpAddressScreen(IPAddress displayIP, IPAddress displaySubnet) {
  char stat[64];
  char mode[64];
  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.setCursor(0, 0);              // Start at top-left corner
  display.cp437(true);  // Use full 256 char 'Code Page 437' font

  display.setTextSize(2);
  sprintf(stat, "%s", IpAddress2String(displayIP));
  sprintf(mode, "%s", IpAddress2String(displaySubnet));
  display.println("Network:");
  display.setTextSize(1);
  display.print("IP Addr:");
  display.println(stat);
  display.print("Subnet:");
  display.println(mode);
  display.display();
}



unsigned long status_clear_timer;
String Previous_Status_Text;

void drawRX() {
  // const char* toPrint = received.c_str(); 
  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.setCursor(0, 0);              // Start at top-left corner
  display.cp437(true);  // Use full 256 char 'Code Page 437' font


  display.setTextSize(1);
  display.print(F("Selected Cam: "));
  char sel_cam[8];
  //Serial.println(SELECTED_CAMERA);
  if (SELECTED_CAMERA == 1){
    sprintf(sel_cam, "A");
  } else if (SELECTED_CAMERA == 2){
    sprintf(sel_cam, "B");
  } else if (SELECTED_CAMERA == 3){
    sprintf(sel_cam, "C");
  } else if (SELECTED_CAMERA == 4){
    sprintf(sel_cam, "D");
  } else {
    sprintf(sel_cam, "N/A");
  }
  display.print(sel_cam);


  //connected indicator
  char connection_indicator[8];
  if (eth_connected && udp_connected){
    sprintf(connection_indicator, "   + +");
  } else if (eth_connected){
    sprintf(connection_indicator, "   + -");
  }else if (udp_connected){
    sprintf(connection_indicator, "   - +");
  }else{
    sprintf(connection_indicator, "   - -");
  }
  udp_connected = false;
  display.print(connection_indicator);




  //status display clear timer
  if (STATUS_TEXT != Previous_Status_Text){
    Previous_Status_Text = STATUS_TEXT;
    status_clear_timer = millis() + 3000;
  }

  if (status_clear_timer <= millis()){
    STATUS_TEXT = "";
  }

  display.setCursor(0, 10);
  char buffer[256];
  sprintf(buffer, " %s", STATUS_TEXT.c_str());
  display.print(buffer);




  //focus type selection
  display.setCursor(4, 28);
  char focusMode[32];

  // mode2=auto    mode3=manual
  if (cameras[SELECTED_CAMERA].focus_mode == 2){
    sprintf(focusMode, "Focus: AUTO");
  } else {
    sprintf(focusMode, "Focus: Manual");
  }
  display.print(focusMode);




  // max speed values
  display.setCursor(4, 37);
  char max_title[32];
  sprintf(max_title, "Max Speed Settings");
  display.print(max_title);

  display.setCursor(0, 46);
  char max_values_x[32];
  sprintf(max_values_x, "X:%d", cameras[SELECTED_CAMERA].max_pan);
  display.print(max_values_x);

  display.setCursor(35, 46);
  char max_values_y[32];
  sprintf(max_values_y, "Y:%d", cameras[SELECTED_CAMERA].max_tilt);
  display.print(max_values_y);

  display.setCursor(80, 46);
  char max_values_z[32];
  sprintf(max_values_z, "Zoom:%d", cameras[SELECTED_CAMERA].max_zoom);
  display.print(max_values_z);

  //underline selected max value - timeout after 5 seconds
  if (selected_max == 1){
    display.drawFastHLine(0, 54, 20, SSD1306_WHITE);
  } else if (selected_max == 2){
    display.drawFastHLine(35, 54, 20, SSD1306_WHITE);
  } else if (selected_max == 3){
    display.drawFastHLine(80, 54, 20, SSD1306_WHITE);
  }
    
  
  


  //joystick x, y, z values
  char joystick_x_print[32];
  sprintf(joystick_x_print, "X:%d", JOYSTICK_X);

  char joystick_y_print[32];
  sprintf(joystick_y_print, "Y:%d", JOYSTICK_Y);

  char joystick_twist_print[32];
  sprintf(joystick_twist_print, "Zoom:%d", JOYSTICK_TWIST);
  
  display.setCursor(0, 57);
  display.print(joystick_x_print);

  display.setCursor(35, 57);
  display.print(joystick_y_print);

  display.setCursor(80, 57);
  display.print(joystick_twist_print);


  display.display();
}

void DrawCameraSetup(int oct1, int oct2, int oct3, int oct4,
                     int cameraVISCAPort, int selectedValue) {
  char ipAddress1[8];
  char ipAddress2[8];
  char ipAddress3[8];
  char ipAddress4[8];
  sprintf(ipAddress1, "%03d.",oct1);
  sprintf(ipAddress2, "%03d.",oct2);
  sprintf(ipAddress3, "%03d.",oct3);
  sprintf(ipAddress4, "%03d",oct4);

  char port[16];
  sprintf(port, "%d",cameraVISCAPort);

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.setCursor(0, 0);              // Start at top-left corner
  display.cp437(true);  // Use full 256 char 'Code Page 437' font


  display.setTextSize(1);
  display.print(F("Setup Cam: "));
  char sel_cam[8];
  //Serial.println(SELECTED_CAMERA);
  if (SELECTED_CAMERA == 1){
    sprintf(sel_cam, "A");
  } else if (SELECTED_CAMERA == 2){
    sprintf(sel_cam, "B");
  } else if (SELECTED_CAMERA == 3){
    sprintf(sel_cam, "C");
  } else if (SELECTED_CAMERA == 4){
    sprintf(sel_cam, "D");
  } else {
    sprintf(sel_cam, "N/A");
  }
  display.print(sel_cam);


  // sprintf(ipAddress, "%s", IpAddress2String(cameras[SELECTED_CAMERA].ipAddress));
  // sprintf(port, "%d", cameras[SELECTED_CAMERA].visca_port);
  display.setTextSize(1);

  const int row = 30;
  const int startCol = 5;
  const int octetWidth = 24;
  
  display.setCursor(0, row -10);
  display.print("IP Addr:");

  display.setCursor(startCol, row);
  display.print(ipAddress1);
  //display.print(".");

  display.setCursor(startCol + octetWidth, row);
  display.print(ipAddress2);
  //display.print(".");

  display.setCursor(startCol + 2 * octetWidth, row);
  display.print(ipAddress3);
  //display.print(".");

  display.setCursor(startCol + 3 * octetWidth, row);
  display.print(ipAddress4);


  display.setCursor(0, row +15);
  display.print("Port:");
  display.setCursor(5, row +25);
  display.print(port);


  switch (selectedValue){
    //DHCP mode
    case 1:
    display.drawFastHLine(5, row + 33, 24, SSD1306_WHITE);
    break;

    //ip address
    case 2:
    display.drawFastHLine(startCol + 0 * octetWidth , row + 8, 18, SSD1306_WHITE);
    break;

    case 3:
    display.drawFastHLine(startCol + 1 * octetWidth, row + 8, 18, SSD1306_WHITE);
    break;

    case 4:
    display.drawFastHLine(startCol + 2 * octetWidth, row + 8, 18, SSD1306_WHITE);
    break;

    case 5:
    display.drawFastHLine(startCol + 3 * octetWidth, row + 8, 18, SSD1306_WHITE);
    break;
  }



  display.display();
}

void DrawDeviceSetup(int oct1, int oct2, int oct3, int oct4, int sub1, int sub2,
                     int sub3, int sub4, bool DHCPMode, int selectedValue) {
  char ipAddress1[8];
  char ipAddress2[8];
  char ipAddress3[8];
  char ipAddress4[8];
  sprintf(ipAddress1, "%03d.",oct1);
  sprintf(ipAddress2, "%03d.",oct2);
  sprintf(ipAddress3, "%03d.",oct3);
  sprintf(ipAddress4, "%03d",oct4);

  char subnetMask1[8];
  char subnetMask2[8];
  char subnetMask3[8];
  char subnetMask4[8];
  sprintf(subnetMask1, "%03d.",sub1);
  sprintf(subnetMask2, "%03d.",sub2);
  sprintf(subnetMask3, "%03d.",sub3);
  sprintf(subnetMask4, "%03d",sub4);

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.setCursor(0, 0);              // Start at top-left corner
  display.cp437(true);  // Use full 256 char 'Code Page 437' font

  display.setTextSize(1);
  display.print(F("Setup Device: "));

  display.setTextSize(1);


  // ip address
  const int row = 20;
  const int startCol = 5;
  const int octetWidth = 24;
  
  display.setCursor(0, row -10);
  display.print("IP Addr:");

  display.setCursor(startCol, row);
  display.print(ipAddress1);
  //display.print(".");

  display.setCursor(startCol + octetWidth, row);
  display.print(ipAddress2);
  //display.print(".");

  display.setCursor(startCol + 2 * octetWidth, row);
  display.print(ipAddress3);
  //display.print(".");

  display.setCursor(startCol + 3 * octetWidth, row);
  display.print(ipAddress4);



  // subnet mask
  display.setTextSize(1);

  const int subnetRow = 40;
  
  display.setCursor(0, subnetRow -10);
  display.print("Subnet:");

  display.setCursor(startCol, subnetRow);
  display.print(subnetMask1);
  //display.print(".");

  display.setCursor(startCol + octetWidth, subnetRow);
  display.print(subnetMask2);
  //display.print(".");

  display.setCursor(startCol + 2 * octetWidth, subnetRow);
  display.print(subnetMask3);
  //display.print(".");

  display.setCursor(startCol + 3 * octetWidth, subnetRow);
  display.print(subnetMask4);



  // DHCP mode

  display.setCursor(0, subnetRow + 10);
  display.print("Mode:");
  if (DHCPMode){
    display.println("DHCP");    //DHCP or Static
  } else {
    display.println("Static");  //DHCP or Static
  }

    //underline selected max value - timeout after 5 seconds
  switch (selectedValue){
    //DHCP mode
    case 1:
    display.drawFastHLine(29, subnetRow + 18, 24, SSD1306_WHITE);
    break;

    //ip address
    case 2:
    display.drawFastHLine(startCol + 0 * octetWidth , row + 8, 18, SSD1306_WHITE);
    break;

    case 3:
    display.drawFastHLine(startCol + 1 * octetWidth, row + 8, 18, SSD1306_WHITE);
    break;

    case 4:
    display.drawFastHLine(startCol + 2 * octetWidth, row + 8, 18, SSD1306_WHITE);
    break;

    case 5:
    display.drawFastHLine(startCol + 3 * octetWidth, row + 8, 18, SSD1306_WHITE);
    break;

    //subnet
    case 6:
    display.drawFastHLine(startCol + 0 *octetWidth, subnetRow + 8, 18, SSD1306_WHITE);
    break;

    case 7:
    display.drawFastHLine(startCol + 1 * octetWidth, subnetRow + 8, 18, SSD1306_WHITE);
    break;

    case 8:
    display.drawFastHLine(startCol + 2 * octetWidth, subnetRow + 8, 18, SSD1306_WHITE);
    break;

    case 9:
    display.drawFastHLine(startCol + 3 * octetWidth, subnetRow + 8, 18, SSD1306_WHITE);
    break;
  } 


  display.display();
}