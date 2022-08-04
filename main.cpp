#include "mbed.h"
#include "cycle_count_delay.h"
#include "coldatom.h"
#include <cstdio>


// main() runs in its own thread in the OS
int main() {

    enableCycleCounter();
    cycle_delay_ms(5000);

    COLDATOM coldatom(true);
    coldatom.initialise();
    while (true) {
        coldatom.run();

    }
}

