#ifndef _COLDATOM_H_
#define _COLDATOM_H_

#include "mbed.h"
#include "Drivers/MAX11300/max11300.h"  

using drivers::max11300::MAX11300;

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
    void PGC();
    void MOT_Temp();
    void drop_test();
    void diagnostic();
    void interrogate();
    void detection();
    void fraction();
    void experimental_cycle();

    // Variables
    bool ready;
    double pd_fraction_;
    uint16_t atom_number_;

    // Analog Outputs
    MAX11300::MAX11300_Ports AOM_1_FREQ_;
    MAX11300::MAX11300_Ports AOM_1_ATTE_;
    MAX11300::MAX11300_Ports AOM_2_FREQ_;
    MAX11300::MAX11300_Ports AOM_2_ATTE_;
    MAX11300::MAX11300_Ports AOM_3_FREQ_;
    MAX11300::MAX11300_Ports AOM_3_ATTE_;

    // Analog Inputs
    MAX11300::MAX11300_Ports PHOTODIODE_;

    // Ramp Definitions
    MAX11300::RampAction PGC_Ramp;
    MAX11300::RampAction REPUMP_UP_Ramp;
    MAX11300::RampAction REPUMP_DOWN_Ramp;

};

#endif // _COLDATOM_H_
