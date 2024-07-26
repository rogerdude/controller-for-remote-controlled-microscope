/** 
 **************************************************************
 * @file project/s4743527_rcmcont.c
 * @author Hamza K
 * @date 29042024
 * @brief FSM task for RCM Control
 * REFERENCE: csse3010_project.pdf
 ***************************************************************
 * EXTERNAL FUNCTIONS
 ***************************************************************
 * s4743527_tsk_rcmcont_init() - Initialises the RCM control task.
 *************************************************************** 
 */

#include "s4743527_rcmcont.h"
#include "s4743527_txradio.h"
#include "s4743527_rcmdisplay.h"
#include "s4743527_mfs_led.h"
#include "s4743527_board_pb.h"
#include "s4743527_console.h"
#include "nrf24l01plus.h"
#include "s4743527_hamming.h"
#include "s4743527_rgb.h"
#include "s4743527_lta1000g.h"
#include "s4743527_mfs_ssd.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "event_groups.h"
#include "queue.h"

#include "board.h"
#include "debug_log.h"

/**
 * FSM for RCM Control.
 * 
 * Returns: None
 */
void rcm_fsm_task(void) {

    taskENTER_CRITICAL();

    BRD_debuguart_init();

    // Initialise LED bar
    s4743527_reg_lta1000g_init();

    // Initialise RGB LED
    s4743527_reg_rgb_init();
    
    // Initialise MFS LEDs
    s4743527_reg_mfs_led_init();

    // Initialise USER pushbutton.
    s4743527_reg_board_pb_init();

    // Initialise nrf24l01
    s4743527_reg_radio_init();

    // Initialise MFS SSD
    s4743527_reg_mfs_ssd_init();
    s4743527_reg_mfs_ssd_clear();

    S4743527_REG_RGB_BLACK();

    taskEXIT_CRITICAL();

    // Start radio task.
    s4743527_tsk_radio_init();

    // Start console task
    s4743527_tsk_console_init();

    // Start display task
    s4743527_tsk_rcmdisplay_init();

    // Start MFS SSD task
    s4743527_tsk_mfs_ssd_init();

    uint8_t state = JOIN;

    // Initialise variables for console input and event bits.
    EventBits_t uxBits;
    char validInput[25] = INPUT;
    char keyPressed = EMPTY;

    // Integer form of position data.
    int xPos = 0;
    int yPos = 0;
    int zPos = 0;
    int zoom = 1;
    int rotate = 0;

    // ASCII form of position data.
    char x3;
    char x2;
    char x1;
    char y3;
    char y2;
    char y1;
    char z2;
    char z1;
    char zoomAscii;
    char r3;
    char r2;
    char r1;

    // Structs for position data.
    RCMData displayData;
    SSDData ssdData;

    for (;;) {

        switch (state) {
            case JOIN:
                // Check if button was pressed.
                if (s4743527SemaphorePushbutton != NULL) {
                    
                    if (xSemaphoreTake(s4743527SemaphorePushbutton, 10)) {

                        // Form the JOIN packet.
                        uint8_t uncodedPacket[16] = {0x20, 0x47, 0x43, 0x52, 0x78, 'J', 'O', 'I', 'N',
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

                        // Send the packet
                        xQueueSend(s4743527QueueRadioPacket, (void*) &uncodedPacket, (portTickType) 10);

                        // Reset event group bits if any key was pressed
                        // before join packet was sent.
                        uxBits = xEventGroupClearBits(s4743527GroupEventConsoleInput,
                                INPUT_EVT_MASK);

                        state = IDLE;
                    }
                }
                break;
            
            case IDLE:

                // Check event group bits and allow 10ms wait time
                uxBits = xEventGroupWaitBits(s4743527GroupEventConsoleInput,
                        INPUT_EVT_MASK, pdTRUE, pdFALSE, 10);

                // Check which key was pressed.
                for (uint8_t i = 0; i < 24; i++) {
                    if (uxBits & (1 << i)) {

                        keyPressed = validInput[i];
                        
                        // Check which type of key press it is.
                        if ((i <= 17) || (i == 22)) { // Move position

                            // Determine if distance moved should be postive or negative.
                            int numberType = POSITIVE;
                            if ((i % 2) == 1) {
                                numberType = NEGATIVE;
                            }
                            
                            // Determine the distance to move by.
                            int distance = 0;
                            if (i <= 5) {
                                distance = 2;
                            } else if (i >= 6 && i <= 11) {
                                distance = 10;
                            } else if (i >= 12 && i <= 17) {
                                distance = 50;
                            }

                            distance *= numberType;

                            // Determine which position (x,y, or z) to move.
                            switch (keyPressed) {
                                case 'W':
                                case 'Q':
                                case 'A':
                                case 'S':
                                case 'Z':
                                case 'X':

                                    if ((xPos == 200 && distance > 0) ||
                                            (xPos == 0 && distance < 0)) {
                                        uxBits &= ~(1 << i);
                                    }

                                    xPos += distance;
                                    if (xPos < 0) {
                                        xPos = 0;
                                    } else if (xPos > 200){
                                        xPos = 200;
                                    }
                                    break;
                                
                                case 'E':
                                case 'R':
                                case 'D':
                                case 'F':
                                case 'C':
                                case 'V':

                                    if ((yPos == 200 && distance > 0) ||
                                            (yPos == 0 && distance < 0)) {
                                        uxBits &= ~(1 << i);
                                    }

                                    yPos += distance;
                                    if (yPos < 0) {
                                        yPos = 0;
                                    } else if (yPos > 200){
                                        yPos = 200;
                                    }
                                    break;
                                
                                case 'T':
                                case 'Y':
                                case 'G':
                                case 'H':
                                case 'B':
                                case 'N':

                                    if ((zPos == 99 && distance > 0) ||
                                            (zPos == 0 && distance < 0)) {
                                        uxBits &= ~(1 << i);
                                    }

                                    zPos += distance;
                                    if (zPos < 0) {
                                        zPos = 0;
                                    } else if (zPos > 99){
                                        zPos = 99;
                                    }
                                    break;
                                
                                case '5':
                                    xPos = 0;
                                    yPos = 0;
                                    zPos = 0;
                                    zoom = 1;
                                    rotate = 0;
                                    break;

                                default:
                                    break;
                            }
                        
                        } else if (i == 18 || i == 19) { // Move zoom

                            int zoomInc = POSITIVE;
                            if (i == 19) {
                                zoomInc = NEGATIVE;
                            }

                            if ((zoom == 1 && zoomInc == NEGATIVE) ||
                                    (zoom == 9 && zoomInc == POSITIVE)) {
                                uxBits &= ~(1 << i);
                            }

                            zoom += zoomInc;
                            if (zoom < 1) {
                                zoom = 1;
                            } else if (zoom > 9) {
                                zoom = 9;
                            }
                        
                        } else if (i == 20 || i == 21) { // Rotate

                            int rotateType = POSITIVE;
                            if (i == 21) {
                                rotateType = NEGATIVE;
                            }

                            int rotateInc = 10 * rotateType;

                            if ((rotate == 0 && rotateType == NEGATIVE) ||
                                    (rotate == 180 && rotateType == POSITIVE)) {
                                uxBits &= ~(1 << i);
                            }
                            
                            rotate += rotateInc;
                            if (rotate < 0) {
                                rotate = 0;
                            } else if (rotate > 180) {
                                rotate = 180;
                            }
                        }
                        state = PACKET;
                    }
                }
                break;

            case PACKET:
                
                // Get ASCII form of each digit of position data.

                x3 = s4743527_lib_console_dec2ascii(xPos / 100);
                x2 = s4743527_lib_console_dec2ascii((xPos / 10) % 10);
                x1 = s4743527_lib_console_dec2ascii(xPos % 10);

                y3 = s4743527_lib_console_dec2ascii(yPos / 100);
                y2 = s4743527_lib_console_dec2ascii((yPos / 10) % 10);
                y1 = s4743527_lib_console_dec2ascii(yPos % 10);

                z2 = s4743527_lib_console_dec2ascii((zPos / 10) % 10);
                z1 = s4743527_lib_console_dec2ascii(zPos % 10);

                zoomAscii = s4743527_lib_console_dec2ascii(zoom);

                r3 = s4743527_lib_console_dec2ascii(rotate / 100);
                r2 = s4743527_lib_console_dec2ascii((rotate / 10) % 10);
                r1 = s4743527_lib_console_dec2ascii(rotate % 10);

                // Check which key was pressed.
                for (uint8_t i = 0; i < 23; i++) {
                    if (uxBits & (1 << i)) {
                        
                        // Check which type of key press it is.
                        if (i <= 17) { // Position packet

                            uint8_t uncodedPacket[16] = {0x22, 0x47, 0x43, 0x52, 0x78, 'X', 'Y', 'Z', 
                                    x3, x2, x1, y3, y2, y1, z2, z1};

                            // Send the packet
                            xQueueSend(s4743527QueueRadioPacket, (void*) &uncodedPacket, (portTickType) 10);

                        } else if (i == 18 || i == 19) { // Zoom packet

                            uint8_t uncodedPacket[16] = {0x25, 0x47, 0x43, 0x52, 0x78, 'Z', 'O', 'O', 'M',
                                    zoomAscii, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

                            // Send the packet
                            xQueueSend(s4743527QueueRadioPacket, (void*) &uncodedPacket, (portTickType) 10);

                        } else if (i == 20 || i == 21) { // Rotate Packet

                            uint8_t uncodedPacket[16] = {0x23, 0x47, 0x43, 0x52, 0x78, 'R', 'O', 'T', 
                                    r3, r2, r1, 0x00, 0x00, 0x00, 0x00, 0x00};

                            // Send the packet
                            xQueueSend(s4743527QueueRadioPacket, (void*) &uncodedPacket, (portTickType) 10);

                        } else if (i == 22) { // Reset position to origin
                            
                            // Form the 3 packets.
                            uint8_t uncodedPositionPacket[16] = {0x22, 0x47, 0x43, 0x52, 0x78, 'X', 'Y', 'Z', 
                                    x3, x2, x1, y3, y2, y1, z2, z1};
                            uint8_t uncodedZoomPacket[16] = {0x25, 0x47, 0x43, 0x52, 0x78, 'Z', 'O', 'O', 'M',
                                    zoomAscii, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
                            uint8_t uncodedRotatePacket[16] = {0x23, 0x47, 0x43, 0x52, 0x78, 'R', 'O', 'T', 
                                    r3, r2, r1, 0x00, 0x00, 0x00, 0x00, 0x00};

                            // Send the 3 packets.

                            xQueueSend(s4743527QueueRadioPacket, (void*) &uncodedRotatePacket, (portTickType) 10);
                            vTaskDelay(300);

                            xQueueSend(s4743527QueueRadioPacket, (void*) &uncodedZoomPacket, (portTickType) 10);
                            vTaskDelay(500);

                            xQueueSend(s4743527QueueRadioPacket, (void*) &uncodedPositionPacket, (portTickType) 10);
                        }

                        // Send updated position data
                        displayData.xPos = xPos;
                        displayData.yPos = yPos;
                        displayData.zPos = zPos;
                        displayData.zoom = zoom;
                        displayData.rotate = rotate;
                        xQueueSend(s4743527QueueDisplayData, (void*) &displayData, (portTickType) 10);

                        // Send position to MFS SSD task
                        ssdData.xPos = xPos;
                        ssdData.yPos = yPos;
                        ssdData.zPos = zPos;
                        xQueueSend(s4743527QueueSSD, (void*) &ssdData, (portTickType) 10);
                    }
                }

                state = IDLE;
                break;
            default:
                state = JOIN;
                break;
        }

        vTaskDelay(5);
    }
}

/**
 * Initialises the RCM control task.
 * 
 * Returns: None
 */
extern void s4743527_tsk_rcmcont_init(void) {
    
    // Create task for RCM control
    xTaskCreate((void*) &rcm_fsm_task, (const signed char *) "RCM Control",
            TASK_RCM_CONT_STACK_SIZE, NULL, TASK_RCM_CONT_PRIORITY, NULL);
    
    // Start schedular
    vTaskStartScheduler();
}