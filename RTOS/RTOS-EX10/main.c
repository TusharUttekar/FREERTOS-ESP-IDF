/**
 * @file main.c
 * @author Tushar Uttekar
 * @brief 
 * The Below code shows the use case of basic FREERTOS Queue API's used for sending and receiving data from a 
 * multiple instance of the sending task and receiving the data in one of the receiving tasks with a finite timeout
 * which serves as a blocking period
 * @version 0.1
 * @date 2022-05-26
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

xQueueHandle xQueue;

static void Sender_Task(void* pvParameters)
{
    int32_t Sending_Value;
    BaseType_t xStatus;

    Sending_Value = (int32_t) pvParameters;
    
    for(;;)
    {
        //Sending the data to the queue as we receive it in the arguments without any timeout
        xStatus = xQueueSendToBack(xQueue,&Sending_Value,100);

        if(xStatus != pdPASS)
        {
            printf("Unable to send data to queue!!\r\n");
        }
    }
}

static void Receiver_Task(void* pvParameters)
{
     int32_t Receiving_Value;
     BaseType_t xStatus;
     const TickType_t Timeout = pdMS_TO_TICKS(100);

     for(;;)
     {
         //Receiving Data from the sender task with a timeout of 100ms
         xStatus = xQueueReceive(xQueue,&Receiving_Value,Timeout);

         if(xStatus == pdPASS)
         {
             printf("Received Data = %d\r\n",Receiving_Value);
         }
         else
         {
             printf("Unable to receive queue data!\r\n");
         }

         //Should be zero always since we are removing data as soon as we receive any data
         if(uxQueueMessagesWaiting(xQueue) != 0)
         {
             printf("Queue should be empty!\r\n");
         }
     }
}

void app_main(void)
{
    xQueue = xQueueCreate(5,sizeof(int32_t));

    if(xQueue != NULL)
    {
        //Sender Task two independent instances
        xTaskCreate(Sender_Task,"Sender_I1",2048,(void*)100,1,NULL);
        xTaskCreate(Sender_Task,"Sender_I2",2048,(void*)200,1,NULL);

        //Receiver Task
        xTaskCreate(Receiver_Task,"Receiver",2048,NULL,2,NULL);
    }
    else
    {
        /*Represents that queue was not created due to insufficient heap space*/
    }
}