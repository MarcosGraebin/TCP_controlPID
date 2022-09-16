/*
 * key_board_lib.c
 *
 *  Created on: 16.12.2019
 *      Author: marco
 */

#include "key_board_lib.h"

uint8_t keyBoardRead(void){
    volatile uint8_t a = (uint8_t) (15-GPIOPinRead(GPIO_PORTM_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7)/16);
    switch(a){
    case 0:
        return 0;

    case 1:
        return 1;

    case 2:
        return 2;

    case 4:
        return 3;

    case 8:
        return 4;

    default:
        return 0;
    }
    //return 15-(uint32_t) GPIOPinRead(GPIO_PORTM_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7)/16;
}

char keyBoard(void){
    volatile char charsKeyboard[4][4] = {{'1','2','3','A'}, {'4','5','6','B'}, {'7','8','9','C'}, {'*','0','#','D'}};
    volatile uint8_t i, j;
    volatile uint32_t aux, aux2;
    while(1){
        for(i = 0; i < 4; i++){
            keyBoardSweep(i);
            j = keyBoardRead();
            aux2 = j;
            while(j){
                j = keyBoardRead();
                Task_sleep(sleepTickCount);
            }
            if (aux2 != 0)
                return charsKeyboard[i][(aux2-1)];
        }
        Task_sleep(sleepTickCount);
    }
}

void keyBoardInit(void){
    //Init the sweep output
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOM));
    GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

    //Init the sweep input
    GPIOPinTypeGPIOInput(GPIO_PORTM_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);          //GPIO J0 is input
    HWREG(GPIO_PORTM_BASE + GPIO_O_PUR) = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 ;           //Pull up resistor
}

void keyBoardSweep(uint32_t pin){
    switch(pin){
    case 0:
        GPIOPinWrite(GPIO_PORTM_BASE, 15, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
        break;

    case 1:
        GPIOPinWrite(GPIO_PORTM_BASE, 15, GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3);
        break;

    case 2:
        GPIOPinWrite(GPIO_PORTM_BASE, 15, GPIO_PIN_1 | GPIO_PIN_0 | GPIO_PIN_3);
        break;

    case 3:
        GPIOPinWrite(GPIO_PORTM_BASE, 15, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_0);
        break;

    default:
        break;
    }
}
