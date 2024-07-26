/** 
 **************************************************************
 * @file mylib/s4743527_mfs_trimpot.h
 * @author Hamza K
 * @date 29022024
 * @brief MFS Trimpot Driver
 * REFERENCE: csse3010_mylib_reg_mfs_trimpot.pdf
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4743527_reg_mfs_trimpot_init() - Initialise GPIO and ADC
 * s4743527_reg_mfs_trimpot_get() - Gets ADC value of Trimpot.
 ***************************************************************
 */

#ifndef S4743527_MFS_TRIMPOT_H
#define S4743527_MFS_TRIMPOT_H

// Pin number for trimpot on port A
#define TRIMPOT_INPUT_PIN 3

// Offset for ADC value for Trimpot
#define S4743527_REG_MFS_TRIMPOT_ZERO_CAL_OFFSET 0 

// Timeout for ADC conversion (ms)
#define ADC_CONVERSION_TIMEOUT 10

// Function prototypes
// Initialises GPIO pins and ADC for trimpot.
extern void s4743527_reg_mfs_trimpot_init(void);

// Reads trimpot's input voltage and converts to ADC value.
extern int s4743527_reg_mfs_trimpot_get(void);

#endif