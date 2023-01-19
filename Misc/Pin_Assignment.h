#ifndef _PIN_ASSIGNMENT_H_
#define _PIN_ASSIGNMENT_H_

#include "mbed.h"
#include "coldatom.h"

/*
Pin Assignment
1. COM Buses
2. Digital I/O
3. Analog I/O
*/


// COM Buses
SPI MAX11300_SPI(SPI_MOSI, SPI_MISO, SPI_SCK); // MOSI, MISO, SCLK
MAX11300 MAX11300(MAX11300_SPI, SPI_CS, NC, NC);

// // COM Buses
// SPI MAX11300_SPI(PE_6, PE_5, PE_2); // MOSI, MISO, SCLK
// MAX11300 MAX11300(MAX11300_SPI, PE_4, NC, NC);

// Digital Output
DigitalOut COOLING_TTL(PG_2),
    REPUMP_TTL(PG_3),
    COIL_TTL(PD_3),
    MAKO_TTL(PC_3),
    ALVIUM_TTL(PC_2);

// Digital In
// DigitalIn  CMOS_FRAME_TTL(PC_2);

#endif // _PIN_ASSIGNMENT_H_
