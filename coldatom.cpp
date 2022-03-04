#include "cycle_count_delay.h"
#include "coldatom.h"
#include "mbed.h"
#include "serial.h"
#include "Drivers/MAX11300/max11300.h"
#include "Misc/macros.h"
#include "Pin_Assignment.h"
#include "settings.h"

MAX11300::RampAction PGC_Ramp;

void coldatom_init()
{
    /*
    1. Set all outputs to the intial values
    2. Precompute ramps requried for experiment
    */

    printf("Initialising...\n\r");

    // Initial Values
    COOLING_SHUTTER_TTL = 0;
    REPUMP_SHUTTER_TTL = 0;

    MAX11300.single_ended_dac_write(MAX11300::PORT10, 0);
    MAX11300.single_ended_dac_write(MAX11300::PORT11, 0);

    // Run the precompute function to calculate ramps
    coldatom_precomp();

    return;
}


void coldatom_precomp()
{
    /*
    1. Precompute ramps required for experiment
    */

    // Define individual ramp specifics
    MAX11300::Ramp PGC_Ramps[] = {
        {AOM_1_, to_dac(0), to_dac(2.5)},
        {MAX11300::PORT10, to_dac(0), to_dac(5)}
    };

    // Define global ramp specifics
    PGC_Ramp.configured = 0;
    PGC_Ramp.num_ramps = ARRAYSIZE(PGC_Ramps);
    PGC_Ramp.num_steps = 30;
    PGC_Ramp.step_time_us = 100;

    // Prepare ramp function
    MAX11300.prepare_ramps(&PGC_Ramp, PGC_Ramps);

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
    
    //MAX11300.run_ramps(&PGC_Ramp);

    // cycle_delay_ms(250);
    // COOLING_SHUTTER_TTL = 1;
    // cycle_delay_ms(250);
    // COOLING_SHUTTER_TTL = 0;

    cycle_delay_ms(2000);
    REPUMP_SHUTTER_TTL = 1;
    cycle_delay_ms(2000);
    REPUMP_SHUTTER_TTL = 0;

    return;
}


void coldatom_MOT_Temp()
{
    /*
    1. Perform the PGC cooling
	2. Image the MOT
    */

    // printf("MOT_Temperature_Measurement\n\r");

    coldatom_PGC();
    //Image_MOT();

    return;
}


void coldatom_detection()
{
    /*
    1. Pulse 1: Cooling light pulse for N_4 
	2. Pulse 2: Repump light to return to F=4
    3. Pulse 3: Cooling light pulse for N_34
    */

    // Pulse 1
    // Line of code to pulse the cooling light on
    MAX11300.max_speed_adc_read(MAX11300_Ports port, int *value, int num_samples);
    cycle_delay_ms(const int ms);

    // Pulse 2
    // Line of code to pulse the repump light on
    MAX11300.max_speed_adc_read(MAX11300_Ports port, int *value, int num_samples);
    cycle_delay_ms(const int ms);

    // Pulse 3
    // Line of code to pulse the cooling light on
    MAX11300.max_speed_adc_read(MAX11300_Ports port, int *value, int num_samples);
    cycle_delay_ms(const int ms);

    // Calculate the fraction
    coldatom_fraction();

    return;
}


void coldatom_fraction()
{
    /*
    1. Determine the area under fluoresence plots
	2. Calculate the transition probability
    */

    uint32_t N_4 = 0, N_34 = 0, BG = 6;

    size_t i = 0;
    for (; i < 127; i++){
        N_4 += pd_samples[i];
    }
    for (; i < 127 + 127; i++){
        N_34 += pd_samples[i];
    }
    for (; i < 127 + 127 + 127; i++){
        BG += pd_samples[i];
    }

    double fraction = (static_cast<double>(N_4) - static_cast<double>(BG)) / (static_cast<double>(N_34) - static_cast<double>(BG));
    // printf("atom_num: %lu\n\n", f34 - bg);
    // atom_number_ = N_34 - BG;
    // Transition = fraction;

    return;
}


void coldatom_experimental_cycle()
{
    /*
    1. Run one full experimental cycle
    */

    coldatom_PGC();
    // microwave interrogation
    coldatom_detection();

    return;
}


// Experiment State Handler //////////////////// 
// Various States
typedef enum tSTATE { 
    USER_INPUT,
    MOT_TEMP,
    EXPERIMENTAL_CYCLE 
} tSTATE;

tSTATE STATE;
void coldatom_run()
{
    switch(STATE)
    {
        // Get user input
        ///////////////////////////////////////
        case (USER_INPUT):
        {
            char COMMAND[BUFFER_SIZE];
            get_userinput(COMMAND);
            printf("Function Entered: %s\n\r", COMMAND);
            cycle_delay_ms(2000);

            // Work out the command
            if (strcmp(COMMAND,"MOT_TEMP") == 0){
                STATE = MOT_TEMP;
            }
            else if (strcmp(COMMAND,"EXPERIMENTAL_CYCLE") == 0){
                STATE = EXPERIMENTAL_CYCLE;
            }
            else {
                error_handler(1);
            }
            break;
        }
        ///////////////////////////////////////

        // Perform MOT temperature measurement
        ///////////////////////////////////////
        case (MOT_TEMP):
        {
            // printf("STATE_B\n\r");
            coldatom_PGC();
            //cycle_delay_ms(1000);
            STATE = MOT_TEMP;
            break;
        }
        ///////////////////////////////////////

        // Perform one clock cycle
        ///////////////////////////////////////
        case (EXPERIMENTAL_CYCLE):
        {
            // printf("STATE_C\n\r");
            coldatom_experimental_cycle();
            STATE = USER_INPUT;
            break;
        }
        ///////////////////////////////////////

        default:
            STATE = USER_INPUT;
            break;

    }

}