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

// SHUTTER VALUES (in us)
constexpr uint16_t MECH_DELAY_OPEN = 6500;
constexpr uint16_t MECH_DELAY_CLOSE = 1500;
constexpr uint8_t AOM_DELAY_OPEN = 20;
constexpr uint8_t AOM_DELAY_CLOSE = 5;

// MOT PHASE
constexpr float MOT_LOCK_FREQ = 0.832;
constexpr float MOT_TRAP_FREQ = 2.000; 
constexpr float MOT_REPUMP_FREQ = 7.14;

constexpr float MOT_LOCK_ATTE = 1.3;
constexpr float MOT_TRAP_ATTE = 1.3;
constexpr float MOT_REPUMP_ATTE = 1.3;

constexpr float MOT_C_FIELD = 0;

// PGC PHASE
constexpr float PGC_LOCK_FREQ = 0.832;
constexpr float PGC_TRAP_FREQ = 6.440;
constexpr float PGC_REPUMP_FREQ = 7.14;

constexpr float PGC_LOCK_ATTE = 1.3;
constexpr float PGC_TRAP_ATTE = 0.9;
constexpr float PGC_REPUMP_ATTE = 1.3;

// DETECT PHASE
constexpr float DETECT_LOCK_FREQ = 0.832;
constexpr float DETECT_TRAP_FREQ = 1.299;           // 0.832, 1.066, 1.183, 1.299, 1.767 (0,0.5,0.75,1,2 xGAMMA)
constexpr float DETECT_REPUMP_FREQ = 7.14;          // 7.14

constexpr float DETECT_LOCK_ATTE = 1.3;
constexpr float DETECT_TRAP_ATTE = 1.3;
constexpr float DETECT_REPUMP_ATTE = 1.3;

constexpr float DETECT_C_FIELD_ = 8;

constexpr uint8_t DROP_TIME = 198;

constexpr uint16_t REPUMP_PULSE_TIME = 150;        // us

constexpr uint16_t ADC_SAMPLES = 127;              // samples durign one call of ADC_READ
constexpr uint16_t PD_ARRAY_SIZE = 3*ADC_SAMPLES;  // x3 ADC_SAMPLES because there are x3 instances of reading, N4, N4+N3, BG
uint16_t PD_ARRAY[PD_ARRAY_SIZE];

// u_WAVE PHASE
constexpr uint16_t u_WAVE_MOD_OPEN = 5;
constexpr uint16_t u_WAVE_MOD_CLOSE = 4;
constexpr uint16_t RABI_PULSE = 5;                 // ms



// Integer Values
constexpr uint8_t num_images = 5; // images to take during MOT Temp measurement
// constexpr uint16_t ADC_samples = 127; // sample number when reading the ADC for detection


#endif // _SETTINGS_H_