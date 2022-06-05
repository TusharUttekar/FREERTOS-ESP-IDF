/*
This example is used to demonstrate the use of idlehook function which currently prints its execution count
in the output window which basically represents how many time the idle task is been called/executed during the 
blocking time of the main executing task.

Note : Comment Out the idle hook function of ESP which is already implemented in freertos_hooks.c file to execute your
       own implemented function.(Increase The stack size as well)
       ->   Idle task should never be blocking/suspending and should be countinously executed
       ->   If the application makes use of the vTaskDelete() API function, then the Idle task hook
            must always return to its caller within a reasonable time period. This is because the
            Idle task is responsible for cleaning up kernel resources after a task has been deleted.
*/


#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_system.h"

volatile uint32_t TickCount;

void vApplicationIdleHook(void)
{
    TickCount++;
}

void vTaskFucntion(void *pvParameters)
{
    const char* printstr = "Task is executing...\r\n";
    const TickType_t Delayms = pdMS_TO_TICKS(50);

    for(;;)
    {
        //Output Statement/String
        printf(printstr);
        printf("TickCount = %d\r\n",TickCount);
        //Delay
        vTaskDelay(Delayms);
    }
}

void app_main(void)
{
    //Create Task instances for individual tasks
    xTaskCreate(vTaskFucntion,"TASK",3000,NULL,1,NULL);

    //Call the scheduler once the task are created
    //vTaskStartScheduler();

    while(1);
}