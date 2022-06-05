/**
 * @file main.c
 * @author Tushar Uttekar
 * 
 * @brief 
 * 
 * In this example we demonstrate the Task Notification concept which can be an efficient replacement for data and even sharing
 * for the receiving task instead of the standard communication objects such are semaphore,event group & Queue.
 * 
 * In the code the handler task and ISR are the one's receiving and sending the notification respectively to the each other.
 * 
 * The periodic function generates the interrupt at a particular interval which in turn enable's the ISR to send the notification
 * to the handler task which removes the handler task from the blocking state to running state and print out the string on the
 * terminal.
 * 
 * Here the clearcountonexit argument of the tasknotifytake is set to FALSE which basically decrements the notification value
 * and indicates how many pending notification are still to be processed.
 * 
 * @version 0.1
 * @date 2022-06-05
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/portmacro.h"
#include "freertos/xtensa_api.h"
#include "xtensa/core-macros.h"
#include "esp_intr_alloc.h"
#include "esp_log.h"
#include "esp_system.h"

#define SW_ISR_LEVEL_3      29

TaskHandle_t Handler_t;

static void Periodic_Function(void* pvParameters)
{
    for(;;)
    {
        vTaskDelay(pdMS_TO_TICKS(500));

        printf("About to generate the interrupt........\r\n");
        xt_set_intset(1 << SW_ISR_LEVEL_3);
        printf("Interrupt generated......!!!!!\r\n\r\n");
    }
}

static void Handler_Function(void* pvParameters)
{
    for(;;)
    {
        if(ulTaskNotifyTake(pdFALSE,pdMS_TO_TICKS(750)) != NULL)
        {
            //printing string for output from the hadler function
            printf("Handler Function is executing after receiving task notification!!!!\r\n");
        }
        else
        {
            //When there is no notification received from the ISR routine
        }
    }
}

static void Interrupt_Handler(void *arg)
{
    BaseType_t xHigherPriorityTaskWoken;

    xt_set_intclear(1 << SW_ISR_LEVEL_3);

    //Setting the variable to FALSE for the first time when interrupt occurs as it will TRUE if context switch is required
    xHigherPriorityTaskWoken = pdFALSE;

    //Sending notification to the task where interrupt processing is deferred
    vTaskNotifyGiveFromISR(Handler_t,&xHigherPriorityTaskWoken);
    vTaskNotifyGiveFromISR(Handler_t,&xHigherPriorityTaskWoken);
    vTaskNotifyGiveFromISR(Handler_t,&xHigherPriorityTaskWoken);
    vTaskNotifyGiveFromISR(Handler_t,&xHigherPriorityTaskWoken);

    //To Switch context to the higher priority task for deferring work to be done
    portYIELD_FROM_ISR();
}

void app_main(void)
{
    //Create Task's for Periodic and Handler functions
    xTaskCreate(Handler_Function,"Handler",2048,NULL,3,&Handler_t);
    xTaskCreate(Periodic_Function,"Periodic",2048,NULL,1,NULL);

    //Setting up interrupt handler based on the xtensa port function
    esp_intr_alloc(ETS_INTERNAL_SW1_INTR_SOURCE,0,Interrupt_Handler,NULL,NULL);

    while(1);
}