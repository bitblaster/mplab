/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/
#define LOCAL_PORT  GP0
#define LED_PORT    GP5
#define OUTPUT_PORT GP2

#define DELAY_LOCAL_TOGGLE 40
#define DELAY_STEP 10

//#define DEVICE_ADDRESS 0x0001

#define _XTAL_FREQ 4000000
//#define __delay_ms(x) _delay((unsigned long)((x)*1000 /*(_XTAL_FREQ/4000.0) */))

/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/

typedef enum {
    ACTION_INIT=1,
    ACTION_QUERY_STATUS=2,
    ACTION_UPDATE_STATUS=3,
    ACTION_DEVICE_STATUS=4,
    ACTION_ADD_GROUP=5,
    ACTION_DELETE_GROUP=6,
    ACTION_DELETE_ALL_GROUPS=7,
    ACTION_ACK=8,
    ACTION_NACK=9
} RfAction;

typedef enum {
    STATUS_OFF=1,
    STATUS_ON=2,
    STATUS_UNKNOWN=3,
} RfStatus;

typedef struct RfMessage_struct {
    uint8_t src_device_id;
    uint8_t action : 4;
    uint8_t status : 4;
    uint8_t dst_group;
    uint8_t dst_device_id;
    uint32_t message_counter;
} RfMessage;

void InitApp(void);         /* I/O and Peripheral Initialization */
unsigned char getOutputPortValue(void);
void setOutputPortValue(unsigned char);
void restoreOutputValue(void);
