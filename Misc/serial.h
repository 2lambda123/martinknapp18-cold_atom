#include "mbed.h"
#include <cstdint>
#include "cycle_count_delay.h"
#include <string.h>

#ifndef SERIAL_H
#define SERIAL_H

// INITIALISE SERIAL BUS
#define BUFFER_SIZE 512
#define BAUD 460800
BufferedSerial pc(CONSOLE_TX, CONSOLE_RX, BAUD);


// Provide it with an empty array and the array will be filled with user input
void serial_initialize()
{
    char BUFFER[BUFFER_SIZE];
    char COMMAND[BUFFER_SIZE];
    char *BUFFERptr = COMMAND;
    while (true)
    {
        if (pc.readable() == 1)
        {
            pc.read(BUFFER, BUFFER_SIZE);
            *BUFFERptr = BUFFER[0];
            // was enter pressed?
            if (*BUFFERptr == '\0')
            {
                if (strcmp(COMMAND,"init") == 0)
                {
                    break;  
                }
            }
            BUFFERptr++;
        }
    }
}


// Provide it with an empty array and the array will be filled with user input
void serial_get_user_input(char COMMAND_[])
{
    char BUFFER[BUFFER_SIZE];
    char *BUFFERptr = COMMAND_;

    while (true)
    {
        pc.read(BUFFER, 1);
        *BUFFERptr = BUFFER[0];
        // was enter pressed?
        if (*BUFFERptr == '\0')
        {
            *BUFFERptr = '\0';
            // printf("Finished\n\r");
            // printf("Function Entered: %s\n\r", COMMAND_);
            break; 
        }
        BUFFERptr++;
    }
}


// Tell python that data is ready to printed
void serial_data_ready()
{
    printf("DATA\n\r");
}


// Tell python that data is ready to printed
void serial_data_done()
{
    printf("DONE\n\r");
}


// When an error is required to be processed, this function should be called with the required error code as a variable
void error_handler(int errorcode){ 

    switch(errorcode){
        case 1:
            printf("Invalid Function\n\r");
            return;
        case 2:
            printf("Invalid Value\n\r");
            return;
    }
}


#endif /*SERIAL_H*/