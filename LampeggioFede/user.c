/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#include "user.h"

/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

/* <Initialize variables in user.h and insert code for user algorithms.> */

void InitApp(void)
{
#if defined(PIC12F683)
    // Registro OSCCON (opzioni oscillatore)
    OSCCONbits.SCS=1; // System Clock Select (1=INTERNAL)
    OSCCONbits.IRCF=0b111; // IRCF=111 -> 8MHz
#endif
    __delay_ms(100);
    GPIO=0;

    // Registro TRISIO (quali porte input e quali output)
    TRISIO=0b00010000; // i pin di input devono essere 1
    
#if defined(PIC12F683)    
    CMCON0bits.CM=0b111; // Comparatore disattivato
    CMCON1bits.T1GSS=0b1; // Comparatore disattivato
#else
    CMCONbits.CM=0b111; // Comparatore disattivato
#endif
    
    ANSEL=0; // Porte impostate tutte su digitale

    // Registro OPTION_REG
    OPTION_REG=0;
    OPTION_REGbits.nGPPU=0; // GPIO pull-ups are enabled by individual port latch values in WPU register

    // Registro INTCON
    INTCON=0;
    IOC=0; // Interrupt on change disabilitati
    
    // Registro WPU (weak pull-up)
    // Le configurazioni dei weak pull-up devono stare dopo quelle del TRISIO
    WPU=0b00000000; // weak pull-up abilitati sui pin di input

    GPIO=0;
    
    T1CON = 0x00;               //Initialize Timer Module
}
