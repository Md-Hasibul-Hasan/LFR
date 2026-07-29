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

void SetMotorSpeed(float pid)
{
    int leftMotorSpeed = baseSpeed + pid;
    int rightMotorSpeed = baseSpeed - pid;

    DriveMotor(leftMotorSpeed, rightMotorSpeed);
}