#ifndef _PIN_ASSIGNMENT_H_
#define _PIN_ASSIGNMENT_H_

#include "mbed.h"
#include "coldatom.h"
#include "BurstSPI.h"


/*
Pin Assignment
1. COM Buses
2. Digital I/O
3. Analog I/O
*/


// COM Buses
// SPI MAX11300_SPI(SPI_MOSI, SPI_MISO, SPI_SCK); // MOSI, MISO, SCLK
// MAX11300 MAX11300(MAX11300_SPI, SPI_CS, NC, NC);

// // DigitalOut ADC_CNVT(PE_11);
// SPI MAX11300_SPI(SPI_MOSI, SPI_MISO, SPI_SCK);
// BurstSPI MAX11300_SPI(SPI_MOSI, SPI_MISO, SPI_SCK);
// MAX11300 MAX11300(MAX11300_SPI, SPI_CS, NC, PE_11);

// SPI AD7195_SPI(PE_6, PE_5, PE_2);
// AD7195 AD7195(AD7195_SPI, PE_4);

// Digital OUT
DigitalOut COOLING_TTL(ARDUINO_UNO_A0),
    REPUMP_TTL(ARDUINO_UNO_A1),
    COIL_TTL(ARDUINO_UNO_A3),
    MAKO_TTL(PC_3),
    ALVIUM_TTL(PC_2),
    TRAP_AOM_SWITCH(PF_13),
    u_WAVE_TTL(PD_3);
// Camera TTLs need redefining when i swapped board out



// Digital IN
// DigitalIn  CMOS_FRAME_TTL(PC_2);

#endif // _PIN_ASSIGNMENT_H_
