#include "mbed.h"
#include "cycle_count_delay.h"

#ifndef MOT_Imaging_H
#define MOT_Imaging_H

// INITIALISE SERIAL BUS
DigitalOut CMOS_TTL(PG_2);

void Image_MOT(){
    CMOS_TTL = 1;
    // cycle_delay_us(10);
    CMOS_TTL = 0;
}

#endif /*MOT_Imaging_H*/