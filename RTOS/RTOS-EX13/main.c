/**
 * @file main.c
 * @author Tushar Uttekar
 * 
 * @brief 
 * 
 * In this example code we are covering the basic's of the software timer's of the freertos.
 * We have implemented how the timer's are created using the freertos timer API, eg: xTimerCreate
 * We have implemented how the timer's are started using the freertos timer API, eg: xTimerStart
 * We have implemented how the timer's callback routines for each of the timer which is created.
 * 
 * There are two types of timer which are created in this code
 * 1) One shot timer which executes only once when its tick periods completes and then it goes from running state to daemon state.
 * 2) Periodic timer which executed periodically by reloading it's tick count once it is expired.
 * 
 * Timer can be stop exclusively by calling the xTimerStop function anytime.
 * Once the timer is stopped it goes from running state to the daemon state.
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

#define ONE_SHOT_TIMER_PERIOD   (pdMS_TO_TICKS(3000))
#define PERIODIC_TIMER_PERIOD   (pdMS_TO_TICKS(500))

xTimerHandle OneShot_Handle, Periodic_Handle;
BaseType_t OneShotTimer_Started, PeriodicTimer_Started;

static void OneShotCallBack(TimerHandle_t Timer)
{
    TickType_t CurrentTime;

    CurrentTime = xTaskGetTickCount();
    printf("One shot timer call back routine executing at time %d.\r\n",CurrentTime);
}

static void PeriodicCallBack(TimerHandle_t Timer)
{
    TickType_t CurrentTime;

    CurrentTime = xTaskGetTickCount();
    printf("Periodic timer call back routine executing at time %d.\r\n",CurrentTime);
}

void app_main(void)
{
    //Creating one shot and periodic timer
    OneShot_Handle = xTimerCreate("OneShotTimer",ONE_SHOT_TIMER_PERIOD,pdFAIL,0,OneShotCallBack);
    Periodic_Handle = xTimerCreate("PeriodicTimer",PERIODIC_TIMER_PERIOD,pdTRUE,0,PeriodicCallBack);

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