/** 
 **************************************************************
 * @file mylib/myconfig.h
 * @author Hamza K
 * @date 03052024
 * @brief Config file for radio channel and address of nrf24L01P
 ***************************************************************
 */

#ifndef MYCONFIG_H
#define MYCONFIG_H

#include <stdint.h>

#define MYRADIOCHAN 52

uint8_t myradiotxaddr[] = {0x30, 0x10, 0x00, 0x00, 0x52};

#endif