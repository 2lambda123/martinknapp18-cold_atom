#ifndef _COLDATOM_H_
#define _COLDATOM_H_

#include "mbed.h"
#include "Drivers/MAX11300/max11300.h"

using drivers::max11300::MAX11300;

void coldatom_init();
void coldatom_precomp();
void coldatom_run();
void coldatom_PGC();
void coldatom_MOT_Temp();

// Ramps
// MAX11300::RampAction PGC_Ramp;

// class coldatom{
//     public:
//     coldatom();

//     // Analog Outputs
//     MAX11300::MAX11300_Ports AOM_1;

//     // Ramps
//     MAX11300::RampAction PGC_Ramp;

// };

#endif // _COLDATOM_H_
