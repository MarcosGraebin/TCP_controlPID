/*
 * Copyright (c) 2015-2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== mutex.c ========
 */

/* XDC module Headers */
#include <xdc/std.h>
#include <xdc/runtime/System.h>


/* BIOS module Headers */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/queue.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "inc/hw_pwm.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/adc.h"

#include "lcd_lib.h"
#include "key_board_lib.h"
#include "common.h"
#define FREQPWM 100000
#define CPUFREQ 80000000

/* Example/Board Header files */
#include "Board.h"

#define TASKSTACKSIZE   512

Void task1Fxn(UArg arg0, UArg arg1);
Void task2Fxn(UArg arg0, UArg arg1);
Void task3Fxn(UArg arg0, UArg arg1);
Void task4Fxn(UArg arg0, UArg arg1);
Void task5Fxn(UArg arg0, UArg arg1);

Int resource = 0;
Int finishCount = 0;

Task_Struct task1Struct, task2Struct, task3Struct, task4Struct, task5Struct;
Char task1Stack[TASKSTACKSIZE], task2Stack[TASKSTACKSIZE], task3Stack[TASKSTACKSIZE], task4Stack[TASKSTACKSIZE];
Semaphore_Struct semStruct;
Semaphore_Handle semHandle;


//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>Blinky (blinky)</h1>
//!
//! A very simple example that blinks the on-board LED using direct register
//! access.
//
//*****************************************************************************

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
    while(1);
}
#endif

/*
 *  ======== main ========
 */

Int main(){
    str_keys = Queue_create(NULL, NULL);
    int16_t_set_point = Queue_create(NULL, NULL);
    temperaturaADC = Queue_create(NULL, NULL);

    /* Construct BIOS objects */
    Task_Params taskParams;
    Semaphore_Params semParams;

    /* Call board init functions */
    Board_initGeneral();

    /* Construct writer/reader Task threads */
    Task_Params_init(&taskParams);
    taskParams.stackSize = TASKSTACKSIZE;

    taskParams.stack = &task1Stack;
    taskParams.priority = 1;
    Task_construct(&task1Struct, (Task_FuncPtr)task1Fxn, &taskParams, NULL);

    taskParams.stack = &task2Stack;
    taskParams.priority = 1;
    Task_construct(&task2Struct, (Task_FuncPtr)task2Fxn, &taskParams, NULL);

    taskParams.stack = &task3Stack;
    taskParams.priority = 1;
    Task_construct(&task3Struct, (Task_FuncPtr)task3Fxn, &taskParams, NULL);

    taskParams.stack = &task4Stack;
    taskParams.priority = 1;
    Task_construct(&task4Struct, (Task_FuncPtr)task4Fxn, &taskParams, NULL);

    taskParams.stack = &task5Stack;
    taskParams.priority = 1;
    Task_construct(&task5Struct, (Task_FuncPtr)task5Fxn, &taskParams, NULL);

    /* Construct a Semaphore object to be use as a resource lock, inital count 1 */
    Semaphore_Params_init(&semParams);
    Semaphore_construct(&semStruct, 1, &semParams);

    /* Obtain instance handle */
    semHandle = Semaphore_handle(&semStruct);

    BIOS_start();    /* Does not return */
    return(0);
}

/*
 *  ======== task1Fxn ======== (PWM)
 */
Void task1Fxn(UArg arg0, UArg arg1){
    volatile uint32_t ui32Loop;

    volatile uint32_t i;

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); // Habilita GPIO F (LED D3 = PF4, LED D4 = PF0)
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)); // Aguarda final da habilitao

    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0); // LED D3 como saida
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, 0); // LEDs D3 apagado
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_STRENGTH_12MA, GPIO_PIN_TYPE_STD);

    SysCtlPWMClockSet(SYSCTL_PWMDIV_1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinConfigure(GPIO_PF0_M0PWM0);

    //
    // Configure the PWM function for this pin.
    // Consult the data sheet to see which functions are allocated per pin.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_0);
    PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);

    // use the following equation: N = (1 / f) * SysClk.  Where N is the
    // function parameter, f is the desired frequency, and SysClk is the
    // system clock frequency.
    // the maximum period you can set is 2^16.
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, CPUFREQ/FREQPWM);
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, PWMGenPeriodGet(PWM0_BASE, PWM_GEN_0) / 2);
    PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT, true);
    PWMGenEnable(PWM0_BASE, PWM_GEN_0);

    while(1){
        //PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, PWMGenPeriodGet(PWM0_BASE, PWM_GEN_0) / 2);
        Task_sleep(sleepTickCount*200);
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, 10);
        //PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, PWMGenPeriodGet(PWM0_BASE, PWM_GEN_0) / 4);
        Task_sleep(sleepTickCount*200);
    }
}

/*
 *  ======== task2Fxn (keyBoard) ========
 */
Void task2Fxn(UArg arg0, UArg arg1){
    keyBoardToDispStr send;
    TemperaturaInt16_t send_2_pid, send_2_disp;
    volatile char newChar;

    volatile uint8_t dec_value = 0, i;
    volatile int8_t string_position = 0;

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION));
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);

    keyBoardInit();

    GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

    send_2_disp.temperatura = MINTEMPERATURE;
    Queue_enqueue(int16_t_set_point, &(send_2_disp.elem));
    while(1){
        newChar = keyBoard();
        if ((newChar >= '0' && newChar <= '9') && string_position < MAXCHARSKEYBOARD){        //If it is a number and it's fits in the display
            send.temperatura[string_position++] = newChar;
            send.temperatura[string_position] = '\0';
            Queue_enqueue(str_keys, &(send.elem));
        }
        else if (newChar == 'B' && string_position > 0){             //If it is a backspace
            send.temperatura[--string_position] = '\0';
            Queue_enqueue(str_keys, &(send.elem));
        }

        else if (newChar == 'C'){             //If it is a confirm
            dec_value = atoi(send.temperatura);
            send_2_disp.temperatura = (dec_value>MAXTEMPERATURE)?MAXTEMPERATURE:(dec_value<MINTEMPERATURE)?MINTEMPERATURE:(int16_t) dec_value;
            Queue_enqueue(int16_t_set_point, &(send_2_disp.elem));
            //envia pid
            send.temperatura[0] = '\0';
            string_position = 0;
        }
        Task_sleep(sleepTickCount);
    }
}

/*
 *  ======== task3Fxn (display) ========
 */
Void task3Fxn(UArg arg0, UArg arg1){
    volatile uint32_t ui32Loop;
    volatile TemperaturaInt16_t* receive_from_key;
    volatile TemperaturaUint8_t* receive_from_ADC;
    volatile keyBoardToDispStr* fromKeyBoard;
    volatile char str_out[16], str_aux[16];

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION));
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);                //Enable as GPIO
    while(!(SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ)));       //wait
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0);          //GPIO J0 is input
    HWREG(GPIO_PORTJ_BASE + GPIO_O_PUR) = GPIO_PIN_0;           //Pull up resistor

    //volatile uint8_t a;
    lcdInit();

    strcpy((char*)str_out, "Temp max: ");
    if(MAXTEMPERATURE > 99){
        str_aux[0] = MAXTEMPERATURE/100 + '0';
        str_aux[1] = (MAXTEMPERATURE/10)%10 + '0';
        str_aux[2] = MAXTEMPERATURE%10 + '0';
        str_aux[3] = 223;
        str_aux[4] = 'C';
        str_aux[5] = '\0';
    }
    else{
        str_aux[0] = MAXTEMPERATURE/10 + '0';
        str_aux[1] = MAXTEMPERATURE%10 + '0';
        str_aux[2] = 223;
        str_aux[3] = 'C';
        str_aux[4] = '\0';
    }
    strcat((char*)str_out, (char*)str_aux);
    LcdStr((char*)str_out,1);

    strcpy((char*)str_out, "Temp min: ");
        if(MINTEMPERATURE > 99){
            str_aux[0] = MINTEMPERATURE/100 + '0';
            str_aux[1] = (MINTEMPERATURE/10)%10 + '0';
            str_aux[2] = MINTEMPERATURE%10 + '0';
            str_aux[3] = 223;
            str_aux[4] = 'C';
            str_aux[5] = '\0';
        }
        else{
            str_aux[0] = MINTEMPERATURE/10 + '0';
            str_aux[1] = MINTEMPERATURE%10 + '0';
            str_aux[2] = 223;
            str_aux[3] = 'C';
            str_aux[4] = '\0';
        }
        strcat((char*)str_out, (char*)str_aux);
        LcdStr((char*)str_out,2);
        //receive_from_key->temperatura = MINTEMPERATURE;

    while(1){
        if (!Queue_empty(str_keys)){
            LcdStr("                ", 2);
            fromKeyBoard = Queue_dequeue(str_keys);
            LcdStr((char*)(fromKeyBoard->temperatura), 2);
        }
        if(!Queue_empty(int16_t_set_point) || !Queue_empty(temperaturaADC)){
            LcdStr("                ", 1);
            if(!Queue_empty(int16_t_set_point))
                receive_from_key = Queue_dequeue(int16_t_set_point);
            else
                receive_from_ADC = Queue_dequeue(temperaturaADC);

            if(receive_from_key->temperatura > 99){
                str_out[0] = receive_from_key->temperatura/100 + '0';
                str_out[1] = (receive_from_key->temperatura/10)%10 + '0';
                str_out[2] = receive_from_key->temperatura%10 + '0';
                str_out[3] = 223;
                str_out[4] = 'C';
                str_out[5] = '\0';
            }
            else{
                str_out[0] = receive_from_key->temperatura/10 + '0';
                str_out[1] = receive_from_key->temperatura%10 + '0';
                str_out[2] = 223;
                str_out[3] = 'C';
                str_out[4] = '\0';
            }
            strcpy((char*) str_aux, (char*) str_out );
            strcpy((char*) str_out , "SP:");
            strcat((char*)str_out, (char*) str_aux);


            if(receive_from_ADC->temperatura > 99){
                str_aux[0] = receive_from_ADC->temperatura/100 + '0';
                str_aux[1] = (receive_from_ADC->temperatura/10)%10 + '0';
                str_aux[2] = receive_from_ADC->temperatura%10 + '0';
                str_aux[3] = 223;
                str_aux[4] = 'C';
                str_aux[5] = '\0';
            }
            else{
                str_aux[0] = receive_from_ADC->temperatura/10 + '0';
                str_aux[1] = receive_from_ADC->temperatura%10 + '0';
                str_aux[2] = 223;
                str_aux[3] = 'C';
                str_aux[4] = '\0';
            }
            strcat((char*)str_out, " CT:");
            strcat((char*)str_out, (char*)str_aux);

            LcdStr((char*)str_out, 1);
            LcdStr("\0", 2);

        }
        Task_sleep(sleepTickCount);
    }
}

void adcInit(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);
    GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_3, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_ANALOG);
    ADCSequenceDisable(ADC0_BASE, 0);
    ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH0);
    ADCSequenceStepConfigure(ADC0_BASE,0, 1, ADC_CTL_CH1 | ADC_CTL_IE | ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 0);
}

/*
 *  ======== task4Fxn (ADC) ========
 */
Void task4Fxn(UArg arg0, UArg arg1){
    volatile uint32_t ADCValue;
    volatile uint64_t cumSum = 0;
    volatile float analogTemp = 0;
    volatile TemperaturaUint8_t enviar;
    volatile uint8_t last_sent = 0;
    volatile uint8_t i;
    adcInit();

        while(1){
#define ORDER 50.0
            for(i = 0; i < ORDER; i++){
                ADCIntClear(ADC0_BASE, 0);
                ADCProcessorTrigger(ADC0_BASE, 0);
                while(!ADCIntStatus(ADC0_BASE, 0, false))
                    Task_sleep(sleepTickCount);
                ADCSequenceDataGet(ADC0_BASE, 0, (uint32_t *) &ADCValue);
                cumSum =  ADCValue + cumSum;
            }
            analogTemp = ((((float)cumSum)*330.0))/(ORDER*4096.0);

            enviar.temperatura = (uint8_t) analogTemp;
            if(last_sent!=enviar.temperatura){
                Queue_enqueue(temperaturaADC, &(enviar.elem));
                last_sent = enviar.temperatura;
            }
            cumSum = 0;
        }
}
/*
 *  ======== task5Fxn (PID) ========
 */
void task5Fxn(UArg arg0, UArg arg1){
#define P 0.005
#define I 0.00001
#define D 0.001
    volatile uint32_t oldTickValue = 0, ticks, deltaTicks = 0;

    while(1){
        ticks = Clock_getTicks();
        deltaTicks = ticks - oldTickValue;
        oldTickValue = ticks;


        error = currentTemperature - temperatureSetPoint;

        secs = ((unsigned long)ticks * Clock_tickPeriod) / 1000000;
    }
}
