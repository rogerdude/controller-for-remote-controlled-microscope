/** 
 **************************************************************
 * @file project/s4743527_txradio.h
 * @author Hamza K
 * @date 29042024
 * @brief Radio task functions for RCM.
 * REFERENCE: csse3010_project.pdf
 ***************************************************************
 * EXTERNAL FUNCTIONS
 ***************************************************************
 * s4743527_reg_radio_init() - Initialises registers for radio.
 * s4743527_tsk_radio_init() - Initialises task for RCM radio.
 *************************************************************** 
 */

#ifndef S4743527_TXRADIO_H
#define S4743527_TXRADIO_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

// Task Priority
#define TASK_RCM_RADIO_PRIORITY  (tskIDLE_PRIORITY + 1)

// Task Stack Allocation
#define TASK_RCM_RADIO_STACK_SIZE   (configMINIMAL_STACK_SIZE * 5)

// States for FSM
#define STANDBY     0
#define TRANSMIT    1

// Handle for radio packet queue
extern QueueHandle_t s4743527QueueRadioPacket;

// Function prototypes

// Initialises the radio register pins.
extern void s4743527_reg_radio_init(void);

// Initialises the RCM radio task. 
extern void s4743527_tsk_radio_init(void);

#endif