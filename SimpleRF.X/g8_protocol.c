#include "g8_protocol.h"

typedef enum _g8_status {
    STATUS_UNKNOWN, STATUS_PREAMBLE_START, STATUS_PREAMBLE_END, STATUS_PULSE_START, STATUS_PULSE_ONE_END, STATUS_PULSE_ZERO_END
} g8_status;

#define LENGTH(x, len, tolerance) (x<=len+tolerance && x>=len-tolerance)

#define PREAMBLE_PULSE_HIGH_LEN 4770
#define PREAMBLE_PULSE_LOW_LEN 1550
#define PULSE_HIGH_ONE 650
#define PULSE_LOW_ONE 430
#define PULSE_HIGH_ZERO 300
#define PULSE_LOW_ZERO 780
#define TARGET_BIT_COUNT 40

static g8_message g8Message = {0, 0, 0};
static g8_status status = STATUS_UNKNOWN;
static uint8_t bitCount = 0;
static uint8_t curByte = 0;

static inline void reset() {
    status = STATUS_UNKNOWN;
    bitCount = 0;
    g8Message = (g8_message){0, 0, 0};
    curByte = 0;
}

static void gotBit(const uint8_t value) {
    curByte = (curByte << 1) | (value & 1);

    bitCount++;

    if (bitCount % 8 == 0) {
        if(bitCount <= 24)
            g8Message.system = (g8Message.system << 8) | curByte;
        else if(bitCount == 32)
            g8Message.address = curByte;
        else if(bitCount == 40)
            g8Message.command = curByte;
        //g8Message.bytes[bitCount/8-1] = curByte;
        curByte=0;
    }        

    if (bitCount >= TARGET_BIT_COUNT) {
        received_g8_message(&g8Message);
        reset();
    }
}

void decode_g8(const uint8_t risingEdge, const uint16_t duration) {

    switch (status) {
        case STATUS_UNKNOWN:
            if (risingEdge) {
                if (LENGTH(duration, PREAMBLE_PULSE_LOW_LEN, 200))
                    status = STATUS_PULSE_START;
                else
                    status = STATUS_PREAMBLE_START;
            } else if (LENGTH(duration, PREAMBLE_PULSE_HIGH_LEN, 300))
                status = STATUS_PREAMBLE_END;
            break;

        case STATUS_PREAMBLE_START:
            if (!risingEdge && LENGTH(duration, PREAMBLE_PULSE_HIGH_LEN, 300))
                status = STATUS_PREAMBLE_END;
            else
                reset();
            break;

        case STATUS_PREAMBLE_END:
            if (risingEdge && LENGTH(duration, PREAMBLE_PULSE_LOW_LEN, 200)) {
                
                status = STATUS_PULSE_START;
            }
            break;

        case STATUS_PULSE_START:
            if (!risingEdge) {
                if (LENGTH(duration, PULSE_HIGH_ONE, 80))
                    status = STATUS_PULSE_ONE_END;
                else if (LENGTH(duration, PULSE_HIGH_ZERO, 80))
                    status = STATUS_PULSE_ZERO_END;
                else
                    reset();
            } else
                reset();

            break;

        case STATUS_PULSE_ONE_END:
            //if (risingEdge && LENGTH(delta, PULSE_LOW_ONE)) {
            status = STATUS_PULSE_START;
            gotBit(1);
            //}
            //else
            //    m_Status = STATUS_UNKNOWN;
            break;

        case STATUS_PULSE_ZERO_END:
            //if (risingEdge && LENGTH(delta, PULSE_LOW_ZERO)) {
            status = STATUS_PULSE_START;
            gotBit(0);
            //}
            //else
            //    m_Status = STATUS_UNKNOWN;
            break;
    }
}
