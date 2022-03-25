#ifndef _CMOS_H_
#define _CMOS_H_

#include "mbed.h"
#include "cycle_count_delay.h"
#include "Pin_Assignment.h"
#include <cstdint>

void IMAGE_SINGLE()
{
    CMOS_TTL = 1;
    cycle_delay_us(230);
    CMOS_TTL = 0;
    //cycle_delay_us(50);
}


void IMAGE_MULTI(uint8_t subs)
{
    for(uint8_t i=0; i<subs; i++)
    {
        CMOS_TTL = 1;
        cycle_delay_us(230);
        CMOS_TTL = 0;
        //cycle_delay_us(50);

        while(CMOS_FRAME_TTL==0){}
        cycle_delay_ms(50);
    }

    return;
}

#endif // _CMOS_H_