// Live Sensor View

#include "Globals.h"

void DrawSensorView()
{
    char buf[20];

    u8g2.firstPage();
    do
    {
        u8g2.setFont(u8g2_font_5x8_tf);

        u8g2.drawStr(30, 8, "SENSOR VIEW");
        u8g2.drawHLine(0, 10, 128);

        // Left Side: S0 - S3
        sprintf(buf, "S0:%d/%d", rawValue[0], norValue[0]);
        u8g2.drawStr(0, 20, buf);

        sprintf(buf, "S1:%d/%d", rawValue[1], norValue[1]);
        u8g2.drawStr(0, 29, buf);

        sprintf(buf, "S2:%d/%d", rawValue[2], norValue[2]);
        u8g2.drawStr(0, 38, buf);

        sprintf(buf, "S3:%d/%d", rawValue[3], norValue[3]);
        u8g2.drawStr(0, 47, buf);


        // Right Side: S4 - S7
        sprintf(buf, "S4:%d/%d", rawValue[4], norValue[4]);
        u8g2.drawStr(65, 20, buf);

        sprintf(buf, "S5:%d/%d", rawValue[5], norValue[5]);
        u8g2.drawStr(65, 29, buf);

        sprintf(buf, "S6:%d/%d", rawValue[6], norValue[6]);
        u8g2.drawStr(65, 38, buf);

        sprintf(buf, "S7:%d/%d", rawValue[7], norValue[7]);
        u8g2.drawStr(65, 47, buf);


        sprintf(buf, "TRK:%s", inverseTrack ? "W" : "B");
        u8g2.drawStr(0, 60, buf);

        sprintf(buf, "Pos:%d", position);
        u8g2.drawStr(40, 60, buf);

        u8g2.drawStr(90, 60, "OK:Back");


    } while (u8g2.nextPage());
}


void HandleSensorView()
{

    ReadSensors();

    if (okEvent == Button::Event::SHORT)
    {
        currentScreen = Screen::DASHBOARD;
    }
}