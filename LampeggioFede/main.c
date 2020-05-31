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

//    OUTPUT_PORT = 0;
        
//    __delay_ms(2000);
    int a;
    
    while (1) {
//        OUTPUT_PORT = 1;
//        __delay_ms(500);
//        OUTPUT_PORT = 0;
//        __delay_ms(500);
        
        TMR1H = 0;                //Sets the Initial Value of Timer
        TMR1L = 0;                //Sets the Initial Value of Timer

        OUTPUT_TRIGGER = 1;                  //TRIGGER HIGH
        __delay_us(15);           //20uS Delay 
        OUTPUT_TRIGGER = 0;                  //TRIGGER LOW

        while(!INPUT_ECHO);              //Waiting for Echo
        TMR1ON = 1;               //Timer Starts
        while(INPUT_ECHO);               //Waiting for Echo goes LOW
        TMR1ON = 0;               //Timer Stops

        a = (TMR1L | (TMR1H<<8)); //Reads Timer Value
        //a = a/58.82;              //Converts Time to Distance
        a = a/47;              //Converts Time to Distance
        a = a + 1;                //Distance Calibration
        //if(a>=2 && a<=400) {       //Check whether the result is valid or not
        if(a < 7) {
            OUTPUT_RED=1;
            OUTPUT_GREEN=1;
            OUTPUT_BLUE=1;
        }
        else if(a < 15) {
            OUTPUT_RED=1;
            OUTPUT_GREEN=0;
            OUTPUT_BLUE=0;
        }
        else if(a < 20) {
            OUTPUT_RED=0;
            OUTPUT_GREEN=1;
            OUTPUT_BLUE=0;
        }
        else if(a < 30) {
            OUTPUT_RED=0;
            OUTPUT_GREEN=0;
            OUTPUT_BLUE=1;
        }
        else {
            OUTPUT_RED=0;
            OUTPUT_GREEN=0;
            OUTPUT_BLUE=0;            
        }
        
        //}
        
        __delay_ms(400);
    }
   

}

