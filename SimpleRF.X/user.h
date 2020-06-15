/* 
 * File:   user.h
 * Author: mozzicator
 *
 * Created on 7 giugno 2020, 14.53
 */

#ifndef USER_H
#define	USER_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <xc.h>
#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

#define _XTAL_FREQ 8000000
//TEST!!!!
//#define _XTAL_FREQ 1000000
//TEST!!!!

#ifdef _12F683

    //#define INPUT_SEND GP0
    #define INPUT_RECEIVE GP1
    #define OUTPUT_SEND GP2
    #define OUTPUT_LED GP5
#endif

#ifdef _12F1840

    #define OUTPUT_SEND RA2
    #ifdef PRODUCTION
        #define INPUT_RECEIVE RA1
    #else
        #define INPUT_RECEIVE RA4
    #endif
    #define OUTPUT_LED RA5
#endif


#ifdef	__cplusplus
}
#endif

#endif	/* USER_H */

