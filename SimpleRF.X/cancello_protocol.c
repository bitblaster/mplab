#include "user.h"
#include "cancello_protocol.h"

#define PREAMBLE_PULSE_LOW_LEN 9250
#define PULSE_HIGH_ONE 425
#define PULSE_LOW_ONE 1025
#define PULSE_HIGH_ZERO 865
#define PULSE_LOW_ZERO 585

static inline void sendBit0(void) {
    OUTPUT_SEND = 1;
    __delay_us(PULSE_HIGH_ZERO);
    OUTPUT_SEND = 0;
    __delay_us(PULSE_LOW_ZERO);
}

static inline void sendBit1(void) {
    OUTPUT_SEND = 1;
    __delay_us(PULSE_HIGH_ONE);
    OUTPUT_SEND = 0;
    __delay_us(PULSE_LOW_ONE);
}

static inline void sendPreamble() {
    OUTPUT_SEND = 0;
    __delay_us(PREAMBLE_PULSE_LOW_LEN);
    sendBit0();
    sendBit1();
    sendBit0();
    sendBit0();
}

void send_cancello(uint24_t code) {
    // PREAMBLE
    for(int i=0; i < 20; i++) {
        sendPreamble();

        for(uint8_t pos=0; pos < sizeof(code); pos++) {
            if(code & (1 << (sizeof(code) - pos)))
                sendBit1();
            else
                sendBit0();
        }
    }
}