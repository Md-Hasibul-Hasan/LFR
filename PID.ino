// #include "Globals.h"

// float kp = 0.4;
// float ki = 0.0;
// float kd = 2.5;
// float P;
// float I = 0.0;
// float D;
// float lastError = 0.0;

// float CalculatePID(int error){
//   P = error;
//   I += error;
//   I = constrain(I, -500, 500);
//   D = error - lastError;
//   lastError = error;
//   float pid = (kp*P) + (ki*I) + (kd*D);
//   return pid;

// }




#include "Globals.h"

float kp = 0.4;
float ki = 0.0;
float kd = 2.5;

float integral = 0;
float lastError = 0;

const int MAX_ERROR = 350;  //max weight = abs(weight[SENSOR_COUNT - 1]);

void ResetPID()
{
    integral = 0;
    lastError = 0;
}

float CalculatePID(int error)
{
    // -350...+350 -> -1.0...+1.0
    float e = error / (float)MAX_ERROR;

    float kp_u = kp;
    float kd_u = kd;

    // Small error -> smooth
    if (abs(error) < 80)
    {
        kp_u *= 0.3;
        kd_u *= 0.3;
    }

    // Medium error -> normal kp/kd
    else if (abs(error) < 200)
    {
        kp_u = kp;
        kd_u = kd;
    }

    // Large error -> aggressive correction
    else
    {
        kp_u *= 1.7;
        kd_u *= 1.8;
    }

    integral += e;
    integral = constrain(integral, -1.0f, 1.0f);

    float derivative = e - lastError;
    lastError = e;

    float pid =
        (kp_u * e) +
        (ki * integral) +
        (kd_u * derivative);

    pid *= 255.0f;

    return constrain(pid, -255.0f, 255.0f);
}