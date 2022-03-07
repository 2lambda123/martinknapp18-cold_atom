#ifndef _COLDATOM_H_
#define _COLDATOM_H_

#include "mbed.h"
#include "Drivers/MAX11300/max11300.h"

using drivers::max11300::MAX11300;

class COLDATOM
{
    public:
    // Constructor
    COLDATOM();

    // Functions
    void init();
    void run();
    void precomp();
    void PGC();
    void MOT_Temp();
    void detection();
    void fraction();
    void experimental_cycle();

    // Variables
    float pd_fraction_;
    uint32_t atom_number_;

    // Instantiate the MAX11300 class
    MAX11300 MAX11300;

    // Analog Outputs
    MAX11300::MAX11300_Ports AOM_1_FREQ_;
    MAX11300::MAX11300_Ports AOM_1_ATTE_;
    MAX11300::MAX11300_Ports AOM_2_FREQ_;
    MAX11300::MAX11300_Ports AOM_2_ATTE_;

    // Analog Inputs
    MAX11300::MAX11300_Ports PD_;

    // Ramp Definitions
    MAX11300::RampAction PGC_Ramp;

};

#endif // _COLDATOM_H_
