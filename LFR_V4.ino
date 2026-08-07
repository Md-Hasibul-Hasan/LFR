#include <U8g2lib.h>
#include <Wire.h>
#include "Button.h"
#include "Globals.h"




Screen currentScreen = Screen::DASHBOARD;

void FactoryReset()
{
    // PID
    kp = 0.40f;
    ki = 0.00f;
    kd = 2.50f;

    // Speed
    baseSpeed = 150;

    // Track
    inverseTrack = false;

    // Lost line preference
    llrIdx = 0;          // Left

    // Timers
    GAP_TIMEOUT = 250;
    TURN_MIN_TIME = 100;
    TURN_TIMEOUT = 300;
    FINISH_CONFIRM_TIME = 500;
    TRACK_SWITCH_TIME = 50;

    // Thresholds
    SENSOR_THRESHOLD = 500;
    LINE_LOST_THRESHOLD = 300;
    TRACK_DIFF_THRESHOLD = 150;

    // Calibration
    for (byte i = 0; i < SENSOR_COUNT; i++)
    {
        minValue[i] = 0;
        maxValue[i] = 1023;
    }

    ResetPID();
    ResetTrackDetection();

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

        case Screen::SET_TIMER:
        DrawTimerPage();
        HandleTimerPage();
        break;


    }
}