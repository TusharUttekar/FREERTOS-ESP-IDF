/**
 * @file main.c
 * @author Tushar Uttekar
 * 
 * @brief 
 * 
 * In the example we demonstrate the concept of the event synchronization which can be usefull when mutiple task's needs to be
 * synced together for the proper operation.
 * 
 * We are using the basic concept for synching in our case in which we are just synchronizing three different instances of the same
 * task.
 * 
 * Once the event bits for all the instances are acheived the task will unblock for each instance and will print out string on the
 * terminal representing synching is achieved.
 * 
 * @version 0.1
 * @date 2022-06-05
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "freertos/portmacro.h"
#include "freertos/xtensa_api.h"
#include "xtensa/core-macros.h"
#include "esp_intr_alloc.h"
#include "esp_log.h"
#include "esp_system.h"

#define RTOS                "FREERTOS"
#define FIRST_BIT_SET       (1 << 0)
#define SECOND_BIT_SET      (1 << 1)
#define THIRD_BIT_SET       (1 << 2)

EventGroupHandle_t EventSynchronous_Handle;

static void Synchronous_Task(void* pvParameters)
{
    const TickType_t Max_Delay = pdMS_TO_TICKS(4000);
    const TickType_t Min_Delay = pdMS_TO_TICKS(500);
    TickType_t Delay_Time;
    EventBits_t ThisTaskSyncBit;
    const EventBits_t AllSyncBit = (FIRST_BIT_SET|SECOND_BIT_SET|THIRD_BIT_SET);

    ThisTaskSyncBit = (EventBits_t) pvParameters;

    for(;;)
    {
        Delay_Time = (rand() % Max_Delay) + Min_Delay;
        vTaskDelay(Delay_Time);

        //printf("%s reached synch point.\r\n",pcTaskGetTaskName(NULL));

        ESP_LOGI(RTOS,"%s reached synch point.\r\n",pcTaskGetTaskName(NULL));

        //The event group will unblock the task once the sync condition is achieved
        xEventGroupSync(EventSynchronous_Handle,ThisTaskSyncBit,AllSyncBit,portMAX_DELAY);

        //printf("%s exited synch point.\r\n",pcTaskGetTaskName(NULL));

        ESP_LOGI(RTOS,"%s exited synch point.\r\n",pcTaskGetTaskName(NULL));           
    }
}

void app_main(void)
{
    //Create the event group for the synching the task's
    EventSynchronous_Handle = xEventGroupCreate();

    //Create three instance of the same task which will be synched
    xTaskCreate(Synchronous_Task,"Synching-1",2048,FIRST_BIT_SET,1,NULL);
    xTaskCreate(Synchronous_Task,"Synching-2",2048,SECOND_BIT_SET,1,NULL);
    xTaskCreate(Synchronous_Task,"Synching-3",2048,THIRD_BIT_SET,1,NULL);

    while(1);
}