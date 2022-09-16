/*
 * lcd_lib.h
 *
 *  Created on: 07.11.2019
 *      Author: marcos
 */

#ifndef LCD_LIB_H_
#define LCD_LIB_H_

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "common.h"

#define TODOS (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7)
#define CNTDELAY 8000000
#define DELAYS 800000*120*2/25
#define DELAYMS DELAYS/1000
#define DELAYUS DELAYMS/1000

void delayus(volatile const uint32_t time);
void delayms(volatile const uint32_t time);
void LcdCmd(volatile const uint8_t command);
void lcdInit(void);
void LcdChar(unsigned char data);
uint8_t LcdStr(char* str, uint8_t linha);

#endif /* LCD_LIB_H_ */
