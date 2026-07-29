#include "Globals.h"

void RobotInit(){

    pinMode(S0, OUTPUT);
    pinMode(S1, OUTPUT);
    pinMode(S2, OUTPUT);
    pinMode(S3, OUTPUT);

    pinMode(LEFT_FWD, OUTPUT);
    pinMode(LEFT_BWD, OUTPUT);

    pinMode(RIGHT_FWD, OUTPUT);
    pinMode(RIGHT_BWD, OUTPUT);

    pinMode(LEFT_PWM, OUTPUT);
    pinMode(RIGHT_PWM, OUTPUT);


}

void RobotStop(){
   DriveMotor(0, 0);
}

void RobotUpdate(){
    ReadSensors();
    SetMotorSpeed(CalculatePID(error));
}



void LineFollow(){
    
    RobotUpdate();

    u8g2.firstPage();
    do
    {
        u8g2.setFont(u8g2_font_6x12_tf);
        u8g2.drawStr(0, 15, "LINE FOLLOW");

        u8g2.setCursor(0, 30);
        u8g2.print("Pos: ");
        u8g2.print(position);

        u8g2.setCursor(0, 45);
        u8g2.print("Err: ");
        u8g2.print(error);

        u8g2.drawStr(0, 62, "OK: Back");
    }
    while (u8g2.nextPage());

    if (okEvent == Button::Event::SHORT)
    {
        RobotStop();
        currentScreen = Screen::DASHBOARD;
    }
}