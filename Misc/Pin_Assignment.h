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


// Digital OUT
DigitalOut COOLING_TTL(ARDUINO_UNO_A0),
    REPUMP_TTL(ARDUINO_UNO_A1),
    COIL_TTL(ARDUINO_UNO_A3),
    MAKO_TTL(PC_3),
    ALVIUM_TTL(PC_2),
    TRAP_AOM_SWITCH(PF_13),
    u_WAVE_TTL(PD_3);
// Camera TTLs need redefining when i swapped board out

#endif // _PIN_ASSIGNMENT_H_
