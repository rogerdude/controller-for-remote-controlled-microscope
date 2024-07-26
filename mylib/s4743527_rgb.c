/** 
 **************************************************************
 * @file mylib/s4743527_rgb.c
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

#include "s4743527_rgb.h"
#include "processor_hal.h"

// Global variables
// Ports used for RGB LED in order of blue, green, red
static GPIO_TypeDef* rgbPorts[NUM_OF_COLOUR_PINS] = RGB_LED_PORTS;
// Pins used for RGB LED in order of blue, green, red
static uint8_t rgbPins[NUM_OF_COLOUR_PINS] = RGB_LED_PINS;
// Brightness level of RGB LED
static uint8_t brightnessLevel;
// Current colour of RGB LED in format 0bRGB
static uint8_t currentColour;

/**
 * Initialises GPIO pins and PWM for RGB LED.
 * 
 * Returns: None
 */
extern void s4743527_reg_rgb_init(void) {

    // Initialise currentColour variable to black.
    currentColour = INITIAL_COLOUR;

    // Enable clock for port E and F
    __GPIOE_CLK_ENABLE();
    __GPIOF_CLK_ENABLE();

    for (uint8_t i = 0; i < NUM_OF_COLOUR_PINS; i++) {
        // Clear bits and set to output.
        rgbPorts[i]->MODER &= ~(0x03 << (rgbPins[i] * 2));
        rgbPorts[i]->MODER |= (0x01 << (rgbPins[i] * 2));

        // Set to output push pull
        rgbPorts[i]->OTYPER &= ~(0x01 << rgbPins[i]);

        // Clear bits and set to high speed.
        rgbPorts[i]->OSPEEDR &= ~(0x03 << (rgbPins[i] * 2));
        rgbPorts[i]->OSPEEDR |= (0x02 << (rgbPins[i] * 2));

        // Clear bits and set to pull-down
        rgbPorts[i]->PUPDR &= ~(0x03 << (rgbPins[i] * 2));
        rgbPorts[i]->PUPDR |= (0x02 << (rgbPins[i] * 2));
    }

    // Initialise PWM
    // Clear and set to alternate function mode
    GPIOE->MODER &= ~(0x03 << (9 * 2)); 
    GPIOE->MODER |= (0x02 << (9 * 2));

    // Clear and set to high speed
    GPIOE->OSPEEDR &= ~(0x03 << (9 * 2));
    GPIOE->OSPEEDR |= (0x02 << (9 * 2));

    // Set to no pull-up, pull-down
    GPIOE->PUPDR &= ~(0x03 << (9 * 2));
 
    // Clear and set alternate function
    GPIOE->AFR[1] &= ~(0x0F << ((9 - 8) * 4));
    GPIOE->AFR[1] |= (GPIO_AF1_TIM1 << ((9 - 8) * 4));

    // Enable TIM1 clock
    __TIM1_CLK_ENABLE();

    // Set prescalar
    TIM1->PSC = ((SystemCoreClock / 2) / TIMER_COUNTER_FREQ) - 1;

    // Set to counting up (0)
    TIM1->CR1 &= ~TIM_CR1_DIR;

    // Set ARR
    TIM1->ARR = PWM_PERIOD_TICKS;

    // Set output compare value to 200 for 0% brightness
    TIM1->CCR1 = PWM_PERIOD_TICKS;
    brightnessLevel = 0;

    TIM1->CCMR1 &= ~(TIM_CCMR1_OC1M); // Clear channel 1
    TIM1->CCMR1 |= (0x6 << 4); // Enable PWM mode 1
    TIM1->CCMR1 |= (TIM_CCMR1_OC1CE); // Enable output preload

    TIM1->CR1 |= (TIM_CR1_ARPE); // Set auto reload preload enable 
    TIM1->CCER |= TIM_CCER_CC1E; // Set CC1E Bit
	TIM1->CCER |= TIM_CCER_CC1NE; // Set CC1NE Bit for active low output

    // Set Main Output Enable (MOE) bit, Off-State Selection for Run mode
    // (OSSR) bit and Off-State Selection for Idle mode (OSSI) bit
	TIM1->BDTR |= TIM_BDTR_MOE | TIM_BDTR_OSSR | TIM_BDTR_OSSI; 

	TIM1->CR1 |= TIM_CR1_CEN; // Enable the counter
}

/**
 * Changes PWM duty cycle based on brightness level.
 * 
 * level: brightness level between 0 and 100 to change to.
 * 
 * Returns: None 
 */
extern void s4743527_reg_rgb_brightness_write(int level) {

    if (level < 0 || level > 100) {
        return;
    }
    
    brightnessLevel = level;
    TIM1->CCR1 = PWM_PERIOD_TICKS - (((level / 2) * PWM_PERIOD_TICKS) / 100);
}

/**
 * Gets the current brightness level.
 * 
 * Returns: current brightness level between 0 and 100.
 */
extern int s4743527_reg_rgb_brightness_read(void) {

    return brightnessLevel;
}

/**
 * Sets the colour of the RGB LED.
 * 
 * rgb_mask: the colour to set in form of 0bRGB.
 * 
 * Returns: None
 * 
 * (Note: RGB LED colours are active low)
 */
extern void s4743527_reg_rgb_colour_set(unsigned char rgb_mask) {

    currentColour = rgb_mask;

    for (uint8_t colour = 0; colour < NUM_OF_COLOUR_PINS; colour++) {
        if ((rgb_mask & (0x01 << colour)) == (0x01 << colour)) {
            rgbPorts[colour]->ODR |= (0x01 << rgbPins[colour]);
        } else {
            rgbPorts[colour]->ODR &= ~(0x01 << rgbPins[colour]);
        }
    }
}

/**
 * Gets the current colour of RGB LED.
 * 
 * Returns: colour of RGB LED in form of 0bRGB.
 * 
 * (Note: RGB LED colours are active low)
 */
extern int s4743527_reg_rgb_colour_get(void) {

    return currentColour;
}