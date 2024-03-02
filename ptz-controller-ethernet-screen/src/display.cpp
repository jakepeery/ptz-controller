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

void drawRX(String received) {
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