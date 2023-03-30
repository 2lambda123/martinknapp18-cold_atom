# COLD_ATOM

Code for controlling the DPhil experiment.

The architecture resembles a state machine where initially the experiment idles waiting for user input. Upon input of a valid function the state changes to execute the corresponding experimental sequence.

User input is provided via serial interface.

## Code Structure
```
├── mbed-os # MBED library.
├── Drivers # For peripherals.
│   └── MAX11300 # Code for MAX11300 peripheral board (ADC/DAC).
├── Misc # Collection of small useful pieces of code.
│   ├── BURST_SPI # Code for performing high speed SPI communications.
|   ├── cycle_count_delay.h # Code for cycle counter and nanosecond delays on Cortex-M.
|   └── settings.h # Header file with predefined values for experimental ramps.
└── Python # Code for python.
```