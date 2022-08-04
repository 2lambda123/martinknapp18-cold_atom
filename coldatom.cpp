#include "mbed.h"
#include "cycle_count_delay.h"
#include "coldatom.h"

#include "serial.h"
#include "Misc/macros.h"
#include "settings.h"
#include "Pin_Assignment.h"
#include <cstdint>
#include <cstdio>

#include "Drivers/MAX11300/max11300.h"

using drivers::max11300::MAX11300;

// Array for saving ADC read values
uint16_t pd_samples[3*ADC_samples];


COLDATOM::COLDATOM(bool ready)
{
    /*
    ** This constructor handles all the pin assignments
    1. COM Buses
    2. Digital I/O
    3. Analog I/O
    */

    printf("\n\r");
    printf("\n\r");
    printf("Constructor\n\r");

    // Analog Output
    AOM_1_FREQ_ = MAX11300.PORT16;
    AOM_1_ATTE_ = MAX11300.PORT17;
    AOM_2_FREQ_ = MAX11300.PORT14;
    AOM_2_ATTE_ = MAX11300.PORT15;
    AOM_3_FREQ_ = MAX11300.PORT18;
    AOM_3_ATTE_ = MAX11300.PORT19;

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
    // printf("SystemCoreClock is %lu MHz\n\r", SystemCoreClock/1000000);
    // printf("\n\r");

    //Initial Values
    COOLING_TTL = 0;
    REPUMP_TTL = 0;
    COIL_TTL = 1;
    MAKO_TTL = 0;

    //TRAP AOM (3.5,1.3)
    MAX11300.single_ended_dac_write(AOM_1_FREQ_, to_dac(3.5));
    MAX11300.single_ended_dac_write(AOM_1_ATTE_, to_dac(1.3));
    //LOCK AOM (4.5,1.3)
    MAX11300.single_ended_dac_write(AOM_2_FREQ_, to_dac(4.5));
    MAX11300.single_ended_dac_write(AOM_2_ATTE_, to_dac(1.3));
    //REPUMP AOM (7.43,1.3 with 10dB attenuator)
    MAX11300.single_ended_dac_write(AOM_3_FREQ_, to_dac(7.43));
    MAX11300.single_ended_dac_write(AOM_3_ATTE_, to_dac(1.3));

    // Run the precompute function to calculate ramps
    // precomp();

    return;
}


void COLDATOM::precomp()
{
    /*
    1. Precompute ramps required for experiment
    */

    // Define individual ramp specifics
    MAX11300::Ramp PGC_Ramps[] = {
        {AOM_1_FREQ_, to_dac(3.5), to_dac(1)},
        {AOM_1_ATTE_, to_dac(1.3), to_dac(0.8)}
    };

    // Define global ramp specifics
    PGC_Ramp.configured = 0;
    PGC_Ramp.num_ramps = ARRAYSIZE(PGC_Ramps);
    PGC_Ramp.num_steps = 10;
    PGC_Ramp.step_time_us = 500;

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

    // COIL_TTL = 0;
    // printf("HERE\n\r");
    cycle_delay_ms(12);
    MAX11300.run_ramps(&PGC_Ramp);
    MAX11300.single_ended_dac_write(AOM_1_ATTE_, to_dac(0));
    MAX11300.single_ended_dac_write(AOM_3_ATTE_, to_dac(0));
    // COOLING_TTL = 1;
    // REPUMP_TTL = 1;
    MAX11300.single_ended_dac_write(AOM_1_FREQ_, to_dac(4.5));

    return;
}


void COLDATOM::MOT_Temp()
{
    /*
    1. Perform the PGC cooling
	2. Image the MOT
    */
    // printf("HERE\n\r");

    //////////// Release Recapture Image ///////////
    PGC();
    cycle_delay_ms(15);
    // COOLING_SHUTTER_TTL = 0;
    // cycle_delay_ms(1);
    MAX11300.single_ended_dac_write(AOM_1_ATTE_, to_dac(1.3));

    //return to MOT stage
    MAX11300.single_ended_dac_write(AOM_1_FREQ_, to_dac(3.5));
    MAX11300.single_ended_dac_write(AOM_3_ATTE_, to_dac(1.3));
    // COOLING_TTL = 0;
    // REPUMP_TTL = 0;
    // COIL_TTL = 1;
    cycle_delay_ms(4000); //steady state atom number reached
    //////////////////////////////////////////

    // //////////// Multi Drop Tests ///////////
    // for(int i = 190; i < 200; i+=1)
    // {
    //     PGC();
    //     cycle_delay_ms(i);
    //     // Shift the light back onto resonance and open shutters
    //     MAX11300.single_ended_dac_write(AOM_1_FREQ_, to_dac(2)),
    //         COOLING_SHUTTER_TTL = 0,
    //         //REPUMP_SHUTTER_TTL = 0,
    //         MAX11300.single_ended_dac_write(AOM_1_ATTE_, to_dac(1.6));
    //     // delay to see the atoms
    //     cycle_delay_ms(500);
    //     // Return to MOT loading values
    //     MAX11300.single_ended_dac_write(AOM_1_FREQ_, to_dac(2)),
    //         MAX11300.single_ended_dac_write(AOM_2_FREQ_, to_dac(1)),
    //         MAX11300.single_ended_dac_write(AOM_1_ATTE_, to_dac(1.6)),
    //         REPUMP_SHUTTER_TTL = 0,
    //         MOT_COIL_TTL = 1;
    //     cycle_delay_ms(2000); //steady state atom number reached
    // }
    // //////////////////////////////////////////

    // //////////// Single Drop Test ///////////
    // PGC();
    // cycle_delay_ms(145);
    // // Shift the light back onto resonance and open shutters
    // MAX11300.single_ended_dac_write(AOM_1_FREQ_, to_dac(2)),
    //     COOLING_SHUTTER_TTL = 0,
    //     REPUMP_SHUTTER_TTL = 0,
    //     MAX11300.single_ended_dac_write(AOM_1_ATTE_, to_dac(1.6));
    // // delay to see the atoms
    // cycle_delay_ms(500);
    // // Return to MOT loading values
    // MAX11300.single_ended_dac_write(AOM_1_FREQ_, to_dac(2)),
    //     MAX11300.single_ended_dac_write(AOM_2_FREQ_, to_dac(1)),
    //     MOT_COIL_TTL = 1;
    // cycle_delay_ms(4000); //steady state atom number reached
    // //////////////////////////////////////////

    // //////////// Single Drop Test ///////////
    // PGC();
    // cycle_delay_ms(198);
    // // Shift the light back onto resonance and open shutters
    // MAX11300.single_ended_dac_write(AOM_1_FREQ_, to_dac(4.5));
    // // COOLING_SHUTTER_TTL = 0;
    // // REPUMP_SHUTTER_TTL = 0;
    // MAX11300.single_ended_dac_write(AOM_1_ATTE_, to_dac(1.3));

    // // delay to see the atoms
    // cycle_delay_ms(50);
    // // Return to MOT loading values
    // MAX11300.single_ended_dac_write(AOM_1_FREQ_, to_dac(3.5)),
    //     COOLING_SHUTTER_TTL = 0;
    //     REPUMP_SHUTTER_TTL = 0,
    //     MOT_COIL_TTL = 1;
    // cycle_delay_ms(4000); //steady state atom number reached
    // //////////////////////////////////////////

    // // AOM Ramps
    // float V_RAMP = 7;
    // for(int i = 0; V_RAMP < 8.1; i++)
    // {
    //     printf("VOLTAGE: %.2f\n\r", V_RAMP);
    //     MAX11300.single_ended_dac_write(AOM_3_FREQ_, to_dac(V_RAMP));
    //     V_RAMP += 0.1;
    //     MOT_COIL_TTL = 0;
    //     cycle_delay_ms(10);
    //     MOT_COIL_TTL = 1;
    //     cycle_delay_ms(500);
    // }

    // AOM Ramps
    //MAX11300.single_ended_dac_write(AOM_2_FREQ_, to_dac(0.5));
    // MOT_COIL_TTL = 0;
    // cycle_delay_ms(2000);
    // MOT_COIL_TTL = 1;
    // float V_RAMP_RED = 2;
    // float V_RAMP_BLUE = 2;
    // for(int i = 0; V_RAMP_RED < 3; i++)
    // {
    //     printf("VOLTAGE: %.2f\n\r", V_RAMP_RED);
    //     MAX11300.single_ended_dac_write(AOM_2_FREQ_, to_dac(V_RAMP_RED));
    //     V_RAMP_RED += 0.05;
    //     MAX11300.single_ended_dac_write(AOM_1_FREQ_, to_dac(V_RAMP_BLUE));
    //     V_RAMP_BLUE -= 0.05;
    //     cycle_delay_ms(500);
    // }
    // MAX11300.single_ended_dac_write(AOM_2_FREQ_, to_dac(2));
    // MAX11300.single_ended_dac_write(AOM_1_FREQ_, to_dac(2));

    return;
}

void COLDATOM::diagnostic()
{
    // Shutter ON/OFF
    cycle_delay_ms(1000);
    COOLING_TTL = 1;
    REPUMP_TTL = 1;
    cycle_delay_ms(1000);
    COOLING_TTL = 0;
    REPUMP_TTL = 0;

    // Shutter ON/OFF
    cycle_delay_ms(1000);
    COIL_TTL = 0;
    cycle_delay_ms(1000);
    COIL_TTL = 1;

    // MAKO ON/OFF
    cycle_delay_ms(1000);
    MAKO_TTL = 1;
    cycle_delay_ms(1000);
    MAKO_TTL = 0;

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

    // // Pulse 1
    // COOLING_TTL = 1;
    // MAX11300.max_speed_adc_read(PHOTODIODE_, pd_samples, ADC_samples);
    // COOLING_TTL = 0;
    // cycle_delay_us(10);

    // // Pulse 2
    // REPUMP_TTL = 1;
    // cycle_delay_us(10);
    // REPUMP_TTL = 0;

    // // Pulse 3
    // COOLING_TTL = 1;
    // MAX11300.max_speed_adc_read(PHOTODIODE_, &pd_samples[1*ADC_samples], ADC_samples);
    // COOLING_TTL = 0;
    // cycle_delay_us(10);

    // // Pulse 4
    // COOLING_TTL = 1;
    // MAX11300.max_speed_adc_read(PHOTODIODE_, &pd_samples[2*ADC_samples], ADC_samples);
    // COOLING_TTL = 0;
    // cycle_delay_us(10);

    // // Calculate the fraction
    // fraction();

    // return;
}


void COLDATOM::fraction()
{
    /*
    1. Determine the area under fluoresence plots
	2. Calculate the transition probability
    */

    int N_4 = 0, N_34 = 0, BG = 0;

    uint16_t i = 0;
    for (; i < ADC_samples; i++)
    {
        N_4 += pd_samples[i];
    }
    for (; i < 2*ADC_samples; i++)
    {
        N_34 += pd_samples[i];
    }
    for (; i < 3*ADC_samples; i++)
    {
        BG += pd_samples[i];
    }

    // double fraction_ = (static_cast<double>(N_4) - static_cast<double>(BG)) / (static_cast<double>(N_34) - static_cast<double>(BG));
    // atom_number_ = N_34 - BG; //do i need to multiply by interval width for number?
    // pd_fraction_ = fraction_;
    // printf("Atom Number: %lu\n\r", atom_number_);

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
    MOT_CYCLE,
    DIAGNOSTIC 
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
            // printf("Function Entered: %s\n\r", COMMAND);
            cycle_delay_ms(1000);

            // Work out the command
            if (strcmp(COMMAND,"MOT_TEMP") == 0){
                STATE = MOT_TEMP;
            }
            else if (strcmp(COMMAND,"MOT_CYCLE") == 0){
                STATE = MOT_CYCLE;
            }
            else if (strcmp(COMMAND,"DIAGNOSTIC") == 0){
                STATE = DIAGNOSTIC;
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
            STATE = USER_INPUT;
            break;
        }
        ///////////////////////////////////////

        // Perform one clock cycle
        ///////////////////////////////////////
        case (MOT_CYCLE):
        {
            MOT_Temp();
            STATE = MOT_CYCLE;
            break;
        }
        ///////////////////////////////////////
                
        // Perform DIAGNOSTICS
        ///////////////////////////////////////
        case (DIAGNOSTIC):
        {
            diagnostic();
            STATE = USER_INPUT;
            break;
        }
        ///////////////////////////////////////

        default:
            STATE = USER_INPUT;
            break;

    }

}