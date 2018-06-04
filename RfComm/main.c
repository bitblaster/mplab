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
#include "user.h"
//#include "virtualwire.h"          /* User funct/params, such as InitApp */
#include "tea_encryption.h"          /* User funct/params, such as InitApp */

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/


/* i.e. uint8_t <variable_name>; */


/******************************************************************************/
/* Main Program                                                               */

/******************************************************************************/
//uint8_t buffer[VW_MAX_MESSAGE_LEN * 2];
uint32_t v[2];
    
void main(void) {
/* Configure the oscillator for the device */
    ConfigureOscillator();

    /* Initialize I/O and Peripherals for application */
    InitApp();

    v[0]=0xB9F4AC87;
    v[1]=0x8F24C2AB;
    encrypt(v, 2);
    decrypt(v, 2);
    //vw_send("ciao", 4);
    
    //vw_recv(buffer, 4);
    uint8_t conta;
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

