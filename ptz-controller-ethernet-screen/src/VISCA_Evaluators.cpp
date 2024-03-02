#include "Arduino.h"

#define up char(1)    //"\x01"
#define down char(2)  //"\x02"
#define left char(1)  //"\x01"
#define right char(2) //"\x02"
#define stop char(3)  //"\x03"
// expects a pan / x and tilt / y value for the intended speed\
// pan range = -24 to 24 decimal
// tilt range = -23 to 23 decimal 
void EvaluatePanTilt(int x, int y, char* command){
  char ptzPrefix[] ="\x81\x01\x06\x01";
  char x_direction, y_direction;
  char x_speed, y_speed;

  //limit incoming to available VISCA speed options
  if (x > 24) {x = 24;}
  if (x < -24) {x = -24;}
  if (y > 23) {y = 23;}
  if (y < -23) {y = -23;}
  if (x == 0){
      x_direction = stop;
      x_speed = char(1);
  } else if (x < 0){
      x_direction = left;
      x_speed = char(abs(x));
  } else if (x > 0){
      x_direction = right;
      x_speed = char(abs(x));
  }

    if (y == 0){
      y_direction = stop;
      y_speed = char(1); 
  } else if (y < 0){
      y_direction = left;
      y_speed = char(abs(y));
  } else if (y > 0){
      y_direction = right;
      y_speed = char(abs(y));
    }
  //Serial.printf("%s%c%c%c%c%c", ptzPrefix, x_speed, y_speed, x_direction, y_direction, char(255));
  char temp[24];
  sprintf(temp, "%s%c%c%c%c%c", ptzPrefix, x_speed, y_speed, x_direction, y_direction, char(255));
  strcpy(command, temp);
}



void EvaluateZoom(int zoom, char* command){
  char ptzPrefix[] = "\x81\x01\x04\x07";
  char cmd;
  Serial.print("EvaluateZoom");
  Serial.println(zoom);

  //limit incoming to available VISCA speed options
  if (zoom > 8) {zoom = 8;}
  if (zoom < -8) {zoom = -8;}

  if (zoom == 0){
    cmd = char(0);
  } else if (zoom < 0){ //zoom out
    cmd = char(47 + abs(zoom));
  } else if (zoom > 0){ //zoom in
    cmd = char(31 + abs(zoom));
  } else {
    cmd = char(0);
  }

  //Serial.printf("%s%c%c", ptzPrefix, cmd, char(255));
  char temp[24];
  snprintf(temp, 24, "%s%c%c", ptzPrefix, cmd, char(255));
  
  for (int i=0; i<24; i++){
    command[i] = temp[i];
    Serial.printf("$%x ",temp[i]);
  }

}

//8x 01 04 3F 02 pp FF  pp = 00 - 63
void EvaluatePresetRecall(int preset, char* command){
  char ptzPrefix[] = "\x81\x01\x04\x3F\x02";
  char cmd;
  Serial.print("EvaluatePresetRecall ");
  Serial.println(preset);
  switch (preset){
    case 0:
      cmd = char(0);
      break;
    case 1:
      cmd = char(1);
      break;
    case 2:
      cmd = char(2);
      break;
    case 3:
      cmd = char(3);
      break;
    case 4:
      cmd = char(4);
      break;
    case 5:
      cmd = char(5);
      break;
    case 6:
      cmd = char(6);
      break;
    case 7:
      cmd = char(7);
      break;
    case 8:
      cmd = char(8);
      break;
    case 9:
      cmd = char(9);
      break;
  }

  //Serial.printf("%s%c%c", ptzPrefix, cmd, char(255));
  char temp[24];
  snprintf(temp, 24, "%s%c%c", ptzPrefix, cmd, char(255));
  
  for (int i=0; i<24; i++){
    command[i] = temp[i];
    Serial.printf("$%x ",temp[i]);
  }
}

//8x 01 04 3F 01 pp FF  pp = 00 - 63
void EvaluatePresetStore(int preset, char* command){
  char ptzPrefix[] = "\x81\x01\x04\x3F\x01";
  char cmd;
  Serial.print("EvaluatePresetStore ");
  Serial.println(preset);
  switch (preset){
    case 0:
      cmd = char(0);
      break;
    case 1:
      cmd = char(1);
      break;
    case 2:
      cmd = char(2);
      break;
    case 3:
      cmd = char(3);
      break;
    case 4:
      cmd = char(4);
      break;
    case 5:
      cmd = char(5);
      break;
    case 6:
      cmd = char(6);
      break;
    case 7:
      cmd = char(7);
      break;
    case 8:
      cmd = char(8);
      break;
    case 9:
      cmd = char(9);
      break;
  }

  //Serial.printf("%s%c%c", ptzPrefix, cmd, char(255));
  char temp[24];
  snprintf(temp, 24, "%s%c%c", ptzPrefix, cmd, char(255));
  
  for (int i=0; i<24; i++){
    command[i] = temp[i];
    Serial.printf("$%x ",temp[i]);
  }
}


void OnePushFocus(char* command){
    char ptzPrefix[] ="\x81\x01\x04\x18\x01";
    char temp[24];
    sprintf(temp, "%s%c", ptzPrefix, char(255));

    //*command = String(temp);
    strcpy(command, temp);
}

//8x 01 04 38 pp FF   pp: 02=Auto Focus, 03=Manual Focus, 
void FocusManual(char* command){
    char ptzPrefix[] ="\x81\x01\x04\x38\x03";
    char temp[24];
    sprintf(temp, "%s%c", ptzPrefix, char(255));

    //*command = String(temp);
    strcpy(command, temp);
}

//8x 01 04 38 pp FF   pp: 02=Auto Focus, 03=Manual Focus, 
void FocusAutomatic(char* command){
    char ptzPrefix[] ="\x81\x01\x04\x38\x02";
    char temp[24];
    sprintf(temp, "%s%c", ptzPrefix, char(255));

    //*command = String(temp);
    strcpy(command, temp);
}

// stop   8x 01 04 08 00 FF
// far    8x 01 04 08 2p FF
// near   8x 01 04 08 3p FF
void FocusFar(char* command){
  char ptzPrefix[] ="\x81\x01\x04\x08\x20";
  char temp[24];
  sprintf(temp, "%s%c", ptzPrefix, char(255));

  strcpy(command, temp);
}


// stop   8x 01 04 08 00 FF
// far    8x 01 04 08 2p FF
// near   8x 01 04 08 3p FF
void FocusNear(char* command){
  char ptzPrefix[] ="\x81\x01\x04\x08\x30";
  char temp[24];
  sprintf(temp, "%s%c", ptzPrefix, char(255));

  strcpy(command, temp);
}

// stop   8x 01 04 08 00 FF
// far    8x 01 04 08 2p FF
// near   8x 01 04 08 3p FF
void FocusStop(char* command){
  char ptzPrefix[] ="\x81\x01\x04\x08";
  char temp[24];
  sprintf(temp, "%s%c%c", ptzPrefix, char(0), char(255));

  for (int i=0; i<24; i++){
    command[i] = temp[i];
    //Serial.printf("$%x ",temp[i]);
  }
}