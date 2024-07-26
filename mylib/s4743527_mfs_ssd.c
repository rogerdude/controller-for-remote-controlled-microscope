/** 
 **************************************************************
 * @file mylib/s4743527_mfs_ssd.c
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

#include "s4743527_mfs_ssd.h"
#include "processor_hal.h"

#ifdef FreeRTOS
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "board.h"
#include "debug_log.h"

// Handle for RCM positions for SSD
QueueHandle_t s4743527QueueSSD;
#endif

// Global variables
// Array that contains ssd version of each digit.
static int numberDisplay[10] = ALL_MFS_SSD_NUMBERS;

/**
 * Initialises GPIO for MFS SSD.
 * 
 * Returns: None.
 */
extern void s4743527_reg_mfs_ssd_init(void) {
    // Enable clock for Port F
	__GPIOF_CLK_ENABLE();

    // Clear bits and set to output.
    GPIOF->MODER &= ~((0x03 << (12 * 2)) | (0x03 << (13 * 2)) | (0x03 << (14 * 2)));
    GPIOF->MODER |= (0x01 << (12 * 2)) | (0x01 << (13 * 2)) | (0x01 << (14 * 2));

    // Clear bits and for output push-pull
    GPIOF->OTYPER &= ~((0x01 << (12 * 2)) | (0x01 << (13 * 2)) | (0x01 << (14 * 2)));

    // Clear bits and set to high speed.
    GPIOF->OSPEEDR &= ~((0x03 << (12 * 2)) | (0x03 << (13 * 2)) | (0x03 << (14 * 2)));
    GPIOF->OSPEEDR |= (0x02 << (12 * 2)) | (0x02 << (13 * 2)) | (0x02 << (14 * 2));

    // Clear bits and set to pull-down
    GPIOF->PUPDR &= ~((0x03 << (12 * 2)) | (0x03 << (13 * 2)) | (0x03 << (14 * 2)));
    GPIOF->PUPDR |= (0x02 << (12 * 2)) | (0x02 << (13 * 2)) | (0x02 << (14 * 2));
}

/**
 * Sends an 8 bit byte to the 74HC595 chip's storage registers.
 * 
 * ssdByte: the 8 bit byte to send.
 * 
 * Returns: None.
 */
void ssd_sendbyte(uint8_t ssdByte) {

    // Send each bit individually starting from MSB.
    for (int i = 7; i >= 0; i--) {

        // Set data pin if bit is 1, clear if 0.
        if (ssdByte & (1 << i)) {
            GPIOF->ODR |= (0x01 << DATA_PIN);
        } else {
            GPIOF->ODR &= ~(0x01 << DATA_PIN);
        }
        
        // Set and then clear clock pin for chip's storage registers to store bit.
        GPIOF->ODR |= (0x01 << CLK_PIN);
        GPIOF->ODR &= ~(0x01 << CLK_PIN);
    }

}

/**
 * Displays a number on the SSD.
 * 
 * number: the number to display from 0 to 9.
 * position: the index of SSD to display digit on (from 0 to 3 and left to right).
 * 
 * Returns: None. 
 */
extern void s4743527_reg_mfs_ssd_display(int number, int position) {

    // Send the SSD version of number.
    if (number == MFS_SSD_CLEAR) {
        ssd_sendbyte(MFS_SSD_CLEAR);
    } else {
        ssd_sendbyte(numberDisplay[number]);
    }

    // Send the index at which number should be displayed. 
    ssd_sendbyte(1 << position);

    // Set and then clear latch pin to output data in storage registers on chip.
    GPIOF->ODR |= (0x01 << LATCH_PIN);
    GPIOF->ODR &= ~(0x01 << LATCH_PIN);
}

/**
 * Clears all digits of SSD.
 * 
 * Returns: None. 
 */
extern void s4743527_reg_mfs_ssd_clear(void) {
    
    for (uint8_t i = 0; i <= 3; i++) {
        s4743527_reg_mfs_ssd_display(MFS_SSD_CLEAR, i);
    }
}

#ifdef FreeRTOS

/**
 * Task for RCM to display its position on SSD.
 * 
 * Returns: None.
*/
void mfs_ssd_task(void) {

    // Initialise queue
    s4743527QueueSSD = xQueueCreate(10, sizeof(SSDData));

    // Struct for holding RCM position data.
    SSDData position;

    int oldXPos = 0;
    int oldYPos = 0;
    int oldZPos = 0;

    // Array for changing between which digit is displayed.
    int numberDisplay[4] = {MFS_SSD_CLEAR, MFS_SSD_CLEAR, MFS_SSD_CLEAR, MFS_SSD_CLEAR};

    // Variable that controls which data to display. 
    int changed = INITIAL;

    // Index of digit to display on SSD.
    int currentPosition = 0;

    for (;;) {

        // Receive position data from RCM Control task.
        if (xQueueReceive(s4743527QueueSSD, &position, 10)) {

            // Update which position to display if any is changed.
            if (oldXPos != position.xPos) {
                oldXPos = position.xPos;
                changed = X_POSITION;
            } else if (oldYPos != position.yPos) {
                oldYPos = position.yPos;
                changed = Y_POSITION;
            } else if (oldZPos != position.zPos) {
                oldZPos = position.zPos;
                changed = Z_POSITION;
            }
        }

        switch (changed) {
            case INITIAL:
                s4743527_reg_mfs_ssd_clear();
                changed = EMPTY_SSD;
                break;
            
            case X_POSITION:
                // Set numbers displayed to x coordinate.
                numberDisplay[MFS_SSD_FOURTH] = position.xPos % 10;
                numberDisplay[MFS_SSD_THIRD] = (position.xPos / 10) % 10;
                numberDisplay[MFS_SSD_SECOND] = position.xPos / 100;
                break;

            case Y_POSITION:
                // Set numbers displayed to y coordinate.
                numberDisplay[MFS_SSD_FOURTH] = position.yPos % 10;
                numberDisplay[MFS_SSD_THIRD] = (position.yPos / 10) % 10;
                numberDisplay[MFS_SSD_SECOND] = position.yPos / 100;
                break;

            case Z_POSITION:
                // Set numbers displayed to z coordinate.
                numberDisplay[MFS_SSD_FOURTH] = position.zPos % 10;
                numberDisplay[MFS_SSD_THIRD] = (position.zPos / 10) % 10;
                numberDisplay[MFS_SSD_SECOND] = MFS_SSD_CLEAR;
                break;
            
            case EMPTY_SSD:
                break;

            default:
                changed = INITIAL;
                break;
        }
        
        // Display each digit on each index every 3 ms.
        s4743527_reg_mfs_ssd_display(numberDisplay[MFS_SSD_FIRST], MFS_SSD_FIRST);
        vTaskDelay(3);
        s4743527_reg_mfs_ssd_display(numberDisplay[MFS_SSD_SECOND], MFS_SSD_SECOND);
        vTaskDelay(3);
        s4743527_reg_mfs_ssd_display(numberDisplay[MFS_SSD_THIRD], MFS_SSD_THIRD);
        vTaskDelay(3);
        s4743527_reg_mfs_ssd_display(numberDisplay[MFS_SSD_FOURTH], MFS_SSD_FOURTH);
        vTaskDelay(3);
    }

}

/**
 * Initialises RCM task for SSD.
 * 
 * Returns: None.
 */
extern void s4743527_tsk_mfs_ssd_init(void) {

    // Create task for MFS SSD
    xTaskCreate((void*) &mfs_ssd_task, (const signed char *) "MFS SSD",
            TASK_MFS_SSD_STACK_SIZE, NULL, TASK_MFS_SSD_PRIORITY, NULL);
}

#endif