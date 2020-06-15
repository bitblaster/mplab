#include "nec_protocol.h"

typedef enum _nec_status {
    STATUS_UNKNOWN, STATUS_PREAMBLE_START, STATUS_PREAMBLE_END, STATUS_PULSE_START, STATUS_PULSE_END
} nec_status;

#define LENGTH(x, len, tolerance) (x<=len+tolerance && x>=len-tolerance)

#define PREAMBLE_PULSE_HIGH_LEN 9000
#define PREAMBLE_PULSE_LOW_LEN 4500
#define PULSE_HIGH 560
#define PULSE_LOW_ONE 1680
#define PULSE_LOW_ZERO 560
#define TARGET_BIT_COUNT 32

static nec_message necMessage = {0, 0};
static nec_status status = STATUS_UNKNOWN;
static uint8_t bitCount = 0;
static uint8_t curByte = 0;

static inline void reset() {
    status = STATUS_UNKNOWN;
    bitCount = 0;
    necMessage = (nec_message){0, 0};
    curByte = 0;
}

static void gotBit(const int value) {
    curByte = (curByte >> 1);
    curByte |= (value << 7);

    bitCount++;

    if (bitCount % 8 == 0) {
        if(bitCount == 8)
            necMessage.address=curByte;
        else if(bitCount == 16 && (curByte ^ necMessage.address) != 0xFF)
            reset();
        else if(bitCount == 24)
            necMessage.command=curByte;
        else if(bitCount == 32 && (curByte ^ necMessage.command) != 0xFF)
            reset();
                
        curByte = 0;
    }

    if (bitCount >= TARGET_BIT_COUNT) {
        received_nec_message(&necMessage);
        reset();
    }
}

void decode_nec(const uint8_t risingEdge, const uint24_t duration) {

    switch (status) {
        case STATUS_UNKNOWN:
            if (risingEdge) {
                if (LENGTH(duration, PREAMBLE_PULSE_LOW_LEN, 300))
                    status = STATUS_PULSE_START;
                else
                    status = STATUS_PREAMBLE_START;
            } else if (LENGTH(duration, PREAMBLE_PULSE_HIGH_LEN, 400))
                status = STATUS_PREAMBLE_END;
            break;

        case STATUS_PREAMBLE_START:
            if (!risingEdge && LENGTH(duration, PREAMBLE_PULSE_HIGH_LEN, 400))
                status = STATUS_PREAMBLE_END;
            else
                reset();
            break;

        case STATUS_PREAMBLE_END:
            if (risingEdge && LENGTH(duration, PREAMBLE_PULSE_LOW_LEN, 300))
                status = STATUS_PULSE_START;
            break;

        case STATUS_PULSE_START:
            if (!risingEdge && LENGTH(duration, PULSE_HIGH, 200))
                status = STATUS_PULSE_END;
            else
                reset();

            break;

        case STATUS_PULSE_END:
            if (risingEdge) {
                if(LENGTH(duration, PULSE_LOW_ONE, 300)) {
                    status = STATUS_PULSE_START;
                    gotBit(1);
                }
                else if (LENGTH(duration, PULSE_LOW_ZERO, 200)) {
                    status = STATUS_PULSE_START;
                    gotBit(0);
                }
                else
                    reset();
            } else
                reset();
            break;
    }
}

static void send_byte_nec(uint8_t byte) {
    for (uint8_t i = 0; i < 8; i++) {
        // If bit is 1 send 560us pulse and 1680us space
        if (byte & (1 << i)) {
            OUTPUT_SEND = 1;
            __delay_us(PULSE_HIGH);
            OUTPUT_SEND = 0;
            __delay_us(PULSE_LOW_ONE);
        }// If bit is 0 send 560us pulse and 560us space
        else {
            OUTPUT_SEND = 1;
            __delay_us(PULSE_HIGH);
            OUTPUT_SEND = 0;
            __delay_us(PULSE_LOW_ZERO);
        }
    }
}

void send_message_nec(const nec_message *msg) {
    // Send 9ms pulse
    OUTPUT_SEND = 1;
    __delay_us(PREAMBLE_PULSE_HIGH_LEN);
    // Send 4.5ms space
    OUTPUT_SEND = 0;
    __delay_us(PREAMBLE_PULSE_LOW_LEN);

    // Send data
    send_byte_nec(msg->address);
    send_byte_nec(~msg->address);
    send_byte_nec(msg->command);
    send_byte_nec(~msg->command);

    // Send end bit
    OUTPUT_SEND = 1;
    __delay_us(PULSE_HIGH);
    OUTPUT_SEND = 0;
    __delay_us(PULSE_LOW_ZERO);
}
