#include "cycle_count_delay.h"
#include "coldatom.h"
#include "mbed.h"
#include "serial.h"

//BufferedSerial pc(CONSOLE_TX, CONSOLE_RX);

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
        case (STATE_A):
        {
            printf("STATE_A\n\r");
            cycle_delay_ms(2000);
            STATE = STATE_B;
            break;
        }

        case (STATE_B):
        {
            printf("STATE_B\n\r");
            cycle_delay_ms(2000);
            STATE = STATE_C;
            break;
        }

        case (STATE_C):
        {
            printf("STATE_C\n\r");
            cycle_delay_ms(2000);
            STATE = STATE_A;
            break;
        }

        default:
            STATE = STATE_A;
            break;
    }

}