/**
 * @file main.c
 * @author Tushar Uttekar
 * 
 * @brief
 * 
 * In this example we are covering how the timer's ID can be used and implementing it with the callback routine application.
 * Also we have demonstrated how to use the software timer ID as timer specific storage.
 * Each software timer keeps a count of the number of times it has expired in its own ID and the auto-reload timer 
 * uses the count to stop itself the Nth time it executes.
 * 
 * From the previous example the change here is that instead of different callback routines for each timer we have used a single
 * callback routine for both the timers and we are just passing there handles in the arguments of the callback routines and based
 * on the arguments the code in the call back routines executes for that particular software timer.
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
#include "freertos/timers.h"
#include "esp_log.h"
#include "esp_system.h"

#define RTOS                    "freertos"
#define ONE_SHOT_TIMER_PERIOD   (pdMS_TO_TICKS(3000))
#define PERIODIC_TIMER_PERIOD   (pdMS_TO_TICKS(500))

xTimerHandle OneShot_Handle, Periodic_Handle;
BaseType_t OneShotTimer_Started, PeriodicTimer_Started;

static void TimerCallBack(TimerHandle_t Timer)
{
    TickType_t CurrentTime;
    uint32_t Execution_Count;

    //Calculating the number of time the execution for a particular timer has occured and changing the timer ID as well
    Execution_Count = (uint32_t) pvTimerGetTimerID(Timer);
    Execution_Count++;
    vTimerSetTimerID(Timer,(void*)Execution_Count);

    CurrentTime = xTaskGetTickCount();

    if(Timer == OneShot_Handle)
    {
        ESP_LOGI(RTOS,"One shot timer callback routine is executing at time %d.\r\n",CurrentTime);
    }
    else
    {
        ESP_LOGI(RTOS,"Periodic timer callback routine is executing at time %d.\r\n",CurrentTime);

        //Stopping the periodic timer after it has executed 10th time 
        if(Execution_Count == 10)
        {
            xTimerStop(Timer,0);
        }
    }
}

void app_main(void)
{
    //Creating one shot and periodic timer
    OneShot_Handle = xTimerCreate("OneShotTimer",ONE_SHOT_TIMER_PERIOD,pdFAIL,0,TimerCallBack);
    Periodic_Handle = xTimerCreate("PeriodicTimer",PERIODIC_TIMER_PERIOD,pdTRUE,0,TimerCallBack);

    //Validating whether timer's are created or not
    if((OneShot_Handle != NULL) && (Periodic_Handle != NULL))
    {
        //Starting both the timers individually
        OneShotTimer_Started = xTimerStart(OneShot_Handle,0);
        PeriodicTimer_Started = xTimerStart(Periodic_Handle,0);

        //Validating whether both the timer's have started or not
        if((OneShotTimer_Started == pdTRUE) && (PeriodicTimer_Started == pdTRUE))
        {
            printf("Both one shot and periodic timers are started!\r\n");
            printf("Scheduler is starting....................\r\n");
        }
    }

    while(1);
}