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

    bool inverseTrack;
    byte llrIdx;

    int minValue[SENSOR_COUNT];
    int maxValue[SENSOR_COUNT];
};

//=====================================================
// Save Settings
//=====================================================

void SaveSettings()
{
    // EEPROMData data;

    // data.magic = EEPROM_MAGIC;

    // data.kp = kp;
    // data.ki = ki;
    // data.kd = kd;

    // data.baseSpeed = baseSpeed;

    // data.inverseTrack = inverseTrack;
    // data.llrIdx = llrIdx;

    // for (byte i = 0; i < SENSOR_COUNT; i++)
    // {
    //     data.minValue[i] = minValue[i];
    //     data.maxValue[i] = maxValue[i];
    // }

    // EEPROM.put(0, data);
}

//=====================================================
// Load Settings
//=====================================================

void LoadSettings()
{
    // EEPROMData data;

    // EEPROM.get(0, data);

    // // First boot or invalid EEPROM
    // if (data.magic != EEPROM_MAGIC)
    // {
    //     SaveSettings();
    //     return;
    // }

    // kp = data.kp;
    // ki = data.ki;
    // kd = data.kd;

    // baseSpeed = data.baseSpeed;

    // inverseTrack = data.inverseTrack;
    // llrIdx = data.llrIdx;

    // for (byte i = 0; i < SENSOR_COUNT; i++)
    // {
    //     minValue[i] = data.minValue[i];
    //     maxValue[i] = data.maxValue[i];
    // }
}