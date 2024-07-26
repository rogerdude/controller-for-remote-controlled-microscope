/** 
 **************************************************************
 * @file project/s4743527_rcmdisplay.c
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

#include "s4743527_rcmdisplay.h"
#include "s4743527_rgb.h"
#include "s4743527_lta1000g.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "debug_log.h"

// Global variable
// Handle for queue that receives rcm data to display.
QueueHandle_t s4743527QueueDisplayData;
// Handle for queue that receives key pressed.
QueueHandle_t s4743527QueueDisplayKey;

/**
 * Moves the position of the cursor on VT100 display.
 * 
 * xPos: the x coordinate to move the cursor.
 * yPos: the y coordinate to move the cursor.
 * 
 * Returns: None
 */
void display_move_cursor(int xPos, int yPos) {
    debug_log("\e[%d;%dH", yPos, xPos);
}

/**
 * Displays text at the specified position in VT100 display.
 * 
 * xPos: the x coordinate to move the cursor.
 * yPos: the y coordinate to move the cursor.
 * text: the text to display.
 * 
 * Returns: None
 */
void display_text(int xPos, int yPos, const signed char* text) {
    debug_log("\e[%d;%dH%s", yPos, xPos, text);
}

/**
 * Task for RCM to display on VT100.
 * 
 * Returns: None
 */
void display_task(void) {

    // Initialise queues
    s4743527QueueDisplayData = xQueueCreate(10, sizeof(RCMData));
    s4743527QueueDisplayKey = xQueueCreate(10, sizeof(char));

    RCMData data;
    char keyPressed;

    int oldXPos = 0;
    int oldYPos = 0;
    int oldZPos = 0;

    S4743527_REG_RGB_BLACK();

    // Clear screen
    debug_log("\e[2J");

    // Move cursor to top left
    debug_log("\e[H");

    // Print top and bottom borders
    for (uint8_t x = 1; x < 104; x++) {
        display_text(x, 1, HORIZONTAL_SYMBOL);
        display_text(x, 103, HORIZONTAL_SYMBOL);
    }

    // Print left and right borders
    for (uint8_t y = 1; y < 104; y++) {
        display_text(1, y, VERTICAL_SYMBOL);
        display_text(103, y, VERTICAL_SYMBOL);
    }

    // Print 'key pressed'
    display_text(110, 50, KEY_PRESSED_MSG);

    // Print initial position
    display_text(INITIAL_X, INITIAL_Y, RCM_POSITION_SYMBOL);
    display_move_cursor(INITIAL_X, INITIAL_Y);

    for (;;) {
        
        // Receive key pressed from console task and display it.
        if (xQueueReceive(s4743527QueueDisplayKey, &keyPressed, 10)) {
            debug_log("\e[%d;%dH%c", 50, 123, keyPressed);
        }

        // Receive RCM position data.
        if (xQueueReceive(s4743527QueueDisplayData, &data, 10)) {
            
            // Display position
            if (data.xPos != oldXPos || data.yPos != oldYPos) {
                
                // Replace old position with empty space
                display_text(INITIAL_X + (oldXPos / 2), INITIAL_Y - (oldYPos / 2), EMPTY_SYMBOL);

                // Print new position in terminal
                display_text(INITIAL_X + (data.xPos / 2), INITIAL_Y - (data.yPos / 2), RCM_POSITION_SYMBOL);

                // Move cursor back to position of rcm.
                display_move_cursor(INITIAL_X + (data.xPos / 2), INITIAL_Y - (data.yPos / 2));

                // Update old position to current position.
                oldXPos = data.xPos;
                oldYPos = data.yPos;
            }

            // Display colour on RGB LED.
            if (data.zPos == 99) {
                S4743527_REG_RGB_RED();
            } else if ((data.zPos - oldZPos) >= 1) {
                S4743527_REG_RGB_GREEN();
            } else if ((data.zPos - oldZPos) <= -1) {
                S4743527_REG_RGB_BLUE();
            }
            oldZPos = data.zPos;

            // Display rotate on LED bar.
            s4743527_reg_lta1000g_write(data.rotate);
        }

        vTaskDelay(50);
    }
}

/**
 * Intialises task for RCM display.
 * 
 * Returns: None
 */
extern void s4743527_tsk_rcmdisplay_init(void) {

    xTaskCreate((void*) &display_task, (const signed char *) "RCM Display",
            TASK_RCM_DISPLAY_STACK_SIZE, NULL, TASK_RCM_DISPLAY_PRIORITY, NULL);
}