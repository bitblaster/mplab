/******************************************************************************/
/* User Level #define Macros                                                  */
#define OUTPUT_RED GP2
#define OUTPUT_GREEN GP1
#define OUTPUT_BLUE GP0
#define INPUT_ECHO  GP4
#define OUTPUT_TRIGGER GP5

#ifdef PRODUZIONE

#endif

#define DELAY_STEP 10

#define _XTAL_FREQ 4000000
#define __delay_ms(x) _delay((unsigned long)((x)*(_XTAL_FREQ/4000.0)))

//#define TIMER_ABILITATO
//#define USA_INTERRUPT

/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/

void InitApp(void);         /* I/O and Peripheral Initialization */
unsigned char getOutputPortValue(void);
void setOutputPortValue(unsigned char);
