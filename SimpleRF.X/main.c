/*
 * File:   main.c
 * Author: mozzicator
 *
 * Created on 1 giugno 2020, 23.03
 */
#ifdef _12F683
    #pragma config FOSC = INTOSCIO // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
    #pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
    #pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT disabled)
    #pragma config MCLRE = OFF      // MCLR Pin Function Select bit (MCLR pin function is digital input, MCLR internally tied to VDD)
    #pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
    #pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
    #pragma config BOREN = ON       // Brown Out Detect (BOR enabled)
    #pragma config IESO = OFF       // Internal External Switchover bit (Internal External Switchover mode is disabled)
    #pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)
#endif

#ifdef _12F1840
    // CONFIG1
    #pragma config FOSC = INTOSC    // Oscillator Selection->INTOSC oscillator: I/O function on CLKIN pin
    #pragma config WDTE = OFF    // Watchdog Timer Enable->WDT disabled
    #pragma config PWRTE = OFF    // Power-up Timer Enable->PWRT disabled
    #pragma config MCLRE = ON    // MCLR Pin Function Select->MCLR/VPP pin function is MCLR
    #pragma config CP = OFF    // Flash Program Memory Code Protection->Program memory code protection is disabled
    #pragma config CPD = OFF    // Data Memory Code Protection->Data memory code protection is disabled
    #pragma config BOREN = ON    // Brown-out Reset Enable->Brown-out Reset enabled
    #pragma config CLKOUTEN = OFF    // Clock Out Enable->CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin
    #pragma config IESO = OFF    // Internal/External Switchover->Internal/External Switchover mode is enabled
    #pragma config FCMEN = ON    // Fail-Safe Clock Monitor Enable->Fail-Safe Clock Monitor is enabled

    // CONFIG2
    #pragma config WRT = OFF    // Flash Memory Self-Write Protection->Write protection off
    #pragma config PLLEN = OFF    // PLL Enable->4x PLL disabled
    #pragma config STVREN = ON    // Stack Overflow/Underflow Reset Enable->Stack Overflow or Underflow will cause a Reset
    #pragma config BORV = LO    // Brown-out Reset Voltage Selection->Brown-out Reset Voltage (Vbor), low trip point selected.
    #pragma config LVP = OFF    // Low-Voltage Programming Enable->Low-voltage programming enabled
#endif

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */
#include "user.h"
#include "g8_protocol.h"
#include "nec_protocol.h"
#include "cancello_protocol.h"

nec_message msgToSend = {0x23, 0x5B}; // numeri a caso

/*
uint8_t receive_byte_nec() {
    uint8_t ret = 0;
    uint8_t count = 0;
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

message receive_message_nec() {
    message ret = {0, 0};
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

    ret.address = receive_byte();
    if (ret.address != ~receive_byte()) {
        ret.address = 0;
        return ret;
    }

    ret.command = receive_byte();
    if (ret.command != ~receive_byte()) {
        ret.address = 0;
        return ret;
    }

    return ret;
}
*/

void InitApp(void) {
#ifndef TEST
    
    OSCILLATOR_SET_XTAL_FREQ(_XTAL_FREQ);
            
    ANALOG_DISABLE();
    GPIO_SET_ALL_PIN_VALUES_AS_MASK(0);
    GPIO_SET_ALL_PIN_MODES_AS_MASK(0);
    GPIO_SET_PIN_MODE(INPUT_RECEIVE_PIN, 1);

    IOC_SET_FLAG(INPUT_RECEIVE_PIN, 0);
    IOC_ON_BOTH_EDGES(INPUT_RECEIVE_PIN);

    IOC_ENABLE();

    TIMER1_CONFIGURE_WITH_INTERNAL_OSCILLATOR();
    
#if _XTAL_FREQ == 8000000
    TIMER1_SET_PRESCALER2();
#elif _XTAL_FREQ == 500000
    TIMER1_SET_PRESCALER1();
#endif

    //TIMER1_INTERRUPT_ENABLE();
    TIMER1_ENABLE();
    
    WEAK_PULLUPS_GLOBAL_ENABLE(0);
    WEAK_PULLUPS_SET_ALL_AS_MASK(0);

#endif
}

void __interrupt() isr() {
#ifndef TEST
    // L'interrupt non è necessario in quanto ci serve solo che timer1 conti e 
    // sapere se va in overflow
    /*static uint8_t timingValid=false;
    if(TIMER1_FLAG) {
        timingValid = false;
        TIMER1_FLAG=0;
    }*/
    
    if(IOC_FLAG) {
        //if(timingValid) {
        if(!TIMER1_FLAG) {
#if _XTAL_FREQ == 8000000    
            uint24_t duration = TMR1;
#elif _XTAL_FREQ == 500000
            uint24_t duration = TMR1<<3;
#endif
            TMR1=0;
            uint8_t rising = INPUT_RECEIVE;
            decode_g8(rising, duration);
            decode_nec(rising, duration);
        }
        
        //timingValid = true;
        TIMER1_FLAG = 0;
        IOC_FLAG = 0;
    }
#endif
}

void received_g8_message(const g8_message *msg) {
    if (msg->system == 0xB740B4 && msg->address == 0x83 && (msg->command == 0x11 || msg->command == 0x1E || msg->command == 0x33 || msg->command == 0x3C)) {
        OUTPUT_LED = 1;
        __delay_ms(1000);
        OUTPUT_LED = 0;
        send_cancello(0x130BDA);
    } else {
        OUTPUT_LED = 1;
        __delay_ms(300);
        OUTPUT_LED = 0;
        __delay_ms(300);
        OUTPUT_LED = 1;
        __delay_ms(300);
        OUTPUT_LED = 0;
    }
}

void received_nec_message(const nec_message *msg) {
    if (msg->address == msgToSend.address && msg->command == msgToSend.command) {
        OUTPUT_LED = 1;
        __delay_ms(2000);
        OUTPUT_LED = 0;
    } else {
        OUTPUT_LED = 1;
        __delay_ms(800);
        OUTPUT_LED = 0;
        __delay_ms(800);
        OUTPUT_LED = 1;
        __delay_ms(800);
        OUTPUT_LED = 0;
    }
}

#ifdef TEST
void decode1_g8() {
    decode_g8(0, 650);decode_g8(1, 430);
}

void decode0_g8() {
    decode_g8(0, 300);decode_g8(1, 780);
}

void decode1_nec() {
    decode_nec(0, 560);decode_nec(1, 1680);
}

void decode0_nec() {
    decode_nec(0, 560);decode_nec(1, 560);
}
#endif

void main() {
    InitApp();

#ifdef TEST
    // Messaggio in protocollo G8: B7 40 B4 81 33
    decode_g8(1, 1);decode_g8(0, 4770);decode_g8(1, 1550);
    decode1_g8();decode0_g8();decode1_g8();decode1_g8();decode0_g8();decode1_g8();decode1_g8();decode1_g8();
    decode0_g8();decode1_g8();decode0_g8();decode0_g8();decode0_g8();decode0_g8();decode0_g8();decode0_g8();
    decode1_g8();decode0_g8();decode1_g8();decode1_g8();decode0_g8();decode1_g8();decode0_g8();decode0_g8();
    decode1_g8();decode0_g8();decode0_g8();decode0_g8();decode0_g8();decode0_g8();decode1_g8();decode1_g8();
    decode0_g8();decode0_g8();decode1_g8();decode1_g8();decode0_g8();decode0_g8();decode1_g8();decode1_g8();

    // Messaggio in protocollo NEC: 23 5B
    decode_nec(1, 1);decode_nec(0, 9000);decode_nec(1, 4500);  
    decode1_nec();decode1_nec();decode0_nec();decode0_nec();decode0_nec();decode1_nec();decode0_nec();decode0_nec();
    decode0_nec();decode0_nec();decode1_nec();decode1_nec();decode1_nec();decode0_nec();decode1_nec();decode1_nec();
    decode1_nec();decode1_nec();decode0_nec();decode1_nec();decode1_nec();decode0_nec();decode1_nec();decode0_nec();
    decode0_nec();decode0_nec();decode1_nec();decode0_nec();decode0_nec();decode1_nec();decode0_nec();decode1_nec();
    decode0_nec();
#endif
    
    for (uint8_t i = 0; i < 3; i++) {
        OUTPUT_LED = 1;
        __delay_ms(200);
        OUTPUT_LED = 0;
        __delay_ms(200);
    }

//#define INVIO
#define RICEZIONE
    
#ifdef RICEZIONE
    INTCONbits.GIE=1;
#endif

    while (true) {
        // TODO provare a inserire qui il ciclo di ricezione andando in polling sul pin di input anziché con l'interrupt
        // TODO a 500kHz col 12F683 non va (col 12F1840 sì))
#ifdef INVIO
        send_message_nec(&msgToSend);
        __delay_ms(5000);
#endif        
        //while (!INPUT_SEND) {
            //
        //}
    }
}

