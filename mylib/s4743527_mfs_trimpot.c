/** 
 **************************************************************
 * @file mylib/s4743527_mfs_trimpot.c
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

#include "s4743527_mfs_trimpot.h"
#include "processor_hal.h"

// Global variables
// Struct to setup and use ADC registers
static ADC_HandleTypeDef AdcHandle;
// Struct to initialise ADC channel
static ADC_ChannelConfTypeDef AdcChanConfig;

/**
 * Initialises GPIO pins and ADC for trimpot.
 * 
 * Returns: None
 */
extern void s4743527_reg_mfs_trimpot_init(void) {

    // Enable clock for port A
    __GPIOA_CLK_ENABLE();
    
    // Set Pin A3 to analog mode
    GPIOA->MODER |= (0x03 << (TRIMPOT_INPUT_PIN * 2));

    // Clear and set to high speed
    GPIOA->OSPEEDR &= ~(0x03<<(TRIMPOT_INPUT_PIN * 2));
	GPIOA->OSPEEDR |= 0x02<<(TRIMPOT_INPUT_PIN * 2);

    // Enable ADC1 clock
    __ADC1_CLK_ENABLE();

    // Initialise ADC1 to 12 bit resolution.
    AdcHandle.Instance = (ADC_TypeDef*) (ADC1_BASE);
    AdcHandle.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV2;
    AdcHandle.Init.Resolution = ADC_RESOLUTION12b;
    AdcHandle.Init.ScanConvMode = DISABLE;
    AdcHandle.Init.ContinuousConvMode = DISABLE;
    AdcHandle.Init.DiscontinuousConvMode = DISABLE;
    AdcHandle.Init.NbrOfDiscConversion = 0;
    AdcHandle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    AdcHandle.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_CC1;
    AdcHandle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    AdcHandle.Init.NbrOfConversion = 1;
    AdcHandle.Init.DMAContinuousRequests = DISABLE;
    AdcHandle.Init.EOCSelection = DISABLE;

    HAL_ADC_Init(&AdcHandle);

    // Configure ADC channel pin A3
    AdcChanConfig.Channel = ADC_CHANNEL_3;
	AdcChanConfig.Rank = 1;
	AdcChanConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	AdcChanConfig.Offset = S4743527_REG_MFS_TRIMPOT_ZERO_CAL_OFFSET;

    // Initialise ADC channel
    HAL_ADC_ConfigChannel(&AdcHandle, &AdcChanConfig);
}

/**
 * Reads trimpot's input voltage and converts to ADC value.
 * 
 * Returns: ADC value converted from trimpot's input.
 */
extern int s4743527_reg_mfs_trimpot_get(void) {

    // Start ADC conversion
    HAL_ADC_Start(&AdcHandle);

    // Wait for ADC conversion to finish
	while (HAL_ADC_PollForConversion(&AdcHandle, ADC_CONVERSION_TIMEOUT) !=
            HAL_OK);
    
    return AdcHandle.Instance->DR;
}