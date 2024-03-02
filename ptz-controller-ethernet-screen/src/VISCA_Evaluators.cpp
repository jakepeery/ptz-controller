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
  
  Serial.print("\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA");
  for (int i=0; i<24; i++){
    command[i] = temp[i];
    Serial.printf("$%x ",temp[i]);
  }
  // //Serial.println(temp);
  // Serial.print("\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB");
  // Serial.println(command);

}


void OnePushFocus(char* command){
    char ptzPrefix[] ="\x81\x01\x04\x18\x01";
    char temp[24];
    sprintf(temp, "%s%c", ptzPrefix, char(255));

    //*command = String(temp);
    strcpy(command, temp);
}