#ifndef _PIN_ASSIGNMENT_H_
#define _PIN_ASSIGNMENT_H_

#include "mbed.h"
#include "coldatom.h"
// #include "Drivers/MAX11300/max11300.h"

// using drivers::max11300::MAX11300;

/*
Pin Assignment
1. COM Buses
2. Digital I/O
3. Analog I/O
*/

// // COM Buses
// SPI MAX11300_SPI(PE_6, PE_5, PE_2); // MOSI, MISO, SCLK
// MAX11300 MAX11300(MAX11300_SPI, PE_4, NC, NC);

// Digital Output
DigitalOut COOLING_SHUTTER_TTL(PG_3);
DigitalOut REPUMP_SHUTTER_TTL(PG_2);
DigitalOut MOT_COIL_TTL(PD_3);
DigitalOut CMOS_TTL(PD_4);

#endif // _PIN_ASSIGNMENT_H_
