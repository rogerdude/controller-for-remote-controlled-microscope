/** 
 **************************************************************
 * @file mylib/s4743527_lta1000g.h
 * @author Hamza K
 * @date 21022024
 * @brief LED Bar Display Register Driver
 * REFERENCE: csse3010_mylib_reg_lta1000g.pdf
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4743527_reg_lta1000g_init() - Initialise LED Bar pins.
 * s4743527_reg_lta1000g_write() - Write LED Bar segment to 0 or 1.
 * s4743527_reg_lta1000g_invert() - Inverts LED Bar segments.
 *************************************************************** 
 */

#ifndef S4743527_LTA1000G_H
#define S4743527_LTA1000G_H

#define NUM_OF_SEGMENTS 10
#define MIN_SEGMENT 0
#define MAX_SEGMENT 9

// The port each segment is connected to in order of segments 0 to 9.
#define LED_BAR_PORTS {GPIOE, GPIOG, GPIOG, GPIOG, GPIOG, GPIOF, \
    GPIOE, GPIOG, GPIOG, GPIOG}

// The pin number each segment is connected to in order of segments 0 to 9.
#define LED_BAR_PINS {6, 15, 10, 13, 11, 11, 0, 8, 5, 6}


// Function prototypes
// Initialises GPIO pins for all LED bar segments.
extern void s4743527_reg_lta1000g_init();

// Displays specified number on LED bar display in binary.
extern void s4743527_reg_lta1000g_write(unsigned short value);

// Displays inverted number on LED Bar display in binary.
extern void s4743527_reg_lta1000g_invert(unsigned short value,
        unsigned short mask);

#endif