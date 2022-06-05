/**
 * @file main.c
 * @author Tushar Uttekar
 * 
 * @brief 
 * 
 * This example demonstrates the use of mutex using which we implement the concept of mutual exclusion for shared resource.
 * 
 * For this example we use two instances of one task and pass the string parameters which are to be printed on the output
 * terminal using the print string function inside which the mutex is taken with which the common resource is kept safe in
 * our case.
 * 
 * We are basically trying to control the access to the common printf function call in the print string function so that at a 
 * particular time only one of the task instance is allowed to print its string completely rather than distorting output from 
 * all the access from different instance of the task.
 * 
 * @version 0.1
 * @date 2022-06-02
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

SemaphoreHandle_t Mutex_Handler;

static void Print_String(const char* InputString)
{
    //Mutex is taken here by particular instance of the task using which mutual exclusion concept is implemented
    xSemaphoreTake(Mutex_Handler,portMAX_DELAY);

    printf("%s",InputString);

    //Mutex is given back so that other task can also have the access to the common resource which is being shared
    xSemaphoreGive(Mutex_Handler);
}

static void Print_Task(void* pvParameters)
{
    char* StringToPrint;
    const TickType_t BlockTime = 0x20;

    StringToPrint = (char*) pvParameters;

    for(;;)
    {
        Print_String(StringToPrint);
        vTaskDelay((rand() % BlockTime));
    }
}

void app_main(void)
{
    //Create Mutex semaphore
    Mutex_Handler = xSemaphoreCreateMutex();

    //Validate whether mutex is created or not
    if(Mutex_Handler != NULL)
    {
        //Create two instances of the printing task with different input strings to be printed as parameters
        xTaskCreate(Print_Task,"First String",2048,"First Symbols --->  ~!@#$%%^&*()_+\r\n",1,NULL);
        xTaskCreate(Print_Task,"Second String",2048,"Second Symbols --->  []|;',./?><:'{}\r\n",2,NULL);
    }

    while(1);
}