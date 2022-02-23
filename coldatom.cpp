#include "cycle_count_delay.h"
#include "coldatom.h"
#include "mbed.h"
#include "serial.h"
#include "Drivers/MAX11300/max11300.h"

void coldatom_pins(){
    /*
    Pin Assignment
    1. COM Buses
    2. Digital I/O
    3. Analog I/O
    */

    // Buses
    SPI MAX11300_SPI(PC_12, PC_11, PC_10); // MOSI, MISO, SCLK, CS
    MAX11300 MAX11300(MAX11300_SPI, PD_2, NC, NC);
    //SPI DDS_SPI(PC_12, PC_11, PC_10, PD_2); // MOSI, MISO, SCLK, CS

    // Digital I/O
    DigitalOut COOLING_SHUTTER_TTL(PD_0);
    DigitalOut REPUMP_SHUTTER_TTL(PD_1);
    DigitalOut MOT_COIL_TTL(PD_3);
    DigitalOut CMOS_TTL(PD_4);

    // Analog Output
    MAX11300::MAX11300_Ports AOM_1_FREQ;
    AOM_1_FREQ = MAX11300::PORT10;
    MAX11300.single_ended_dac_write(AOM_1_FREQ, 0);

    // // Analog Input
    // PD{MAX11300::PORT0};

}


void coldatom_init()
{
    /*
    Set all the pins to their resepctive initial values
    Then need to also precompute any ramps that will be performed
    */    

    printf("Initialising...\n\r");

    // Initial Values
    // MAX11300 single_ended_dac_write(MAX11300::PORT0, 0);

    // Run the precompute function to calculate ramps
    coldatom_precomp();

    return;
}


void coldatom_precomp()
{
    /*
    Precompute all the ramps required to run experiment
    */    

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


void coldatom_MOT_temp()
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