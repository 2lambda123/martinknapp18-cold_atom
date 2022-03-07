#include "mbed.h"

#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#define V_REF   10
#define RES     4095

// Function to convert voltage to decimal value for DAC
constexpr int16_t to_dac(double voltage){
  return static_cast<int16_t>(voltage / V_REF * RES);
}

// Values for precomputing ramps
// constexpr float AOM_1_FREQ = 2.5;
// constexpr float AOM_1_ATTEN = 0;
// constexpr float AOM_2_FREQ = 2.5;
// constexpr float AOM_2_ATTEN = 0;

#endif // _SETTINGS_H_