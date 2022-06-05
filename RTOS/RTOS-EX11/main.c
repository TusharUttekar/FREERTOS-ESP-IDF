/**
 * @file main.c
 * @author Tushar Uttekar
 * 
 * @brief 
 * 
 * The Below code shows the use case of basic FREERTOS Queue API's used for receiving data from multiple 
 * senders/instances of the sending task with a finite timeout which serves as a blocking period and receiving 
 * the data in one of the receiving tasks in a particular structured format.
 * 
 * Receiving Task has lower priority and sending task has higher priority which was vice-versa in example 10.
 * 
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

/*Define the source of the data which helps in identification*/
typedef enum
{
    Source1 = 0,
    Source2
}DataSource;

/*Structure which will be used for sending the data and sender's information along with it*/
typedef struct 
{
    int32_t DataVal;
    DataSource Source;
}QueueStruct;

/*Data which will be passed to the queue*/
static const QueueStruct xSendStruct[2] = { {123 , Source1}, {456 , Source2} };


xQueueHandle xQueue;

static void Sender_Task(void* pvParameters)
{
    BaseType_t xStatus;
    const TickType_t Timeout = pdMS_TO_TICKS(100);

    for(;;)
    {
        //Sending the data to the queue as we receive it in the arguments with a timeout
        xStatus = xQueueSendToBack(xQueue,pvParameters,Timeout);

        if(xStatus != pdPASS)
        {
            printf("Unable to send data to queue!!\r\n");
        }
    }
}

static void Receiver_Task(void* pvParameters)
{
     QueueStruct ReceiveData;
     BaseType_t xStatus;

     for(;;)
     {
         //Receiving Data from the sender task without any timeout
         xStatus = xQueueReceive(xQueue,&ReceiveData,0);

         if(xStatus == pdPASS)
         {
             if(ReceiveData.Source == Source1)
             {
                printf("Received Data from source 1 = %d\r\n",ReceiveData.DataVal);
             }
             else
             {
                printf("Received Data from source 2 = %d\r\n",ReceiveData.DataVal);
             }
         }
         else
         {
             printf("Unable to receive queue data!\r\n");
         }

         //Should be three since the task is of lowest priority it will only execute when sender task is in blocked state
         if(uxQueueMessagesWaiting(xQueue) != 3)
         {
             printf("Queue should be full!\r\n");
         }
     }
}

void app_main(void)
{
    xQueue = xQueueCreate(3,sizeof(QueueStruct));

    if(xQueue != NULL)
    {
        //Sender Task two independent instances
        xTaskCreate(Sender_Task,"Sender_I1",2048,(void*)&xSendStruct[0],2,NULL);
        xTaskCreate(Sender_Task,"Sender_I2",2048,(void*)&xSendStruct[1],2,NULL);

        //Receiver Task
        xTaskCreate(Receiver_Task,"Receiver",2048,NULL,1,NULL);
    }
    else
    {
        /*Represents that queue was not created due to insufficient heap space*/
    }
}