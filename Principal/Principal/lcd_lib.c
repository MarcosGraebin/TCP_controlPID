/*
 * lcd_lib.c
 *
 *  Created on: 07.11.2019
 *      Author: marco
 */

#include "lcd_lib.h"

void delayus(volatile const uint32_t time){
    volatile uint32_t i;
    //Task_sleep(sleepTickCount);
    for(i = 0; i < time*DELAYUS; i++);
}

void delayms(volatile const uint32_t time){
    volatile uint32_t i;
    //Task_sleep(sleepTickCount*time);
    for(i = 0; i < time*DELAYMS; i++);
}

void LcdCmd(volatile const uint8_t command){
    volatile uint32_t i;
    GPIOPinWrite(GPIO_PORTP_BASE, GPIO_PIN_4, 0);               //Enable = 0
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_4 | GPIO_PIN_5, 0);  //RW, RS = 0
    GPIOPinWrite(GPIO_PORTK_BASE, TODOS, command);
    GPIOPinWrite(GPIO_PORTP_BASE, GPIO_PIN_4, GPIO_PIN_4);      //Enable = 1
    if(command < 4)
        delayms(2);
    else
        delayus(40);
    GPIOPinWrite(GPIO_PORTP_BASE, GPIO_PIN_4, 0);      //Enable = 0
}

void lcdInit(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOK));
    GPIOPinTypeGPIOOutput(GPIO_PORTK_BASE, TODOS);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION));
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_4 | GPIO_PIN_5);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOP));
    GPIOPinTypeGPIOOutput(GPIO_PORTP_BASE, GPIO_PIN_4);

    LcdCmd(0x38); //8-bits,2 display lines, 5x7 font
    //delayms(2);
    LcdCmd(0x06); //increments automatically
    //delayms(2);
    LcdCmd(0x0F); //Turn on display
    //delayms(2);
    LcdCmd(0x01); //clear display
    //delayms(2);
    LcdCmd(0x0C);
    delayms(4);
}

void LcdChar(volatile const unsigned char data){
    volatile uint32_t i;
    GPIOPinWrite(GPIO_PORTP_BASE, GPIO_PIN_4, 0);                           //Enable = 0
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_4 | GPIO_PIN_5, GPIO_PIN_4);     //RW = 0, RS = 1
    GPIOPinWrite(GPIO_PORTK_BASE, TODOS, (uint8_t) data);
    GPIOPinWrite(GPIO_PORTP_BASE, GPIO_PIN_4, GPIO_PIN_4);                  //Enable = 1
    delayus(40);
    GPIOPinWrite(GPIO_PORTP_BASE, GPIO_PIN_4, 0);                           //Enable = 0
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_4 | GPIO_PIN_5, 0);
}

uint8_t LcdStr(char* str, uint8_t linha){
    volatile uint32_t length, i, j;
    //const uint8_t maxLength = 16;
    LcdCmd(0x02); //Return Hopme
    if(linha == 2){
        delayms(2);
        LcdCmd(0b11000000);
        delayms(2);
    }
    if(linha > 2){
        LcdStr("Wrong line", 0);
        delayms(2);
        return 2;
    }
    for(length = 0; str[length] != '\0'; ++length);
    if( length > 16){
        LcdStr("Str too large", 0);
        delayms(2);
        return 1;
   }

    for(i = 0; i < length; i++){
        delayms(2);
        LcdChar(str[i]);
    }
    delayms(2);
    return 0;
}

