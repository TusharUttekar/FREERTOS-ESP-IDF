/**
 * @file main.c
 * @author Tushar Uttekar
 * 
 * @brief 
 * 
 * This example demonstrates the use of a gatekeeper concept which can be used for preventing the issue's for the deadlock
 * and priority inversion when implementing the mutual exclusion of the common resources.
 * 
 * Gatekeeper tasks provide a clean method of implementing mutual exclusion without the risk of priority inversion or deadlock.
 * A gatekeeper task is a task that has sole ownership of a resource.
 *  
 * Only the gatekeeper task is allowed to access the resource directly—any other task needing to access the resource can
 * do so only indirectly by using the services of the gatekeeper. 
 * 
 * NOTE : Keep in mind that the vApplicationTickHook function here implemented is specfic to this example only.
 *        ESP32 has its own implementaion for the TickHook function which is used for its application.
 *               
 * Warning : In the below code the Tick Hook function of ESP32 is commented out due to which the core reset's by its own due to the
 *           dependency of other ESP32 related functions on the Tick Hook Function.
 * 
 * @version 0.1
 * @date 2022-06-04
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/portmacro.h"
#include "freertos/xtensa_api.h"
#include "xtensa/core-macros.h"
#include "esp_intr_alloc.h"
#include "esp_log.h"
#include "esp_system.h"

const char* strings[] = {"Task 1 printing the string message through the gatekeeper task\r\n",
                         "Task 2 printing the string message through the gatekeeper task\r\n",
                         "Tick Hook function printing the string message through the gatekeeper task\r\n"
                         };
QueueHandle_t GateKeeper_Queue;

static void GateKeeper_Task(void* pvParameters)
{
    char* print_string;

    for(;;)
    {
        //The received data over the queue will be printed on the terminal output
        xQueueReceive(GateKeeper_Queue,&print_string,portMAX_DELAY);

        printf("%s",print_string);
    }
}

static void Print_Task(void* pvParameters)
{
    char *string;
    
    string = (char*) pvParameters;

    for(;;)
    {
        //Sending the strings which are received from the parameters to the gatekeeper queue
        xQueueSendToBack(GateKeeper_Queue,&string,0);
        vTaskDelay((rand() % (0x20)));
    }
}

void vApplicationTickHook(void)
{
    static int count = 0;

    count++;

    //Here the string will be sent to the front of the queue at an interval of 200 ticks
    if(count >= 200)
    {
        xQueueSendToFrontFromISR(GateKeeper_Queue,&strings[2],NULL);
        count = 0;
    }
}

void app_main(void)
{
    //Create queue for the gatekeeper operation
    GateKeeper_Queue = xQueueCreate(5,sizeof(char*));

    //Validate whether queue is created or not
    if(GateKeeper_Queue != NULL)
    {
        //Create two instance of the task's which will print the string
        xTaskCreate(Print_Task,"Print from 1st instance",2048,(void*)strings[0],1,NULL);
        xTaskCreate(Print_Task,"Print from 2nd instance",2048,(void*)strings[1],2,NULL);

        //GateKeeper task which will print the strings to the output terminal
        xTaskCreate(GateKeeper_Task,"GateKeeper",2048,NULL,0,NULL);
    }

    while(1);
}