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
constexpr size_t num_pd_samples = 1200;
uint16_t pd_samples[num_pd_samples];


COLDATOM::COLDATOM(bool ready)
{
    /*
    ** This constructor handles all the pin assignments
    1. COM Buses
    2. Digital I/O
    3. Analog I/O
    */

    // Wait for serial to initialize
    serial_initialize();

    // Analog Output
    AOM_1_FREQ_ = MAX11300.PORT16;
    AOM_1_ATTE_ = MAX11300.PORT17;
    AOM_2_FREQ_ = MAX11300.PORT14;
    AOM_2_ATTE_ = MAX11300.PORT15;
    AOM_3_FREQ_ = MAX11300.PORT18;
    AOM_3_ATTE_ = MAX11300.PORT19;

    // Analog Input
    PHOTODIODE_ = MAX11300.PORT0;

    // SPI
    // MAX11300_SPI.frequency(20000000);

}


void COLDATOM::initialize()
{
    /*
    1. reset() - set everything to initial values
    2. precomp() - Precompute ramps requried for experiment
    */

    // printf("SystemCoreClock is %lu MHz\n\r", SystemCoreClock/1000000);
    // printf("\n\r");

    // printf("Initializing...\n");
    MAX11300.init();
    reset();
    precomp();
    printf("Initialized\n\r");

    return;
}

void COLDATOM::reset()
{
    /*
    1. RESET to MOT values
    */

    // printf("RESET...\r\n");

    //Initial Values
    COOLING_TTL = 0;
    REPUMP_TTL = 0;
    COIL_TTL = 1;
    MAKO_TTL = 0;

    //TRAP AOM (3.5,1.3)
    MAX11300.single_ended_dac_write(AOM_1_FREQ_, to_dac(MOT_TRAP_FREQ));
    MAX11300.single_ended_dac_write(AOM_1_ATTE_, to_dac(1.3));
    //LOCK AOM (4.5,1.3)
    MAX11300.single_ended_dac_write(AOM_2_FREQ_, to_dac(MOT_LOCK_FREQ));
    MAX11300.single_ended_dac_write(AOM_2_ATTE_, to_dac(1.3));
    //REPUMP AOM (7.14,1.3 with 10dB attenuator)
    MAX11300.single_ended_dac_write(AOM_3_FREQ_, to_dac(MOT_REPUMP_FREQ));
    MAX11300.single_ended_dac_write(AOM_3_ATTE_, to_dac(1.3));

    return;
}


void COLDATOM::precomp()
{
    /*
    1. Precompute ramps required for experiment
    */

    //////////////////////////////////////////////

    // Define individual ramp specifics
    MAX11300::Ramp PGC_Ramps[] = {
        {AOM_1_FREQ_, to_dac(MOT_TRAP_FREQ), to_dac(PGC_TRAP_FREQ)},
        {AOM_1_ATTE_, to_dac(1.3), to_dac(0.6)}
    };

    // Define global ramp specifics
    PGC_Ramp.configured = 0;
    PGC_Ramp.num_ramps = ARRAYSIZE(PGC_Ramps);
    PGC_Ramp.num_steps = 33;
    PGC_Ramp.step_time_us = 15;

    // Prepare ramp function
    MAX11300.prepare_ramps(&PGC_Ramp, PGC_Ramps);

    //////////////////////////////////////////////

    // Define individual ramp specifics
    MAX11300::Ramp REPUMP_UP_Ramps[] = {
        {AOM_1_FREQ_, to_dac(5.266), to_dac(0.359)}
    };

    // Define global ramp specifics
    REPUMP_UP_Ramp.configured = 0;
    REPUMP_UP_Ramp.num_ramps = ARRAYSIZE(REPUMP_UP_Ramps);
    REPUMP_UP_Ramp.num_steps = 10;
    REPUMP_UP_Ramp.step_time_us = (100*1000);

    // Prepare ramp function
    MAX11300.prepare_ramps(&REPUMP_UP_Ramp, REPUMP_UP_Ramps);

    //////////////////////////////////////////////

    // Define individual ramp specifics
    MAX11300::Ramp REPUMP_DOWN_Ramps[] = {
        {AOM_2_FREQ_, to_dac(PGC_LOCK_FREQ), to_dac(MOT_LOCK_FREQ)}
    };

    // Define global ramp specifics
    REPUMP_DOWN_Ramp.configured = 0;
    REPUMP_DOWN_Ramp.num_ramps = ARRAYSIZE(REPUMP_DOWN_Ramps);
    REPUMP_DOWN_Ramp.num_steps = 10;
    REPUMP_DOWN_Ramp.step_time_us = (100*1000);

    // Prepare ramp function
    MAX11300.prepare_ramps(&REPUMP_DOWN_Ramp, REPUMP_DOWN_Ramps);

    //////////////////////////////////////////////

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

    // Turn lasers off whilst we wait for MOT coil to die down
    MAX11300.single_ended_dac_write(AOM_1_ATTE_, to_dac(0));
    MAX11300.single_ended_dac_write(AOM_3_ATTE_, to_dac(0)),
        MAX11300.single_ended_dac_write(AOM_3_FREQ_, to_dac(0));
    
    // Turn the MOT coil off and wait for it to die down
    COIL_TTL = 0;
    cycle_delay_ms(12);

    // Turn the lasers back on and run PGC
    MAX11300.single_ended_dac_write(AOM_1_ATTE_, to_dac(1.3));
    MAX11300.single_ended_dac_write(AOM_3_ATTE_, to_dac(1.3)),
        MAX11300.single_ended_dac_write(AOM_3_FREQ_, to_dac(MOT_REPUMP_FREQ));
    MAX11300.run_ramps(&PGC_Ramp);

    // Turn lasers off and release the cloud
    MAX11300.single_ended_dac_write(AOM_1_ATTE_, to_dac(0));
    MAX11300.single_ended_dac_write(AOM_3_ATTE_, to_dac(0)),
        MAX11300.single_ended_dac_write(AOM_3_FREQ_, to_dac(0));

    // COOLING_TTL = 1;
    REPUMP_TTL = 1;

    return;
}


void COLDATOM::MOT_Temp()
{
    /*
    1. Perform the PGC cooling
	2. Image the MOT
    */

    //////////////////////////////////////////

    // Define the dark times to loop through
    // uint16_t dark_T[] = {2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38};
    // uint16_t dark_T[] = {2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38};
    uint16_t dark_T[] = {4,8,12};
    // uint16_t dark_T[] = {16,20,24};
    // uint16_t dark_T[] = {28,32,36};
    // printf("Dark Time Size: %u\n\r", ARRAYSIZE(dark_T));
    for (uint16_t i=0; i < ARRAYSIZE(dark_T); i++)
    {
        for (uint16_t j=0; j < 8; j++)
        {
            printf("Dark Time: %u\n\r", (dark_T[i]));
            // Post-MOT cooling
            PGC();
            // Dark time
            cycle_delay_ms(dark_T[i]);
            // cycle_delay_ms(20);

            // Turn on cooling beams
            MAX11300.single_ended_dac_write(AOM_1_FREQ_, to_dac(DETECT_TRAP_FREQ));
            MAX11300.single_ended_dac_write(AOM_1_ATTE_, to_dac(1.3));

            cycle_delay_us(25);
            // Image the MOT
            MAKO_TTL = 1;
            cycle_delay_us(1000);
            MAKO_TTL = 0;

            // Return to MOT stage
            reset();
            cycle_delay_ms(5000); //steady state atom number reached
            // cycle_delay_ms(100); // Quick
        }
        // cycle_delay_ms(15000);
        cycle_delay_ms(1000); // Quick
        printf("\n\r");
    }

    // COIL_TTL = 0;
    // cycle_delay_ms(5000);
    // COIL_TTL = 1;
    //////////////////////////////////////////

    return;
}


void COLDATOM::drop_test()
{
    /*
    1. Perform the PGC cooling
	2. Drop the cloud for some time T
    */

    uint16_t DETECT_PULSE_TIME = 10;
    uint16_t REPUMP_PULSE_TIME = 5;

    //////////////////////////////////////////
    // Post-MOT cooling
    PGC();
    // Dark time
    cycle_delay_ms(200);

    // PULSE 1
    MAX11300.single_ended_dac_write(AOM_1_FREQ_, to_dac(DETECT_TRAP_FREQ)),
        MAX11300.single_ended_dac_write(AOM_1_ATTE_, to_dac(1.3));

    cycle_delay_ms(DETECT_PULSE_TIME);

    MAX11300.single_ended_dac_write(AOM_1_ATTE_, to_dac(0));

    // PULSE 2
    MAX11300.single_ended_dac_write(AOM_3_ATTE_, to_dac(MOT_REPUMP_FREQ)),
        MAX11300.single_ended_dac_write(AOM_3_FREQ_, to_dac(1.3));

    cycle_delay_ms(REPUMP_PULSE_TIME);

    MAX11300.single_ended_dac_write(AOM_3_ATTE_, to_dac(0)),
        MAX11300.single_ended_dac_write(AOM_3_FREQ_, to_dac(0));

    // PULSE 3
    MAX11300.single_ended_dac_write(AOM_1_FREQ_, to_dac(DETECT_TRAP_FREQ)),
        MAX11300.single_ended_dac_write(AOM_1_ATTE_, to_dac(1.3));

    cycle_delay_ms(DETECT_PULSE_TIME);

    MAX11300.single_ended_dac_write(AOM_1_ATTE_, to_dac(0));

    // Return to MOT stage
    cycle_delay_ms(100);
    reset();
    cycle_delay_ms(5000); //steady state atom number reached

    return;
}

void COLDATOM::diagnostic()
{
    // // Shutter ON/OFF
    // cycle_delay_ms(100);
    // COOLING_TTL = 1;
    // REPUMP_TTL = 1;
    // cycle_delay_ms(100);
    // COOLING_TTL = 0;
    // REPUMP_TTL = 0;

    // // Shutter ON/OFF
    // cycle_delay_ms(100);
    // COIL_TTL = 0;
    // cycle_delay_ms(100);
    // COIL_TTL = 1;

    // // MAKO ON/OFF
    // cycle_delay_ms(100);
    // MAKO_TTL = 1;
    // cycle_delay_ms(100);
    // MAKO_TTL = 0;

    //////////////////////////////////////////////
    // Serial Tests

    // so it runs the function, which in this case is just measuring the ADC
    uint16_t sample_num = 127;
    MAX11300.max_speed_adc_read(PHOTODIODE_, pd_samples, sample_num);
    serial_data_ready();
    for (uint16_t i=0; i < sample_num; i++)
    {
        printf("%u,\n\r", pd_samples[i]);
    }
    //////////////////////////////////////////////

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
    3. Pulse 3: Cooling light pulse for N_4 + N_3
    4. Pulse 4: Background pulse
    */

    uint16_t DETECT_PULSE_TIME = 10;
    uint16_t REPUMP_PULSE_TIME = 5;
    uint16_t ADC_SAMPLES = 127;
    uint16_t pd_ARRAY[ADC_SAMPLES];

    // Pulse 1
    MAX11300.single_ended_dac_write(AOM_1_FREQ_, to_dac(DETECT_TRAP_FREQ)),
        MAX11300.single_ended_dac_write(AOM_1_ATTE_, to_dac(1.3));
    cycle_delay_ms(1);

    MAX11300.max_speed_adc_read(PHOTODIODE_, pd_ARRAY, ADC_SAMPLES);
    
    MAX11300.single_ended_dac_write(AOM_1_ATTE_, to_dac(0));
    cycle_delay_us(10);

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

    // Return to MOT stage
    reset();
    
    serial_data_ready();
    for (uint16_t i=0; i < ADC_SAMPLES; i++)
    {
        printf("%u,\n\r", pd_samples[i]);
    }

    cycle_delay_ms(1000); //steady state atom number reached

    return;
}


void COLDATOM::fraction()
{
    /*
    1. Determine the area under fluoresence plots
	2. Calculate the transition probability
    */

    // int N_4 = 0, N_34 = 0, BG = 0;

    // uint16_t i = 0;
    // for (; i < ADC_samples; i++)
    // {
    //     N_4 += pd_samples[i];
    // }
    // for (; i < 2*ADC_samples; i++)
    // {
    //     N_34 += pd_samples[i];
    // }
    // for (; i < 3*ADC_samples; i++)
    // {
    //     BG += pd_samples[i];
    // }

    // // double fraction_ = (static_cast<double>(N_4) - static_cast<double>(BG)) / (static_cast<double>(N_34) - static_cast<double>(BG));
    // // atom_number_ = N_34 - BG; //do i need to multiply by interval width for number?
    // // pd_fraction_ = fraction_;
    // // printf("Atom Number: %lu\n\r", atom_number_);

    // return;
}


void COLDATOM::experimental()
{
    /*
    1. Run one full experimental cycle
    */

    PGC();
    // interrogate();
    cycle_delay_ms(200);
    detection();

    return;
}


// Experiment State Handler //////////////////// 
// Various States
typedef enum tSTATE { 
    USER_INPUT,
    MOT_TEMP,
    MOT_CYCLE,
    DROP_TEST,
    DROP_CYCLE,
    DIAGNOSTIC,
    DIAGNOSTIC_CYCLE,
    EXPERIMENTAL,
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
            serial_get_user_input(COMMAND);
            printf("Function Entered: %s\n\r", COMMAND);
            // cycle_delay_ms(100);

            // Work out the command
            if (strcmp(COMMAND,"MOT_TEMP") == 0){
                STATE = MOT_TEMP;
            }
            else if (strcmp(COMMAND,"MOT_CYCLE") == 0){
                STATE = MOT_CYCLE;
            }
            else if (strcmp(COMMAND,"DROP_TEST") == 0){
                STATE = DROP_TEST;
            }
            else if (strcmp(COMMAND,"DROP_CYCLE") == 0){
                STATE = DROP_CYCLE;
            }
            else if (strcmp(COMMAND,"DIAGNOSTIC") == 0){
                STATE = DIAGNOSTIC;
            }
            else if (strcmp(COMMAND,"DIAGNOSTIC_CYCLE") == 0){
                STATE = DIAGNOSTIC_CYCLE;
            }
            else if (strcmp(COMMAND,"EXPERIMENTAL") == 0){
                STATE = EXPERIMENTAL;
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

        // Perform Drop Test
        ///////////////////////////////////////
        case (DROP_TEST):
        {
            drop_test();
            STATE = USER_INPUT;
            break;
        }
        ///////////////////////////////////////

        // Perform Drop Cycle
        ///////////////////////////////////////
        case (DROP_CYCLE):
        {
            drop_test();
            STATE = DROP_CYCLE;
            break;
        }
        ///////////////////////////////////////
                
        // Perform DIAGNOSTIC
        ///////////////////////////////////////
        case (DIAGNOSTIC):
        {
            diagnostic();
            STATE = USER_INPUT;
            break;
        }
        ///////////////////////////////////////

        // Perform DIAGNOSTIC_CYCLE
        ///////////////////////////////////////
        case (DIAGNOSTIC_CYCLE):
        {
            int i = 10000;
            while(i--){
                diagnostic();
            }
            STATE = USER_INPUT;
            break;
        }
        ///////////////////////////////////////

        // Perform EXPERIMENTAL
        ///////////////////////////////////////
        case (EXPERIMENTAL):
        {
            experimental();
            STATE = USER_INPUT;
            break;
        }
        ///////////////////////////////////////

        // Perform EXPERIMENTAL_CYCLE
        ///////////////////////////////////////
        case (EXPERIMENTAL_CYCLE):
        {
            int i = 100;
            while(i--){
                experimental();
            }
            STATE = USER_INPUT;
            break;
        }
        ///////////////////////////////////////

        default:
            STATE = USER_INPUT;
            break;

    }

}