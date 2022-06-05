/**
 * @file main.c
 * @author Tushar Uttekar
 * 
 * @brief 
 * 
 * In this example we will be using counting semaphore do demonstrate how the count for the number of events which occurs can be
 * kept using the semaphore.
 * 
 * The max count value defines the maximum number of events that can be effectively latched and the initial count value is the
 * value from which the semaphore starts to count.
 * 
 * Semaphore give increases its value and semaphore take operation decreases its value.
 * 
 * @version 0.1
 * @date 2022-05-31
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
SemaphoreHandle_t xCountingSemaphore;

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
        //Taking semaphore from the ISR routine to perform further deferring work to be done
        xSemaphoreTake(xCountingSemaphore,portMAX_DELAY);

        //printing string for output from the hadler function
        printf("Handler Function is executing after taking semaphore!!!!\r\n");
    }
}

static void Interrupt_Handler(void *arg)
{
    BaseType_t xHigherPriorityTaskWoken;

    xt_set_intclear(1 << SW_ISR_LEVEL_3);

    //Setting the variable to FALSE for the first time when interrupt occurs as it will TRUE if context switch is required
    xHigherPriorityTaskWoken = pdFALSE;

    //Giving the semphore multiple times will latch it and do its deferred operation accordingly and not loose the latched events
    //as opposed to that in binary semaphore where latched events would be lost
    xSemaphoreGiveFromISR(xCountingSemaphore,&xHigherPriorityTaskWoken);
    xSemaphoreGiveFromISR(xCountingSemaphore,&xHigherPriorityTaskWoken);
    xSemaphoreGiveFromISR(xCountingSemaphore,&xHigherPriorityTaskWoken);
    xSemaphoreGiveFromISR(xCountingSemaphore,&xHigherPriorityTaskWoken);
    xSemaphoreGiveFromISR(xCountingSemaphore,&xHigherPriorityTaskWoken);
    xSemaphoreGiveFromISR(xCountingSemaphore,&xHigherPriorityTaskWoken);

    //To Switch context to the higher priority task for deferring work to be done
    portYIELD_FROM_ISR();
}

void app_main(void)
{
    //Create Counting Semaphore
    xCountingSemaphore = xSemaphoreCreateCounting(10,0);

    //Validate whether semaphore is created or not
    if(xCountingSemaphore != NULL)
    {
        //Create Task's for Periodic and Handler functions
        xTaskCreate(Handler_Function,"Handler",2048,NULL,3,NULL);
        xTaskCreate(Periodic_Function,"Periodic",2048,NULL,1,NULL);

        //Setting up interrupt handler based on the xtensa port function
        esp_intr_alloc(ETS_INTERNAL_SW1_INTR_SOURCE,0,Interrupt_Handler,NULL,NULL);
    }

    while(1);
}