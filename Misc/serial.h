#include "mbed.h"
#include <cstdint>
#include <cstdio>
#include "cycle_count_delay.h"
#include "Misc/macros.h"
#include "settings.h"

#ifndef SERIAL_H
#define SERIAL_H

// INITIALISE SERIAL BUS
#define BUFFER_SIZE 512
#define BAUD 921600
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


// read over serial and check if stop was sent. return 1 = stop, return 0 = continue
int serial_stop_command()
{
    char BUFFER[BUFFER_SIZE];
    char *BUFFERptr = BUFFER;
    int i = 0;

    while (pc.readable()==1){
        // printf("here\n\r");
        pc.read(BUFFER, 4);
        *BUFFERptr = BUFFER[i];
        i++;
        printf("%s\n\r", BUFFERptr);
    }

    *BUFFERptr = BUFFER[0];
    // if (*BUFFERptr == 'SHOT'){
    if (strcmp(BUFFER,"STOP") == 0){
        printf("here\n\r");
        return 1;
    }
    else{
        return 0;
    }

}


// Tell python that data is ready to printed
void serial_data_ready()
{
    printf("DATA\n\r");
}


// Sending ADC data over serial
void serial_send_array(uint16_t* array_, uint16_t size)
{

    printf("Samples: (%u,)\n\r", ADC_SAMPLES);
    printf("ADC: (");
    for (uint16_t i=0; i < size; i++)
    {
        printf("%u,", array_[i]);
    }
    printf (")\n\r");
    // printf("DATA\n\r");
}

// Sending ADC data over serial
void serial_send_array_32(uint32_t* array_, uint16_t size)
{

    printf("Samples: (%u,)\n\r", ADC_SAMPLES);
    printf("ADC: (");
    for (uint16_t i=0; i < size; i++)
    {
        printf("%lu,", array_[i]);
    }
    printf (")\n\r");
    // printf("DATA\n\r");
}

// Sending ADC data over serial
void serial_send_array_doubles(double* array_, uint16_t size)
{

    // printf("Samples: (%u,)\n\r", size);
    printf("DOUBLES: (");
    for (uint16_t i=0; i < size; i++)
    {
        printf("%.7f,", array_[i]);
    }
    printf (")\n\r");
    // printf("DATA\n\r");
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