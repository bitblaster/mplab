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

    long conta=0;
    long accensione=0;
    long ricircoloPrenotato=0;
    
    while (1) {
        if (accensione > 0) {
            conta=0;
            while(SW_PORTA && conta < SOGLIA_INTERRUZIONE_RICIRCOLO) {
                __delay_ms(DELAY_STEP);
                conta += DELAY_STEP;
            }

            if(conta >= SOGLIA_INTERRUZIONE_RICIRCOLO) {
                OUTPUT_PORT=0;
                accensione = 0;
            }
            else {
                __delay_ms(DELAY_STEP);
                accensione -= DELAY_STEP;
                if (accensione <= 0) {
                    OUTPUT_PORT=0;
                    accensione = 0;
                }
            }
        }
        else {
            // Gestione porta
            if(ricircoloPrenotato) {
                // Chiusura
                conta=0;
                while(!SW_PORTA && conta < SOGLIA_ATTIVAZIONE_RICIRCOLO) {
                    __delay_ms(DELAY_STEP);
                    conta += DELAY_STEP;
                }

                if(conta >= SOGLIA_ATTIVAZIONE_RICIRCOLO) {
                    accensione = ricircoloPrenotato;
                    OUTPUT_PORT=1;
                    ricircoloPrenotato = 0;
                }
            }
            else {
                // Apertura
                conta=0;
                // Contiamo fino alla soglia massima che ci interessa
                while(SW_PORTA && conta < SOGLIA_PRENOTAZIONE_LUNGA_RICIRCOLO) {
                    __delay_ms(DELAY_STEP);
                    conta += DELAY_STEP;
                }

                if(conta >= SOGLIA_PRENOTAZIONE_BREVE_RICIRCOLO) {
                    if(conta >= SOGLIA_PRENOTAZIONE_LUNGA_RICIRCOLO)
                        ricircoloPrenotato = DURATA_ACCENSIONE_LUNGA;
                    else
                        ricircoloPrenotato = DURATA_ACCENSIONE_BREVE;
                }
            }
        }
    }

}

