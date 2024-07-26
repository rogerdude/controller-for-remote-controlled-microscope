/** 
 **************************************************************
 * @file mylib/s4743527_mfs_ssd.h
 * @author Hamza K
 * @date 14052024
 * @brief Driver and task functions for SSD on MFS.
 * REFERENCE: https://docs.arduino.cc/tutorials/communication/guide-to-shift-out/#shftout21
 *            https://lastminuteengineers.com/74hc595-shift-register-arduino-tutorial/
 *            Lab 3.11 Multi-Functional Shield (MFS)
 ***************************************************************
 * EXTERNAL FUNCTIONS
 ***************************************************************
 * s4743527_reg_mfs_ssd_init() - Initialises GPIO for SSD.
 * s4743527_reg_mfs_ssd_display() - Displays digit on SSD.
 * s4743527_reg_mfs_ssd_clear() - Clears all digits on SSD.
 * s4743527_tsk_mfs_ssd_init() - Initialises SSD task for RCM.
 *************************************************************** 
 */

#ifndef S4743527_MFS_SSD_H
#define S4743527_MFS_SSD_H

#ifdef FreeRTOS

#include "FreeRTOS.h"
#include "queue.h"

extern QueueHandle_t s4743527QueueSSD;

typedef struct {
    int xPos;
    int yPos;
    int zPos;
} SSDData;

// Task Priority
#define TASK_MFS_SSD_PRIORITY  (tskIDLE_PRIORITY + 2)

// Task Stack Allocation
#define TASK_MFS_SSD_STACK_SIZE    (configMINIMAL_STACK_SIZE * 3)

// States for displaying on ssd.
#define INITIAL -1
#define X_POSITION 0
#define Y_POSITION 1
#define Z_POSITION 2
#define EMPTY_SSD 3

#endif

// Pins used for SSD on port F.
#define LATCH_PIN 14
#define CLK_PIN 13
#define DATA_PIN 12

// SSD version of each digit
#define MFS_SSD_CLEAR   0xFF
#define MFS_SSD_ZERO    0xC0
#define MFS_SSD_ONE     0XF9
#define MFS_SSD_TWO     0XA4
#define MFS_SSD_THREE   0XB0
#define MFS_SSD_FOUR    0X99
#define MFS_SSD_FIVE    0X92
#define MFS_SSD_SIX     0X82
#define MFS_SSD_SEVEN   0XF8
#define MFS_SSD_EIGHT   0X80
#define MFS_SSD_NINE    0X90

// Array for SSD version of each digit from 0 to 9. 
#define ALL_MFS_SSD_NUMBERS {0xC0, 0XF9, 0XA4, 0XB0, 0X99, 0X92, 0X82, 0XF8, 0X80, 0X90}

// Position of each digit from left to right
#define MFS_SSD_FIRST   0
#define MFS_SSD_SECOND  1
#define MFS_SSD_THIRD   2
#define MFS_SSD_FOURTH  3

// Function prototypes

// Initialises GPIO for MFS SSD.
extern void s4743527_reg_mfs_ssd_init(void);

// Displays a number on the SSD.
extern void s4743527_reg_mfs_ssd_display(int number, int position);

// Clears all digits of SSD.
extern void s4743527_reg_mfs_ssd_clear(void);

#ifdef FreeRTOS
// Initialises RCM task for SSD.
extern void s4743527_tsk_mfs_ssd_init(void);
#endif

#endif