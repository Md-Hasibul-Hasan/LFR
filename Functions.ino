#include "Globals.h"

int minValue[SENSOR_COUNT];
int maxValue[SENSOR_COUNT];

int rawValue[SENSOR_COUNT];
int norValue[SENSOR_COUNT];

bool inverseTrack = false;   // false = black line, true = white line

const int weight[SENSOR_COUNT] =
{
    -350, -250, -150, -50,
      50,  150,  250, 350
};

const int CENTER = 0;

int lastPosition = 0;
int totalNorValue = 0;
int activeSensorCount = 0;

int position = 0;
int error = 0;

unsigned long GAP_TIMEOUT = 250;
unsigned long TURN_MIN_TIME = 100;
unsigned long TURN_TIMEOUT = 300;
unsigned long FINISH_CONFIRM_TIME = 500;
unsigned long TRACK_SWITCH_TIME = 50;

int SENSOR_THRESHOLD = 500;
int LINE_LOST_THRESHOLD = 300;
int TRACK_DIFF_THRESHOLD = 150;

bool trackCandidateActive = false;
bool trackCandidate = false;
unsigned long trackCandidateStart = 0;


// ==================================================
// MUX READ
// ==================================================

int ReadMuxChannel(int channel)
{
    digitalWrite(S0, channel & 0x01);
    digitalWrite(S1, (channel >> 1) & 0x01);
    digitalWrite(S2, (channel >> 2) & 0x01);
    digitalWrite(S3, (channel >> 3) & 0x01);

    delayMicroseconds(5);

    return analogRead(SIG_PIN);
}


// ==================================================
// CALIBRATION
// তোমার existing Calibration() exactly এখানে থাকবে
// ==================================================

void Calibration()
{
    while (true)
    {
        while (true)
        {
            UpdateButtons();

            if (okEvent == Button::Event::SHORT)
            {
                LoadSettings();
                currentScreen = Screen::DASHBOARD;
                return;
            }

            if (okEvent == Button::Event::LONG)
                break;

            u8g2.firstPage();
            do
            {
                u8g2.setFont(u8g2_font_6x12_tf);

                u8g2.drawStr(30, 10, "CALIBRATION");
                u8g2.drawHLine(0, 12, 128);

                char buf[20];

                for (byte i = 0; i < 4; i++)
                {
                    sprintf(buf, "S%d %3d %3d",
                            i, minValue[i], maxValue[i]);
                    u8g2.drawStr(0, 22 + i * 10, buf);

                    sprintf(buf, "S%d %3d %3d",
                            i + 4,
                            minValue[i + 4],
                            maxValue[i + 4]);

                    u8g2.drawStr(64, 22 + i * 10, buf);
                }

                u8g2.drawStr(0, 62, "OK:Back      LOK:Cal");
            }
            while (u8g2.nextPage());
        }


        for (byte i = 0; i < SENSOR_COUNT; i++)
        {
            minValue[i] = 1023;
            maxValue[i] = 0;
        }


        unsigned long startTime = millis();

        while (millis() - startTime < 5000)
        {
            UpdateButtons();

            if (okEvent == Button::Event::SHORT)
            {
                LoadSettings();
                currentScreen = Screen::DASHBOARD;
                return;
            }

            for (byte i = 0; i < SENSOR_COUNT; i++)
            {
                int val = ReadMuxChannel(i);

                if (val < minValue[i])
                    minValue[i] = val;

                if (val > maxValue[i])
                    maxValue[i] = val;
            }

            u8g2.firstPage();
            do
            {
                u8g2.setFont(u8g2_font_6x12_tf);
                u8g2.drawStr(18, 30, "Calibrating...");
            }
            while (u8g2.nextPage());
        }

        SaveSettings();
    }
}


// ==================================================
// TRACK TYPE
// ==================================================




void DetectTrack()
{
    int edgeAvg =
        (rawValue[0] +
         rawValue[1] +
         rawValue[6] +
         rawValue[7]) / 4;

    int centerAvg =
        (rawValue[3] +
         rawValue[4]) / 2;


    // Not enough difference
    if (abs(centerAvg - edgeAvg) < TRACK_DIFF_THRESHOLD)
    {
        trackCandidateActive = false;
        return;
    }


    // false = black line
    // true  = white line
    bool detectedTrack = (centerAvg > edgeAvg);


    // Same track type
    if (detectedTrack == inverseTrack)
    {
        trackCandidateActive = false;
        return;
    }


    // New candidate
    if (!trackCandidateActive ||
        trackCandidate != detectedTrack)
    {
        trackCandidate = detectedTrack;
        trackCandidateActive = true;
        trackCandidateStart = millis();
        return;
    }


    // Confirm before switching
    if (millis() - trackCandidateStart >= TRACK_SWITCH_TIME)
    {
        inverseTrack = detectedTrack;

        trackCandidateActive = false;

        ResetPID();
    }
}


// ==================================================
// READ + NORMALIZE
// ==================================================

void ReadSensors(bool detectTrack = false)
{
    activeSensorCount = 0;


    // ---------------- RAW ----------------

    for (int i = 0; i < SENSOR_COUNT; i++)
    {
        rawValue[i] = ReadMuxChannel(i);
    }

    // Detect black/white track transition
    if (detectTrack){
            DetectTrack();
        }
    else{
        trackCandidateActive  = false;
    }


    // ---------------- NORMALIZE ----------------

    for (int i = 0; i < SENSOR_COUNT; i++)
    {
        int range = maxValue[i] - minValue[i];

        if (range <= 0)
        {
            norValue[i] = 0;
            continue;
        }


        long normalized;

        if (inverseTrack)
        {
            normalized =
                (long)(rawValue[i] - minValue[i])
                * 1000L / range;
        }
        else
        {
            normalized =
                (long)(maxValue[i] - rawValue[i])
                * 1000L / range;
        }


        norValue[i] =
            constrain((int)normalized, 0, 1000);


        // Digital-like active sensor
        if (norValue[i] > SENSOR_THRESHOLD)
        {
            activeSensorCount++;
        }
    }


    position = CalculatePosition();
    error = CalculateError(position);
}


// ==================================================
// POSITION
// ==================================================

int CalculatePosition()
{
    long weightedSum = 0;
    long total = 0;

    for (int i = 0; i < SENSOR_COUNT; i++)
    {
        weightedSum +=
            (long)norValue[i] * weight[i];

        total += norValue[i];
    }


    totalNorValue = total;


    // Keep last direction when line disappears
    if (total == 0)
        return lastPosition;


    lastPosition = weightedSum / total;

    return lastPosition;
}


// ==================================================
// ERROR
// ==================================================

int CalculateError(int position)
{
    return position - CENTER;
}


// ==================================================
// LINE LOST
// ==================================================

bool IsLineLost()
{
    /*
       Don't use activeSensorCount == 0 only.

       Normalized sensors may contain small values
       even when no real line exists.
    */

    return totalNorValue < LINE_LOST_THRESHOLD;
}


// ==================================================
// JUNCTION
// ==================================================

bool IsJunction()
{
    bool left =
        norValue[0] > SENSOR_THRESHOLD ||
        norValue[1] > SENSOR_THRESHOLD;

    bool center =
        norValue[3] > SENSOR_THRESHOLD ||
        norValue[4] > SENSOR_THRESHOLD;

    bool right =
        norValue[6] > SENSOR_THRESHOLD ||
        norValue[7] > SENSOR_THRESHOLD;


    /*
       Broad junction candidate.

       We don't try to identify + / T / etc here.
       JunctionState() will decide available paths.
    */

    return
        activeSensorCount >= 5 &&
        (
            (left && center) ||
            (center && right) ||
            (left && right)
        );
}


// ==================================================
// HARD LEFT
// ==================================================

bool IsHardLeft()
{
    bool left =
        norValue[0] > SENSOR_THRESHOLD ||
        norValue[1] > SENSOR_THRESHOLD;

    bool center =
        norValue[3] > SENSOR_THRESHOLD ||
        norValue[4] > SENSOR_THRESHOLD;

    bool right =
        norValue[6] > SENSOR_THRESHOLD ||
        norValue[7] > SENSOR_THRESHOLD;


    return left && !center && !right;
}


// ==================================================
// HARD RIGHT
// ==================================================

bool IsHardRight()
{
    bool left =
        norValue[0] > SENSOR_THRESHOLD ||
        norValue[1] > SENSOR_THRESHOLD;

    bool center =
        norValue[3] > SENSOR_THRESHOLD ||
        norValue[4] > SENSOR_THRESHOLD;

    bool right =
        norValue[6] > SENSOR_THRESHOLD ||
        norValue[7] > SENSOR_THRESHOLD;


    return right && !center && !left;
}


// ==================================================
// ROUNDABOUT
// Later
// ==================================================

bool IsRoundabout()
{
    return false;
}

bool IsFinishTrack()
{
    return activeSensorCount == SENSOR_COUNT;
}

void ResetTrackDetection()
{
    trackCandidateActive = false;
    trackCandidate = false;
    trackCandidateStart = 0;
}




