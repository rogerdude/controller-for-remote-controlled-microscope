/** 
 **************************************************************
 * @file mylib/s4743527_console.c
 * @author Hamza K
 * @date 02042024
 * @brief Console Library Driver for input.
 * REFERENCE: csse3010_mylib_lib_console.pdf
 *            csse3010_project.pdf
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4743527_lib_console_ascii2hex() - Converts ASCII to hex value.
 * s4743527_lib_console_dec2ascii() - Converts digit to ASCII.
 * s4743527_tsk_console_init() - Intialises RCM console task.
 *************************************************************** 
 */

#include "s4743527_console.h"
#include <stdint.h>

#ifdef FreeRTOS
#include "s4743527_mfs_led.h"
#include "s4743527_rcmdisplay.h"
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "queue.h"
#include "board.h"
#include "debug_log.h"
#endif

// Global variable
// Event group for user input.
EventGroupHandle_t s4743527GroupEventConsoleInput;

/**
 * Converts an ASCII hexadecimal value to binary hexadecimal value.
 * 
 * value: an ASCII hexadecimal value e.g. 0-9, A-F, and a-f.
 * 
 * Returns: the binary hexadecimal value for the input. 
 */
extern int s4743527_lib_console_ascii2hex(char value) {

    if (value >= 'A' && value <= 'F') {
        return value - 55;
    } else if (value >= 'a' && value <= 'f') {
        return value - 87;
    } else if (value >= '0' && value <= '9') {
        return value - 48;
    } else {
        return -1;
    }
}

/**
 * Converts a digit to its ASCII equivalent character.
 * 
 * value: a digit between 0 and 9.
 * 
 * Returns: the ASCII equivalent character of the value.
 */
extern char s4743527_lib_console_dec2ascii(int value) {

    if (value >= 0 && value <= 9) {
        return value + 48;
    }

    return 0;
}

#ifdef FreeRTOS

/**
 * Task for RCM console that takes input from user and sets event bits.
 * 
 * Returns: None
 */
void console_task(void) {

    // Intialise event group for input.
    s4743527GroupEventConsoleInput = xEventGroupCreate();

    // Initialise array with valid input characters.
    char validInput[25] = INPUT;

    // Variable to receive character from console.
    char recv;

    // Event group bits that are set when key is pressed.
    EventBits_t uxBits;

    for (;;) {

        // Receive user input
        if ((recv = BRD_debuguart_getc(0)) != EMPTY) {

            // Convert lowercase letter to uppercase.
            if ((recv >= 'a') && (recv <= 'z')) {
                recv -= 32;
            }
            
            S4743527_REG_MFS_LED_D2_TOGGLE();

            // Send key pressed to display task to print in console
            xQueueSend(s4743527QueueDisplayKey, (void*) &recv, (portTickType) 10);

            // Validate user input
            for (uint8_t i = 0; i < 24; i++) {

                if (recv == validInput[i]) {

                    // Set event group bit.
                    uxBits = xEventGroupSetBits(s4743527GroupEventConsoleInput, 1 << i);
                    break;
                }
            }
        }

        vTaskDelay(50);
    }
}

/**
 * Intialises the RCM console task and event group bits.
 * 
 * Returns: None.
 */
extern void s4743527_tsk_console_init(void) {

    xTaskCreate((void *) &console_task, (const signed char *) "Console Input",
            TASK_CONSOLE_STACK_SIZE, NULL, TASK_CONSOLE_PRIORITY, NULL);
}

#endif