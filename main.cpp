#include "mbed.h"
#include "cycle_count_delay.h"
#include "coldatom.h"


// main() runs in its own thread in the OS
int main() {

    enableCycleCounter();

    //coldatom_pins();
    coldatom_init();
    while (true) {
        coldatom_run();

    }
}

