#pragma warning disable 359
/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include <xc.h>         /* XC8 General Include File */

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"
#include "tea_encryption.h"
#include "virtualwire.h"          /* User funct/params, such as InitApp */

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
#define MSG_REPEAT_COUNT 2
#define THRESHOLD_COUNTER 20

#define EQUAL_MSG(m1, m2) ((m1).action == (m2).action && (m1).mode == (m2).mode && (m1).device_id == (m2).device_id && (m1).message_counter == (m2).message_counter)

#define BROADCAST_GROUP 0xFF
#define CENTRAL_DEVICE_ID 1
#define DEVICE_ID       2

#define IGNORE_MESSAGE 0xFF
#define NO_RESPONSE 0

#define processAction len

static eeprom uint8_t device_groups[8];
static RfMessage msg;
static uint32_t lastMessageCounter = 1;
static uint8_t len;
static bit waitForAck=0;

/******************************************************************************/
/* Main Program                                                               */

/******************************************************************************/

void sendMessage(RfAction action, RfStatus status) {
    msg.src_device_id = DEVICE_ID;
    msg.dst_device_id = CENTRAL_DEVICE_ID;
    msg.dst_group = 0;
    //msg.mode = mode;
    msg.action = action;
    msg.status = status;
    msg.message_counter = ++lastMessageCounter;
        
    encrypt(&msg);
#ifdef DEBUG
    LED_PORT=1;
    __delay_ms(600);
    LED_PORT=0;
    __delay_ms(200);
#endif
    vw_send(&msg, sizeof(RfMessage), MSG_REPEAT_COUNT);
}

void sendCurrentStatus(RfAction action) {
    if(getOutputPortValue())
        sendMessage(action, STATUS_ON);
    else
        sendMessage(action, STATUS_OFF);
}

void main(void) {
/* Configure the oscillator for the device */
    ConfigureOscillator();

    /* Initialize I/O and Peripherals for application */
    InitApp();

    vw_setup();
     
    LED_PORT=1;
    __delay_ms(1000);
    LED_PORT=0;
    
    restoreOutputValue();

/*    /////////////////////////////////7
    //sendMessage(ACTION_DEVICE_STATUS, STATUS_ON);
    
    msg.src_device_id = 123;
    msg.dst_device_id = 234;
    msg.dst_group = 0;
    //msg.mode = mode;
    msg.action = 12;
    msg.status = 9;
    msg.message_counter = 1654865249;
   
    // Mettere breakpoint per vedere i valori nella vista File register
    encrypt(&msg);

    // Mettere breakpoint per vedere i valori nella vista File register
    decrypt(&msg);

    // Mettere breakpoint per vedere i valori nella vista File register    
    //////////////////////////////////7
*/
    len = DEVICE_ID;
    while(len--)
        __delay_ms(500);
    sendCurrentStatus(ACTION_INIT);

    vw_rx_start();
    
    while (1) {
        if(vw_have_message()) {
            len = sizeof(RfMessage);
            if(vw_recv(&msg, &len)) {
                vw_rx_stop();
#ifdef DEBUG
                LED_PORT=1;
                __delay_ms(200);
                LED_PORT=0;
                __delay_ms(200);
#endif
                decrypt(&msg);

                if(msg.src_device_id == CENTRAL_DEVICE_ID) {
                    uint8_t i;
                    processAction = IGNORE_MESSAGE;
                    if(msg.dst_device_id != 0) {
                        if(msg.dst_device_id == DEVICE_ID) {
                            if (msg.message_counter > lastMessageCounter && msg.message_counter - lastMessageCounter < THRESHOLD_COUNTER) {
                                lastMessageCounter = msg.message_counter;
                                processAction = ACTION_ACK;
                            } else {
                                processAction = ACTION_NACK;
                            }
                        }
                    } else if(msg.dst_group != 0) {
                        for(i=0; i < sizeof(device_groups); i++) {
                            if(msg.dst_group == device_groups[i]) {
                                processAction = NO_RESPONSE;
                                break;
                            }
                        }
                    }

                    if(processAction != IGNORE_MESSAGE) {
                        switch(msg.action) {
                            case ACTION_INIT:
                                lastMessageCounter=msg.message_counter-1;
                                break;
                            case ACTION_UPDATE_STATUS:
                                if(msg.status == STATUS_OFF)
                                    setOutputPortValue(0);
                                else if(msg.status == STATUS_ON)
                                    setOutputPortValue(1);
                                else if(msg.dst_device_id == DEVICE_ID)
                                    processAction = ACTION_NACK;
                                break;
                            case ACTION_QUERY_STATUS:
                                //sendCurrentStatus(ACTION_DEVICE_STATUS);
                                //processAction = 0;
                                break;
                            case ACTION_ADD_GROUP:
                                processAction = ACTION_NACK;
                                for(i=0; i < sizeof(device_groups); i++) {
                                    if(device_groups[i] == 0) {
                                        device_groups[i] = msg.dst_group;
                                        processAction = ACTION_ACK;
                                        break;
                                    }
                                }
                                break;
                            case ACTION_DELETE_GROUP:
                                for(i=0; i < sizeof(device_groups); i++) {
                                    if(device_groups[i] == msg.dst_group) {
                                        device_groups[i] = 0;
                                        break;
                                    }
                                }
                                break;
                            case ACTION_DELETE_ALL_GROUPS:
                                for(i=0; i < sizeof(device_groups); i++) {
                                    device_groups[i] = 0;
                                }
                                break;
                            case ACTION_NACK:
                                if(waitForAck)
                                    sendCurrentStatus(ACTION_DEVICE_STATUS);
                            case ACTION_ACK:
                                waitForAck = 0;
                                break;
                            default:
                                if(msg.dst_device_id == DEVICE_ID)
                                    processAction = ACTION_NACK;
                        }

                        if (processAction != NO_RESPONSE) {
                            // Send the ACK/NACK
                            sendMessage(processAction, msg.status);
                        }
                    }
                }
                vw_rx_start();
            }
        }
        
#ifdef ENABLE_LOCAL_PORT        
        len=0;
        while(!LOCAL_PORT && len < DELAY_LOCAL_TOGGLE) {
            __delay_ms(DELAY_STEP);
            len += DELAY_STEP;
        }

        if(len >= DELAY_LOCAL_TOGGLE) {
            setOutputPortValue(!getOutputPortValue());
            waitForAck=true;
            sendCurrentStatus(ACTION_DEVICE_STATUS);

            while(!LOCAL_PORT)
                __delay_ms(DELAY_STEP);
            __delay_ms(500);
        }
#endif
    }
}
