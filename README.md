# COLD_ATOM

Code for controlling the DPhil experiment.

The architecture resembles a state machine where initially the experiment idles waiting for user input. Upon input of a valid function the state changes to execute the corresponding experimental sequence.

User input is provided via serial interface.

## Code Structure
```
├── Drivers # For peripherals.
│   └── MAX11300 # Code for MAX11300 peripheral board (ADC/DAC).
├── Experiment # Header file with predefined values for experimental ramps.
├── mbed-os # MBED library.
└── Misc # Collection of small useful pieces of code.
```