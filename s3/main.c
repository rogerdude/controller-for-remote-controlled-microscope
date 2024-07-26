/**
  ******************************************************************************
  * @file    s3/main.c
  * @author  Hamza K
  * @date    02042024
  * @brief   Hamming encoder and decoder which corrects 1 bit errors and detects
  *          2 bit errors, and displays encoded and decoded bytes on LED bar
  *          display.
  ******************************************************************************
  */

#include "board.h"
#include "processor_hal.h"
#include "s4743527_lta1000g.h"
#include "s4743527_mfs_pb.h"
#include "s4743527_console.h"
#include "s4743527_mfs_led.h"
#include "s4743527_hamming.h"
#include "debug_log.h"
#include <stdbool.h>

// Timer frequency
#define TIMER_COUNTER_FREQ 1000

// Period of the required PWM frequency = 1/freqency_PWM
#define PWM_PERIOD_SECONDS 0.05

// Period in ticks 2*(PWM_PERIOD_SECONDS/(1/TIMER_COUNTER_FREQ))
#define PWM_PERIOD_TICKS 100

// Empty input
#define EMPTY '\0'

// Index of lower and upper bytes in arrays.
#define LOWER 0
#define UPPER 1

#define INVALID -1

// States
#define S0 0    /* IDLE State */
#define S1 1    /* Encode State */
#define S2 2    /* Decode State */

// Global variables
// User input from console in ASCII.
static char input[2] = {EMPTY, EMPTY};

// Current state of program.
static int currentState = S0;

// Lower and upper encoded bytes to display on LED bar display.
static int ledBytes[2] = {0x00, 0x00};

// Index of ledBytes to display on LED Bar display.
static int displayByte = LOWER;

// Function prototypes
void hardware_init(void);
int validate_user_input(void);
int syndrome_error(int value);
int fsm_processing(int currentState);
void reset_leds(void);

int main(void) {

    HAL_Init(); // Initialise board
    hardware_init(); // Initialise hardware modules

    // Turn off all LEDs.
    reset_leds();

    while (1) {
        
    }

    return 0;
}

/**
 * Converts user input from ASCII to hex and validates it.
 * 
 * Returns: -1 if the input is invalid, otherwise returns the 
 *          hexadecimal format of input.
 */
int validate_user_input(void) {

    // Convert input to hex number and check if valid.
    int upperInput = s4743527_lib_console_ascii2hex(input[UPPER]);
    int lowerInput = s4743527_lib_console_ascii2hex(input[LOWER]);

    if (upperInput == INVALID || lowerInput == INVALID) {
        return INVALID;
    }

    int inputByte = lowerInput | (upperInput << 4);
    return inputByte;
}

/**
 * Calculates the syndrome bits to check for error.
 * 
 * value: a hamming encoded byte
 * 
 * Returns: True if there was 1 bit or 2 bit error, and 
 *          returns false if no error.
 */
int syndrome_error(int value) {
    uint8_t d0, d1, d2, d3; // Bits 0-3 of value.
    uint8_t h0, h1, h2; // Hamming bits
    uint8_t s0, s1, s2; // Syndrome bits

    // Extract value bits
    d0 = !!(value & (0x01 << 4));
    d1 = !!(value & (0x01 << 5));
    d2 = !!(value & (0x01 << 6));
    d3 = !!(value & (0x01 << 7));
    uint8_t values[4] = {d0, d1, d2, d3};

    // Extract hamming bits
    h0 = !!(value & (0x01 << 1));
    h1 = !!(value & (0x01 << 2));
    h2 = !!(value & (0x01 << 3));

    // Calculate syndrome bits
    s0 = d1 ^ d2 ^ d3 ^ h0;
    s1 = d0 ^ d2 ^ d3 ^ h1;
    s2 = d0 ^ d1 ^ d3 ^ h2;
    uint8_t s = s0 | (s1 << 1) | (s2 << 2);

    if (s) {
        return true;
    }
    
    return false;
}

/**
 * Cyclic executive which processes inputs and determines next state.
 * 
 * currentState: the current state of the program (S0, S1, or S2)
 * 
 * Returns: the next state of the program (S0, S1, or S2)
 */
int fsm_processing(int currentState) {

    int nextState;

    switch (currentState) {
        
        case S0:
            reset_leds();

            if (input[LOWER] == '#' && input[UPPER] == '#') {
                nextState = S1;
            } else if (input[LOWER] == '*' && input[UPPER] == '*') {
                nextState = S2;
            } else {
                nextState = S0;
            }

            break;
        
        case S1:
            if (input[LOWER] == '!' && input[UPPER] == '!') {
                
                nextState = S0;
                reset_leds();
            } else if (input[LOWER] == '*' && input[UPPER] == '*') {
                
                nextState = S2;
                reset_leds();
            } else {

                nextState = S1;

                // Check if user input is valid.
                int inputByte = validate_user_input();
                if (inputByte == INVALID) {

                    debug_log("Invalid input\n\r");
                    break;
                }

                // Encode byte
                uint16_t encoded = s4743527_lib_hamming_byte_encode(inputByte);

                debug_log("Encoded 0x%1x => 0x%2x\n\r", inputByte, encoded);

                uint8_t lowerByte = encoded & 0x00FF;
                uint8_t upperByte = (encoded & 0xFF00) >> 8;

                ledBytes[LOWER] = lowerByte;
                ledBytes[UPPER] = upperByte;

                s4743527_reg_lta1000g_write(ledBytes[displayByte]);

            }
            
            break;
        
        case S2:
            if (input[LOWER] == '!' && input[UPPER] == '!') {
                
                nextState = S0;
                reset_leds();
            } else if (input[LOWER] == '#' && input[UPPER] == '#') {
                
                nextState = S1;
                reset_leds();
            } else {

                nextState = S2;

                // Check if user input is valid.
                int inputByte = validate_user_input();
                if (inputByte == INVALID) {

                    debug_log("Invalid input\n\r");
                    break;
                }

                // Decode byte
                uint8_t decoded = s4743527_lib_hamming_byte_decode(inputByte);
                debug_log("Decoded 0x%1x => 0x%1x\n\r", inputByte, decoded);

                s4743527_reg_lta1000g_write(decoded);

                // Check for 1 bit or 2 bit error. 
                if (!(syndrome_error(inputByte))) { // Syndrome is 0
                    
                    if (s4743527_lib_hamming_parity_error(inputByte)) { // Parity is incorrect.
                    
                        S4743527_REG_MFS_LED_D1_ON(); // 1 bit error
                        S4743527_REG_MFS_LED_D2_OFF();
                    } else { // Parity is correct
                    
                        // No error
                        S4743527_REG_MFS_LED_D1_OFF();
                        S4743527_REG_MFS_LED_D2_OFF();
                    }
                } else { // Syndrome is not 0
                    
                    if (s4743527_lib_hamming_parity_error(inputByte)) { // Parity is incorrect.
                    
                        S4743527_REG_MFS_LED_D1_ON(); // 1 bit error
                        S4743527_REG_MFS_LED_D2_OFF();
                    } else { // Parity is correct
                    
                        S4743527_REG_MFS_LED_D2_ON(); // 2 bit error
                        S4743527_REG_MFS_LED_D1_OFF();
                    }
                }
            }
            break;

        default:
            nextState = currentState;
            break;
    }

    return nextState;
}

/**
 * Turns OFF MFS LEDs and resets number on LED bar display to 0.
 */
void reset_leds(void) {
    S4743527_REG_MFS_LED_D1_OFF();
    S4743527_REG_MFS_LED_D2_OFF();
    S4743527_REG_MFS_LED_D3_OFF();
    S4743527_REG_MFS_LED_D4_OFF();

    displayByte = LOWER;

    s4743527_reg_lta1000g_write(0);
}

/**
 * Initialises hardware
 * 
 * Returns: None
 */
void hardware_init(void) {
    
    // Initialise LTA1000G LED bar
    s4743527_reg_lta1000g_init();

    // Initialise MFS pushbutton S1
    s4743527_reg_mfs_pb_init(S4743527_REG_MFA_PB_S1);

    // Initialise MFS LEDs
    s4743527_reg_mfs_led_init();

    // Initialise UART for debug log.
    BRD_debuguart_init();

    // Initialise timer 2
    // Enable Timer 2 clock
    __TIM2_CLK_ENABLE();

    // Set prescalar
    TIM2->PSC = ((SystemCoreClock / 2) / TIMER_COUNTER_FREQ) - 1;

    // Set to counting up (0)
    TIM2->CR1 |= TIM_CR1_DIR;

    // Set ARR and CNT to 50ms period.
    TIM2->ARR = PWM_PERIOD_TICKS;
    TIM2->CNT = PWM_PERIOD_TICKS;

    // Enable interrupt
    TIM2->DIER |= TIM_DIER_UIE;

    // Enable priority 10 and interrupt callback
	HAL_NVIC_SetPriority(TIM2_IRQn, 10, 0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);

    // Enable counter
    TIM2->CR1 |= TIM_CR1_CEN;
}

/*
 * Timer update interrupt callback
 *
 * Returns: None
 */
void input_isr(void) {

    // Get user input of 2 characters from console.
    char recv;
    if ((recv = BRD_debuguart_getc(0)) != EMPTY) {

        if (input[UPPER] == EMPTY) {

            input[UPPER] = recv;
            debug_log("%c", input[UPPER]);

            // Toggle on when 1st byte is received.
            S4743527_REG_MFS_LED_D4_ON();

        } else if (input[LOWER] == EMPTY) {
            
            input[LOWER] = recv;
            debug_log("%c\n\r", input[LOWER]);
            
            // Toggle off when 2nd byte is received.
            S4743527_REG_MFS_LED_D4_OFF();

            // Process user input
            currentState = fsm_processing(currentState);

            // Reset input for next 2 bytes from console.
            input[UPPER] = EMPTY;
            input[LOWER] = EMPTY;
        }
    }

    // Check for button press.
    if (s4743527_reg_mfs_pb_press_get(S4743527_REG_MFA_PB_S1)) {
        
        // Display encoded byte on LED bar display.
        if (currentState == S1) {
            if (displayByte == LOWER) {
                displayByte = UPPER;
                S4743527_REG_MFS_LED_D3_ON();
            } else {
                displayByte = LOWER;
                S4743527_REG_MFS_LED_D3_OFF();
            }

            s4743527_reg_lta1000g_write(ledBytes[displayByte]);
        } else {
            displayByte = LOWER;
        }

        s4743527_reg_mfs_pb_press_reset(S4743527_REG_MFA_PB_S1);
    }
    
    

}

/*
 * Interrupt handler for Timer 2
 *
 * Returns: None
 */ 
void TIM2_IRQHandler(void) {

    //Check and clear overflow flag.
    if((TIM2->SR & TIM_SR_UIF) == TIM_SR_UIF) { 
        
        // Clear the UIF Flag
        TIM2->SR &= ~TIM_SR_UIF;

        // Callback function
		input_isr();
	}
}