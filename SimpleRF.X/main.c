/*
 * File:   main.c
 * Author: mozzicator
 *
 * Created on 1 giugno 2020, 23.03
 */

#include <xc.h>
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

//#define INPUT_SEND GP0
#define OUTPUT_SEND GP2
#define INPUT_RECEIVE GP1
#define OUTPUT_RECEIVE GP5

typedef struct _message {
    uint8_t address;
    uint8_t command;
} message;

uint8_t receive_byte() {
    uint8_t ret=0;
    uint8_t count=0;
    for (uint8_t i = 0; i < 8; i++) {
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
            ret |= (1 << i); // Write 1 to bit (31 - i)
        else // If space width < 1ms
            ret &= ~(1 << i); // Write 0 to bit (31 - i)
    }

    return ret;
}

message receive_message() {
    message ret = { 0, 0 };
    uint16_t count = 0;

    // Check 9ms pulse (remote control sends logic high)
    while (INPUT_RECEIVE && count < 100) {
        count++;
        __delay_ms(1);
    }
    if (count > 12 || count < 5)
        return ret;
    
    count = 0;
    // Check 4.q5ms space (remote control sends logic low)
    while (!INPUT_RECEIVE && (count < 10000)) {
        count++;
        __delay_us(100);
    }
    if ((count > 60) || (count < 22)) // NEC protocol?
        return ret;
    
    // Read message (32 bits)

    ret.address=receive_byte();
    if(ret.address != ~receive_byte()) {
        ret.address = 0;
        return ret;
    }

    ret.command=receive_byte();
    if(ret.command != ~receive_byte()) {
        ret.address = 0;
        return ret;
    }
    
    return ret;
}

void send_byte(uint8_t byte) {
    for (uint8_t i = 0; i < 8; i++) {
        // If bit is 1 send 560us pulse and 1680us space
        if (byte & (1 << i)) {
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

void send_message(message msg) {
    uint8_t i;
    // Send 9ms pulse
    OUTPUT_SEND = 1;
    __delay_ms(9);
    // Send 4.5ms space
    OUTPUT_SEND = 0;
    __delay_us(4500);
    
    // Send data
    send_byte(msg.address);
    send_byte(~msg.address);
    send_byte(msg.command);
    send_byte(~msg.command);
    
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
    
    message msgToSend = {.address=0x23, .command=0x58}; // numeri a caso
    
    while (true) {
        //while (!INPUT_SEND) {
            for (int i = 0; i < 1; i++) { // mettere 3!
                send_message(msgToSend);
                __delay_ms(10);
            }
            __delay_ms(1000);
        //}
        
        if(INPUT_RECEIVE) {
            message recv = receive_message();
            if (recv.address == msgToSend.address && recv.command == msgToSend.command) {
                OUTPUT_RECEIVE=1;
                __delay_ms(100);
                OUTPUT_RECEIVE=0;
            }
        }
    }
}
