/*
This example is used to demonstrate how the priority and delay/blocking time for each task affect the execution time
for individual tasks using the vTaskDelay API which converts the delay specified in millis to the number of ticks 
with the help of macro pdMS_TO_TICKS.
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
    const TickType_t xDelay10ms = pdMS_TO_TICKS(10);

    for(;;)
    {
        //Output Statement/String
        printf(printstr);
        //Soft Delay
        vTaskDelay(xDelay10ms);
    }
}

void vTask2(void *pvParameters)
{
    const char* printstr = "Task 2 is executing....\r\n";
    const TickType_t xDelay100ms = pdMS_TO_TICKS(100);

    for(;;)
    {
        //Output Statement/String
        printf(printstr);
        //Soft Delay
        vTaskDelay(xDelay100ms);
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