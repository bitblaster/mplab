/* 
 * File:   nec_protocol.h
 * Author: mozzicator
 *
 * Created on 7 giugno 2020, 14.26
 */

#ifndef NEC_PROTOCOL_H
#define	NEC_PROTOCOL_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>        /* For uint8_t definition */
#include "user.h"

typedef struct _nec_message {
    uint8_t address;
    uint8_t command;
} nec_message;

void decode_nec(const uint8_t risingEdge, const uint24_t duration);
void received_nec_message(const nec_message *msg);
void send_message_nec(const nec_message *msg);

#ifdef	__cplusplus
}
#endif

#endif	/* NEC_PROTOCOL_H */

