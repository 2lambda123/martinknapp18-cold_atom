#include "mbed.h"

#ifndef SERIAL_H
#define SERIAL_H

// INITIALISE SERIAL BUS
#define BUFFER_SIZE 32
BufferedSerial pc(CONSOLE_TX, CONSOLE_RX);

#endif /*SERIAL_H*/