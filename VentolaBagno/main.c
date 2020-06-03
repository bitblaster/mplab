#include <xc.h>         /* XC8 General Include File */
#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

// Configuration Bits for PIC12F675
#pragma config FOSC = INTRCIO   // Oscillator Selection bits (INTOSC oscillator: I/O function on GP4/OSC2/CLKOUT pin, I/O function on GP5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = ON       // Power-Up Timer Enable bit (PWRT enabled)
#pragma config MCLRE = OFF      // GP3/MCLR pin function select (GP3/MCLR pin function is digital I/O, MCLR internally tied to VDD)
#pragma config BOREN = ON       // Brown-out Detect Enable bit (BOD enabled)
#pragma config CP = OFF         // Code Protection bit (Program Memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)

// Frequency defines
#define SYS_FREQ        500000L
#define FCY             SYS_FREQ/4
#define _XTAL_FREQ 4000000
#define __delay_ms(x) _delay((unsigned long)((x)*(_XTAL_FREQ/4000.0)))

// Program defines
#define INPUT    GP0
#define OUTPUT   GP2

#define TIMEOUT  12000    // 10min

void InitApp(void) {
    // Registro TRISIO (quali porte input e quali output)
    // default: --11 1111
    // TRISIO=0b00001000; // singola impostazione, occupa meno spazio ma è meno chiaro
    TRISIObits.TRISIO0=1; // GP0 input
    TRISIObits.TRISIO1=0; // GP1 output
    TRISIObits.TRISIO2=0; // GP2 output
    TRISIObits.TRISIO3=1; // GP3 input
    TRISIObits.TRISIO4=0; // GP4 output
    TRISIObits.TRISIO5=0; // GP5 output

    CMCONbits.CM=0b111; // Comparatore disattivato
    ANSEL=0; // Porte impostate tutte su digitale

    // OPTION_REG ? OPTION REGISTER (ADDRESS: 81h)
    // default: 1111 1111
    // Note: To achieve a 1:1 prescaler assignment for TMR0, assign the prescaler to the WDT bysetting PSA bit to 1 (OPTION<3>).
    // nGPPU INTEDG T0CS T0SE PSA PS2 PS1 PS0
    //OPTION_REG=0b01010111; // singola impostazione, occupa meno spazio ma è meno chiaro
    OPTION_REGbits.nGPPU=0;
    OPTION_REGbits.T0CS=0;
    OPTION_REGbits.PSA=0;
    OPTION_REGbits.PS=0b111; // prescaler 1:256
    //OPTION_REGbits.PS2=1;
    //OPTION_REGbits.PS1=1;
    //OPTION_REGbits.PS0=0; 

    // Registro INTCON
    INTCONbits.GPIE=0;
    INTCONbits.T0IE=1;  
    
    IOC4=1; // Interrupt on change abilitato su GP4
    
    // Registro WPU (weak pull-up)
    // Le configurazioni dei weak pull-up devono stare dopo quelle del TRISIO
    WPUbits.WPU0=1;
    WPUbits.WPU1=0;
    WPUbits.WPU2=0;
    //WPUbits.WPU3=0;
    WPUbits.WPU4=0;
    WPUbits.WPU5=0;

    INTCONbits.T0IF=0;
}

void __interrupt () isr() {
    static bool active=false;
    static unsigned char debounce=0;
    static unsigned int countdown=0;

    if(INTCONbits.T0IF) {
        TMR0=61; // 1us*(256-61)*prescaler(256) ~= 50ms
        if(countdown > 0) {
            countdown--;
            if(countdown == 0) {
                active=false;
                OUTPUT=0;
            }
        }            
        
        if(INPUT)
            debounce=0;
        else {
            // 1 timer cycle debounce (50ms) is ok
            if(debounce == 0) {
                debounce=1;
            } else if(debounce == 1) {
                active=!active;
                OUTPUT=active;
                if(active)
                    countdown=TIMEOUT;
                else
                    countdown=0;
                debounce=2;
            }                
        }

        INTCONbits.T0IF=0;
    }
}

void main(void) {
    InitApp();
    GPIO=0;
    
    __delay_ms(200);

    INTCONbits.GIE=1;
    
    while (1) {        
    }
}
