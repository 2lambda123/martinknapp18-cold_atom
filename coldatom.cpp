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
    pc.set_baud(BAUD);
    // serial_initialize();

    // Analog Output
    AOM_1_FREQ_ = MAX11300.PORT16;
    AOM_1_ATTE_ = MAX11300.PORT17;
    AOM_2_FREQ_ = MAX11300.PORT14;
    AOM_2_ATTE_ = MAX11300.PORT15;
    AOM_3_FREQ_ = MAX11300.PORT18;
    AOM_3_ATTE_ = MAX11300.PORT19;
    C_FIELD_MOD_ = MAX11300.PORT13;
    u_WAVE_MOD_ = MAX11300.PORT1;

    // Analog Input
    PD_1_ = MAX11300.PORT0;
    // PD_2_ = MAX11300.PORT0;
}


void COLDATOM::initialize()
{
    /*
    1. reset() - sets everything to initial values
    2. precomp() - Precompute ramps requried for experiment
    */

    // printf("SystemCoreClock is %lu MHz\n\r", SystemCoreClock/1000000);
    // printf("\n\r");

    // printf("Initializing...\n");
    MAX11300.init();
    reset();
    precomp();
    // printf("Initialized\n\r");

    return;
}


void COLDATOM::reset()
{
    /*
    1. RESET to MOT values
    */

    // printf("RESET...\n\r");

    //Initial Values
    COOLING_TTL = 0;
    REPUMP_TTL = 0;
    COIL_TTL = 1;
    MAKO_TTL = 0;
    ALVIUM_TTL = 0;

    //TRAP AOM
    MAX11300.single_ended_dac_write(AOM_1_FREQ_, to_dac(MOT_TRAP_FREQ));
    MAX11300.single_ended_dac_write(AOM_1_ATTE_, to_dac(MOT_TRAP_ATTE));
    //LOCK AOM
    MAX11300.single_ended_dac_write(AOM_2_FREQ_, to_dac(MOT_LOCK_FREQ));
    MAX11300.single_ended_dac_write(AOM_2_ATTE_, to_dac(MOT_LOCK_ATTE));
    //REPUMP AOM
    MAX11300.single_ended_dac_write(AOM_3_FREQ_, to_dac(MOT_REPUMP_FREQ));
    MAX11300.single_ended_dac_write(AOM_3_ATTE_, to_dac(MOT_REPUMP_ATTE));

    // C_FIELD_MOD
    MAX11300.single_ended_dac_write(C_FIELD_MOD_, to_dac(MOT_C_FIELD));

    // u_WAVE_MOD
    MAX11300.single_ended_dac_write(u_WAVE_MOD_, to_dac(u_WAVE_MOD_CLOSE));

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
        {AOM_1_ATTE_, to_dac(MOT_TRAP_ATTE), to_dac(PGC_TRAP_ATTE)}
    };

    // Define global ramp specifics
    PGC_Ramp.configured = 0;
    PGC_Ramp.num_ramps = ARRAYSIZE(PGC_Ramps);
    PGC_Ramp.num_steps = 6;
    PGC_Ramp.step_time_us = 10; // remember 80us delay in SPI single_dac_write() funtion

    // Prepare ramp function
    MAX11300.prepare_ramps(&PGC_Ramp, PGC_Ramps);

    //////////////////////////////////////////////

    // // Define individual ramp specifics
    // MAX11300::Ramp C_FIELD_Ramps[] = {
    //     {C_FIELD_MOD_, to_dac(MOT_C_FIELD_), to_dac(DETECT_C_FIELD_)}
    // };

    // // Define global ramp specifics
    // PGC_Ramp.configured = 0;
    // PGC_Ramp.num_ramps = ARRAYSIZE(C_FIELD_Ramps);
    // PGC_Ramp.num_steps = 32;
    // PGC_Ramp.step_time_us = 156;

    // // Prepare ramp function
    // MAX11300.prepare_ramps(&C_FIELD_Ramp, C_FIELD_Ramps);

    // //////////////////////////////////////////////

    return;
}


void COLDATOM::precomp_optimise(float detuning_, float atte_)
{
    /*
    1. Precompute ramps required for experiment
    */

    //////////////////////////////////////////////

    // Define individual ramp specifics
    MAX11300::Ramp OPT_Ramps[] = {
        {AOM_1_FREQ_, to_dac(MOT_TRAP_FREQ), to_dac(detuning_)},
        {AOM_1_ATTE_, to_dac(MOT_TRAP_ATTE), to_dac(atte_)}
    };

    // Define global ramp specifics
    OPT_Ramp.configured = 0;
    OPT_Ramp.num_ramps = ARRAYSIZE(OPT_Ramps);
    OPT_Ramp.num_steps = 33;
    OPT_Ramp.step_time_us = 15;

    // Prepare ramp function
    MAX11300.prepare_optimise_ramps(&OPT_Ramp, OPT_Ramps);

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

    // //////////////////////////////////////////////////////////
    // // WITH SHUTTERS
    // // Turn lasers off whilst we wait for MOT coil to die down
    // COOLING_TTL = 1,
    //     REPUMP_TTL = 1;
    // cycle_delay_us(MECH_DELAY_CLOSE);
    // MAX11300.single_ended_dac_write(AOM_1_ATTE_, to_dac(0)),
    //     MAX11300.single_ended_dac_write(AOM_3_ATTE_, to_dac(0)),
    //     MAX11300.single_ended_dac_write(AOM_3_FREQ_, to_dac(0)),
    // cycle_delay_us(AOM_DELAY_CLOSE);

    // // Turn the MOT coil off and wait for it to die down
    // COIL_TTL = 0;
    // cycle_delay_ms(8);

    // // Turn the lasers back on
    // COOLING_TTL = 0,
    //     REPUMP_TTL = 0;
    // cycle_delay_us(MECH_DELAY_OPEN);
    // MAX11300.single_ended_dac_write(AOM_1_ATTE_, to_dac(MOT_TRAP_ATTE)),
    //     MAX11300.single_ended_dac_write(AOM_3_ATTE_, to_dac(MOT_REPUMP_ATTE)),
    //     MAX11300.single_ended_dac_write(AOM_3_FREQ_, to_dac(MOT_REPUMP_FREQ));
    // cycle_delay_us(AOM_DELAY_OPEN);

    // // Run the PGC and turn the lasers off
    // COOLING_TTL = 1,
    //     REPUMP_TTL = 1;
    // cycle_delay_us(1000);

    // MAX11300.run_ramps(&PGC_Ramp);

    // MAX11300.single_ended_dac_write(AOM_1_ATTE_, to_dac(0)),
    // cycle_delay_us(100);
    // MAX11300.single_ended_dac_write(AOM_3_ATTE_, to_dac(0)),
    //     MAX11300.single_ended_dac_write(AOM_3_FREQ_, to_dac(0));

    // MAX11300.single_ended_dac_write(C_FIELD_MOD_, to_dac(DETECT_C_FIELD_));
    //////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////
    // JUST AOMS
    // Turn lasers off
    MAX11300.single_ended_dac_write(AOM_1_ATTE_, to_dac(0)),
    // MAX11300.single_ended_dac_write(AOM_1_FREQ_, to_dac(0)),
        MAX11300.single_ended_dac_write(AOM_3_ATTE_, to_dac(0)),
        MAX11300.single_ended_dac_write(AOM_3_FREQ_, to_dac(0));

    // Turn the MOT coil off and wait for it to die down
    COIL_TTL = 0;
    cycle_delay_ms(10);

    MAX11300.single_ended_dac_write(AOM_1_ATTE_, to_dac(MOT_TRAP_ATTE)),
    // MAX11300.single_ended_dac_write(AOM_1_FREQ_, to_dac(MOT_TRAP_FREQ)),
        MAX11300.single_ended_dac_write(AOM_3_ATTE_, to_dac(MOT_REPUMP_ATTE)),
        MAX11300.single_ended_dac_write(AOM_3_FREQ_, to_dac(MOT_REPUMP_FREQ));

    MAX11300.run_ramps(&PGC_Ramp);

    // cycle_delay_ms(10);

    MAX11300.single_ended_dac_write(AOM_1_ATTE_, to_dac(0));
    MAX11300.single_ended_dac_write(AOM_1_FREQ_, to_dac(0));

    cycle_delay_us(200);
    MAX11300.single_ended_dac_write(AOM_3_ATTE_, to_dac(0));
    MAX11300.single_ended_dac_write(AOM_3_FREQ_, to_dac(0));

    // MAX11300.single_ended_dac_write(C_FIELD_MOD_, to_dac(DETECT_C_FIELD_));

    //////////////////////////////////////////////////////////

    return;
}


void COLDATOM::PGC_OPT()
{
    /*
    1. Turn off the MOT quadrupole coils
	2. Wait for the residual B field to reach zero
	3. Simeltaneously RAMP cooling intensity down AND cooling detuning furhter to red
    4. Turn lasers off with AOM and shutters
    */

    // Turn lasers off whilst we wait for MOT coil to die down
    cooling_light(0, MOT_TRAP_FREQ, 0);
    repump_light(0, MOT_REPUMP_FREQ, 0);

    // Turn the MOT coil off and wait for it to die down
    COIL_TTL = 0;
    cycle_delay_ms(14);

    // Turn the lasers back on and run PGC
    cooling_light(1, MOT_TRAP_FREQ, 0.95),
        repump_light(1, MOT_REPUMP_FREQ, 1);

    MAX11300.run_ramps(&OPT_Ramp);

    // Turn lasers off and release the cloud
    cooling_light(0, PGC_TRAP_FREQ, 0);
    cycle_delay_us(100);
    repump_light(0, PGC_REPUMP_FREQ, 0);

    return;
}


void COLDATOM::MOT_Temp()
{
    /*
    1. Perform the PGC cooling
	2. Image the MOT
    3. Perform background image, turn coils off, run sequence again 
    */

    ////////////////////////////////////////

    // Define the dark times to loop through
    // uint16_t dark_T[] = {8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38};
    // uint16_t dark_T[] = {30,30,30,30,30,30,30,1,2,3,4,5,6,8,10,12,14,16,18,20,22,24,26,28,30};
    // uint16_t dark_T[] = {30,30,30,30,10,10,10,10,10,10};
    uint16_t dark_T[] = {5};
    // uint16_t dark_T[] = {5,6,5,6,5,6,5,6,5,6,5,6,5,6,5,6,5,6,5,6,5,6,5,6,5,6,5,6};
    for (uint16_t i=0; i < ARRAYSIZE(dark_T); i++)
    {
        for (uint16_t j=0; j < 10; j++)
        {
            PGC();
            cycle_delay_ms(dark_T[i]);

            // Turn on cooling beams
            // MAX11300.single_ended_dac_write(AOM_1_ATTE_, to_dac(MOT_TRAP_ATTE));
            // cycle_delay_us(AOM_DELAY_OPEN);
            // cycle_delay_ms(2);
            MAX11300.single_ended_dac_write(AOM_1_FREQ_, to_dac(DETECT_TRAP_FREQ));
            MAX11300.single_ended_dac_write(AOM_1_ATTE_, to_dac(DETECT_TRAP_ATTE));
            cycle_delay_us(50);

            // Image the MOT
            MAKO_TTL = 1;
            cycle_delay_us(1000);
            MAKO_TTL = 0;

            // Return to MOT stage
            cycle_delay_ms(100);
            reset();
            cycle_delay_ms(500); //steady state atom number reached
        }
    }
    
    ////////////////////////////////////////

    return;
}


void COLDATOM::drop_test()
{
    /*
    1. Perform the PGC cooling
	2. Drop the cloud for some time T
    */

    uint16_t DETECT_PULSE_TIME = 500;
    uint16_t REPUMP_PULSE_TIME = 250;

    //////////////////////////////////////////

    // Post-MOT cooling
    PGC();

    // MAX11300.single_ended_dac_write(AOM_1_ATTE_, to_dac(0));

    // cycle_delay_us(500);
    // MAX11300.single_ended_dac_write(AOM_3_ATTE_, to_dac(0));
    // MAX11300.single_ended_dac_write(AOM_3_FREQ_, to_dac(0));

    // DROP
    // MAX11300.single_ended_dac_write(AOM_1_FREQ_, to_dac(DETECT_TRAP_FREQ));
    MAX11300.single_ended_dac_write(u_WAVE_MOD_, to_dac(u_WAVE_MOD_OPEN));
    cycle_delay_ms(5);
    MAX11300.single_ended_dac_write(u_WAVE_MOD_, to_dac(u_WAVE_MOD_CLOSE));


    // Pulse 1
    MAX11300.single_ended_dac_write(AOM_1_FREQ_, to_dac(DETECT_TRAP_FREQ));
    MAX11300.single_ended_dac_write(AOM_1_ATTE_, to_dac(DETECT_TRAP_ATTE));
    cycle_delay_us(DETECT_PULSE_TIME);
    MAX11300.single_ended_dac_write(AOM_1_ATTE_, to_dac(0));

    // Pulse 2
    MAX11300.single_ended_dac_write(AOM_3_FREQ_, to_dac(DETECT_REPUMP_FREQ));
    MAX11300.single_ended_dac_write(AOM_3_ATTE_, to_dac(DETECT_REPUMP_ATTE));
    cycle_delay_us(REPUMP_PULSE_TIME);
    MAX11300.single_ended_dac_write(AOM_3_ATTE_, to_dac(0));
    MAX11300.single_ended_dac_write(AOM_3_FREQ_, to_dac(0));

    // Pulse 3
    // MAX11300.single_ended_dac_write(AOM_1_FREQ_, to_dac(0.832));
    MAX11300.single_ended_dac_write(AOM_1_ATTE_, to_dac(DETECT_TRAP_ATTE));
    cycle_delay_ms(100);
    MAX11300.single_ended_dac_write(AOM_1_ATTE_, to_dac(0));

    // // Pulse 4 (F3 and F4 light together for the final pulse)
    // MAX11300.single_ended_dac_write(AOM_3_FREQ_, to_dac(DETECT_REPUMP_FREQ));
    // MAX11300.single_ended_dac_write(AOM_3_ATTE_, to_dac(DETECT_REPUMP_ATTE));
    // MAX11300.single_ended_dac_write(AOM_1_ATTE_, to_dac(DETECT_TRAP_ATTE));
    // cycle_delay_ms(100);


    // RESET
    reset();
    cycle_delay_ms(1000); //steady state atom number reached

    return;
}


void COLDATOM::diagnostic()
{
    // // Shutter ON/OFF
    // int i = 5000;
    // while(i--)
    // {
    // cycle_delay_us(500);
    // COOLING_TTL = 1;
    // // REPUMP_TTL = 1;
    // cycle_delay_us(500);
    // COOLING_TTL = 0;
    // // REPUMP_TTL = 0;
    // }

    // AOM ON/OFF
    int i = 30000;
    int pulse = 500;
    while(i--)
    {
        cycle_delay_us(pulse);
        MAX11300.single_ended_dac_write(AOM_1_ATTE_, to_dac(0));

        cycle_delay_us(pulse);
        MAX11300.single_ended_dac_write(AOM_1_ATTE_, to_dac(MOT_TRAP_ATTE));
    }
    reset();


    // // u_WAVE_MOD_
    // int i = 10;
    // while(i--)
    // {
    //     cycle_delay_ms(4000);
    //     MAX11300.single_ended_dac_write(u_WAVE_MOD_, to_dac(u_WAVE_MOD_OPEN_));

    //     cycle_delay_ms(4000);
    //     MAX11300.single_ended_dac_write(u_WAVE_MOD_, to_dac(u_WAVE_MOD_CLOSE_));
    // }


    // // Shutter ON/OFF
    // cycle_delay_ms(500);
    // cooling_light(1, DETECT_TRAP_FREQ, DETECT_TRAP_ATTE);
    // cycle_delay_ms(500);
    // cooling_light(0, DETECT_TRAP_FREQ, DETECT_TRAP_ATTE);

    // // Shutter DELAY optimise
    // int i = 150;
    // while(i--)
    // {
    // COOLING_TTL = 1;
    // cycle_delay_us(MECH_DELAY_CLOSE);
    // MAX11300.single_ended_dac_write(AOM_1_ATTE_, to_dac(0.95));
    // cycle_delay_ms(1);

    // cycle_delay_ms(100);

    // COOLING_TTL = 0;
    // cycle_delay_us(MECH_DELAY_OPEN);
    // MAX11300.single_ended_dac_write(AOM_1_ATTE_, to_dac(1.3));
    // cycle_delay_ms(1);

    // cycle_delay_ms(100);
    // }

    // // MAKO ON/OFF
    // cycle_delay_ms(10);
    // MAKO_TTL = 1;
    // cycle_delay_us(1000);
    // MAKO_TTL = 0;

    // // ALVIUM ON/OFF
    // cycle_delay_ms(100);
    // ALVIUM_TTL = 1;
    // cycle_delay_us(1000);
    // ALVIUM_TTL = 0;

    // // CFIELD ON/OFF
    // int i = 10;
    // while(i--)
    // {
    // cycle_delay_ms(1000);
    // MAX11300.run_ramps(&C_FIELD_Ramp);
    // cycle_delay_ms(1000);
    // MAX11300.single_ended_dac_write(C_FIELD_MOD_, to_dac(MOT_C_FIELD_));
    // }

    // // AOM ON/OFF
    // int i = 10000;
    // while(i--)
    // {
    //     PGC();
    //     cooling_light(1, MOT_TRAP_FREQ, MOT_TRAP_ATTE);
    //     cycle_delay_us(1000);
    //     cooling_light(0, MOT_TRAP_FREQ, MOT_TRAP_ATTE);
    //     cycle_delay_ms(100);
    // }
    // reset();

    // //////////////////////////////////////////////

    // float VARIABLE_array[] = {3.169,
    //     3.402,
    //     3.636,
    //     3.870,
    //     4.103,
    //     4.337,
    //     4.571,
    //     4.804,
    //     5.038,
    //     5.272,
    //     5.505,
    //     5.739,
    //     5.973,
    //     6.206,
    //     6.440,
    //     6.674,
    //     6.907,
    //     7.141,
    //     7.375,
    //     7.608,
    //     7.842,
    //     8.076,
    //     8.309,
    //     8.543,
    //     8.777,
    //     9.010,
    //     9.244};


    // for (uint16_t i=0; i < ARRAYSIZE(VARIABLE_array); i++)
    // {
    //     float VARIABLE = VARIABLE_array[i];
    //     precomp_optimise(VARIABLE, PGC_TRAP_ATTE);
    //     for (uint16_t j=0; j < 5; j++)
    //     { 
    //         PGC_OPT();
    //         cycle_delay_ms(200);
    //         detection();
    //         printf("%.3f,\n\r", VARIABLE);
    //         printf("%u,\n\r", j);
    //     }
    // }

    return;

}


void COLDATOM::interrogate()
{
    /*
    1. Perform microwave interrogation
    */

    MAX11300.single_ended_dac_write(u_WAVE_MOD_, to_dac(u_WAVE_MOD_OPEN));
    cycle_delay_ms(RABI_PULSE);
    MAX11300.single_ended_dac_write(u_WAVE_MOD_, to_dac(u_WAVE_MOD_CLOSE));

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

    // Pulse 1
    MAX11300.single_ended_dac_write(AOM_1_FREQ_, to_dac(DETECT_TRAP_FREQ));
    MAX11300.single_ended_dac_write(AOM_1_ATTE_, to_dac(DETECT_TRAP_ATTE));
    MAX11300.max_speed_adc_read(PD_1_, PD_ARRAY, ADC_SAMPLES);
    MAX11300.single_ended_dac_write(AOM_1_ATTE_, to_dac(0));

    // Pulse 2
    MAX11300.single_ended_dac_write(AOM_3_FREQ_, to_dac(DETECT_REPUMP_FREQ));
    MAX11300.single_ended_dac_write(AOM_3_ATTE_, to_dac(DETECT_REPUMP_ATTE));
    cycle_delay_us(REPUMP_PULSE_TIME);
    MAX11300.single_ended_dac_write(AOM_3_ATTE_, to_dac(0));
    MAX11300.single_ended_dac_write(AOM_3_FREQ_, to_dac(0));

    // Pulse 3
    MAX11300.single_ended_dac_write(AOM_1_ATTE_, to_dac(DETECT_TRAP_ATTE));
    MAX11300.max_speed_adc_read(PD_1_, &PD_ARRAY[127], ADC_SAMPLES);

    // Background Pulse
    cycle_delay_ms(50);
    MAX11300.max_speed_adc_read(PD_1_, &PD_ARRAY[127+127], ADC_SAMPLES);
    MAX11300.single_ended_dac_write(AOM_1_ATTE_, to_dac(0));

    // Calculate the fraction
    fraction();

    // Return to MOT stage
    cycle_delay_ms(100);
    reset();
    
    serial_data_ready();
    for (uint16_t i=0; i < ADC_SAMPLES; i++)
    {
        printf("%u,\n\r", PD_ARRAY[i]);
    }

    cycle_delay_ms(3000); //steady state atom number reached

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
    for (; i < ADC_SAMPLES; i++)
    {
        N_4 += PD_ARRAY[i];
    }
    for (; i < 2*ADC_SAMPLES; i++)
    {
        N_34 += PD_ARRAY[i];
    }
    for (; i < 3*ADC_SAMPLES; i++)
    {
        BG += PD_ARRAY[i];
    }

    double fraction_ = (static_cast<double>(N_4) - static_cast<double>(BG)) / (static_cast<double>(N_34) - static_cast<double>(BG));
    pd_fraction_ = fraction_;
    // atom_number_ = N_34 - BG; //do i need to multiply by interval width for number?
    // printf("Atom Number: %lu\n\r", atom_number_);

    return;
}


void COLDATOM::experimental()
{
    /*
    1. Run one full experimental cycle
    */

    PGC();
    interrogate();
    detection();

    return;
}


void COLDATOM::cooling_light(bool state_, float detuning_, float power_)
{
    if (state_ == 1)
    {
        // OPEN
        COOLING_TTL = 0;
        cycle_delay_us(MECH_DELAY_OPEN);
        MAX11300.single_ended_dac_write(AOM_1_ATTE_, to_dac(power_));
        // cycle_delay_ms(1);
        cycle_delay_us(AOM_DELAY_OPEN);
    }

    if (state_ == 0)
    {
        // CLOSE
        COOLING_TTL = 1;
        cycle_delay_us(MECH_DELAY_CLOSE);
        MAX11300.single_ended_dac_write(AOM_1_ATTE_, to_dac(0));
        // cycle_delay_ms(1);
        cycle_delay_us(AOM_DELAY_CLOSE);
    }

    return;
}


void COLDATOM::repump_light(bool state_, float detuning_, float power_)
{
    if (state_ == 1)
    {
        REPUMP_TTL = 0;
        cycle_delay_ms(MECH_DELAY_OPEN);
        MAX11300.single_ended_dac_write(AOM_3_FREQ_, to_dac(detuning_)),
            MAX11300.single_ended_dac_write(AOM_3_ATTE_, to_dac(power_));
        cycle_delay_us(AOM_DELAY_OPEN);
    }

    if (state_ == 0)
    {
        REPUMP_TTL = 0;
        MAX11300.single_ended_dac_write(AOM_3_ATTE_, to_dac(0));
        MAX11300.single_ended_dac_write(AOM_3_FREQ_, to_dac(0));
        cycle_delay_us(AOM_DELAY_CLOSE);
    }

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
                printf("DONE\n\r");
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
            printf("DONE\n\r");
            break;
        }
        ///////////////////////////////////////

        // Perform one clock cycle
        ///////////////////////////////////////
        case (MOT_CYCLE):
        {
            MOT_Temp();
            STATE = MOT_CYCLE;
            printf("DONE\n\r");
            break;
        }
        ///////////////////////////////////////

        // Perform Drop Test
        ///////////////////////////////////////
        case (DROP_TEST):
        {
            drop_test();
            STATE = USER_INPUT;
            printf("DONE\n\r");
            break;
        }
        ///////////////////////////////////////

        // Perform Drop Cycle
        ///////////////////////////////////////
        case (DROP_CYCLE):
        {
            int i = 30;
            while(i--){
                drop_test();
            }
            STATE = USER_INPUT;
            printf("DONE\n\r");
            break;
        }
        ///////////////////////////////////////
                
        // Perform DIAGNOSTIC
        ///////////////////////////////////////
        case (DIAGNOSTIC):
        {
            diagnostic();
            STATE = USER_INPUT;
            printf("DONE\n\r");
            break;
        }
        ///////////////////////////////////////

        // Perform DIAGNOSTIC_CYCLE
        ///////////////////////////////////////
        case (DIAGNOSTIC_CYCLE):
        {
            int i = 100;
            while(i--){
                diagnostic();
            }
            STATE = USER_INPUT;
            printf("DONE\n\r");
            break;
        }
        ///////////////////////////////////////

        // Perform EXPERIMENTAL
        ///////////////////////////////////////
        case (EXPERIMENTAL):
        {
            experimental();
            STATE = USER_INPUT;
            printf("DONE\n\r");
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
            printf("DONE\n\r");
            break;
        }
        ///////////////////////////////////////

        default:
            STATE = USER_INPUT;
            break;

    }

}