/*
This Example describes how we can use parameter passing in task create fucntion for multiple instances of the same task.
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_system.h"

//Different Text fields for Task instances(Parameters)
static const char* Text1 = "Text 1 for instance 1.\r\n";
static const char* Text2 = "Text 2 for instance 2.\r\n";

void vTaskFunction(void *pvParameters)
{
    char *printstr;
    printstr = (char*) pvParameters;
    
    for(;;)
    {
        //Output Statement/String
        printf(printstr);
        //Soft Delay
        for(volatile uint32_t ul = 0; ul < 100; ul++)
        {

        }
    }
}

void app_main(void)
{
    //Create Task instances for individual tasks
    xTaskCreate(vTaskFunction,"TASKInstacne1",2048,(void*)Text1,1,NULL);
    xTaskCreate(vTaskFunction,"TASKInstacne2",2048,(void*)Text2,1,NULL);
    //Call the scheduler once the task are created
    //vTaskStartScheduler();

    while(1);
}