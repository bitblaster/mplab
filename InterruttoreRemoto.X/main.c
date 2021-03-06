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
    restoreOutputValue();

//    SLEEP();
    int conta=0;
    while (1) {
        conta=0;
        while(REMOTE_PORT && conta < DELAY_REMOTE_ON) {
            __delay_ms(DELAY_STEP);
            conta += DELAY_STEP;
        }

        if(conta >= DELAY_REMOTE_ON) {
            setOutputPortValue(1);
            while(REMOTE_PORT)
                __delay_ms(DELAY_STEP);
            __delay_ms(1000);
        }
        else if(conta > 0) {
            setOutputPortValue(0);
            while(REMOTE_PORT)
                __delay_ms(DELAY_STEP);
            __delay_ms(1000);
        }

        conta=0;
        while(!LOCAL_PORT && conta < DELAY_LOCAL_TOGGLE) {
            __delay_ms(DELAY_STEP);
            conta += DELAY_STEP;
        }

        if(conta >= DELAY_LOCAL_TOGGLE) {
            setOutputPortValue(!getOutputPortValue());
            while(!LOCAL_PORT)
                __delay_ms(DELAY_STEP);
            __delay_ms(1000);
        }

        /*if(REMOTE_PORT) {
            __delay_ms(DELAY_REMOTE_OFF);
            if (REMOTE_PORT) {
                __delay_ms(DELAY_REMOTE_ON);
                if (REMOTE_PORT)
                    setOutputPortValue(1);
                else
                    setOutputPortValue(0);
            }
            while(REMOTE_PORT)
                __delay_ms(10);
            __delay_ms(80);
            while(REMOTE_PORT)
                __delay_ms(10);
        }
        else if (!LOCAL_PORT) {
            __delay_ms(DELAY_LOCAL_TOGGLE);
            if (!LOCAL_PORT) {
                setOutputPortValue(!getOutputPortValue());
            }
            while(!LOCAL_PORT)
                __delay_ms(10);
            __delay_ms(80);
            while(!LOCAL_PORT)
                __delay_ms(10);
        }*/
    }

}

