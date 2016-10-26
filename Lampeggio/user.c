/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#include "user.h"

/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

unsigned short outputValue=0;
/* <Initialize variables in user.h and insert code for user algorithms.> */

void InitApp(void)
{
    SCS=1;

    ANSEL=0;
    CMCON0=0;

    TRISIO=0;

    OPTION_REG=0;
    nGPPU=0;

    INTCON=0;
}
