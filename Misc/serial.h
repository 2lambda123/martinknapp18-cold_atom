#include "mbed.h"

#ifndef SERIAL_H
#define SERIAL_H

// INITIALISE SERIAL BUS
#define BUFFER_SIZE 32
BufferedSerial pc(CONSOLE_TX, CONSOLE_RX);

void get_userinput(){

    char BUFFER[BUFFER_SIZE];
    char COMMAND[BUFFER_SIZE];

    printf("User Input: \n\r");
        for (int i = 0; i <= BUFFER_SIZE; i++){
            pc.read(BUFFER, BUFFER_SIZE);
            COMMAND[i] = BUFFER[0];
            pc.write(BUFFER, 1);
            // printf("BUFFER: %s\n\r", BUFFER);
            // printf("COMMAND: %s\n\r", &COMMAND[i]);

            if (COMMAND[i] == '\r'){
                COMMAND[i] = '\0';
                printf("Function Entered: %s\n\r", COMMAND);
                break;
            }
        }
}


#endif /*SERIAL_H*/