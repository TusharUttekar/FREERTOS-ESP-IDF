/*
This example is used to demonstrate how the priority of the task can be changed using the vTaskPrioritySet API with which
we can change the priority of the task from any other relative task to it and we can also change the priority of its own
by passing the NULL as an argument(Task).

Parameters which needs to be passed for changing the priority are the task of which the priority is to be changed and the
new priority state. 
*/


#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_system.h"

xTaskHandle xTask2Handler = NULL;

void vTask1(void *pvParameters)
{
    const char* printstr = "Task 1 is executing....\r\n";
    UBaseType_t PriorityGet;
    TickType_t  xTaskLastWakeTime;
    xTaskLastWakeTime = xTaskGetTickCount();

    PriorityGet = uxTaskPriorityGet(NULL);

    for(;;)
    {
        //Output Statement/String
        printf(printstr);
        //Changing the task priority
        printf("Changing the priority of task 2 higher then task 1\r\n");
        vTaskPrioritySet(xTask2Handler,(PriorityGet + 1));
        //Soft Delay
        vTaskDelayUntil(&xTaskLastWakeTime,pdMS_TO_TICKS(50));
    }
}

void vTask2(void *pvParameters)
{
    const char* printstr = "Task 2 is executing....\r\n";
    UBaseType_t PriorityGet;
    TickType_t  xTaskLastWakeTime;
    xTaskLastWakeTime = xTaskGetTickCount();

    PriorityGet = uxTaskPriorityGet(NULL);

    for(;;)
    {
        //Output Statement/String
        printf(printstr);
        //Changing the task priority
        printf("Changing the priority of task 2 lower then task 1\r\n");
        vTaskPrioritySet(NULL,(PriorityGet - 2));
        //Soft Delay
        vTaskDelayUntil(&xTaskLastWakeTime,pdMS_TO_TICKS(50));
    }    
}

void app_main(void)
{
    //Create Task instances for individual tasks
    xTaskCreate(vTask1,"TASK1",1000,NULL,1,NULL);
    xTaskCreate(vTask2,"TASK2",1000,NULL,0,&xTask2Handler);

    //Call the scheduler once the task are created
    //vTaskStartScheduler();

    while(1);
}