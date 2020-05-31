/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
#include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
#include <htc.h>        /* HiTech General Include File */
#endif

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/


/* i.e. uint8_t <variable_name>; */


/******************************************************************************/
/* Main Program                                                               */

/******************************************************************************/

inline void bit0(void) {
    OUTPUT_PORT=1;
    __delay_us(900); // 865 ma il pic è + lento
    OUTPUT_PORT=0;
    __delay_us(585); // 585 
}

inline void bit1(void) {
    OUTPUT_PORT=1;
    __delay_us(460); // 425
    OUTPUT_PORT=0;
    __delay_us(1035); //     1025
}

void main(void) {
/* Configure the oscillator for the device */
    ConfigureOscillator();

    /* Initialize I/O and Peripherals for application */
    InitApp();

    // 0100000100110000101111011010
    while(1) {
        bit0();
        bit1();
        bit0();
        bit0();
        bit0();
        bit0();
        bit0();
        bit1();
        bit0();
        bit0();
        bit1();
        bit1();
        bit0();
        bit0();
        bit0();
        bit0();
        bit1();
        bit0();
        bit1();
        bit1();
        bit1();
        bit1();
        bit0();
        bit1();
        bit1();
        bit0();
        bit1();
        bit0();

        // PREAMBLE
        OUTPUT_PORT=0;
        __delay_us(9250);
    }
}

