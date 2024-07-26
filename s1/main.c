/**
  ******************************************************************************
  * @file    s1/main.c
  * @author  Hamza K
  * @date    21022024
  * @brief   Displays Fibonacci sequence in binary on LED bar display, uses
  *          button to invert upper 5 bits, and pin D10 toggles every second.
  ******************************************************************************
  */

#include "board.h"
#include "processor_hal.h"
#include <stdbool.h>
#include "s4743527_lta1000g.h"
#include "s4743527_mfs_pb.h"

// Bits to invert in inverted mode.
#define INVERT_MASK 0b1111100000

// Highest number in Fibonacci sequence to display.
#define MAX_FIB_NUMBER 610

// Interval (s) to update heartbeat and Fibonacci sequence display.
#define INTERVAL 1000

// Pin of output heartbeat
#define HEARTBEAT_PIN 10

// Struct for storing parameters for Fibonacci calculator function.
struct Fibonacci {
    uint16_t fibCurrent;
    uint16_t fibBefore;
    uint8_t fibTerm;
};

// Function Prototypes
void hardware_init(void);
uint16_t fibonacci_calculator(struct Fibonacci* fibonacciTerms);

int main(void)  {

    // Initalise board and hardware modules
    HAL_Init();
    hardware_init();

    // Initialise Fibonacci struct.
    struct Fibonacci fibonacciTerms;
    fibonacciTerms.fibCurrent = 0;
    fibonacciTerms.fibBefore = 0;
    fibonacciTerms.fibTerm = 0;

    bool invertedMode = false;

    // Display 0th term of Fibonacci sequence.
    s4743527_reg_lta1000g_write(fibonacci_calculator(&fibonacciTerms));
    
    uint32_t prevTime = 0;

    while (1) {
        // Calculate next Fibonacci number every second.
        if (HAL_GetTick() - prevTime >= INTERVAL) {

            GPIOD->ODR ^= (0x01 << HEARTBEAT_PIN); // Toggle heartbeat

            // Reset Fibonacci sequence at 15th term.
            if (fibonacciTerms.fibCurrent == MAX_FIB_NUMBER) {

                fibonacciTerms.fibCurrent = 0;
                fibonacciTerms.fibBefore = 0;
                fibonacciTerms.fibTerm = 0;
            }

            // Display next number in Fibonacci sequence.
            if (!invertedMode) {

                s4743527_reg_lta1000g_write(
                        fibonacci_calculator(&fibonacciTerms));
            } else {

                s4743527_reg_lta1000g_invert(
                        fibonacci_calculator(&fibonacciTerms), INVERT_MASK);
            }
            
            prevTime = HAL_GetTick();
        }

        // Switch to invert mode if button was pressed.
        if (s4743527_reg_mfs_pb_press_get(S4743527_REG_MFA_PB_S1)) {

            if (!invertedMode) {

                invertedMode = true;
                s4743527_reg_lta1000g_invert(fibonacciTerms.fibCurrent,
                        INVERT_MASK);
            } else {

                invertedMode = false;
                s4743527_reg_lta1000g_write(fibonacciTerms.fibCurrent);
            }
            s4743527_reg_mfs_pb_press_reset(S4743527_REG_MFA_PB_S1);
        }

    }
    return 0;
}

/**
 * Initialises hardware
 * 
 * Returns: None
 */
void hardware_init(void) {

    // Initialise LED Bar pins.
    s4743527_reg_lta1000g_init();

    // Initialise pushbutton pins and interrupt.
    s4743527_reg_mfs_pb_init(S4743527_REG_MFA_PB_S1);

    // Initialise pin D10 for heartbeat
    __GPIOD_CLK_ENABLE();

    // Clear and set to output
    GPIOD->MODER &= ~(0x03 << (HEARTBEAT_PIN * 2));
    GPIOD->MODER |= (0x01 << (HEARTBEAT_PIN * 2));

    // Clear for output push-pull
    GPIOD->OTYPER &= ~(0x01 << HEARTBEAT_PIN);

    // Clear and set to high speed
    GPIOD->OSPEEDR &= ~(0x03 << (HEARTBEAT_PIN * 2));
    GPIOD->OSPEEDR |= (0x02 << (HEARTBEAT_PIN * 2));

    // Clear and set to pull down.
    GPIOD->PUPDR &= ~(0x03 << (HEARTBEAT_PIN * 2));
    GPIOD->PUPDR |= (0x02 << (HEARTBEAT_PIN * 2));
}

/**
 * Calculate next number in Fibonacci sequence and updates Fibonacci struct.
 * 
 * fibonacciTerms: a struct containing previous and current term of 
 *     Fibonacci sequence.
 * 
 * Returns: the next number in Fibonacci sequence.
 */
uint16_t fibonacci_calculator(struct Fibonacci* fibonacciTerms) {

    if (fibonacciTerms->fibTerm == 0) {

        fibonacciTerms->fibTerm++;
        return 0;
    } else if (fibonacciTerms->fibTerm == 1) {

        fibonacciTerms->fibCurrent = 1;
        fibonacciTerms->fibBefore = 0;
        fibonacciTerms->fibTerm++;
        return 1;
    } else {

        // Calculate next Fibonacci number and update struct.
        uint16_t newFib = fibonacciTerms->fibCurrent +
                fibonacciTerms->fibBefore;
        fibonacciTerms->fibBefore = fibonacciTerms->fibCurrent;
        fibonacciTerms->fibCurrent = newFib;
        fibonacciTerms->fibTerm++;
        return newFib;
    }
}