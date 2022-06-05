/**
 * @file main.c
 * @author Tushar Uttekar
 * 
 * @brief 
 * 
 * This example demonstrates how the queue can receive and send data from the same ISR routine with interrupt safe API's.
 * 
 * Two different queues and tasks are created
 * 1)Integer task and queue to generate integer number and send it to the ISR.
 * 2)String task and queue to receive string from the ISR and print it on the terminal.
 * 
 * @version 0.1
 * @date 2022-06-01
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/portmacro.h"
#include "freertos/xtensa_api.h"
#include "xtensa/core-macros.h"
#include "esp_intr_alloc.h"
#include "esp_log.h"
#include "esp_system.h"

#define SW_ISR_LEVEL_3  29

QueueHandle_t IntegerQueue, StringQueue;

static void IntegerGenerator(void* pvParameters)
{
    static TickType_t LastExecutionTime = 0;
    uint32_t Value_Queue = 0, Loop = 0;

    LastExecutionTime = xTaskGetTickCount();

    for(;;)
    {
        vTaskDelayUntil(&LastExecutionTime,pdMS_TO_TICKS(200));

        for(Loop = 0; Loop < 5; Loop++)
        {
            xQueueSendToBack(IntegerQueue,&Value_Queue,0);
            Value_Queue++;
        }

        printf("About to generate the interrupt........\r\n");
        xt_set_intset(1 << SW_ISR_LEVEL_3);
        printf("Interrupt generated......!!!!!\r\n\r\n");            
    }
}

static void StringReceptor(void* pvParameters)
{
    char* string;

    for(;;)
    {
        xQueueReceive(StringQueue,&string,portMAX_DELAY);
        printf("%s",string);
    }
}

static void Interrupt_Handler(void *arg)
{
    static const char *pcStrings[] ={
                                    "String 0\r\n",
                                    "String 1\r\n",
                                    "String 2\r\n",
                                    "String 3\r\n"
                                    };

    BaseType_t xHigherPriorityTaskWoken;
    uint32_t Received_Number;

    xt_set_intclear(1 << SW_ISR_LEVEL_3);

    //Setting the variable to FALSE for the first time when interrupt occurs as it will TRUE if context switch is required
    xHigherPriorityTaskWoken = pdFALSE;

    /* Read from the queue until the queue is empty. */
    while(xQueueReceiveFromISR(IntegerQueue,&Received_Number,&xHigherPriorityTaskWoken) != errQUEUE_EMPTY)
    {
        /* Truncate the received value to the last two bits (values 0 to 3 inclusive),
           then use the truncated value as an index into the pcStrings[] array to select
           a string (char *) to send on the other queue. */
        Received_Number &= 0x03;
        xQueueSendToBackFromISR(StringQueue,&pcStrings[Received_Number],&xHigherPriorityTaskWoken);
    }

    //To Switch context to the higher priority task for deferring work to be done
    portYIELD_FROM_ISR();
}

void app_main(void)
{
    //Create queue for integer and string respectively
    IntegerQueue = xQueueCreate(10,sizeof(uint32_t));
    StringQueue  = xQueueCreate(10,sizeof(char*));

    //Create task for integer generation and string reception respectively
    xTaskCreate(IntegerGenerator,"Integer",2048,NULL,1,NULL);
    xTaskCreate(StringReceptor,"String",2048,NULL,2,NULL);

    //Setting up interrupt handler based on the xtensa port function
    esp_intr_alloc(ETS_INTERNAL_SW1_INTR_SOURCE,0,Interrupt_Handler,NULL,NULL);

    while(1);    
}