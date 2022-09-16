/*
 * common.h
 *
 *  Created on: 16.12.2019
 *      Author: marco
 */

#ifndef COMMON_H_
#define COMMON_H_
#include <ti/sysbios/knl/queue.h>

#define sleepTickCount  10000/Clock_tickPeriod
#define MAXCHARSKEYBOARD 3
#define MAXTEMPERATURE 80
#define MINTEMPERATURE 35

typedef struct{
    ti_sysbios_knl_Queue_Elem elem;
    char temperatura[MAXCHARSKEYBOARD+3];
} keyBoardToDispStr;

typedef struct{
    ti_sysbios_knl_Queue_Elem elem;
    int16_t temperatura;
} TemperaturaInt16_t;

typedef struct{
    ti_sysbios_knl_Queue_Elem elem;
    uint8_t temperatura;
} TemperaturaUint8_t;

typedef struct{
    ti_sysbios_knl_Queue_Elem elem;
    int16_t temperatura;
} temperaturaFloat;

Queue_Handle str_keys, int16_t_set_point, temperaturaADC;

#endif /* COMMON_H_ */
