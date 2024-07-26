/** 
 **************************************************************
 * @file mylib/s4743527_lta1000g.c
 * @author Hamza K
 * @date 21022024
 * @brief LED Bar Display Register Driver
 * REFERENCE: csse3010_mylib_reg_lta1000g.pdf
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4743527_reg_lta1000g_init() - Intialise LED Bar pins.
 * s4743527_reg_lta1000g_write() - Write LED Bar segment to 0 or 1.
 * s4743527_reg_lta1000g_invert() - Inverts LED Bar segments.
 *************************************************************** 
 */

#include "s4743527_lta1000g.h"
#include "processor_hal.h"

// Global Variables
// All GPIO ports for LED bar display in order of segments.
static GPIO_TypeDef* portType[NUM_OF_SEGMENTS] = LED_BAR_PORTS;
// All pin numbers for LED bar display in order of segments.
static uint8_t pinNumber[NUM_OF_SEGMENTS] = LED_BAR_PINS;

/**
 * Initialises GPIO pins for all LED bar segments.
 * 
 * Returns: None
 */
extern void s4743527_reg_lta1000g_init() {

    // Enable the GPIO Clock for port E, F, G
    __GPIOE_CLK_ENABLE();
    __GPIOF_CLK_ENABLE();
    __GPIOG_CLK_ENABLE();

    for (uint8_t pin = 0; pin < NUM_OF_SEGMENTS; pin++) {
        // Clear bits and set to output.
        portType[pin]->MODER &= ~(0x03 << (pinNumber[pin] * 2));
        portType[pin]->MODER |= (0x01 << (pinNumber[pin] * 2));

        // Set to output push pull
        portType[pin]->OTYPER &= ~(0x01 << pinNumber[pin]);

        // Clear bits and set to high speed.
        portType[pin]->OSPEEDR &= ~(0x03 << (pinNumber[pin] * 2));
        portType[pin]->OSPEEDR |= (0x02 << (pinNumber[pin] * 2));

        // Clear bits and set to pull-down
        portType[pin]->PUPDR &= ~(0x03 << (pinNumber[pin] * 2));
        portType[pin]->PUPDR |= (0x02 << (pinNumber[pin] * 2));
    }
}

/**
 * Sets the specified LED bar segment to low or high.
 * 
 * segment: the segment to change (between 0 and 9).
 * value: a 0 or 1 to set segment low or high.
 * 
 * Returns: None
 */
void lta1000g_seg_set(int segment, unsigned char value) {

    if (!value) {
        portType[segment]->ODR &= ~(0x01 << pinNumber[segment]);
    } else {
        portType[segment]->ODR |= (0x01 << pinNumber[segment]);
    }
}

/**
 * Displays specified number on LED bar display in binary.
 * 
 * value: the number to display.
 * 
 * Returns: None
 */
extern void s4743527_reg_lta1000g_write(unsigned short value) {

    for (uint8_t bit = 0; bit <= MAX_SEGMENT; bit++) {
        lta1000g_seg_set(bit, (value & (0x01 << bit)) >> bit);
    }
}

/**
 * Displays inverted number on LED Bar display in binary.
 * 
 * value: the number to display.
 * mask: the bits to invert.
 * 
 * Returns: None
 */
extern void s4743527_reg_lta1000g_invert(unsigned short value,
    unsigned short mask) {

    uint16_t invertedValue = value ^ mask;
    for (uint8_t bit = 0; bit <= MAX_SEGMENT; bit++) {
        lta1000g_seg_set(bit, (invertedValue & (0x01 << bit)) >> bit);
    }
}
