/** 
 **************************************************************
 * @file mylib/s4743527_board_pb.c
 * @author Hamza K
 * @date 02052024
 * @brief USER Pushbutton on board.
 ***************************************************************
 * EXTERNAL FUNCTIONS
 ***************************************************************
 * s4743527_reg_board_pb_init() - Initialises GPIO, interrupt, 
 *                                and semaphore.
 *************************************************************** 
 */

#ifndef S4743527_BOARD_PB_H
#define S4743527_BOARD_PB_H

#include "FreeRTOS.h"
#include "semphr.h"

// Global variable
// Semaphore for USER button.
extern SemaphoreHandle_t s4743527SemaphorePushbutton;

// Pin number for USER pushbutton on port C
#define USER_BUTTON_PIN 13

// Function prototype
// Initialises GPIO pins for USER pushbutton.
extern void s4743527_reg_board_pb_init(void);

#endif