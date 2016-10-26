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

    __delay_ms(100);

    // IN DEBUG METTERE UN BREAKPOINT QUI E METTERE ALTA SW_ASCENSORE (GP0)
    bool acceso=getOutputPortValue();
    if(!acceso) {
        setOutputPortValue(true);

        unsigned long conta=0;
        while(conta < DELAY_ACCENSIONE) {
            __delay_ms(DELAY_STEP);
            conta += DELAY_STEP;
        }
    }

    setOutputPortValue(false);

    __delay_ms(100);

    SLEEP();

    NOP();
}

