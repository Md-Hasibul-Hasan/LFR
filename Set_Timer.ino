#include "Globals.h"

byte settingSelected = 0;
byte settingTop = 0;
bool settingEditing = false;

const byte SETTING_COUNT = 8;
const byte visibleSettingItems = 5;
const byte cursorX = 0;
const byte labelX = 10;
const byte valueX = 86;
const byte titleY = 10;
const byte firstRowY = 22;
const byte rowGap = 10;

void DrawTimerPage()
{
    bool blink = (millis() / 500) & 1;
    char buf[16];

    if (settingSelected < settingTop)
        settingTop = settingSelected;
    if (settingSelected >= settingTop + visibleSettingItems)
        settingTop = settingSelected - visibleSettingItems + 1;

    u8g2.firstPage();
    do
    {
        u8g2.setFont(u8g2_font_6x12_tf);

        u8g2.drawStr(28, titleY, "SET TIMER");
        u8g2.drawHLine(0, 12, 128);

        for (byte i = 0; i < visibleSettingItems; i++)
        {
            byte index = settingTop + i;
            if (index >= SETTING_COUNT)
                break;

            int y = firstRowY + i * rowGap;

            if (index == settingSelected)
                u8g2.drawStr(cursorX, y, ">");

            switch (index)
            {
            case 0:
                u8g2.drawStr(labelX, y, "Gap");
                sprintf(buf, "%lu", GAP_TIMEOUT);
                break;

            case 1:
                u8g2.drawStr(labelX, y, "Turn Min");
                sprintf(buf, "%lu", TURN_MIN_TIME);
                break;

            case 2:
                u8g2.drawStr(labelX, y, "Turn Out");
                sprintf(buf, "%lu", TURN_TIMEOUT);
                break;

            case 3:
                u8g2.drawStr(labelX, y, "Finish");
                sprintf(buf, "%lu", FINISH_CONFIRM_TIME);
                break;

            case 4:
                u8g2.drawStr(labelX, y, "Track Sw");
                sprintf(buf, "%lu", TRACK_SWITCH_TIME);
                break;

            case 5:
                u8g2.drawStr(labelX, y, "Sens Th");
                sprintf(buf, "%d", SENSOR_THRESHOLD);
                break;

            case 6:
                u8g2.drawStr(labelX, y, "Line Lost");
                sprintf(buf, "%d", LINE_LOST_THRESHOLD);
                break;

            case 7:
                u8g2.drawStr(labelX, y, "Track Diff");
                sprintf(buf, "%d", TRACK_DIFF_THRESHOLD);
                break;
            }

            if (settingEditing && index == settingSelected)
            {
                if (blink)
                {
                    u8g2.drawBox(84, y - 9, 34, 11);
                    u8g2.setDrawColor(0);
                    u8g2.drawStr(valueX, y, buf);
                    u8g2.setDrawColor(1);
                }
                else
                {
                    u8g2.drawStr(valueX, y, buf);
                }
            }
            else
            {
                u8g2.drawStr(valueX, y, buf);
            }
        }

        if (SETTING_COUNT > visibleSettingItems)
        {
            int barHeight = (visibleSettingItems * 50) / SETTING_COUNT;
            if (barHeight < 8)
                barHeight = 8;
            int barY = 15 + ((50 - barHeight) * settingTop) / (SETTING_COUNT - visibleSettingItems);
            u8g2.drawFrame(124, 15, 4, 50);
            u8g2.drawBox(125, barY, 2, barHeight);
        }


    }
    while (u8g2.nextPage());
}

void HandleTimerPage()
{
    if (settingEditing)
    {
        if (upEvent == Button::Event::SHORT ||
            upEvent == Button::Event::REPEAT)
        {
            switch (settingSelected)
            {
            case 0: GAP_TIMEOUT += 10; break;
            case 1: TURN_MIN_TIME += 5; break;
            case 2: TURN_TIMEOUT += 10; break;
            case 3: FINISH_CONFIRM_TIME += 10; break;
            case 4: TRACK_SWITCH_TIME += 5; break;
            case 5: SENSOR_THRESHOLD += 10; break;
            case 6: LINE_LOST_THRESHOLD += 10; break;
            case 7: TRACK_DIFF_THRESHOLD += 10; break;
            }
        }

        if (downEvent == Button::Event::SHORT ||
            downEvent == Button::Event::REPEAT)
        {
            switch (settingSelected)
            {
            case 0:
                if (GAP_TIMEOUT >= 10) GAP_TIMEOUT -= 10;
                break;

            case 1:
                if (TURN_MIN_TIME >= 5) TURN_MIN_TIME -= 5;
                break;

            case 2:
                if (TURN_TIMEOUT >= 10) TURN_TIMEOUT -= 10;
                break;

            case 3:
                if (FINISH_CONFIRM_TIME >= 10) FINISH_CONFIRM_TIME -= 10;
                break;

            case 4:
                if (TRACK_SWITCH_TIME >= 5) TRACK_SWITCH_TIME -= 5;
                break;

            case 5:
                if (SENSOR_THRESHOLD > 10) SENSOR_THRESHOLD -= 10;
                break;

            case 6:
                if (LINE_LOST_THRESHOLD > 10) LINE_LOST_THRESHOLD -= 10;
                break;

            case 7:
                if (TRACK_DIFF_THRESHOLD > 10) TRACK_DIFF_THRESHOLD -= 10;
                break;
            }
        }

        if (okEvent == Button::Event::SHORT)
        {
            settingEditing = false;
            SaveSettings();
        }

        return;
    }

    if (upEvent == Button::Event::SHORT ||
        upEvent == Button::Event::REPEAT)
    {
        if (settingSelected == 0)
            settingSelected = SETTING_COUNT - 1;
        else
            settingSelected--;
    }

    if (downEvent == Button::Event::SHORT ||
        downEvent == Button::Event::REPEAT)
    {
        settingSelected++;

        if (settingSelected >= SETTING_COUNT)
            settingSelected = 0;
    }

    if (okEvent == Button::Event::SHORT)
    {
        settingEditing = true;
    }

    if (okEvent == Button::Event::LONG)
    {
        SaveSettings();
        currentScreen = Screen::DASHBOARD;
    }
}