#include "mbed.h"

#ifndef SERIAL_H
#define SERIAL_H

// INITIALISE SERIAL BUS
#define BUFFER_SIZE 32
BufferedSerial pc(CONSOLE_TX, CONSOLE_RX);


// Provide it with an empty array and the array will be filled with user input
void get_userinput(char COMMAND_[]){

    char BUFFER[BUFFER_SIZE];
    char *BUFFERptr = COMMAND_;

    printf("User Input: \n\r");

        for (int i = 0; i <= BUFFER_SIZE; i++){
            //read the char typed, save it in command, then print char back to terminal
            pc.read(BUFFER, BUFFER_SIZE);
            *BUFFERptr = BUFFER[0];
            pc.write(BUFFER, 1);

            // was enter pressed?
            if (*BUFFERptr == '\r'){
                *BUFFERptr = '\0';
                // printf("Function Entered: %s\n\r", COMMAND_);
                break; 
            }

            //inrement pointer to next address in COMMAND_
            BUFFERptr++;

        }

}

// When an error is required to be processed, this function should be called with the required error code as a variable
void error_handler(int errorcode){ 

    switch(errorcode){
        case 1:
            printf("Invalid Function\r\n");
            return;

        case 2:
            printf("Invalid Value\r\n");
            return;
    }
}


#endif /*SERIAL_H*/