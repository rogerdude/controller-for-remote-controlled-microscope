/**
  ******************************************************************************
  * @file    project/main.c
  * @author  Hamza K
  * @date    28042024
  * @brief   Controller for Remote Controlled Microscope (RCM)
  ******************************************************************************
  */

#include "board.h"
#include "processor_hal.h"
#include "s4743527_mfs_led.h"
#include "s4743527_rgb.h"
#include "s4743527_lta1000g.h"
#include "s4743527_board_pb.h"
#include "s4743527_txradio.h"
#include "s4743527_rcmcont.h"

// Function prototypes
void hardware_init(void);

int main(void) {

    HAL_Init(); // Initialise board
    hardware_init(); // Initialise hardware

    return 0;
}

/**
 * Initialises hardware
 * 
 * Returns: None
 */
void hardware_init(void) {

    // Intialise RCM Control task
    s4743527_tsk_rcmcont_init();
}
