#include "main.h"

#include "webserver.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


void StartupScreen() {
  char stat[64];
  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.setCursor(0, 0);              // Start at top-left corner
  display.cp437(true);  // Use full 256 char 'Code Page 437' font

  display.setTextSize(2);
  sprintf(stat, "%s", "255.255.255.0");
  display.println("BOOTING");
  display.println("Please");
  display.println("Wait..");
  display.display();
}

void IpAddressScreen() {
  char stat[64];
  char mode[64];
  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.setCursor(0, 0);              // Start at top-left corner
  display.cp437(true);  // Use full 256 char 'Code Page 437' font

  display.setTextSize(2);
  sprintf(stat, "%s", "255.255.255.0");
  sprintf(mode, "%s", "255.255.255.0");
  display.println("Wifi");
  display.setTextSize(1);
  display.print("IP Addr ");
  display.println(stat);
  display.println("");
  display.print("Wifi Mode ");
  display.println("Busted");
  display.display();
}

void drawScreen(float tmp1, float tmp2, float tmp3, float highTemp,
                float lowTemp, int mode, int selected) {
  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.setCursor(0, 0);              // Start at top-left corner
  display.cp437(true);  // Use full 256 char 'Code Page 437' font

  char stat[64];
  switch (mode) {
    case 1:
      strcpy(stat, "UnOccupied");
      break;
    case 2:
      strcpy(stat, "Heat On");
      break;
    case 3:
      strcpy(stat, "Full Off");
      break;
    case 4:
      strcpy(stat, "Exhaust");
      break;
    case 5:
      strcpy(stat, "Force Fan");
      break;
    case 6:
      sprintf(stat, "%s", "255.255.255.0");
      break;
    default:
      strcpy(stat, "BROKEN");
  }

  display.setTextSize(2);
  if (selected == 0) {
    display.setTextSize(2);
    display.println(stat);

    display.setTextSize(1);
    char highTempPrint[64];
    if (mode == 2) {
      sprintf(highTempPrint, "High %.0f%cF active", highTemp, (char)9);
    } else {
      sprintf(highTempPrint, "High %.0f%cF", highTemp, (char)9);
    }
    display.println(highTempPrint);

    display.setTextSize(1);
    char lowTempPrint[64];
    if (mode == 1) {
      sprintf(lowTempPrint, "Low  %.0f%cF active", lowTemp, (char)9);
    } else {
      sprintf(lowTempPrint, "Low  %.0f%cF", lowTemp, (char)9);
    }
    display.println(lowTempPrint);
  } else if (selected == 1) {
    display.setTextSize(1);
    display.println(stat);

    display.setTextSize(2);
    char highTempPrint[64];
    if (mode == 2) {
      sprintf(highTempPrint, "High %.0f%cF", highTemp, (char)9);
    } else {
      sprintf(highTempPrint, "High %.0f%cF", highTemp, (char)9);
    }
    display.println(highTempPrint);

    display.setTextSize(1);
    char lowTempPrint[64];
    if (mode == 1) {
      sprintf(lowTempPrint, "Low  %.0f%cF active", lowTemp, (char)9);
    } else {
      sprintf(lowTempPrint, "Low  %.0f%cF", lowTemp, (char)9);
    }
    display.println(lowTempPrint);
  } else if (selected == 2) {
    display.setTextSize(1);
    display.println(stat);

    display.setTextSize(1);
    char highTempPrint[64];
    if (mode == 2) {
      sprintf(highTempPrint, "High %.0f%cF active", highTemp, (char)9);
    } else {
      sprintf(highTempPrint, "High %.0f%cF", highTemp, (char)9);
    }
    display.println(highTempPrint);

    display.setTextSize(2);
    char lowTempPrint[64];
    if (mode == 1) {
      sprintf(lowTempPrint, "Low  %.0f%cF", lowTemp, (char)9);
    } else {
      sprintf(lowTempPrint, "Low  %.0f%cF", lowTemp, (char)9);
    }
    display.println(lowTempPrint);
  }

  display.setTextSize(1);
  display.println(F("    Sensor Temps"));

  // handles two vs 3 digit temps for left alignment
  char buffer[256];
  if (tmp1 >= 100.0) {
    sprintf(buffer, " Room       %.1f%cF", tmp1, (char)9);
  } else {
    sprintf(buffer, " Room        %.1f%cF", tmp1, (char)9);
  }
  display.println(buffer);

  // handles two vs 3 digit temps for left alignment
  char buffer2[256];
  if (tmp2 >= 100.0) {
    sprintf(buffer2, " Lower Vent %.1f%cF", tmp2, (char)9);
  } else {
    sprintf(buffer2, " Lower Vent  %.1f%cF", tmp2, (char)9);
  }
  display.println(buffer2);

  // handles two vs 3 digit temps for left alignment
  char buffer3[256];
  if (tmp3 >= 100.0) {
    sprintf(buffer3, " Top Vent   %.1f%cF", tmp3, (char)9);
  } else {
    sprintf(buffer3, " Top Vent    %.1f%cF", tmp3, (char)9);
  }
  display.println(buffer3);

  // State of the system (heating, cooling, standby, etc)

  display.display();
}



void drawRX(String received) {
  const char* toPrint = received.c_str(); 
  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.setCursor(0, 0);              // Start at top-left corner
  display.cp437(true);  // Use full 256 char 'Code Page 437' font


  display.setTextSize(1);
  display.println(F("    Sensor Temps"));

  // handles two vs 3 digit temps for left alignment
  char buffer[256];
  sprintf(buffer, "RX %s", toPrint);

  display.print(buffer);


  // State of the system (heating, cooling, standby, etc)

  display.display();
}





void EvaluateRXString(String RX) {
  const char* toPrint = RX.c_str(); 
  Serial.printf("Evaluating RX: %s\n", toPrint);
}





void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  Wire.setPins(OLED_SDA, OLED_SCL);

  //-----------------------------------------------------------OLED
  // Setup------------------------------------
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }
  delay(500);
  display.clearDisplay();
  Serial.println("Booting Yo");
  StartupScreen();


   IpAddressScreen();
  delay(5000);

}

unsigned long displayTimer;
String RXString;
String toPrint;
char messageRX;

// const char CR = "\r";
// const char LF = "\n";

void loop () {
  if (Serial2.available()){
    //messageRX = char(Serial2.readStringUntil('\n'));
    RXString = Serial2.readStringUntil('\n');
    Serial.print("RX ");
    Serial.println(RXString);
    EvaluateRXString(RXString);
  }

// RXString.concat(messageRX);
//   if (messageRX == '\r' || messageRX == '\n'){
//     Serial.println("This is what i'm looing for");
//     RXString.clear();
//   }else {
    
  // }

  if (RXString != toPrint && RXString != "") {
    toPrint = RXString;
  }

  if (millis() > displayTimer){
    displayTimer = millis() + 30;
    drawRX(toPrint);
  }
}
