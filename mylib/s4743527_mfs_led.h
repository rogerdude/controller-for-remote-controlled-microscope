/** 
 **************************************************************
 * @file mylib/s4743527_mfs_led.h
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

#ifndef S4743527_MFS_LED_H
#define S4743527_MFS_LED_H

#include "processor_hal.h"

// Number of LEDs on MFS
#define NUM_OF_LEDS 4

// Ports and pins for MFS LEDS in order of D1, D2, D3, D4
#define MFS_LED_PORTS {GPIOA, GPIOA, GPIOA, GPIOD}
#define MFS_LED_PINS {5, 6, 7, 14}

// Function defines for turning LED ON and OFF.
#define S4743527_REG_MFS_LED_D1_ON()        GPIOA->ODR &= ~(0x01 << 5)
#define S4743527_REG_MFS_LED_D1_OFF()       GPIOA->ODR |= (0x01 << 5)
#define S4743527_REG_MFS_LED_D1_TOGGLE()    GPIOA->ODR ^= (0x01 << 5)
#define S4743527_REG_MFS_LED_D2_ON()        GPIOA->ODR &= ~(0x01 << 6)
#define S4743527_REG_MFS_LED_D2_OFF()       GPIOA->ODR |= (0x01 << 6)
#define S4743527_REG_MFS_LED_D2_TOGGLE()    GPIOA->ODR ^= (0x01 << 6)
#define S4743527_REG_MFS_LED_D3_ON()        GPIOA->ODR &= ~(0x01 << 7)
#define S4743527_REG_MFS_LED_D3_OFF()       GPIOA->ODR |= (0x01 << 7)
#define S4743527_REG_MFS_LED_D4_ON()        GPIOD->ODR &= ~(0x01 << 14)
#define S4743527_REG_MFS_LED_D4_OFF()       GPIOD->ODR |= (0x01 << 14)

// Function prototypes
// Initialises GPIO for MFS LEDs
extern void s4743527_reg_mfs_led_init();

#endif