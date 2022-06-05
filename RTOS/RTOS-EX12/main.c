/**
 * @file main.c
 * @author Tushar Uttekar
 * 
 * @brief 
 * 
 * This example creates two sending tasks and one receiving task. The sending tasks send data
 * to the receiving task on two separate queues, one queue for each task. The two queues are
 * added to a queue set, and the receiving task reads from the queue set to determine which of
 * the two queues contain data.
 * 
 * @version 0.1
 * @date 2022-05-29
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdio.h>
#include "freertos/FREERTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_system.h"

static xQueueHandle xQueue1 = NULL , xQueue2 = NULL;
static xQueueSetHandle xQueueSet = NULL;


void Sending_Task1(void* pvParameters)
{
    const char* const Queue_String = "Sending the string from task1!!!\r\n";

    for(;;)
    {
        xQueueSendToBack(xQueue1,&Queue_String,0);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void Sending_Task2(void* pvParameters)
{
    const char* const Queue_String = "Sending the string from task2!!!\r\n";

    for(;;)
    {
        xQueueSendToBack(xQueue2,&Queue_String,0);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void Receiving_Task(void* pvParameters)
{
    xQueueHandle xQueue_Data_Receive;
    char* ReceiveString;

    for(;;)
    {
        xQueue_Data_Receive = (xQueueHandle) xQueueSelectFromSet(xQueueSet,pdMS_TO_TICKS(200));
        xQueueReceive(xQueue_Data_Receive,&ReceiveString,0);
        printf("%s",ReceiveString);
    }
}

void app_main(void)
{
    xQueue1 = xQueueCreate(1,sizeof(char*));
    xQueue2 = xQueueCreate(1,sizeof(char*));

    xQueueSet = xQueueCreateSet(1 * 2);
    
    xQueueAddToSet(xQueue1,xQueueSet);
    xQueueAddToSet(xQueue2,xQueueSet);

    xTaskCreate(Sending_Task1,"Sending1",2048,NULL,0,NULL);
    xTaskCreate(Sending_Task2,"Sending2",2048,NULL,0,NULL);

    xTaskCreate(Receiving_Task,"Receiving",2048,NULL,1,NULL);

    while(1);
}