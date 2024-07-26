/** 
 **************************************************************
 * @file project/s4743527_txradio.c
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
#include "s4743527_txradio.h"
#include <stdint.h>
#include "nrf24l01plus.h"
#include "s4743527_hamming.h"
#include "s4743527_mfs_led.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

// Global variable
// Handle for radio packet queue
QueueHandle_t s4743527QueueRadioPacket;

/**
 * Task for RCM radio which sends hamming encoded packets.
 * 
 * Returns: None.
 */
void radio_fsm_task(void) {

    // Initialise queue
    s4743527QueueRadioPacket = xQueueCreate(10, sizeof(uint8_t) * 16);

    uint8_t state = STANDBY;

    uint8_t uncodedPacket[16];
    uint8_t encodedPacket[32];

    for (;;) {

        switch (state) {
            case STANDBY:

                // Receive 16 byte packet from queue.
                if (xQueueReceive(s4743527QueueRadioPacket, &uncodedPacket, 10)) {

                    // Hamming encode the packet.
                    for (uint8_t j = 0; j < 16; j++) {
                        uint16_t encodedBytes = s4743527_lib_hamming_byte_encode(uncodedPacket[j]);
                        encodedPacket[j * 2] = encodedBytes & 0xFF;
                        encodedPacket[(j * 2) + 1] = (encodedBytes & 0xFF00) >> 8;
                    }

                    state = TRANSMIT;
                }
                break;

            case TRANSMIT:

                // Send hamming encoded packet.
                taskENTER_CRITICAL();
                nrf24l01plus_send(encodedPacket);
                taskEXIT_CRITICAL();
                
                S4743527_REG_MFS_LED_D1_TOGGLE();
                
                state = STANDBY;
                break;
            
            default:
                state = STANDBY;
                break;
        }

        vTaskDelay(10);
    }
}

/**
 * Initialises the radio register pins.
 * 
 * Returns: None
 */
extern void s4743527_reg_radio_init(void) {
    nrf24l01plus_init();
}

/**
 * Initialises the RCM radio task.
 * 
 * Returns: None
 */
extern void s4743527_tsk_radio_init(void) {

    xTaskCreate((void*) &radio_fsm_task, (const signed char *) "RCM Radio",
            TASK_RCM_RADIO_STACK_SIZE, NULL, TASK_RCM_RADIO_PRIORITY, NULL);
}