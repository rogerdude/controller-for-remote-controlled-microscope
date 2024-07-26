/**
  ******************************************************************************
  * @file    s2/main.c
  * @author  Hamza K
  * @date    29022024
  * @brief   Controls colour and brightness of RGB LED, where colour is changed
  *          with button, and brightness is changed using trimpot and its level
  *          displayed on LED bar display.
  ******************************************************************************
  */

#include "board.h"
#include "processor_hal.h"
#include "s4743527_mfs_trimpot.h"
#include "s4743527_rgb.h"
#include "s4743527_mfs_pb.h"
#include "s4743527_lta1000g.h"

// Sequence of colours
#define BLACK   0
#define RED     1
#define GREEN   2
#define BLUE    3
#define CYAN    4
#define MAGENTA 5
#define YELLOW  6
#define WHITE   7

// Difference in ADC required for one turn
#define TURN_DURATION 250

// Interval to increase or decrease brightness level per turn
#define BRIGHTNESS_INTERVAL 10

// Function prototypes
void hardware_init(void);
int adc_to_brightness(int previousAdc);
void colourChanger(uint8_t colourSequence);

int main(void) {

    HAL_Init(); // Initialise board
    hardware_init(); // Initialise hardware modules

    // Initialise BrightnessParameters struct.
    int previousAdc = s4743527_reg_mfs_trimpot_get();

    // Set first colour to black.
    uint8_t colourSequence = BLACK;
    colourChanger(colourSequence);

    while (1) {

        // Change brightness level if trimpot is turned once.
        previousAdc = adc_to_brightness(previousAdc);

        // Display brightness level on LED bar display.
        if (s4743527_reg_rgb_brightness_read() == MIN_BRIGHTNESS) {
            s4743527_reg_lta1000g_write(0);
        } else {

            s4743527_reg_lta1000g_write(0x01 <<
                    ((s4743527_reg_rgb_brightness_read() / 10) - 1));
        }

        // Change colour if button is pressed.
        if (s4743527_reg_mfs_pb_press_get(S4743527_REG_MFA_PB_S1)) {

            if ((++colourSequence) == (WHITE + 1)) {
                colourSequence = BLACK;
            }
            colourChanger(colourSequence);
            s4743527_reg_mfs_pb_press_reset(S4743527_REG_MFA_PB_S1);
        }
    }

    return 0;
}

/**
 * Updates brightness level based on ADC value.
 * 
 * previousAdc: previous ADC value from trimpot to determine whether trimpot
 *         has been turned once.
 * 
 * Returns: the previous ADC value from trimpot for future comparisons 
 *         for one turn.
 */
int adc_to_brightness(int previousAdc) {
    
    // Check if trimpot has been turned once.
    if ((previousAdc - s4743527_reg_mfs_trimpot_get()) >= TURN_DURATION) {
        
        // Update previous ADC value of trimpot for future comparison.
        previousAdc = s4743527_reg_mfs_trimpot_get();

        // Change brightness level
        if (s4743527_reg_rgb_brightness_read() == MIN_BRIGHTNESS) {
            s4743527_reg_rgb_brightness_write(MAX_BRIGHTNESS);
        } else {
            s4743527_reg_rgb_brightness_write(
                    s4743527_reg_rgb_brightness_read() - BRIGHTNESS_INTERVAL);
        }
    } else if ((previousAdc - s4743527_reg_mfs_trimpot_get()) <=
            ((-1) * TURN_DURATION)) {
        
        previousAdc = s4743527_reg_mfs_trimpot_get();

        if (s4743527_reg_rgb_brightness_read() == MAX_BRIGHTNESS) {
            s4743527_reg_rgb_brightness_write(MIN_BRIGHTNESS); 
        } else {
            s4743527_reg_rgb_brightness_write(
                    s4743527_reg_rgb_brightness_read() + BRIGHTNESS_INTERVAL);
        }
    }

    return previousAdc;
}

/**
 * Changes the colour of the RGB LED based on the sequence of colours.
 * 
 * colourSequence: the current index in the sequence of colours to change to.
 * 
 * Returns: None
 */
void colourChanger(uint8_t colourSequence) {
    switch (colourSequence) {
        
        case BLACK:
            S4743527_REG_RGB_BLACK();
            break;

        case RED:
            S4743527_REG_RGB_RED();
            break;
        
        case GREEN:
            S4743527_REG_RGB_GREEN();
            break;
        
        case BLUE:
            S4743527_REG_RGB_BLUE();
            break;
        
        case CYAN:
            S4743527_REG_RGB_CYAN();
            break;
        
        case MAGENTA:
            S4743527_REG_RGB_MAGENTA();
            break;
        
        case YELLOW:
            S4743527_REG_RGB_YELLOW();
            break;
        
        case WHITE:
            S4743527_REG_RGB_WHITE();
            break;
        
        default:
            break;
    }
}

/**
 * Initialises hardware
 * 
 * Returns: None
 */
void hardware_init(void) {
    
    // Initialise trimpot
    s4743527_reg_mfs_trimpot_init();

    // Initialise RGB LED
    s4743527_reg_rgb_init();

    // Initialise pushbutton s1
    s4743527_reg_mfs_pb_init(S4743527_REG_MFA_PB_S1);

    // Initialise LED bar display
    s4743527_reg_lta1000g_init();
}