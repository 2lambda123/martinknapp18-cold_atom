#ifndef _DIAGNOSITC_H_
#define _DIAGNOSITC_H_

#include "mbed.h"
#include "cycle_count_delay.h"
#include "coldatom.h"

#include "serial.h"
#include "Misc/macros.h"
#include "settings.h"
#include <cstdint>
#include <cstdio>

#include "Drivers/MAX11300/max11300.h"

using drivers::max11300::MAX11300;


void AOM_ONOFF(drivers::max11300::MAX11300::MAX11300_Ports port){

    float ONOFF[] = {1.08,0};
    int j =0;
    char BUFFER[1];

    for(int i=0; i <= 60; i++)
    {
        //read the char typed, if char was 'o' then turn off/on the AOM
        pc.read(BUFFER, 1);
        if (*BUFFER == 'o')
        {
                MAX11300.single_ended_dac_write(port, to_dac(ONOFF[j]));
                j = !j;
        }

    }

}
    

void VOLTAGE_FREQUENCY_ATTENUATION(drivers::max11300::MAX11300::MAX11300_Ports port_ATTE, drivers::max11300::MAX11300::MAX11300_Ports port_FREQ){

    // This function ramps over a number of voltage to determine
    // the voltage tuning for AOM frequency and attenuation
    float FREQ = 1;
    float ATTE = 0;
    for(; ATTE <= 1.1; )
    {
        printf("%.2f\n\r", ATTE);
        MAX11300.single_ended_dac_write(port_ATTE, to_dac(ATTE));
        MAX11300.single_ended_dac_write(port_FREQ, to_dac(FREQ));
        cycle_delay_ms(500);
        //COOLING_SHUTTER_TTL = !COOLING_SHUTTER_TTL;
        FREQ += 0.08;
        ATTE += 0.01;
    }
    MAX11300.single_ended_dac_write(port_ATTE, to_dac(0));

}

#endif // _DIAGNOSITC_H_