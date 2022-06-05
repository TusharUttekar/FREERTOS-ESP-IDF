/*
This example is used to demonstrate how the priority and delay/blocking time for each task affect the execution time
for individual tasks but instead of using the vTaskDelay API for blocking we will be using the vTaskDelayUntil which can 
precisly wakeup the task from the blocking state based on the input parameters of last tick count before removing from blocking
state and the frequency based on which the task will execute periodically
*/


#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_system.h"

void vTask1(void *pvParameters)
{
    const char* printstr = "Task 1 is executing....\r\n";
    TickType_t  xTaskLastWakeTime;
    xTaskLastWakeTime = xTaskGetTickCount();

    for(;;)
    {
        //Output Statement/String
        printf(printstr);
        //Soft Delay
        vTaskDelayUntil(&xTaskLastWakeTime,pdMS_TO_TICKS(10));
    }
}

void vTask2(void *pvParameters)
{
    const char* printstr = "Task 2 is executing....\r\n";
    TickType_t  xTaskLastWakeTime;
    xTaskLastWakeTime = xTaskGetTickCount();

    for(;;)
    {
        //Output Statement/String
        printf(printstr);
        //Soft Delay
        vTaskDelayUntil(&xTaskLastWakeTime,pdMS_TO_TICKS(50));
    }    
}

void app_main(void)
{
    //Create Task instances for individual tasks
    xTaskCreate(vTask1,"TASK1",1000,NULL,0,NULL);
    xTaskCreate(vTask2,"TASK2",1000,NULL,1,NULL);

    //Call the scheduler once the task are created
    //vTaskStartScheduler();

    while(1);
}