/*
 * File:   main.c
 * Author: roberto.mozzicato@gmail.com
 *
 * Created on 4/25/2020 2:30:04 PM UTC
 * "Created in MPLAB"
 */

#include <xc.h>

#define RF_DATA_INPUT   RA0
#define RF_DATA_OUTPUT  RA1

#ifndef _XTAL_FREQ
#define _XTAL_FREQ 8000000
#endif

#define THRESHOLD_NOISE                 5
#define THRESHOLD_CONSECUTIVE_CHANGES   30
#define THRESHOLD_SINGLE_CHANGE         100

// CONFIG1
#pragma config FOSC = INTOSC // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = ON // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config CPD = OFF // Data Memory Code Protection (Data memory code protection is disabled)
#pragma config BOREN = ON // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = OFF // Internal/External Switchover (Internal/External Switchover mode is enabled)
#pragma config FCMEN = ON // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)

// CONFIG2
#pragma config WRT = OFF // Flash Memory Self-Write Protection (Write protection off)
#pragma config PLLEN = OFF // PLL Enable (4x PLL enabled)
#pragma config STVREN = ON // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LVP = ON // Low-Voltage Programming Enable (Low-voltage programming enabled)


unsigned long currentValTime;
unsigned char lastVal;
unsigned char previousLongSignal;
unsigned char val;
unsigned char receiving;

/* Timer utilisation
Timer2  1ms interrupt timer
 */

/*
void Init1msTimer(void) {
    // we want 1mss period so
    // clock of 8Mhz give  125 ns   1ms / 125ns = 2000
    // 2000  prescaler of 16 = 125 start count
    T2CON = 0;
    PR2 = 20;
    TMR2 = 0;
    T2CON = 0b00000110; // Timer on prescaller 16
    // Enable IRQ
    TMR2IF = 0;
    currentValTime = 0;
    TMR2IE = 1;
    PEIE = 1;
    GIE = 1;
}

void __interrupt() isr(void) {
    // Timer 1 ms
    if (TMR2IF) {
        TMR2IF = 0;

        currentValTime++;
    }
}
*/
int main(void) {
    OSCCON = 0b01110011; // 8MHz  internal clock
    OPTION_REG = 0b10000011; // TMR0 @ Fosc/4/16 ( need to interrupt on every 80 clock 16*5)
    ANSELA = 0b00000; // NO Analog
    PORTA = 0b00000000;
    //WPUA		= 0b00111011;	// pull-up ON
    TRISA = 0b00000001; // ALL OUTPUT except RA0
    INTCON = 0b00000000; // no interrupt

    //  A/D & FVR OFF
    ADCON0 = 0;
    FVRCON = 0;

    currentValTime = 0;
    previousLongSignal = 0;
    lastVal = 0;
    receiving = 0;
    
    __delay_ms(100);

    while (1) {
        val = RF_DATA_INPUT;
        if(val == lastVal) {
            currentValTime++;
            if(currentValTime > THRESHOLD_SINGLE_CHANGE ||
               currentValTime > THRESHOLD_CONSECUTIVE_CHANGES && previousLongSignal) {
                receiving = 1;
            }
        } else {
            if(currentValTime < THRESHOLD_NOISE) {
                receiving = 0;
                RF_DATA_OUTPUT = 0;
            }
            previousLongSignal = currentValTime > THRESHOLD_CONSECUTIVE_CHANGES;
            currentValTime = 0;
            lastVal = val;
        }
        
        if (receiving)
            RF_DATA_OUTPUT = val;
        
    }
}



