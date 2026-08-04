#include "Button.h"
#include <U8g2lib.h>
#ifndef GLOBALS_H
#define GLOBALS_H

// ---------------- PIN DEFINITIONS ----------------
#define LEFT_FWD 7  //Ain1
#define LEFT_BWD 6  //Ain2
#define RIGHT_FWD  8  //Bin1
#define RIGHT_BWD  12 //Bin2
#define LEFT_PWM 9
#define RIGHT_PWM  10

// Multiplexer
#define S0 2
#define S1 3
#define S2 4
#define S3 5
#define SIG_PIN A0

constexpr int SENSOR_COUNT = 8;

//=====================================================
// Screen
//=====================================================
extern U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2;

extern Button::Event upEvent;
extern Button::Event downEvent;
extern Button::Event okEvent;

enum class Screen
{
    CALIBRATION,
    LINE_FOLLOW,
    DASHBOARD,
    SENSOR_VIEW,
    MOTOR_TEST,
};

extern Screen currentScreen;

extern float kp;
extern float ki;
extern float kd;

extern bool inverseTrack;
extern int baseSpeed;
extern int error;
extern int position;
extern bool editing;

extern byte llrIdx;


extern int minValue[SENSOR_COUNT];
extern int maxValue[SENSOR_COUNT];
extern int rawValue[SENSOR_COUNT];
extern int norValue[SENSOR_COUNT];

void SaveSettings();
void LoadSettings();

#endif // GLOBALS_H
