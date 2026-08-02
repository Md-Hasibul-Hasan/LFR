#include "Globals.h"

struct MotorStep
{
    const char *name;
    int left;
    int right;
};

MotorStep testList[] =
{
    {"FORWARD",     baseSpeed,  baseSpeed},
    {"BACKWARD",   -baseSpeed, -baseSpeed},
    {"RIGHT",       baseSpeed, 0},
    {"LEFT",        0, baseSpeed},
    {"RIGHT SPIN",  baseSpeed,-baseSpeed},
    {"LEFT SPIN",  -baseSpeed, baseSpeed},
};

void HandleMotorTest()
{
    const byte TEST_COUNT = sizeof(testList) / sizeof(testList[0]);

    for(byte i = 0; i < TEST_COUNT; i++)
    {
        unsigned long start = millis();

        while(millis() - start < 2000)
        {
            UpdateButtons();

            if(okEvent == Button::Event::SHORT)
            {
                RobotStop();
                currentScreen = Screen::DASHBOARD;
                return;
            }

            DriveMotor(testList[i].left, testList[i].right);

            u8g2.firstPage();
            do
            {
                u8g2.setFont(u8g2_font_6x12_tf);

                u8g2.drawStr(22,12,"MOTOR TEST: ");
                u8g2.print(baseSpeed);
                u8g2.drawHLine(0,14,128);

                u8g2.setCursor(20,35);
                u8g2.print(testList[i].name);

                u8g2.drawStr(10,60,"OK : Stop");
            }
            while(u8g2.nextPage());
        }

        RobotStop();
        delay(200);
    }

    RobotStop();
    currentScreen = Screen::DASHBOARD;
}