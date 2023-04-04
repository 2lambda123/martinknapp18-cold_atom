// Taken from Anthony VH
// http://www.anthonyvh.com/2017/05/18/cortex_m-cycle_counter/

#ifndef _CYCLE_DELAY_H_
#define _CYCLE_DELAY_H_

#include <stdint.h>
// #include <system_stm32f7xx.h>
#include "mbed.h"
// #include <core_cm7.h>
#include <math.h>

#include <system_stm32f4xx.h>
#include <core_cm4.h>


static bool module_init = false;

inline void enableCycleCounter() {
    if (module_init) {
      return;
    }
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

    // DWT->LAR = 0xC5ACCE55; // F4 proccessor doesn't have the LAR software lock 
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    module_init = true;
}

inline void delayCycles(uint32_t const numCycles) {
    uint32_t const startCycles = DWT->CYCCNT;
    while ((DWT->CYCCNT - startCycles) < numCycles) {
    }
}

inline uint32_t usToCycles(uint32_t const us) {
    return ceil(us * ((float)SystemCoreClock / 1e6f)) - 16;
}

inline uint32_t nsToCycles(uint32_t const ns) {
    return ceil(ns * ((float)SystemCoreClock / 1e9f)) - 16;
}

inline void cycle_delay_ns(uint32_t const ns) { delayCycles(nsToCycles(ns)); }
inline void cycle_delay_us(uint32_t const us) { delayCycles(usToCycles(us)); }
inline void cycle_delay_ms(uint32_t const ms) { delayCycles(usToCycles(ms * 1000)); }

#endif // _CYCLE_DELAY_H_
