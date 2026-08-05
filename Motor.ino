#include "Globals.h"

int baseSpeed = 150;

void DriveMotor(int leftMotorSpeed, int rightMotorSpeed)
{
    leftMotorSpeed = constrain(leftMotorSpeed, -255, 255);
    rightMotorSpeed = constrain(rightMotorSpeed, -255, 255);

    // Left Motor
    if (leftMotorSpeed >= 0)
    {
        digitalWrite(LEFT_FWD, HIGH);
        digitalWrite(LEFT_BWD, LOW);
    }
    else
    {
        digitalWrite(LEFT_FWD, LOW);
        digitalWrite(LEFT_BWD, HIGH);
    }

    // Right Motor
    if (rightMotorSpeed >= 0)
    {
        digitalWrite(RIGHT_FWD, HIGH);
        digitalWrite(RIGHT_BWD, LOW);
    }
    else
    {
        digitalWrite(RIGHT_FWD, LOW);
        digitalWrite(RIGHT_BWD, HIGH);
    }

    analogWrite(LEFT_PWM, abs(leftMotorSpeed));
    analogWrite(RIGHT_PWM, abs(rightMotorSpeed));
}



void SetMotorSpeed(float pid){
    int corr = constrain((int)pid, -200, 200);

    int speed = baseSpeed;

    // Sharp correction -> slow down
    if (abs(corr) > 130)
        speed = baseSpeed * 55 / 100;

    // Almost straight -> speed up
    else if (abs(corr) < 40)
        speed = baseSpeed * 125 / 100;

    int leftMotorSpeed  = speed + corr;
    int rightMotorSpeed = speed - corr;

    DriveMotor(leftMotorSpeed, rightMotorSpeed);
}