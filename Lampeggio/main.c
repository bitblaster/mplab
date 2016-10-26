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

short attivo=0;

/******************************************************************************/
/* Main Program                                                               */

/******************************************************************************/

#include <stdio.h>
void main(void) {
printf ( "test ouput"); //hao add for testing fprintf


/* Configure the oscillator for the device */
    ConfigureOscillator();

    /* Initialize I/O and Peripherals for application */
    InitApp();
    
    while (1) {
       //CLRWDT();
        attivo=!attivo;
        OUTPUT_PORT=attivo;
        __delay_ms(1000);
    }



}

