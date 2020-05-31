/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/
#define SW_PORTA GP4
//#define SW_ASCENSORE  GP0
#define OUTPUT_PORT GP2

#ifdef PRODUZIONE
#define DURATA_ACCENSIONE_BREVE 300000 // 5 min
#else
#define DURATA_ACCENSIONE_BREVE 10000
#endif

#define DURATA_ACCENSIONE_LUNGA 600000 // 10 min

#ifdef PRODUZIONE
#define SOGLIA_PRENOTAZIONE_BREVE_RICIRCOLO 20000
#else
#define SOGLIA_PRENOTAZIONE_BREVE_RICIRCOLO 5000
#endif

#ifdef PRODUZIONE
#define SOGLIA_PRENOTAZIONE_LUNGA_RICIRCOLO 60000
#else
#define SOGLIA_PRENOTAZIONE_LUNGA_RICIRCOLO 10000
#endif

#define SOGLIA_ATTIVAZIONE_RICIRCOLO 2000
#define SOGLIA_INTERRUZIONE_RICIRCOLO 500

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
