/*
This example is used to demonstrate blocking and non blocking tasks running together; difference between them is
two tasks have same priority which prints the string continously on the other hand the last task has highest priority
which prints the string based on its periodic execution.
*/


#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_system.h"

void vTask1(void *pvParameters)
{
    const char* printstr = "Task 1 is executing non stop\r\n";

    for(;;)
    {
        //Output Statement/String
        printf(printstr);
    }
}

void vTask2(void *pvParameters)
{
    const char* printstr = "Task 2 is executing non stop\r\n";

    for(;;)
    {
        //Output Statement/String
        printf(printstr);
    }    
}

void vTask3(void *pvParameters)
{
    const char* printstr = "Task 3 is executing periodically\r\n";
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
    xTaskCreate(vTask1,"TASK1",1000,NULL,1,NULL);
    xTaskCreate(vTask2,"TASK2",1000,NULL,1,NULL);
    xTaskCreate(vTask3,"TASK3",1000,NULL,2,NULL);

    //Call the scheduler once the task are created
    //vTaskStartScheduler();

    while(1);
}