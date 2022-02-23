#ifndef _COLDATOM_H_
#define _COLDATOM_H_

#include "mbed.h"
#include "Drivers/MAX11300/max11300.h"

void coldatom_pins();
void coldatom_init();
void coldatom_precomp();
void coldatom_run();
void coldatom_PGC();
void coldatom_MOT_Temp();

// class Coldatom{
//     public:
//     protected:

//     // Analog Outputs
//     MAX11300::MAX11300_Ports AOM_1_FREQ;
// };

#endif // _COLDATOM_H_
