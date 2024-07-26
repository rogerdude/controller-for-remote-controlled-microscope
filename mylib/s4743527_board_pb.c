/** 
 **************************************************************
 * @file mylib/s4743527_board_pb.c
 * @author Hamza K
 * @date 02052024
 * @brief USER Pushbutton on board.
 ***************************************************************
 * EXTERNAL FUNCTIONS
 ***************************************************************
 * s4743527_reg_board_pb_init() - Initialises GPIO, interrupt, 
 *                                and semaphore.
 *************************************************************** 
 */
#include "s4743527_board_pb.h"
#include "processor_hal.h"
#include <stdbool.h>
#include "FreeRTOS.h"
#include "semphr.h"

// Global variables
// Semaphore for USER button.
SemaphoreHandle_t s4743527SemaphorePushbutton;

/**
 * Initialises GPIO pins for USER pushbutton.
 * 
 * Returns: None
 */
extern void s4743527_reg_board_pb_init(void) {
    
    // Enable clock for port C
    __GPIOC_CLK_ENABLE();

    // Enable EXTI clock
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    // Initialise USER pushbutton on board
    GPIOC->MODER &= ~(0x03 << (USER_BUTTON_PIN * 2)); // Clear for input mode
    GPIOC->OSPEEDR &= ~(0x03 << (USER_BUTTON_PIN * 2)); // Clear bits for speed
    GPIOC->OSPEEDR |= (0x02 << (USER_BUTTON_PIN * 2)); // Set to high speed
    GPIOC->PUPDR &= ~(0x03 << (USER_BUTTON_PIN * 2)); // Clear for no push/pull

    // Initialise interrupt
    SYSCFG->EXTICR[3] &= ~SYSCFG_EXTICR4_EXTI13; // Clear bits
    SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC; // Set interrupts on PC13
    EXTI->RTSR |= EXTI_RTSR_TR13; // Enable rising edge.
    EXTI->FTSR |= EXTI_FTSR_TR13; // Enable falling edge.
    EXTI->IMR |= EXTI_IMR_IM13; // Enable external interrupt

    // Set priority to 10 and enable interrupt callback.
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 10, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

    // Create semaphore for USER pushbutton.
    s4743527SemaphorePushbutton = xSemaphoreCreateBinary();
}

/**
 * Interrupt service routine for when button is pressed.
 * 
 * Returns: None
 */
void user_pb_isr(void) {

    if ((GPIOC->IDR & (0x01 << USER_BUTTON_PIN)) == (0x01 << USER_BUTTON_PIN)) {

        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        
        // Check if semaphore exists
        if (s4743527SemaphorePushbutton != NULL) {

            // Give PB Semaphore from ISR
            xSemaphoreGiveFromISR(s4743527SemaphorePushbutton, &xHigherPriorityTaskWoken);
        }

        // Perform context switching, if required.
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}


/**
 * Interrupt handler for when USER button is pressed.
 * 
 * Returns: None
 */
void EXTI15_10_IRQHandler(void) {

    NVIC_ClearPendingIRQ(EXTI15_10_IRQn);

    if ((EXTI->PR & EXTI_PR_PR13) == EXTI_PR_PR13) {

        EXTI->PR |= EXTI_PR_PR13; // Clear interrupt flag

        user_pb_isr();
    }
}