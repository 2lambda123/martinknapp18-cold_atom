// Analog values
constexpr float AO1_MOT = 6.6;
constexpr float AO2_MOT = 1.76;
constexpr float AO3_MOT = 5;
constexpr float EO_MOT = 8.93;
constexpr float NS_MOT = 0.7;
constexpr float WE_MOT = 1.2;
constexpr float BIAS_MOT = 0.11;


#define K_UP 1

// DDS Paramaeters
// TODO(split out for different experiment and maybe pass in as object)

// #include "dds_params/declare_T_80_fringes.h"
// #include "dds_params/declare_T_120_fringes.h"
// #include "dds_params/declare_T_120_16_fringes.h"
// #include "dds_params/declare_T_115_fringes_17e_7.h"
#if K_UP
#include "dds_params/T_130_fringes.h"
#include "dds_params/declare_spectroscopy.h"
#else
#include "dds_params/k_down.h"
#include "dds_params/k_down_spec.h"
#endif
