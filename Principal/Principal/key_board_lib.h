/*
 * key_board_lib.h
 *
 *  Created on: 16.12.2019
 *      Author: marco
 */

#ifndef KEY_BOARD_LIB_H_
#define KEY_BOARD_LIB_H_

/* BIOS module Headers */
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "common.h"

uint8_t keyBoardRead(void);
char keyBoard(void);
void keyBoardInit(void);
void keyBoardSweep(uint32_t pin);

#endif /* KEY_BOARD_LIB_H_ */
