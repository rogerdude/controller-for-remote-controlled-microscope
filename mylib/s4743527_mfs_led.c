/** 
 **************************************************************
 * @file mylib/s4743527_mfs_led.c
 * @author Hamza K
 * @date 02042024
 * @brief MFS LEDs Register Driver
 * REFERENCE: csse3010_mylib_reg_mfs_led.pdf
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4743527_reg_mfs_led_init() - Initialises GPIO for LEDs.
 *************************************************************** 
 */

#include "s4743527_mfs_led.h"
#include "processor_hal.h"

// Global variables
// Ports used for MFS LEDs in order of D1, D2, D3, D4
static GPIO_TypeDef* ledPorts[NUM_OF_LEDS] = MFS_LED_PORTS;
// Pins used for MFS LEDs in order of D1, D2, D3, D4
static uint8_t ledPins[NUM_OF_LEDS] = MFS_LED_PINS;

/**
 * Initialises GPIO for MFS LEDs
 * 
 * Returns: None
 */
extern void s4743527_reg_mfs_led_init() {

    // Enable clock for ports A and D
    __GPIOA_CLK_ENABLE();
    __GPIOD_CLK_ENABLE();

    for (uint8_t i = 0; i < NUM_OF_LEDS; i++) {
        // Clear bits and set to output.
        ledPorts[i]->MODER &= ~(0x03 << (ledPins[i] * 2));
        ledPorts[i]->MODER |= (0x01 << (ledPins[i] * 2));

        // Set to output push pull
        ledPorts[i]->OTYPER &= ~(0x01 << ledPins[i]);

        // Clear bits and set to high speed.
        ledPorts[i]->OSPEEDR &= ~(0x03 << (ledPins[i] * 2));
        ledPorts[i]->OSPEEDR |= (0x02 << (ledPins[i] * 2));

        // Clear bits and set to pull-up
        ledPorts[i]->PUPDR &= ~(0x03 << (ledPins[i] * 2));
        ledPorts[i]->PUPDR |= (0x01 << (ledPins[i] * 2));
    }

    // Set all LEDs to OFF
    S4743527_REG_MFS_LED_D1_OFF();
    S4743527_REG_MFS_LED_D2_OFF();
    S4743527_REG_MFS_LED_D3_OFF();
    S4743527_REG_MFS_LED_D4_OFF();
}