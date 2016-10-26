/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
#include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
#include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/


/* i.e. uint8_t <variable_name>; */


/******************************************************************************/
/* Main Program                                                               */

/******************************************************************************/

void main(void) {
/* Configure the oscillator for the device */
    ConfigureOscillator();

    /* Initialize I/O and Peripherals for application */
    InitApp();

    __delay_ms(500);

    // IN DEBUG METTERE UN BREAKPOINT QUI E METTERE ALTA SW_ASCENSORE (GP0)
    OUTPUT_PORT=0;

    /*int a=1;
    while (1) {
        __delay_ms(1000);
        a = !a;
        OUTPUT_PORT=a;
    }*/

    int conta=0;
    long accensione=0;
    bool portaAperta=SW_PORTA;
    bool accensioneDaPorta=false;
    while (1) {
        // Gestione push su tasto ascensore
        conta=0;
        while(!SW_ASCENSORE && conta < DELAY_SW_ASCENSORE) {
            __delay_ms(DELAY_STEP);
            conta += DELAY_STEP;
        }

        if(conta >= DELAY_SW_ASCENSORE) {
            OUTPUT_PORT=1;
            accensione = DELAY_ACCENSIONE;
            while(!SW_ASCENSORE)
                __delay_ms(DELAY_STEP);
            __delay_ms(1000);
        }

        // Gestione porta
        if(portaAperta) {
            // Chiusura
            conta=0;
            while(!SW_PORTA && conta < DELAY_SW_PORTA) {
                __delay_ms(DELAY_STEP);
                conta += DELAY_STEP;
            }

            if(conta >= DELAY_SW_PORTA) {
                portaAperta=false;
                if(!accensioneDaPorta) {
                    if(accensione > 0) {
                        OUTPUT_PORT=0;
                        accensione = 0;
                    }
                    else {
                        OUTPUT_PORT=1;
                        accensione = DELAY_ACCENSIONE;
                    }
                }
            }
        }
        else {
            // Apertura
            conta=0;
            while(SW_PORTA && conta < DELAY_SW_PORTA) {
                __delay_ms(DELAY_STEP);
                conta += DELAY_STEP;
            }

            if(conta >= DELAY_SW_PORTA) {
                if (accensione == 0)
                    accensioneDaPorta = true;
                portaAperta = true;
                OUTPUT_PORT=1;
                accensione = DELAY_ACCENSIONE;
            }
        }

        if (accensione > 0) {
            __delay_ms(DELAY_STEP);
            accensione -= DELAY_STEP;
            if (accensione <= 0) {
                OUTPUT_PORT=0;
                accensione = 0;
                accensioneDaPorta = false;
            }
        }
    }

}

