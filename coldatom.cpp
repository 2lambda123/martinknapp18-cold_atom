#include "mbed.h"
#include "cycle_count_delay.h"
#include "coldatom.h"

#include "serial.h"
#include "Misc/macros.h"
#include "settings.h"
#include "Pin_Assignment.h"
#include <cstdint>

#include "Drivers/MAX11300/max11300.h"

using drivers::max11300::MAX11300;

// Array for saving ADC read values
uint16_t num_pd_samples = 1200;
uint16_t pd_samples[num_pd_samples];

COLDATOM::COLDATOM()
{
    /*
    ** This constructor handles all the pin assignments
    1. COM Buses
    2. Digital I/O
    3. Analog I/O
    */

    printf("Constructor\n\r");

    // COM Buses
    SPI MAX11300_SPI(PE_6, PE_5, PE_2); // MOSI, MISO, SCLK
    MAX11300.MAX11300(MAX11300_SPI, PE_4, NC, NC);

    // Digital Output
    DigitalOut COOLING_SHUTTER_TTL(PG_3);
    DigitalOut REPUMP_SHUTTER_TTL(PG_2);
    DigitalOut MOT_COIL_TTL(PD_3);
    DigitalOut CMOS_TTL(PD_4);

    // Analog Output
    AOM_1_FREQ_ = MAX11300.PORT16;
    AOM_1_ATTE_ = MAX11300.PORT17;
    AOM_2_FREQ_ = MAX11300.PORT18;
    AOM_2_ATTE_ = MAX11300.PORT19;

    // Analog Input
    PHOTODIODE_ = MAX11300.PORT0;
}


void COLDATOM::initialise()
{
    /*
    1. Set all outputs to the intial values
    2. Precompute ramps requried for experiment
    */

    printf("Initialising...\n\r");

    // Initial Values
    COOLING_SHUTTER_TTL = 0;
    REPUMP_SHUTTER_TTL = 0;
    MOT_COIL_TTL = 0;
    CMOS_TTL = 0;

    MAX11300.single_ended_dac_write(AOM_1_FREQ_, to_dac(2.5));
    MAX11300.single_ended_dac_write(AOM_1_ATTE_, to_dac(0));
    MAX11300.single_ended_dac_write(AOM_2_FREQ_, to_dac(2.5));
    MAX11300.single_ended_dac_write(AOM_2_ATTE_, to_dac(0));

    // Run the precompute function to calculate ramps
    precomp();

    return;
}


void COLDATOM::precomp()
{
    /*
    1. Precompute ramps required for experiment
    */

    // Define individual ramp specifics
    MAX11300::Ramp PGC_Ramps[] = {
        {AOM_1_FREQ_, to_dac(1), to_dac(2)},
        {AOM_2_FREQ_, to_dac(0), to_dac(1)}
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


void COLDATOM::PGC()
{
    /*
    1. Turn off the MOT quadrupole coils
	2. Wait for the residual B field to reach zero
	3. Simeltaneously RAMP cooling intensity down AND cooling detuning furhter to red
    4. Turn lasers off with AOM and shutters
    */
    
    // MOT_COIL_TTL = 1;
    // cycle_delay_us(10);
    // MAX11300.run_ramps(&PGC_Ramp);
    // COOLING_SHUTTER_TTL = 1,
    //     REPUMP_SHUTTER_TTL = 1,
    //     MAX11300.single_ended_dac_write(AOM_1_ATTE_, to_dac(0)),
    //     MAX11300.single_ended_dac_write(AOM_2_ATTE_, to_dac(0));

    // This bit is just to test some things
    cycle_delay_ms(250);
    COOLING_SHUTTER_TTL = 1;
    cycle_delay_ms(250);
    COOLING_SHUTTER_TTL = 0;

    cycle_delay_ms(2000);
    REPUMP_SHUTTER_TTL = 1;
    cycle_delay_ms(2000);
    REPUMP_SHUTTER_TTL = 0;

    return;
}


void COLDATOM::MOT_Temp()
{
    /*
    1. Perform the PGC cooling
	2. Image the MOT
    */

    PGC();
    
    for(int i = 0; i <= num_images; i++)
    {
        CMOS_TTL = 1;
        CMOS_TTL = 0;
        cycle_delay_us(10);
    }

    return;
}


void COLDATOM::interrogate()
{
    /*
    1. Perform microwave interrogation
    */

    return;
}


void COLDATOM::detection()
{
    /*
    1. Pulse 1: Cooling light pulse for N_4 
	2. Pulse 2: Repump light to return to F=4
    3. Pulse 3: Cooling light pulse for N_34
    4. Pulse 4: Background pulse
    */


    // Pulse 1
    COOLING_SHUTTER_TTL = 1;
    MAX11300.max_speed_adc_read(PHOTODIODE_, pd_samples, ADC_samples);
    COOLING_SHUTTER_TTL = 0;
    cycle_delay_us(10);

    // Pulse 2
    REPUMP_SHUTTER_TTL = 1;
    cycle_delay_us(10);
    REPUMP_SHUTTER_TTL = 0;

    // Pulse 3
    COOLING_SHUTTER_TTL = 1;
    MAX11300.max_speed_adc_read(PHOTODIODE_, &pd_samples[1*ADC_samples], ADC_samples);
    COOLING_SHUTTER_TTL = 0;
    cycle_delay_us(10);

    // Pulse 4
    COOLING_SHUTTER_TTL = 1;
    MAX11300.max_speed_adc_read(PHOTODIODE_, &pd_samples[2*ADC_samples], ADC_samples);
    COOLING_SHUTTER_TTL = 0;
    cycle_delay_us(10);

    // Calculate the fraction
    fraction();

    return;
}


void COLDATOM::fraction()
{
    /*
    1. Determine the area under fluoresence plots
	2. Calculate the transition probability
    */

    int N_4 = 0, N_34 = 0, BG = 0;

    uint16_t i = 0;
    for (; i < ADC_samples; i++){
        N_4 += pd_samples[i];
    }
    for (; i < (2*ADC_samples)); i++){
        N_34 += pd_samples[i];
    }
    for (; i < (3*ADC_samples)); i++){
        BG += pd_samples[i];
    }

    pd_fraction_ = (static_cast<double>(N_4) - static_cast<double>(BG)) / (static_cast<double>(N_34) - static_cast<double>(BG));
    atom_number_ = N_34 - BG; //do i need to multiply by interval width for number?
    printf("Atom Number: %lu\n\r", atom_number_);

    return;
}


void COLDATOM::experimental_cycle()
{
    /*
    1. Run one full experimental cycle
    */

    PGC();
    interrogate();
    detection();

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
void COLDATOM::run()
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
            MOT_Temp();
            STATE = MOT_TEMP;
            break;
        }
        ///////////////////////////////////////

        // Perform one clock cycle
        ///////////////////////////////////////
        case (EXPERIMENTAL_CYCLE):
        {
            experimental_cycle();
            STATE = USER_INPUT;
            break;
        }
        ///////////////////////////////////////

        default:
            STATE = USER_INPUT;
            break;

    }

}