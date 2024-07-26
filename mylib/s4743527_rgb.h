/** 
 **************************************************************
 * @file mylib/s4743527_rgb.h
 * @author Hamza K
 * @date 29022024
 * @brief RGB LED Driver
 * REFERENCE: csse3010_mylib_reg_rgb.pdf 
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4743527_reg_rgb_init() - Initialise GPIO and PWM for LED
 * s4743527_reg_rgb_brightness_write() - Change brightness level
 * s4743527_reg_rgb_brightness_read() - Read brightness level
 * s4743527_reg_rgb_colour_set() - Set colour of RGB LED
 * s4743527_reg_rgb_colour_get() - Get colour of RGB LED
 ***************************************************************
 */

#ifndef S4743527_RGB_H
#define S4743527_RGB_H

// Frequency of timer
#define TIMER_COUNTER_FREQ 10000

// Period of the required PWM frequency = 1/freqency_PWM
#define PWM_PERIOD_SECONDS 0.01

// Period in ticks 2*(PWM_PERIOD_SECONDS/(1/TIMER_COUNTER_FREQ))
#define PWM_PERIOD_TICKS 200

// Number of primary colours
#define NUM_OF_COLOUR_PINS 3

// Ports and pins used for RGB LED in order of blue, green, red
#define RGB_LED_PORTS {GPIOF, GPIOF, GPIOE}
#define RGB_LED_PINS {15, 10, 11}

// Colour functions for changing LED colour
#define S4743527_REG_RGB_BLACK()    s4743527_reg_rgb_colour_set(0x07)
#define S4743527_REG_RGB_WHITE()    s4743527_reg_rgb_colour_set(0x00)
#define S4743527_REG_RGB_RED()      s4743527_reg_rgb_colour_set(0x03)
#define S4743527_REG_RGB_GREEN()    s4743527_reg_rgb_colour_set(0x05)
#define S4743527_REG_RGB_BLUE()     s4743527_reg_rgb_colour_set(0x06)
#define S4743527_REG_RGB_YELLOW()   s4743527_reg_rgb_colour_set(0x01)
#define S4743527_REG_RGB_CYAN()     s4743527_reg_rgb_colour_set(0x04)
#define S4743527_REG_RGB_MAGENTA()  s4743527_reg_rgb_colour_set(0x02)

// Minimum and maximum levels of brightness.
#define MIN_BRIGHTNESS 0
#define MAX_BRIGHTNESS 100

// Initial colour of RGB LED (black)
#define INITIAL_COLOUR 0x07

// Function prototypes
// Initialises GPIO pins and PWM for RGB LED.
extern void s4743527_reg_rgb_init(void);

// Changes PWM duty cycle based on brightness level.
extern void s4743527_reg_rgb_brightness_write(int level);

// Gets the current brightness level.
extern int s4743527_reg_rgb_brightness_read(void);

// Sets the colour of the RGB LED.
extern void s4743527_reg_rgb_colour_set(unsigned char rgb_mask);

// Gets the current colour of RGB LED.
extern int s4743527_reg_rgb_colour_get(void);

#endif