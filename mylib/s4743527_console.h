/** 
 **************************************************************
 * @file mylib/s4743527_console.h
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

#ifndef S4743527_CONSOLE_H
#define S4743527_CONSOLE_H

#ifdef FreeRTOS
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

// Global variable
// Event group for user input.
extern EventGroupHandle_t s4743527GroupEventConsoleInput;

// Task Priority
#define TASK_CONSOLE_PRIORITY  (tskIDLE_PRIORITY + 1)

// Task Stack Allocation
#define TASK_CONSOLE_STACK_SIZE    (configMINIMAL_STACK_SIZE * 5)

// Mask for event group bits.
#define INPUT_EVT_MASK 0xFFFFFF

// Valid input from console. 
#define EMPTY '\0'
#define INPUT "QWERTYASDFGHZXCVBN12345"

#endif

// Function prototypes

// Converts an ASCII hexadecimal value to binary hexadecimal value.
extern int s4743527_lib_console_ascii2hex(char value);

// Converts a digit to its ASCII equivalent character.
extern char s4743527_lib_console_dec2ascii(int value);

#ifdef FreeRTOS
// Intialises the RCM console task and event group bits.
extern void s4743527_tsk_console_init(void);
#endif

#endif