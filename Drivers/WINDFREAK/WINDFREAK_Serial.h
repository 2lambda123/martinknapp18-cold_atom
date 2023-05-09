#include "mbed.h"
#include <cstdint>
#include "cycle_count_delay.h"
#include <iostream>
#include <string>
#include <cstring>
#include <string.h>
#include <bits/stdc++.h>


#ifndef WINDFREAK_Serial_H
#define WINDFREAK_Serial_H

// INITIALISE SERIAL BUS
#define MAX_COMMAND_SIZE 512
#define BAUD_WF 115200
BufferedSerial WF(PC_12, PD_2, BAUD_WF);
DigitalOut WF_TTL(PG_2);

// Various pre-defined commands to be sent
char str_1[] =                  "f9010";
char str_2[] =                  "f8990";
char FREQUENCY_INITIAL[] =      "f9192.631770";
char POWER_INITIAL[] =          "W10";
char MUTE_ON[] =                "h0";
char MUTE_OFF[] =               "h1";
char EXTERNAL_REFERENCE[] =     "x0";
char REFERENCE_FREQUENCY[] =    "*10";
char TRIGGER_FUNCTION[] =       "w2";
char SWEEP_UPPER[] =            "u9192632770.0";
char SWEEP_LOWER[] =            "l9192630770.0";
char SWEEP_STEP[] =             "s0000000000.001";


// for sending predefined strings
void WF_command_write(char *string){

    int len = strlen(string);
    WF.write(string,len);

    return;
}


// for sending arbitrary command letters and data
void WF_COMMAND_write(char COMMAND_LETTER_, double DATA_){

    // append the command letters with the data
    string COMMAND_string = string(1, COMMAND_LETTER_);

    // convert data to string and remove trailing zeros
    string DATA_string = to_string(DATA_);
    DATA_string.erase ( DATA_string.find_last_not_of('0') + 1, std::string::npos );
    DATA_string.erase ( DATA_string.find_last_not_of('.') + 1, std::string::npos );
    // printf("%c\n\r", COMMAND_LETTER_);
    // printf("%s\n\r", COMMAND_string);

    COMMAND_string.append(DATA_string);

    // convert to chars for sending over UART
    char* COMMAND = &COMMAND_string[0];
    // printf("%s\n\r", SWEEP_UPPER_command);

    // send over UART
    WF.write(COMMAND, strlen(COMMAND));
    // printf("%s\n\r", COMMAND);

    return;
}


// for setting up the frequency sweep parameters
void WF_build_frequency_sweep(double f0_, double SWEEP_SIZE_, int N_){

    // Calculate UPPER, LOWER, STEP
    double SWEEP_UPPER = f0_ + (SWEEP_SIZE_/2);
    double SWEEP_LOWER = f0_ - (SWEEP_SIZE_/2);
    double SWEEP_STEP = SWEEP_SIZE_/N_;
    // printf("%.5f\n\r", SWEEP_UPPER);
    // printf("%.5f\n\r", SWEEP_LOWER);
    // printf("%.5f\n\r", SWEEP_STEP);

    // Create strings of UPPER, LOWER, STEP command letters
    string SWEEP_UPPER_string = "u";
    string SWEEP_LOWER_string = "l";
    string SWEEP_STEP_string = "s";
    // cout << "String is: \n\r" << to_string(SWEEP_UPPER);
    // cout << "String is: \n\r" << to_string(SWEEP_LOWER);
    // cout << "String is: \n\r" << to_string(SWEEP_STEP);

    // append the command letters with the data
    string UPPER_STR = to_string(SWEEP_UPPER);
    string LOWER_STR = to_string(SWEEP_LOWER);
    string STEP_STR = to_string(SWEEP_STEP);

    SWEEP_UPPER_string.append(UPPER_STR);
    SWEEP_LOWER_string.append(LOWER_STR);
    SWEEP_STEP_string.append(STEP_STR);

    // convert to chars for sending over UART
    char* SWEEP_UPPER_command = &SWEEP_UPPER_string[0];
    char* SWEEP_LOWER_command = &SWEEP_LOWER_string[0];
    char* SWEEP_STEP_command = &SWEEP_STEP_string[0];

    // send over UART
    WF.write(SWEEP_UPPER_command, strlen(SWEEP_UPPER_command));
    WF.write(SWEEP_LOWER_command, strlen(SWEEP_LOWER_command));
    WF.write(SWEEP_STEP_command, strlen(SWEEP_STEP_command));
    // printf("%s\n\r", SWEEP_UPPER_command);
    // printf("%s\n\r", SWEEP_LOWER_command);
    // printf("%s\n\r", SWEEP_STEP_command);

    return;
}


// for muting the RF output, 0 = mute off, 1 = mute on
void WF_uWAVE_MUTE(bool state){

    if (state == 0){
        WF_command_write(MUTE_OFF);
    }
    if (state == 1){
        WF_command_write(MUTE_ON);
    }

    return;
}


// for toggling the state of the trigger input port
void WF_TTL_state(bool state){

    if (state == 0){
        WF_TTL = 0;
    }
    if (state == 1){
        WF_TTL = 1;
    }

    return;
}


// for initialising all settings of WINDFREAK device
void WF_init(){

    WF_COMMAND_write('f', 9192.631770);         // Frequency
    WF_COMMAND_write('W', 0);                   // Power
    WF_COMMAND_write('h', 0);                   // Mute
    WF_COMMAND_write('x', 0);                   // External Reference
    WF_COMMAND_write('*', 10);                  // External Referece Frequency
    WF_COMMAND_write('w', 2);                   // Trigger I/O Functionality
    WF_COMMAND_write('^', 1);                   // Sweep Direction
    WF_COMMAND_write('X', 0);                   // Sweep Type
    // WF_COMMAND_write('X', 0);                   // Sweep Type


    return;
}


// for querying all settings of WINDFREAK device
void WF_query(){

    char query[] = "?";
    int len = strlen(query);

    char BUFFER[4096];
    int BUFFER_index = 0;
    char matchString[] = "EOM";

    // send the "?" command
    WF.write(query,len);

    // save the response into buffer
    while (true){
        char c;
        WF.read(&c, 1);
        BUFFER[BUFFER_index] = c;
        // printf("%c\n\r", c);

        // check if the matchString is in the response, if so break the loop
        char *p = strstr(BUFFER, matchString);
        if (p){
            // printf("Y\n\r");
            break;
        }

        BUFFER_index++;
    }

    // print the response, somehow this prints to the pc UART and not the WF UART???
    printf("%s\n\r", BUFFER);

    return;
}

#endif /*WINDFREAK_Serial_H*/