/** 
 **************************************************************
 * @file project/s4743527_rcmcont.h
 * @author Hamza K
 * @date 29042024
 * @brief FSM task for RCM Control
 * REFERENCE: csse3010_project.pdf
 ***************************************************************
 * EXTERNAL FUNCTIONS
 ***************************************************************
 * s4743527_tsk_rcmcont_init() - Initialises the RCM control task.
 *************************************************************** 
 */

#ifndef S4743527_RCMCONT_H
#define S4743527_RCMCONT_H

// Task Priority
#define TASK_RCM_CONT_PRIORITY  (tskIDLE_PRIORITY + 2)

// Task Stack Allocation
#define TASK_RCM_CONT_STACK_SIZE    (configMINIMAL_STACK_SIZE * 10)

// States for FSM
#define JOIN        0
#define IDLE        1
#define PACKET      2

#define POSITIVE 1
#define NEGATIVE -1

// Function prototype
// Initialises the RCM control task.
extern void s4743527_tsk_rcmcont_init(void);

#endif