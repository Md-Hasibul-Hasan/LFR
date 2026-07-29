#include <U8g2lib.h>
#include <Wire.h>
#include "Button.h"
#include "Globals.h"




Screen currentScreen = Screen::DASHBOARD;

void FactoryReset(){
    kp = 20.0;
    ki = 0.0;
    kd = 10.0;
    baseSpeed = 150;
    inverseTrack = false;
    llrIdx = 0;

    for (byte i = 0; i < SENSOR_COUNT; i++){
        minValue[i] = 0;
        maxValue[i] = 1023;
    }

    SaveSettings();
}

void setup(){

    Serial.begin(9600);
    RobotInit();
    LoadSettings();
    OLEDInit();
}

void loop(){
    UpdateButtons();


    if (upEvent == Button::Event::LONG &&
        downEvent == Button::Event::LONG)
    {
        FactoryReset();
        currentScreen = Screen::DASHBOARD;
    }

    switch(currentScreen)
    {
        case Screen::DASHBOARD:
            RobotStop();
            HandleDashboard();
            DrawDashboard();
            break;

        case Screen::CALIBRATION:
            RobotStop();
            Calibration();
            break;

        case Screen::LINE_FOLLOW:
            LineFollow();
            break;

        case Screen::SENSOR_VIEW:
            RobotStop();
            HandleSensorView();
            DrawSensorView();
            break;

        case Screen::MOTOR_TEST:
            HandleMotorTest();
            break;


    }
}