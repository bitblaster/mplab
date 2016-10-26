/******************************************************************************/
/*Files to Include                                                            */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */
#include "user.h"          /* User funct/params, such as InitApp */

/******************************************************************************/
/* Interrupt Routines                                                         */
/******************************************************************************/

/* Baseline devices don't have interrupts. Unfortunately the baseline detection 
 * macro is named _PIC12 */

#ifndef _PIC12

#ifdef USA_INTERRUPT
#define SOGLIA_PROGRAMMA 28
#define SOGLIA_ACCENDI 9
#define SOGLIA_SPEGNI  1
#define PROGRAM_BIT_NUMBER 11

unsigned char remoteCounter=0;
unsigned long timeout=0;
bool programmazione=false;
bool startup=true;
bool interruttorePremuto=true;
unsigned char bitCorrente;

void interrupt isr(void)
{
    /* This code stub shows general interrupt handling.  Note that these
    conditional statements are not handled within 3 seperate if blocks.
    Do not use a seperate if block for each interrupt flag to avoid run
    time errors. */

    // Metto prima il test per Timer0 perché altrimenti se il timer scatta
    // mentre andora non ho finito l'elaborazione con i GPIO rientra nel giro del GPIO
#ifdef TIMER_ABILITATO
    if(TMR0IF) {
        if(!programmazione && timeout > 0) {
            timeout--;
            if(timeout == 0) {
                setOutputPortValue(0);
//                SLEEP();
            }
        }

        if(remoteCounter > 0) {
            remoteCounter++;
        }

        if(startup) {
            restoreOutputValue();
        }

        TMR0IF=0;
    }
    else
#endif
    if(GPIF) {
        if(!programmazione)
            timeout=0;

        if(REMOTE_PORT && remoteCounter==0) {
            remoteCounter=1;
            __delay_ms(80);
            if (REMOTE_PORT) {
                __delay_ms(250);
                if (REMOTE_PORT)
                    setOutputPortValue(1);
                else
                    setOutputPortValue(0);
            }
        }
        else if(!REMOTE_PORT && remoteCounter == 1) {
            remoteCounter=0;
        }
#ifndef TIMER_ABILITATO
        else if(LOCAL_PORT && interruttorePremuto) {
            interruttorePremuto=false;
        }
        else if (!LOCAL_PORT && !interruttorePremuto) {
            interruttorePremuto=true;
            __delay_ms(60);
            if (!LOCAL_PORT) {
                setOutputPortValue(!getOutputPortValue());
//                    SLEEP();
            }
        }
#else
        else {
            if(!REMOTE_PORT && remoteCounter > 0) {
                if(remoteCounter > SOGLIA_PROGRAMMA) {
                    if(!programmazione) {
                        // Se non eravamo in modo programmazione entriamo
                        // azzerando timeout e posizione bit
                        timeout=0;
                        bitCorrente=0;
                        programmazione=true;
                        PROGRAM_PORT=1;
                        setOutputPortValue(0);
                    }
                    else {
                        // Se eravamo in modo programmazione ne usciamo
                        // e verifichiamo di aver ricevuto tutti i bit,
                        // altrimenti azzeriamo il timeout
                        if(bitCorrente < PROGRAM_BIT_NUMBER)
                            timeout=0;
                        else {
                            timeout *= 2560;
                            timeout /= 3;
                            setOutputPortValue(1);
                        }
                        PROGRAM_PORT=0;
                        programmazione=false;
                    }
                }
                else if(remoteCounter > SOGLIA_ACCENDI) {
                    // Se siamo in modo programmazione abbiamo ricevuto un bit '1'
                    if(programmazione) {
                        if(bitCorrente < PROGRAM_BIT_NUMBER) {
                            timeout |= 1 << bitCorrente;
                            bitCorrente++;
                            PROGRAM_PORT=0;
                            __delay_ms(1000);
                            PROGRAM_PORT=1;
                        }
                        else {
                            lampeggia();
                        }
                    }
                    else {
                        setOutputPortValue(1);
//                        SLEEP();
                    }
                }
                else if(remoteCounter > SOGLIA_SPEGNI) {
                    // Se siamo in modo programmazione abbiamo ricevuto un bit '0'
                    if(programmazione) {
                        if(bitCorrente < PROGRAM_BIT_NUMBER) {
                            timeout &= ~(1 << bitCorrente);
                            bitCorrente++;
                            PROGRAM_PORT=0;
                            __delay_ms(200);
                            PROGRAM_PORT=1;
                        }
                        else {
                            lampeggia();
                        }
                    }
                    else {
                        setOutputPortValue(0);
//                        SLEEP();
                    }
                }
                remoteCounter=-1;
            }
            else if(LOCAL_PORT && interruttorePremuto) {
                interruttorePremuto=false;
            }
            else if (!LOCAL_PORT && !interruttorePremuto) {
                interruttorePremuto=true;
                __delay_ms(60);
                if (!LOCAL_PORT) {
                    setOutputPortValue(!getOutputPortValue());
//                    SLEEP();
                }
            }
        }
#endif
        GPIF=0;
    }
#if 0

#endif

}

void lampeggia(void) {
    int i;
    for(i=0; i < 3; i++) {
        PROGRAM_PORT=0;
        __delay_ms(200);
        PROGRAM_PORT=1;
        __delay_ms(200);
    }
}
#endif
#endif
