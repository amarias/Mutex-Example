/*
 * Copyright (c) 2015, Texas Instruments Incorporated
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

/**
 * ============ Mutex Example Project by Allison M. ============
 * This project creates 2 threads, speedyThread and slowThread,
 * each of which will turn on the in-board LED blue and green, respectively.
 * The blue LED should be more visible than the green LED due to
 * speedyThread sleeping less and having a higher priority than slowThread.
 * LED is red at startup. Tasks sleep in milliseconds.
 */

/*
 *  ======== empty.c ========
 */
/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Timestamp.h>
#include <xdc/runtime/Types.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>

/* Board Header file */
#include "Board.h"

#define TASKSTACKSIZE   512

Task_Struct speedyThread, slowThread;
Char speedyStack[TASKSTACKSIZE], slowStack[TASKSTACKSIZE];

Semaphore_Struct mutex;

/*
 *  ======== blueLEDFxn ========
 *
 */
Void blueLEDFxn(){

    while(1){

        Task_sleep(200);

        /* Get mutex and turn on the blue LED */
        Semaphore_pend(Semaphore_handle(&mutex), BIOS_WAIT_FOREVER);
        GPIO_write(Board_LED1, Board_LED_OFF);
        GPIO_write(Board_LED2, Board_LED_OFF);
        GPIO_write(Board_LED0, Board_LED_ON);
        Semaphore_post(Semaphore_handle(&mutex));

        Task_sleep(250);

        /* Get mutex and turn on the blue LED */
        Semaphore_pend(Semaphore_handle(&mutex), BIOS_WAIT_FOREVER);
        GPIO_write(Board_LED1, Board_LED_OFF);
        GPIO_write(Board_LED0, Board_LED_ON);
        Semaphore_post(Semaphore_handle(&mutex));

        /* Print Timestamp */
        System_printf("Current Time:    %d ms           speedyThread finished a cycle...\n",Timestamp_get32()/80000);
        System_flush();
    }

}

/*
 *  ======== greenLEDFxn  ========
 *
 */
Void greenLEDFxn(){

    while(1){

        Task_sleep(100);

        /* Get mutex and turn on the green LED */
        Semaphore_pend(Semaphore_handle(&mutex), BIOS_WAIT_FOREVER);
        GPIO_write(Board_LED0, Board_LED_OFF);
        GPIO_write(Board_LED2, Board_LED_OFF);
        GPIO_write(Board_LED1, Board_LED_ON);
        Semaphore_post(Semaphore_handle(&mutex));

        Task_sleep(500);

        /* Get mutex and turn on the green LED */
        Semaphore_pend(Semaphore_handle(&mutex), BIOS_WAIT_FOREVER);
        GPIO_write(Board_LED0, Board_LED_OFF);
        GPIO_write(Board_LED2, Board_LED_OFF);
        GPIO_write(Board_LED1, Board_LED_ON);
        Semaphore_post(Semaphore_handle(&mutex));

        Task_sleep(700);

        /* Print Timestamp */
        System_printf("Current Time:    %d ms           slowThread finished a cycle...\n",Timestamp_get32()/80000);
        System_flush();
    }
}

/*
 *  ======== main ========
 */
int main(void)
{
    Task_Params threadParams;
    Semaphore_Params mutexParams;

    /* Call board init functions */
    Board_initGeneral();
    Board_initGPIO();

    /* Construct Speedy Task thread */
    Task_Params_init(&threadParams);
    threadParams.stackSize = TASKSTACKSIZE;
    threadParams.stack = &speedyStack;
    threadParams.priority = 5;
    Task_construct(&speedyThread, (Task_FuncPtr)blueLEDFxn, &threadParams, NULL);

    /* Construct Slow Task thread */
    Task_Params_init(&threadParams);
    threadParams.stack = &slowStack;
    threadParams.priority = 15;
    Task_construct(&slowThread, (Task_FuncPtr)greenLEDFxn, &threadParams, NULL);

    /* Construct mutex */
    Semaphore_Params_init(&mutexParams);
    mutexParams.mode = Semaphore_Mode_BINARY;
    Semaphore_construct(&mutex, 0, &mutexParams);
    Semaphore_post(Semaphore_handle(&mutex));

    /* Turn on user LED */
    GPIO_write(Board_LED2, Board_LED_ON);

    System_printf("Starting the example\nSystem provider is set to SysMin. "
                  "Halt the target to view any SysMin contents in ROV.\n");
    /* SysMin will only print to the console when you call flush or exit */
    System_flush();

    /* Start BIOS */
    BIOS_start();

    return (0);
}
