/** 
 **************************************************************
 * @file mylib/s4743527_sysmon.h
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

#ifndef S4743527_SYSMON_H
#define S4743527_SYSMON_H

#include "processor_hal.h"

#define SYSMON_NUM_OF_PINS 3

// Ports and pins for system monitor in order of channel 0, 1, 2.
#define SYSMON_PORTS {GPIOD, GPIOG, GPIOG}
#define SYSMON_PINS {10, 7, 4}

// Macro functions for setting and clearing sysmon pins.
#define S4743527_REG_SYSMON_CHANO_CLR() GPIOD->ODR &= ~(0x01 << 10)
#define S4743527_REG_SYSMON_CHANO_SET() GPIOD->ODR |= (0x01 << 10)
#define S4743527_REG_SYSMON_CHAN1_CLR() GPIOG->ODR &= ~(0x01 << 7)
#define S4743527_REG_SYSMON_CHAN1_SET() GPIOG->ODR |= (0x01 << 7)
#define S4743527_REG_SYSMON_CHAN2_CLR() GPIOG->ODR &= ~(0x01 << 4)
#define S4743527_REG_SYSMON_CHAN2_SET() GPIOG->ODR |= (0x01 << 4)

// Function prototypes
// Initialises GPIO pins for logic analyser system monitor.
extern void s4743527_sysmon_init();

#endif