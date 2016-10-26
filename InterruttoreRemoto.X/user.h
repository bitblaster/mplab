/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/
#define REMOTE_PORT GP4
#define LOCAL_PORT  GP0
#define PROGRAM_PORT GP5
#define OUTPUT_PORT GP2

#define DELAY_REMOTE_OFF 0
#define DELAY_REMOTE_ON 400
#define DELAY_LOCAL_TOGGLE 40
#define DELAY_STEP 10

#define _XTAL_FREQ 4000000
#define __delay_ms(x) _delay((unsigned long)((x)*(_XTAL_FREQ/4000.0)))

//#define TIMER_ABILITATO
//#define USA_INTERRUPT

/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/

/* TODO User level functions prototypes (i.e. InitApp) go here */

void InitApp(void);         /* I/O and Peripheral Initialization */
unsigned char getOutputPortValue(void);
void setOutputPortValue(unsigned char);
void restoreOutputValue(void);
#ifdef USA_INTERRUPT
void lampeggia(void);
#endif