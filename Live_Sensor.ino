// LIve sensor view

#include "Globals.h"

void DrawSensorView()
{
    char buf[16];

    u8g2.firstPage();
    do
    {
        u8g2.setFont(u8g2_font_5x8_tf);

        u8g2.drawStr(30, 8, "SENSOR VIEW");
        u8g2.drawHLine(0, 10, 128);

        sprintf(buf, "S0:%4d", rawValue[0]);
        u8g2.drawStr(0, 20, buf);

        sprintf(buf, "S1:%4d", rawValue[1]);
        u8g2.drawStr(0, 29, buf);

        sprintf(buf, "S2:%4d", rawValue[2]);
        u8g2.drawStr(0, 38, buf);

        sprintf(buf, "S3:%4d", rawValue[3]);
        u8g2.drawStr(0, 47, buf);

        sprintf(buf, "S4:%4d", rawValue[4]);
        u8g2.drawStr(68, 20, buf);

        sprintf(buf, "S5:%4d", rawValue[5]);
        u8g2.drawStr(68, 29, buf);

        sprintf(buf, "S6:%4d", rawValue[6]);
        u8g2.drawStr(68, 38, buf);

        sprintf(buf, "S7:%4d", rawValue[7]);
        u8g2.drawStr(68, 47, buf);

        sprintf(buf, "Pos:%4d", position);
        u8g2.drawStr(10, 60, buf);

        u8g2.drawStr(60, 60, "OK:Back");

    } while (u8g2.nextPage());
}

void HandleSensorView()
{
    ReadSensors();

    if(okEvent == Button::Event::SHORT)
    {
        currentScreen = Screen::DASHBOARD;
    }
}