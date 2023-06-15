#include "mbed.h"
#include <cmath>

#ifndef _SETTINGS_H_
#define _SETTINGS_H_


// Function to convert voltage to decimal value for DAC
#define V_REF   10
#define RES     4095 // 2^12 - 1
constexpr int16_t to_dac(double voltage)
{
    return static_cast<int16_t>( round( ( (voltage / V_REF) * RES) ) );
}

// Function to convert voltage to decimal value for DAC when the range goes -5 to 5 V
constexpr int16_t to_dac_negative(double voltage)
{
    return static_cast<int16_t>( round( ( ((voltage + 5) / V_REF) * RES) ) );
}

// Function to convert voltage to decimal value for AD5781DAC
#define V_REFP   5
#define V_REFN   -5
#define AD5781_RES     262143 // 2^18 - 1
constexpr uint32_t to_AD5781dac(double voltage)
{
    return static_cast<uint32_t>( round( (voltage - V_REFN) * ( AD5781_RES/(V_REFP - V_REFN) ) ) );
}

// Variables

// EXPERIMENTAL VALUES
constexpr uint16_t LOAD_TIME = 250; // ms
constexpr uint16_t SHOTS = 10 + 10; // plus 10 for the disregarded shots at start
constexpr uint16_t BG_DELAY = 8027; // us


// SHUTTER VALUES (in us)
constexpr uint16_t MECH_DELAY_OPEN = 6500;
constexpr uint16_t MECH_DELAY_CLOSE = 1500;
constexpr uint8_t AOM_DELAY_OPEN = 20;
constexpr uint8_t AOM_DELAY_CLOSE = 5;
constexpr uint8_t AOM_DELAY = 150;


// MOT PHASE
constexpr float MOT_LOCK_FREQ = 0.832;
constexpr float MOT_TRAP_FREQ = 2.000; 
constexpr float MOT_REPUMP_FREQ = 7.14;

constexpr float MOT_LOCK_ATTE = 1.3;
constexpr float MOT_TRAP_ATTE = 0.85;
constexpr float MOT_REPUMP_ATTE = 1.3;

constexpr float MOT_C_FIELD = 0;


// PGC PHASE
constexpr float PGC_LOCK_FREQ = 0.832;
constexpr float PGC_TRAP_FREQ = 6.440;
constexpr float PGC_REPUMP_FREQ = 7.14;

constexpr float PGC_LOCK_ATTE = 1.3;
constexpr float PGC_TRAP_ATTE = 0.6;
constexpr float PGC_REPUMP_ATTE = 1.3;


// DETECT PHASE
constexpr float DETECT_LOCK_FREQ = 0.832;
constexpr float DETECT_TRAP_FREQ = 1.767;           // 0.832, 1.066, 1.299, 1.533, 1.767, 2.000, 2.234*, 2.468, 2.701, 2.935, 3.169 (0->5 in 0.5 [xGAMMA])
constexpr float DETECT_REPUMP_FREQ = 7.14;          // 7.14

constexpr float DETECT_LOCK_ATTE = 1.3;
constexpr float DETECT_TRAP_ATTE = 0.85;
constexpr float DETECT_REPUMP_ATTE = 1.3;

constexpr float OFF_REPUMP_ATTE = 0;
constexpr float OFF_REPUMP_FREQ = 0;

constexpr float DETECT_C_FIELD = 0.5;

constexpr uint8_t DROP_TIME = 5;

constexpr uint16_t REPUMP_PULSE_TIME = 100;        // us

constexpr uint16_t ADC_SAMPLES = 64;              // samples during one call of ADC_READ
constexpr uint16_t PD_ARRAY_SIZE = 3*ADC_SAMPLES;  // x4 ADC_SAMPLES because there are x3 instances of reading, N4, N4+N3, BG, the BG is x2 size of first two samples
uint16_t PD_ARRAY[PD_ARRAY_SIZE];
uint32_t PD_ARRAY32[PD_ARRAY_SIZE];

constexpr uint32_t FRACTION_ARRAY_SIZE = 10;
double FRACTION_ARRAY[FRACTION_ARRAY_SIZE];


// u_WAVE PHASE
constexpr double u_WAVE_AMP_OPEN = 0;
constexpr double u_WAVE_AMP_CLOSE = -2;
uint16_t RABI_PULSE = 5;                 // ms


// PID locking
double RABI_width = 0.7987 / (static_cast<double>(RABI_PULSE)/1000);
double f0 = 9192.631770;
// double dither_low = f0 - (RABI_width/1000000)/2;
// double dither_high = f0 + (RABI_width/1000000)/2;
double dither = 0.001;
double dither_low = f0 - dither;
double dither_high = f0 + dither;


// Integer Values
constexpr uint8_t num_images = 5; // images to take during MOT Temp measurement
// constexpr uint16_t ADC_samples = 127; // sample number when reading the ADC for detection


#endif // _SETTINGS_H_