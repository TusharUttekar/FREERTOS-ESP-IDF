/**
 * @file main.c
 * @author Tushar Uttekar
 * 
 * @brief 
 * 
 * In this example the binary semaphore use case is explored based on which the synchronous operation between the ISR routine and
 * task happens which does the further processing which is known as Deferred processing.
 * 
 * In our example code we have to two task's and one software ISR routine based on the ESP environment.
 * ->The handler task has the highest priority and it takes semaphore from the ISR and will wait for indefinite time until it
 *   gets the semaphore after that only it will do the Deferred process.
 * ->The Periodic task on the other hand is a low priority task which periodically print's output string on the terminal and
 *   also trigger's the ISR routine so the handler task can get the semaphore once it is given by the ISR routine.
 * 
 * NOTE : Use ISR safe RTOS API's in the ISR routine for proper operation to take place.
 *        Refer ESP software Interrupt generation API's for creating, Setting/Enable and Clearing the ISR routine.
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

xt_handler error;
#define INTERRUPT_NUMBER    3
#define SW_ISR_LEVEL_3      29
SemaphoreHandle_t xBinarySemaphore;

static void Periodic_Function(void* pvParameters)
{
    for(;;)
    {
        vTaskDelay(pdMS_TO_TICKS(500));

        printf("About to generate the interrupt........\r\n");
        xt_set_intset(1 << SW_ISR_LEVEL_3);
        //xthal_interrupt_enable(INTERRUPT_NUMBER);
        //xt_ints_on(INTERRUPT_NUMBER);
        printf("Interrupt generated......!!!!!\r\n\r\n");
    }
}

static void Handler_Function(void* pvParameters)
{
    for(;;)
    {
        //Taking semaphore from the ISR routine to perform further deferring work to be done
        xSemaphoreTake(xBinarySemaphore,portMAX_DELAY);

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

    //Giving the binary semphore to the task for unblocking it
    xSemaphoreGiveFromISR(xBinarySemaphore,&xHigherPriorityTaskWoken);

    //To Switch context to the higher priority task for deferring work to be done
    portYIELD_FROM_ISR();
}

void app_main(void)
{
    //Create Binary Semaphore
    xBinarySemaphore = xSemaphoreCreateBinary();

    //Validate whether semaphore is created or not
    if(xBinarySemaphore != NULL)
    {
        //Create Task's for Periodic and Handler functions
        xTaskCreate(Handler_Function,"Handler",2048,NULL,3,NULL);
        xTaskCreate(Periodic_Function,"Periodic",2048,NULL,1,NULL);

        //Setting up interrupt handler based on the xtensa port function
        //error = xt_set_interrupt_handler(INTERRUPT_NUMBER,Interrupt_Handler,NULL);
        //xt_set_exception_handler(INTERRUPT_NUMBER,Interrupt_Handler);
        esp_intr_alloc(ETS_INTERNAL_SW1_INTR_SOURCE,0,Interrupt_Handler,NULL,NULL);

        /*if(error == 0)
        {
            printf("ERROR!!!!!!!!!\r\n");
        }*/
    }

    while(1);
}