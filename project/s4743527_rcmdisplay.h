/** 
 **************************************************************
 * @file project/s4743527_rcmdisplay.h
 * @author Hamza K
 * @date 08052024
 * @brief Task funciton for RCM Display
 * REFERENCE: csse3010_project.pdf
 ***************************************************************
 * EXTERNAL FUNCTIONS
 ***************************************************************
 * s4743527_tsk_rcmdisplay_init() - Initialises task for RCM display.
 *************************************************************** 
 */

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

// Global variable
// Handle for queue for position data
extern QueueHandle_t s4743527QueueDisplayData;
// Handle for queue for key pressed
extern QueueHandle_t s4743527QueueDisplayKey;

// Symbols for borders
#define HORIZONTAL_SYMBOL "—"
#define VERTICAL_SYMBOL "|"

// Key pressed message for display
#define KEY_PRESSED_MSG "Key Pressed: "

// Symbol for RCM position
#define RCM_POSITION_SYMBOL "+"

// Symbol for empty space
#define EMPTY_SYMBOL " "

// Initial x and y positions on display
#define INITIAL_X 2
#define INITIAL_Y 102

// Task Priority
#define TASK_RCM_DISPLAY_PRIORITY  (tskIDLE_PRIORITY + 1)

// Task Stack Allocation
#define TASK_RCM_DISPLAY_STACK_SIZE    (configMINIMAL_STACK_SIZE * 10)

// Struct for RCM position data.
typedef struct {
    int xPos;
    int yPos;
    int zPos;
    int zoom;
    int rotate;
} RCMData;

// Function Prototype
// Intialises task for RCM display.
extern void s4743527_tsk_rcmdisplay_init(void);