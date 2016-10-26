/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/
#define SW_PORTA GP4
#define SW_ASCENSORE  GP0
#define OUTPUT_PORT GP2

#ifdef PRODUZIONE
#define DELAY_ACCENSIONE 130000 // 2 min
#else
#define DELAY_ACCENSIONE 10000
#endif

#define DELAY_SW_ASCENSORE 40
#define DELAY_SW_PORTA 200
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
