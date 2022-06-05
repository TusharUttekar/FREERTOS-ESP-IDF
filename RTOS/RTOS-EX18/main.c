/**
 * @file main.c
 * @author Tushar Uttekar
 * 
 * @brief 
 * 
 * In this example we are demonstrating the centralized deferred processing using the timer pend fucntionality which can be used
 * instead of a seperate task for deferred processing.
 * 
 * The timer pend functionality transfers the processing task which needs to be performed in the daemon task which does the
 * processing based on the commands available in the timer command queue.
 * 
 * The priority of the daemon task is kept higher than that of the periodic task in this example so that the context switch can
 * happen for deferred processing in the daemon task.
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
#include "freertos/timers.h"
#include "freertos/semphr.h"
#include "freertos/portmacro.h"
#include "freertos/xtensa_api.h"
#include "xtensa/core-macros.h"
#include "esp_intr_alloc.h"
#include "esp_log.h"
#include "esp_system.h"

#define SW_ISR_LEVEL_3  29

/*The task that generates the software interrupt is created at a priority below the priority of the daemon task.*/
const UBaseType_t PeriodicTaskPriority = configTIMER_TASK_PRIORITY + 1;

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

static void vDeferredHandlingFunction( void *pvParameter1, uint32_t ulParameter2 )
{
    /* Process the event - in this case just print out a message and the value of ulParameter2. pvParameter1 is not used in this example. */
    printf("Handler Function is executed %d number of times!!!!\r\n", ulParameter2 );
}

static void Interrupt_Handler(void *arg)
{
    BaseType_t xHigherPriorityTaskWoken;
    static uint32_t Parameter_Daemon_Task = 0;

    xt_set_intclear(1 << SW_ISR_LEVEL_3);

    //Setting the variable to FALSE for the first time when interrupt occurs as it will TRUE if context switch is required
    xHigherPriorityTaskWoken = pdFALSE;

    //Timer pending function will be called for performing the deferred processing in the daemon task in which the function
    //parameter and function to execute is passed as arguments
    xTimerPendFunctionCallFromISR(vDeferredHandlingFunction,NULL,Parameter_Daemon_Task,&xHigherPriorityTaskWoken);

    //Increamenting the parameter value each time the ISR is being called to execute
    Parameter_Daemon_Task++;

    //To Switch context to the higher priority task for deferring work to be done
    portYIELD_FROM_ISR();
}

void app_main(void)
{
    //Create the periodic task for generating software interrupt at a defined interval regularly
    xTaskCreate(Periodic_Function,"Periodic",2048,NULL,PeriodicTaskPriority,NULL);

    //Setting up interrupt handler based on the xtensa port function
    esp_intr_alloc(ETS_INTERNAL_SW1_INTR_SOURCE,0,Interrupt_Handler,NULL,NULL);

    while(1);
}