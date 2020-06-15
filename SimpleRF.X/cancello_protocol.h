/* 
 * File:   cancello_protocol.h
 * Author: mozzicator
 *
 * Created on 7 giugno 2020, 14.26
 */

#ifndef CANCELLO_PROTOCOL_H
#define	CANCELLO_PROTOCOL_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>        /* For uint8_t definition */

void send_cancello(uint24_t code);

#ifdef	__cplusplus
}
#endif

#endif	/* CANCELLO_PROTOCOL_H */

