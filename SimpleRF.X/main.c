/*
 * File:   main.c
 * Author: mozzicator
 *
 * Created on 1 giugno 2020, 23.03
 */


#include <xc.h>
//#include <12f683.cgen.inc>
#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

// 5-Channel RF remote control using PIC18F4550 transmitter CCS C code
// http://ccspicc.blogspot.com/
// electronnote@gmail.com

#pragma config FOSC = INTOSCIO  // Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA4/OSC2/CLKOUT pin, I/O function on RA5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // MCLR Pin Function Select bit (MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown Out Detect (BOR enabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal External Switchover mode is disabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)

#define _XTAL_FREQ 8000000
//#define __delay_ms(x) _delay((unsigned long)((x)*(_XTAL_FREQ/4000.0)))

//#define INPUT_SEND GP0
#define OUTPUT_SEND GP2
#define INPUT_RECEIVE GP1
#define OUTPUT_RECEIVE GP5

uint32_t remote_code;

bool receive_signal() {
    int8_t i;
    uint16_t count = 0;
    // Check 9ms pulse (remote control sends logic high)
    while (INPUT_RECEIVE && count < 100) {
        count++;
        __delay_ms(1);
    }
    if (count > 12 || count < 5)
        return false;
    
    count = 0;
    // Check 4.5ms space (remote control sends logic low)
    while (!INPUT_RECEIVE && (count < 10000)) {
        count++;
        __delay_us(100);
    }
    if ((count > 60) || (count < 22)) // NEC protocol?
        return false;
    
    // Read message (32 bits)
    uint8_t *byte=(uint8_t *)&remote_code;
    for(uint8_t j=0; j<sizeof(remote_code); j++, byte++) {
        for (i = 0; i < 8; i++) {
            count = 0;
            while (INPUT_RECEIVE && count < 20) {
                count++;
                __delay_us(50);
            }
            if (count > 15 || count < 7) // NEC protocol?
                return false;
            
            count = 0;
            while (!INPUT_RECEIVE && count < 50) {
                count++;
                __delay_us(50);
            }
            if (count > 40 || count < 7) // NEC protocol?
                return false;
            
            if (count > 20) // If space width > 1ms
                *byte |= (1 << i); // Write 1 to bit (31 - i)
            else // If space width < 1ms
                *byte &= ~(1 << i); // Write 0 to bit (31 - i)
        }
    }
    return true;
}

void send_signal(uint32_t number) {
    uint8_t i;
    // Send 9ms pulse
    OUTPUT_SEND = 1;
    __delay_ms(9);
    // Send 4.5ms space
    OUTPUT_SEND = 0;
    __delay_us(4500);
    // Send data
    uint8_t *byte=(uint8_t *)&number;
    for(uint8_t j=0; j<sizeof(number); j++, byte++) {
        for (i = 0; i < 8; i++) {
            // If bit is 1 send 560us pulse and 1680us space
            if (*byte & (1 << i)) {
                OUTPUT_SEND = 1;
                __delay_us(560);
                OUTPUT_SEND = 0;
                __delay_us(1680);
            }// If bit is 0 send 560us pulse and 560us space
            else {
                OUTPUT_SEND = 1;
                __delay_us(560);
                OUTPUT_SEND = 0;
                __delay_us(560);
            }
        }
    }
    // Send end bit
    OUTPUT_SEND = 1;
    __delay_us(560);
    OUTPUT_SEND = 0;
    __delay_us(560);
}

void InitApp(void) {
    OSCCONbits.IRCF = 0b111;
    // Registro TRISIO (quali porte input e quali output)
    // default: --11 1111
    // TRISIO=0b00001000; // singola impostazione, occupa meno spazio ma è meno chiaro
    TRISIObits.TRISIO0 = 0;
    TRISIObits.TRISIO1 = 1;
    TRISIObits.TRISIO2 = 0;
    TRISIObits.TRISIO3 = 1;
    TRISIObits.TRISIO4 = 0;
    TRISIObits.TRISIO5 = 0;

    SCS = 1;

    ANSEL = 0;
    CMCON0 = 0;

    // OPTION_REG ? OPTION REGISTER (ADDRESS: 81h)
    // default: 1111 1111
    // Note: To achieve a 1:1 prescaler assignment for TMR0, assign the prescaler to the WDT bysetting PSA bit to 1 (OPTION<3>).
    // nGPPU INTEDG T0CS T0SE PSA PS2 PS1 PS0
    OPTION_REG = 0b00000000; // singola impostazione, occupa meno spazio ma è meno chiaro

    // Registro INTCON
    INTCON = 0;

    // Registro WPU (weak pull-up)
    // Le configurazioni dei weak pull-up devono stare dopo quelle del TRISIO
    WPU = 0b0000000;

    INTCONbits.T0IF = 0;
}

void main() {
    InitApp();
    GPIO = 0;
    for(uint8_t i=0; i<3; i++) {
        OUTPUT_RECEIVE=1;
        OUTPUT_SEND=1;
        __delay_ms(500);
        OUTPUT_RECEIVE=0;
        OUTPUT_SEND=0;
        __delay_ms(500);
    }
    //send_signal(0x40BF00FF);
    while (true) {
        //while (!INPUT_SEND) {
            for (int i = 0; i < 1; i++) { // mettere 3!
                send_signal(0x40BF00FF);
                __delay_ms(10);
            }
            __delay_ms(1000);
        //}
        /*
        if(INPUT_RECEIVE) {
            if (receive_signal()) {
                if (remote_code == 0x40BF00FF) {
                    OUTPUT_RECEIVE=1;
                    __delay_ms(100);
                    OUTPUT_RECEIVE=0;
                }
            }
        }*/
    }
}
