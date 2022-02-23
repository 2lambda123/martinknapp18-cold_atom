#include "mbed.h"
#include <cstdint>
#include <cstring>
#include <string>

#ifndef SERIAL_H
#define SERIAL_H

// INITIALISE SERIAL BUS
#define BUFFER_SIZE 32
BufferedSerial pc(CONSOLE_TX, CONSOLE_RX);

void get_userinput(char* COMMAND_[BUFFER_SIZE]){

    char BUFFER[BUFFER_SIZE];
    char* BUFFERptr = *COMMAND_;

    printf("User Input: \n\r");

        for (int i = 0; i <= BUFFER_SIZE; i++){
            //read the char typed, save it in command, then print char back to terminal
            pc.read(BUFFER, BUFFER_SIZE);
            *BUFFERptr = BUFFER[0];
            pc.write(BUFFER, 1);

            // was enter pressed?
            if (*BUFFERptr == '\r'){
                *BUFFERptr = '\0';
                printf("Function Entered: %s\n\r", *COMMAND_);
                break; 
            }
            BUFFERptr++;

        }

}


#endif /*SERIAL_H*/