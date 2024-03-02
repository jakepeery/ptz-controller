#include "Arduino.h"
void EvaluatePanTilt(int x, int y, char*command);
void EvaluateZoom(int zoom, char* command);
void OnePushFocus(char* command);
void EvaluatePresetRecall(int preset, char* command);
void EvaluatePresetStore(int preset, char* command);

void FocusManual(char* command);
void FocusAutomatic(char* command);
void FocusFar(char* command);
void FocusNear(char* command);
void FocusStop(char* command);