/*
 * File:   main.c
 * Author: mozzicator
 *
 * Created on 1 giugno 2020, 23.03
 */
#ifdef _12F683
    #pragma config FOSC = INTOSCIO // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
#endif

#ifdef _12F1840
    #pragma config FOSC = INTOSC // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
#endif

#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // MCLR Pin Function Select bit (MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown Out Detect (BOR enabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal External Switchover mode is disabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)

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

#ifdef _12F683
    OSCCONbits.IRCF = 0b111; // 111=8MHz
        //TEST!!!!
    //OSCCONbits.IRCF = 0b100; // 111=8MHz
    //TEST!!!!
    GPIO = 0;
    ANSEL = 0;
    CMCON0bits.CM = 0b111; // Comparator disabled
    
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

    // OPTION_REG ? OPTION REGISTER (ADDRESS: 81h)
    // default: 1111 1111
    // Note: To achieve a 1:1 prescaler assignment for TMR0, assign the prescaler to the WDT bysetting PSA bit to 1 (OPTION<3>).
    // nGPPU INTEDG T0CS T0SE PSA PS2 PS1 PS0
    //OPTION_REG = 0b00000000; // singola impostazione, occupa meno spazio ma è meno chiaro
    OPTION_REGbits.nGPPU = 1;
    OPTION_REGbits.T0CS = 0;
    OPTION_REGbits.T0SE = 0;
    OPTION_REGbits.PSA = 0;
    //OPTION_REGbits.PS = 0b010; // prescaler 1:8
    //OPTION_REGbits.PS = 0b001; // prescaler 1:4
    OPTION_REGbits.PS2=0;
    OPTION_REGbits.PS1=1;
    OPTION_REGbits.PS0=0;
        
    // Registro INTCON
    INTCONbits.GPIE=1;
    INTCONbits.T0IE=0;
    INTCONbits.T0IF=0;
    INTCONbits.PEIE=1;    
    
    // Timer1 Registers: 
    // Prescaler=1:2; TMR1 Preset=0; Freq=15.25879Hz; Period=65.536 ms
    T1CONbits.T1CKPS = 0b01;// bits 5-4  Prescaler Rate Select bits 
    //TEST!!!!
    //T1CONbits.T1CKPS = 0b00;// bits 5-4  Prescaler Rate Select bits 
    //TEST!!!!
    T1CONbits.T1OSCEN = 0;// bit 3 Timer1 Oscillator Enable Control: bit 0=off
    T1CONbits.nT1SYNC = 1;// bit 2 Timer1 External Clock Input Synchronization Control bit: 1=Do not synchronize external clock input
    T1CONbits.TMR1CS  = 0;// bit 1 Timer1 Clock Source Select bit: 0=Internal clock (FOSC/4) / 1 = External clock from pin T1CKI (on the rising edge)
    T1CONbits.TMR1ON  = 1;// bit 0 enables timer
    TMR1 = 0;     // preset for timer1 register
    
    PIE1bits.TMR1IE=1;
    
    IOC1=1; // Interrupt on change abilitato su GP4

    // Registro WPU (weak pull-up)
    // Le configurazioni dei weak pull-up devono stare dopo quelle del TRISIO
    WPU = 0b0000000;
#elif defined(_12F1840) || defined(_12F1822)
    OSCCONbits.IRCF = 0b1110; // 8MHz  internal clock
    PORTA=0;
    ANSELA = 0; // NO Analog
    TRISIObits.TRISIO0 = 0;
    TRISIObits.TRISIO1 = 1;
    TRISIObits.TRISIO2 = 0;
    TRISIObits.TRISIO3 = 1;
    TRISIObits.TRISIO4 = 0;
    TRISIObits.TRISIO5 = 0;
    OPTION_REG = 0b10000011; // TMR0 @ Fosc/4/16 ( need to interrupt on every 80 clock 16*5)
    
    //WPUA		= 0b00111011;	// pull-up ON
    TRISA = 0b00000001; // ALL OUTPUT except RA0
    INTCON = 0b00000000; // no interrupt

    //  A/D & FVR OFF
    ADCON0 = 0;
    FVRCON = 0;
#endif

#endif
}

void __interrupt() isr() {
#ifndef TEST
    static uint8_t timingValid=false;
    if(PIR1bits.TMR1IF) {
        /* Timer di 1 sec come test
        TMR1 = 15536;
        conta++;
        if(conta>=20) {
            OUTPUT_LED=!OUTPUT_LED;
            conta=0;
         }*/
        //OUTPUT_LED=!OUTPUT_LED;
        timingValid = false;
        PIR1bits.TMR1IF=0;
    }
    
    if(INTCONbits.GPIF) {
        if(timingValid) {
            uint16_t duration = TMR1;
            TMR1=0;
            uint8_t rising = INPUT_RECEIVE;
            //if((duration) > 1000)
            //    OUTPUT_LED=!OUTPUT_LED;
            decode_g8(rising, duration);
            decode_nec(rising, duration);
            /*if(duration > 8600 && duration < 9400) {
            //if(duration > 1075 && duration < 1175) {
                OUTPUT_LED=!OUTPUT_LED;
            }*/
        }
        
        timingValid = true;
        INTCONbits.GPIF=0;
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
#ifdef INVIO
        send_message_nec(&msgToSend);
        __delay_ms(5000);
#endif        
        //while (!INPUT_SEND) {
            //
        //}
    }
}

