/* 
 * File:   g8_protocol.h
 * Author: mozzicator
 *
 * Created on 7 giugno 2020, 14.26
 */

#ifndef G8_PROTOCOL_H
#define	G8_PROTOCOL_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>        /* For uint8_t definition */

/*typedef struct _g8_message_fields {
    uint24_t system;
    uint8_t address;
    uint8_t command;
} g8_message_fields;
*/
typedef union _g8_message {
    struct {
        uint24_t system;
        uint8_t address;
        uint8_t command;        
    };
    uint8_t bytes[5];
} g8_message;

void decode_g8(const uint8_t risingEdge, const uint16_t duration);
void received_g8_message(const g8_message *msg);

#ifdef	__cplusplus
}
#endif

#endif	/* G8_PROTOCOL_H */

