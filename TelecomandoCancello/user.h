/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/
#define OUTPUT_PORT GP1

#define _XTAL_FREQ 4000000
#define __delay_ms(x) _delay((unsigned long)((x)*(_XTAL_FREQ/4000.0)))

//#define TIMER_ABILITATO
//#define USA_INTERRUPT

/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/

/* TODO User level functions prototypes (i.e. InitApp) go here */

void InitApp(void);         /* I/O and Peripheral Initialization */
