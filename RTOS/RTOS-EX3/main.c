/*
This example is used to demonstrate how the priority change affect the time slicing/time allocation for each task
the task with higher priority will always execute in this case and block the lower priority task.
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
    volatile uint8_t loop;

    for(;;)
    {
        //Output Statement/String
        printf(printstr);
        //Soft Delay
        //vTaskDelay(10);
    }
}

void vTask2(void *pvParameters)
{
    const char* printstr = "Task 2 is executing....\r\n";
    volatile uint8_t loop;

    for(;;)
    {
        //Output Statement/String
        printf(printstr);
        //Soft Delay
        //vTaskDelay(10);
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