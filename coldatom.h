#ifndef _COLDATOM_H_
#define _COLDATOM_H_

#include "mbed.h"
#include "Drivers/MAX11300/max11300.h"  
#include "Drivers/AD7195/AD7195.h"  

// using drivers::max11300::MAX11300;
// using drivers_AD::ad7195::AD7195;

class COLDATOM
{
    public:
    // Constructor
    COLDATOM(bool ready = true);

    // Functions
    void initialize();
    void reset();
    void run();
    void precomp();
    void precomp_optimise(float detuning_, float atte_);
    void PGC();
    void PGC_OPT();
    void MOT_Temp();
    void MOT_Load();
    void drop_test();
    void diagnostic();
    void interrogate();
    void detection();
    double fraction();
    void experimental();
    void rabi();
    void cooling_light(bool state_, float detuning_, float power_);
    void repump_light(bool state_, float detuning_, float power_);

    // Variables
    bool ready;
    double pd_fraction_;
    uint16_t atom_number_;

    // Analog Outputs
    drivers::max11300::MAX11300::MAX11300_Ports AOM_1_FREQ_;
    drivers::max11300::MAX11300::MAX11300_Ports AOM_1_ATTE_;
    drivers::max11300::MAX11300::MAX11300_Ports AOM_2_FREQ_;
    drivers::max11300::MAX11300::MAX11300_Ports AOM_2_ATTE_;
    drivers::max11300::MAX11300::MAX11300_Ports AOM_3_FREQ_;
    drivers::max11300::MAX11300::MAX11300_Ports AOM_3_ATTE_;
    drivers::max11300::MAX11300::MAX11300_Ports C_FIELD_MOD_;
    drivers::max11300::MAX11300::MAX11300_Ports u_WAVE_AMP_;
    drivers::max11300::MAX11300::MAX11300_Ports u_WAVE_FREQ_;

    // Analog Inputs
    drivers::max11300::MAX11300::MAX11300_Ports PD_1_;
    drivers::max11300::MAX11300::MAX11300_Ports PD_2_;

    // Ramp Definitions
    drivers::max11300::MAX11300::RampAction PGC_Ramp;
    drivers::max11300::MAX11300::RampAction OPT_Ramp;
    drivers::max11300::MAX11300::RampAction C_FIELD_Ramp;

    // protected:

    BurstSPI MAX11300_SPI;
    drivers::max11300::MAX11300 MAX11300;

    SPI AD7195_SPI;
    drivers_AD::ad7195::AD7195 AD7195;


};

#endif // _COLDATOM_H_
