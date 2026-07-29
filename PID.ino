#include "Globals.h"

float kp = 0.4;
float ki = 0.0;
float kd = 2.5;
float P;
float I = 0.0;
float D;
float lastError = 0.0;

float CalculatePID(int error){
  P = error;
  I += error;
  I = constrain(I, -500, 500);
  D = error - lastError;
  lastError = error;
  float pid = (kp*P) + (ki*I) + (kd*D);
  return pid;

}