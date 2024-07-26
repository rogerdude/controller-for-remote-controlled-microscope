/** 
 **************************************************************
 * @file mylib/s4743527_sysmon.c
 * @author Hamza K
 * @date 24042024
 * @brief Sysmon Register Driver
 * REFERENCE: csse3010_mylib_reg_sysmon.pdf
 ***************************************************************
 * EXTERNAL FUNCTIONS
 ***************************************************************
 * s4743527_sysmon_init() - Initialises GPIO for system monitor.
 *************************************************************** 
 */

#include "s4743527_sysmon.h"
#include "processor_hal.h"

// Global variables
// Ports for system monitor in order of channels 0, 1, 2.
static GPIO_TypeDef* portType[SYSMON_NUM_OF_PINS] = SYSMON_PORTS;
// Pins for system monitor in order of channels 0, 1, 2.
static int pinNumber[SYSMON_NUM_OF_PINS] = SYSMON_PINS;

/**
 * Initialises GPIO pins for logic analyser system monitor.
 * 
 * Returns: None
*/
extern void s4743527_sysmon_init() {
    
    // Enable clock for port D and G
    __GPIOD_CLK_ENABLE();
    __GPIOG_CLK_ENABLE();

    // Initialise pins
    for (uint8_t i = 0; i < SYSMON_NUM_OF_PINS; i++) {
        // Clear bits and set to output.
        portType[i]->MODER &= ~(0x03 << (pinNumber[i] * 2));
        portType[i]->MODER |= (0x01 << (pinNumber[i] * 2));

        // Set to output push pull
        portType[i]->OTYPER &= ~(0x01 << pinNumber[i]);

        // Clear bits and set to high speed.
        portType[i]->OSPEEDR &= ~(0x03 << (pinNumber[i] * 2));
        portType[i]->OSPEEDR |= (0x02 << (pinNumber[i] * 2));

        // Clear bits and set to pull-down
        portType[i]->PUPDR &= ~(0x03 << (pinNumber[i] * 2));
        portType[i]->PUPDR |= (0x02 << (pinNumber[i] * 2));
    }
}
