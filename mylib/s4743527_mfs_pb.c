/** 
 **************************************************************
 * @file mylib/s4743527_mfs_pb.c
 * @author Hamza K
 * @date 23022024
 * @brief MFS Pushbutton Register Driver
 * REFERENCE: csse3010_mylib_reg_mfs_pushbutton.pdf
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4743527_reg_mfs_pb_init() - Initialise pushbutton pin and interrupt.
 * s4743527_reg_mfs_pb_isr() - Interrupt service routine for pb.
 * s4743527_reg_mfs_pb_press_get() - Gets press counter value of pb.
 * s4743527_reg_mfs_pb_press_reset() - Resets press counter value.
 ***************************************************************
 */

#include "s4743527_mfs_pb.h"
#include "board.h"
#include "processor_hal.h"
#include <stdbool.h>

#ifdef FreeRTOS
#include "FreeRTOS.h"
#include "semphr.h"

// Semaphore for button S1
SemaphoreHandle_t pbSemaphore;

#endif

// Global variables
// Counter for number of times each button is pressed.
static int MfPbPressCounter[NUMBER_OF_BUTTONS];
// Stores time of button press.
static uint32_t prevTime;
// Indicates whether button is pressed.
static bool pressed;

/**
 * Initialises global variables and GPIO pins for specified pushbutton and
 * its interrupt.
 * 
 * pb_select: the pushbutton to enable
 * 
 * Returns: None
 */
extern void s4743527_reg_mfs_pb_init(int pb_select) {

    // Initialise time, pb counter to 0, and pressed.
    prevTime = 0;
    pressed = false;
    for (uint8_t i = 0; i < NUMBER_OF_BUTTONS; i++) {
        MfPbPressCounter[i] = 0;
    }

    // Enable clock for pushbutton port C
    __GPIOC_CLK_ENABLE();

    // Enable EXTI clock
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    
    // Initialise S1 pushbutton.
    GPIOC->MODER &= ~(0x03 << (BUTTON_S1_PIN * 2)); // Clear for input mode
    GPIOC->OSPEEDR &= ~(0x03 << (BUTTON_S1_PIN * 2)); // Clear bits for speed
    GPIOC->OSPEEDR |= (0x02 << (BUTTON_S1_PIN * 2)); // Set to high speed
    GPIOC->PUPDR &= ~(0x03 << (BUTTON_S1_PIN * 2)); // Clear for no push/pull

    // Initialise interrupt
    SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI0; // Clear bits
    SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PC; // Set interrupts on Port C
    EXTI->RTSR |= EXTI_RTSR_TR0; // Enable rising edge.
    EXTI->FTSR |= EXTI_FTSR_TR0; // Enable falling edge.
    EXTI->IMR |= EXTI_IMR_IM0; // Enable external interrupt

    // Set priority to 10 and enable interrupt callback.
    HAL_NVIC_SetPriority(EXTI0_IRQn, 10, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);

    // Create semaphore
    #ifdef FreeRTOS
    pbSemaphore = xSemaphoreCreateBinary();
    #endif
}

/**
 * Interrupt service routine to read a button press.
 * 
 * pb_select: the pushbutton to read.
 * 
 * Returns: None
 */
extern void s4743527_reg_mfs_pb_isr(int pb_select) {

    // Check if it has been more than 50ms since last release.
    if (((HAL_GetTick() - prevTime) > DEBOUNCE_PERIOD) && !pressed) {

        // Increase counter if button is pressed.
        if ((GPIOC->IDR & (0x01 << BUTTON_S1_PIN)) == (BUTTON_PRESSED)) {
            pressed = true;
            MfPbPressCounter[BUTTON_S1_INDEX]++;

#ifdef FreeRTOS
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
                
            if (pbSemaphore != NULL) {	// Check if semaphore exists
                xSemaphoreGiveFromISR(pbSemaphore, &xHigherPriorityTaskWoken); // Give PB Semaphore from ISR
            }

            // Perform context switching, if required.
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
#endif
        }

    } else {
        pressed = false;
    }

    // Set prevTime to current time to ignore bouncing for next 50ms.
    prevTime = HAL_GetTick();
} 

/**
 * Interrupt handler for when button S1 is pressed.
 * 
 * Returns: None
 */
void EXTI0_IRQHandler(void) {

    NVIC_ClearPendingIRQ(EXTI0_IRQn);

    if ((EXTI->PR & EXTI_PR_PR0) == EXTI_PR_PR0) {

        EXTI->PR |= EXTI_PR_PR0; // Clear interrupt flag

        s4743527_reg_mfs_pb_isr(S4743527_REG_MFA_PB_S1);
    }
}

/**
 * Returns whether the specified pushbutton has been pressed.
 * 
 * pb_select: the pushbutton to check whether it has been pressed.
 * 
 * Returns: the number of times the pushbutton has been pressed.
 */
extern int s4743527_reg_mfs_pb_press_get(int pb_select) {

    switch (pb_select) {

        case S4743527_REG_MFA_PB_S1:
            return MfPbPressCounter[BUTTON_S1_INDEX];

        case S4743527_REG_MFA_PB_S2:
            return MfPbPressCounter[BUTTON_S2_INDEX];
        
        case S4743527_REG_MFA_PB_S3:
            return MfPbPressCounter[BUTTON_S3_INDEX];
        
        default:
            return 0;
    }
}

/**
 * Resets the specified pushbutton's press count to zero.
 * 
 * pb_select: the pushbutton to reset count for.
 * 
 * Returns: None
 */
extern void s4743527_reg_mfs_pb_press_reset(int pb_select) {

    switch (pb_select) {
        
        case S4743527_REG_MFA_PB_S1:
            MfPbPressCounter[BUTTON_S1_INDEX] = 0;
            break;
        
        case S4743527_REG_MFA_PB_S2:
            MfPbPressCounter[BUTTON_S2_INDEX] = 0;
            break;
        
        case S4743527_REG_MFA_PB_S3:
            MfPbPressCounter[BUTTON_S3_INDEX] = 0;
            break;
        
        default:
            break;
    }
}