/*
This example demonstrates how to use the api vTaskDelete for deleting a Task.
In the example below we have created two task with different priorities and we are making one of the task delete itself
in its own task function.

Task1 creates Task2 and then Task2 deletes itself in its main loop when it executes and this process repeates itself.
*/

#include <stdio.h>
#include "freertos/FREERTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_system.h"

xTaskHandle xTask2Handler = NULL;

void vTask2(void* pvParameters)
{
    printf("Task 2 has executed once and is about to delete itself\r\n");
    vTaskDelete(xTask2Handler);
}

void vTask1(void* pvParameters)
{
    const char* print_str = "Task 1 executing....\r\n";

    for(;;)
    {
        printf(print_str);
        xTaskCreate(vTask2,"TASK2",1000,NULL,2,&xTask2Handler);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void app_main(void)
{
    xTaskCreate(vTask1,"TASK1",1000,NULL,1,NULL);

    while(1);
}