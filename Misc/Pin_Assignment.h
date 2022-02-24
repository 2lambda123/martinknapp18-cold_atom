#ifndef _PIN_ASSIGNMENT_H_
#define _PIN_ASSIGNMENT_H_

#include "mbed.h"
#include "coldatom.h"
#include "Drivers/MAX11300/max11300.h"

using drivers::max11300::MAX11300;

/*
Pin Assignment
1. COM Buses
2. Digital I/O
3. Analog I/O
*/

// Buses
SPI MAX11300_SPI(PC_12, PC_11, PC_10); // MOSI, MISO, SCLK
MAX11300 MAX11300(MAX11300_SPI, PD_2, NC, NC);
//SPI DDS_SPI(PC_12, PC_11, PC_10, PD_2); // MOSI, MISO, SCLK, CS

// Digital Output
DigitalOut COOLING_SHUTTER_TTL(PD_0);
DigitalOut REPUMP_SHUTTER_TTL(PD_1);
DigitalOut MOT_COIL_TTL(PD_3);
DigitalOut CMOS_TTL(PD_4);

// Analog Output
MAX11300::MAX11300_Ports AOM_1_ = MAX11300::PORT10;
MAX11300::MAX11300_Ports AOM_2_ = MAX11300::PORT11;

// // Analog Input
// MAX11300::MAX11300_Ports PD_ = MAX11300::PORT10;

#endif // _PIN_ASSIGNMENT_H_
