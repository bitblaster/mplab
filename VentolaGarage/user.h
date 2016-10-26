/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/
#define SW_PORTA GP4
//#define SW_ASCENSORE  GP0
#define OUTPUT_PORT GP2

#ifdef PRODUZIONE
#define DELAY_ACCENSIONE 300000 // 10 min
#else
#define DELAY_ACCENSIONE 10000
#endif

#ifdef PRODUZIONE
#define DELAY_SW_PORTA_APERTURA 20000
#else
#define DELAY_SW_PORTA_APERTURA 5000
#endif
#define DELAY_SW_PORTA_CHIUSURA 2000
#define DELAY_SW_PORTA_INTERRUZIONE 500
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
