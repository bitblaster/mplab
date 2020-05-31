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
#define INPUT_BUZZER GP3
#define OUTPUT1     GP0
#define OUTPUT2     GP1
#define OUTPUT3     GP2

#define LED_OFF GPIO=0b00000000
#define LED_ON GPIO=0b00000111

#define THRESHOLD_SELECTION_BEEP            2    // 50ms (selection beep is 100ms)
#define THRESHOLD_RESET_START_BEEP          10  // 250ms (reset beep is 500ms)
#define THRESHOLD_FINISH_BEEP               100 // 2.5s (finish beep is ~5s)
#define THRESHOLD_SILENCE_BEFORE_START      60  // 1s
#define THRESHOLD_MAX_COUNTERS              200 // 5s (limit of usefulness for counters)

#define WASHING_STATE_RESET 0
#define WASHING_STATE_STARTED 1
#define WASHING_STATE_FINISHED 2

// Global variables
unsigned char washingState=WASHING_STATE_RESET;

void InitApp(void) {
    // Registro TRISIO (quali porte input e quali output)
    // default: --11 1111
    // TRISIO=0b00001000; // singola impostazione, occupa meno spazio ma è meno chiaro
    TRISIObits.TRISIO0=0; // GP0 output
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
    //OPTION_REG=0b11010110; // singola impostazione, occupa meno spazio ma è meno chiaro
    OPTION_REGbits.nGPPU=1;
    OPTION_REGbits.T0CS=0;
    OPTION_REGbits.PSA=0;
    OPTION_REGbits.PS=0b110; // prescaler 1:128
    //OPTION_REGbits.PS2=1;
    //OPTION_REGbits.PS1=1;
    //OPTION_REGbits.PS0=0; 

    // Registro INTCON
    INTCONbits.GIE=1;
    INTCONbits.GPIE=1;
    INTCONbits.T0IE=1;  
    
    IOC3=1; // Interrupt on change abilitato su GP3
    
    // Registro WPU (weak pull-up)
    // Le configurazioni dei weak pull-up devono stare dopo quelle del TRISIO
    WPU=0b00000000; // weak pull-up disabilitati 

    INTCONbits.T0IF=0;
}

void __interrupt () isr() {
    static bool beeperSignal=true; //false
    static unsigned char beeperCounter=0; // counter (in blocks of 25ms)
    static unsigned char silenceCounter=0; // counter (in blocks of 25ms)
    static unsigned char lastSilenceCounter=0;
    static unsigned char countReset=0; // counter (in blocks of 25ms)

    if(INTCONbits.T0IF) {
        TMR0=61; // 1us*(256-195)* prescaler(128) ~= 25ms
        if(beeperSignal) {
            if(beeperCounter < THRESHOLD_MAX_COUNTERS)
                beeperCounter++;
            beeperSignal=false;
            if(silenceCounter > 0) {
                lastSilenceCounter=silenceCounter;
                silenceCounter=0;
            }
        }
        else {
            if(silenceCounter < THRESHOLD_MAX_COUNTERS)
                silenceCounter++;
    
            if(beeperCounter > THRESHOLD_FINISH_BEEP) {
                washingState=WASHING_STATE_FINISHED;
                LED_ON;
            }
            else if(beeperCounter > THRESHOLD_RESET_START_BEEP) {
                if(lastSilenceCounter > THRESHOLD_SILENCE_BEFORE_START) {
                    countReset=1;
                    washingState=WASHING_STATE_STARTED;
                    LED_OFF;
                }
                else {
                    countReset++;
                    if(countReset >= 3) {
                        countReset=0;
                        washingState=WASHING_STATE_RESET;
                        LED_OFF;
                    }
                }
            }
            else if(beeperCounter > THRESHOLD_SELECTION_BEEP) {
                if(washingState != WASHING_STATE_STARTED) {
                    countReset=0;
                    washingState=WASHING_STATE_RESET;
                    LED_OFF;
                }
            }
            beeperCounter=0;
        }            
        
        INTCONbits.T0IF=0;
    }
    
    if(INTCONbits.GPIF) {
        beeperSignal=true;
        INTCONbits.GPIF=0;
    }
}

void main(void) {
    static __eeprom unsigned char savedWashingState=WASHING_STATE_RESET;
    unsigned char lastWashingState=WASHING_STATE_RESET;

    InitApp();
    
    GPIO=0;
    // Restore state from eeprom
    washingState = lastWashingState = savedWashingState;
    
    if(washingState == WASHING_STATE_FINISHED)
        LED_ON;
    else
        LED_OFF;
    
    while (1) {        
        switch(washingState) {
            case WASHING_STATE_RESET:
                LED_ON;
                __delay_ms(300);
                LED_OFF;
                __delay_ms(300);
                break;
            case WASHING_STATE_STARTED:
                LED_ON;
                __delay_ms(200);
                LED_OFF;
                __delay_ms(2000);
                break;
            case WASHING_STATE_FINISHED:
                if(washingState != lastWashingState)
                    LED_ON;
        }
        
        if(washingState != lastWashingState) {
            savedWashingState = washingState;
            lastWashingState = washingState;
        }
    }
}
