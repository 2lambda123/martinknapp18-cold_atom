#include "cycle_count_delay.h"
#include "coldatom.h"
#include "mbed.h"
#include "serial.h"
#include "Drivers/MAX11300/max11300.h"
#include "Misc/macros.h"
#include "Pin_Assignment.h"
#include "settings.h"

void coldatom_init()
{
    /*
    1. Set all outputs to the intial values
    2. Precompute ramps requried for experiment
    */

    printf("Initialising...\n\r");

    // Initial Values
    //MAX11300 single_ended_dac_write(MAX11300::PORT0, 0);

    // Run the precompute function to calculate ramps
    coldatom_precomp();

    return;
}


void coldatom_precomp()
{
    /*
    1. Precompute ramps required for experiment
    */

    //Define individual ramp specifics
    MAX11300::Ramp PGC_Ramps[] = {
        {AOM_1, to_dac(0), to_dac(2.5)}
    };

    // Define global ramp specifics
    PGC_Ramp.num_ramps = ARRAYSIZE(PGC_Ramps);
    PGC_Ramp.num_steps = 30;
    PGC_Ramp.step_time_us = 100;

    // Prepare ramp function
    //prepare_ramps(PGC_Ramp, PGC_Ramps);

    return;
}


void coldatom_PGC()
{
    /*
    1. Turn off the MOT quadrupole coils
	2. Wait for the residual B field to reach zero
	3. Simeltaneously RAMP cooling intensity down AND cooling detuning furhter to red
    4. Turn lasers off with AOM and shutters
    */

    printf("MOT_PGC\n\r");


    return;
}


void coldatom_MOT_Temp()
{
    /*
    1. Perform the PGC cooling
	2. Image the MOT
    */

    printf("MOT_Temperature_Measurement\n\r");

    coldatom_PGC();
    //Image_MOT();

    return;
}


// Experiment State Handler //////////////////// 
// Various States
typedef enum tSTATE { 
    STATE_A,
    STATE_B,
    STATE_C 
} tSTATE;

tSTATE  STATE;
void coldatom_run()
{
    switch(STATE)
    {
        // Get user input
        ///////////////////////////////////////
        case (STATE_A):
        {
            char COMMAND[BUFFER_SIZE];
            get_userinput(COMMAND);
            printf("Function Entered: %s\n\r", COMMAND);
            cycle_delay_ms(2000);

            // Work out the command
            if (strcmp(COMMAND,"STATE_B") == 0){
                STATE = STATE_B;
            }
            else if (strcmp(COMMAND,"STATE_C") == 0){
                STATE = STATE_C;
            }
            else {
                error_handler(1);
            }
            break;
        }
        ///////////////////////////////////////

        // Perform MOT temperature measurement
        ///////////////////////////////////////
        case (STATE_B):
        {
            printf("STATE_B\n\r");
            cycle_delay_ms(1000);
            STATE = STATE_A;
            break;
        }
        ///////////////////////////////////////

        // Perform one clock cycle
        ///////////////////////////////////////
        case (STATE_C):
        {
            printf("STATE_C\n\r");
            cycle_delay_ms(1000);
            STATE = STATE_A;
            break;
        }
        ///////////////////////////////////////

        default:
            STATE = STATE_A;
            break;

    }

}