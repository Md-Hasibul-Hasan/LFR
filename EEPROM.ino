#include <EEPROM.h>
#include "Globals.h"

const uint16_t EEPROM_MAGIC = 0x55AA;

struct EEPROMData
{
    uint16_t magic;

    float kp;
    float ki;
    float kd;

    int baseSpeed;

    byte llrIdx;

    unsigned long gapTimeout;
    unsigned long turnMinTime;
    unsigned long turnTimeout;
    unsigned long finishConfirmTime;
    unsigned long trackSwitchTime;

    int sensorThreshold;
    int lineLostThreshold;
    int trackDiffThreshold;

    int minValue[SENSOR_COUNT];
    int maxValue[SENSOR_COUNT];
};

//=====================================================
// Save Settings
//=====================================================

void SaveSettings()
{
    EEPROMData data;

    data.magic = EEPROM_MAGIC;

    data.kp = kp;
    data.ki = ki;
    data.kd = kd;

    data.baseSpeed = baseSpeed;

    data.llrIdx = llrIdx;

    data.gapTimeout = GAP_TIMEOUT;
    data.turnMinTime = TURN_MIN_TIME;
    data.turnTimeout = TURN_TIMEOUT;
    data.finishConfirmTime = FINISH_CONFIRM_TIME;
    data.trackSwitchTime = TRACK_SWITCH_TIME;

    data.sensorThreshold = SENSOR_THRESHOLD;
    data.lineLostThreshold = LINE_LOST_THRESHOLD;
    data.trackDiffThreshold = TRACK_DIFF_THRESHOLD;

    for (byte i = 0; i < SENSOR_COUNT; i++)
    {
        data.minValue[i] = minValue[i];
        data.maxValue[i] = maxValue[i];
    }

    EEPROM.put(0, data);
}

//=====================================================
// Load Settings
//=====================================================

void LoadSettings()
{
    EEPROMData data;

    EEPROM.get(0, data);

    // First boot or invalid EEPROM
    if (data.magic != EEPROM_MAGIC)
    {
        SaveSettings();
        return;
    }

    kp = data.kp;
    ki = data.ki;
    kd = data.kd;

    baseSpeed = data.baseSpeed;

    llrIdx = data.llrIdx;

    GAP_TIMEOUT = data.gapTimeout;
    TURN_MIN_TIME = data.turnMinTime;
    TURN_TIMEOUT = data.turnTimeout;
    FINISH_CONFIRM_TIME = data.finishConfirmTime;
    TRACK_SWITCH_TIME = data.trackSwitchTime;

    SENSOR_THRESHOLD = data.sensorThreshold;
    LINE_LOST_THRESHOLD = data.lineLostThreshold;
    TRACK_DIFF_THRESHOLD = data.trackDiffThreshold;

    for (byte i = 0; i < SENSOR_COUNT; i++)
    {
        minValue[i] = data.minValue[i];
        maxValue[i] = data.maxValue[i];
    }
}