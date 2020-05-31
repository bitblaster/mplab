/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/
//#define SW_ACCENSIONE  GP0
#define OUTPUT_PORT RA2

#ifdef PRODUZIONE
#define DELAY_ACCENSIONE 325000 // 5 min
#else
#define DELAY_ACCENSIONE 10000
#endif

//#define DELAY_SW_ACCENSIONE 40
#define DELAY_STEP 10

#define _XTAL_FREQ 32000000
#define __delay_ms(x) _delay((unsigned long)((x)*(_XTAL_FREQ/4000.0)))

//#define TIMER_ABILITATO
//#define USA_INTERRUPT

/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/

void InitApp(void);         /* I/O and Peripheral Initialization */
unsigned char getOutputPortValue(void);
void setOutputPortValue(unsigned char);
void restoreOutputValue(void);
