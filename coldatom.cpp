#include "cycle_count_delay.h"
#include "coldatom.h"
#include "mbed.h"
#include "serial.h"
#include <string>


void coldatom_init()
{
    printf("Initialising\n\r");
    return;
}


/* Various states */
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
            STATE = STATE_B;
            break;
        }
        ///////////////////////////////////////

        // Perform MOT temperature measurement
        ///////////////////////////////////////
        case (STATE_B):
        {
            printf("STATE_B\n\r");
            cycle_delay_ms(1000);
            STATE = STATE_C;
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