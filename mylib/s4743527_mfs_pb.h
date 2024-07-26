/** 
 **************************************************************
 * @file mylib/s4743527_mfs_pb.h
 * @author Hamza K
 * @date 23022024
 * @brief MFS Pushbutton Register Driver
 * REFERENCE: csse3010_mylib_reg_mfs_pushbutton.pdf
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4743527_reg_mfs_pb_init() - Initialise pushbutton pin and interrupt.
 * s4743527_reg_mfs_pb_isr() - Interrupt service routine for pb.
 * s4743527_reg_mfs_pb_press_get() - Gets press counter value of pb.
 * s4743527_reg_mfs_pb_press_reset() - Resets press counter value.
 *************************************************************** 
 */

#ifndef S4743527_MFS_PB_H
#define S4743527_MFS_PB_H

#ifdef FreeRTOS
#include "FreeRTOS.h"
#include "semphr.h"

// Semaphore for button S1
extern SemaphoreHandle_t pbSemaphore;
#endif

#define NUMBER_OF_BUTTONS 3

// Pushbutton select parameters for driver's functions.
#define S4743527_REG_MFA_PB_S1 1 << 0
#define S4743527_REG_MFA_PB_S2 1 << 1
#define S4743527_REG_MFA_PB_S3 1 << 2
#define S4743527_REG_MFA_PB_S4 1 << 3

// Indexes of button count in counter global variable.
#define BUTTON_S1_INDEX 0
#define BUTTON_S2_INDEX 1
#define BUTTON_S3_INDEX 2

// Pin numbers of all buttons on Port C
#define BUTTON_S1_PIN 0
#define BUTTON_S2_PIN 3
#define BUTTON_S3_Pin 1

// State of pin when button is pressed.
#define BUTTON_PRESSED 0x00

// Period of time where call to interrupt is ignored after press and release.
#define DEBOUNCE_PERIOD 50

// Prototypes
// Initialises global variables, and GPIO pins for pushbutton and interrupt.
extern void s4743527_reg_mfs_pb_init(int pb_select);

// Interrupt service routine to read a button press.
extern void s4743527_reg_mfs_pb_isr(int pb_select);

// Returns whether the specified pushbutton has been pressed.
extern int s4743527_reg_mfs_pb_press_get(int pb_select);

// Resets the specified pushbutton's press count to zero.
extern void s4743527_reg_mfs_pb_press_reset(int pb_select);

#endif