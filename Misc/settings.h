#include "mbed.h"
#include <cmath>

#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#define V_REF   10
#define RES     4095

// Function to convert voltage to decimal value for DAC
constexpr int16_t to_dac(double voltage)
{
    return static_cast<int16_t>( round( ( (voltage / V_REF) * RES) ) );
}

// Variables

// MOT PHASE
constexpr float MOT_LOCK_FREQ = 5.5;
constexpr float MOT_TRAP_FREQ = 4.332; // 1: 5.033, 1.5: 4.799, 2: 4.565, 3: 4.098, 4: 3.631,
constexpr float MOT_REPUMP_FREQ = 7.14;

constexpr float MOT_LOCK_ATTE = 1.3;
constexpr float MOT_TRAP_ATTE = 1.3;
constexpr float MOT_REPUMP_ATTE = 1.3;

// PGC PHASE
constexpr float PGC_LOCK_FREQ = 5.5;
constexpr float PGC_TRAP_FREQ = 0.359;
constexpr float PGC_REPUMP_FREQ = 7.14;

constexpr float PGC_LOCK_ATTE = 1.3;
constexpr float PGC_TRAP_ATTE = 1.3;
constexpr float PGC_REPUMP_ATTE = 1.3;

// DETECT PHASE
constexpr float DETECT_LOCK_FREQ = 5.5;
constexpr float DETECT_TRAP_FREQ = 5.266; // 0.5: 5.266, 1: 5.033, 1.5: 4.799, 2: 4.565, 3: 4.098, 4: 3.631,
constexpr float DETECT_REPUMP_FREQ = 6.9;

constexpr float DETECT_LOCK_ATTE = 1.3;
constexpr float DETECT_TRAP_ATTE = 1.3;
constexpr float DETECT_REPUMP_ATTE = 1.3;

// Integer Values
constexpr uint8_t num_images = 5; // images to take during MOT Temp measurement
// constexpr uint16_t ADC_samples = 127; // sample number when reading the ADC for detection


#endif // _SETTINGS_H_