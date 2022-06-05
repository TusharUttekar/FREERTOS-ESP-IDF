/**
 * @file main.c
 * @author Tushar Uttekar
 * 
 * @brief 
 * 
 * In this example we are changing the time period of the timer which is running periodically.
 * 
 * If xTimerChangePeriod() is used to change the period of a timer that is already running, then the timer 
 * will use the new period value to recalculate its expiry time. The recalculated expiry time is relative 
 * to when xTimerChangePeriod() was called, not relative to when the timer was originally started.
 * 
 * If xTimerChangePeriod() is used to change the period of a timer that is in the Dormant state (a timer that is not running),
 * then the timer will calculate an expiry time, and transition to the Running state (the timer will start running).
 *
 * We are changing the time period of the timer in the call back routine once it has executed with the old time period for atleast
 * 10 times before it changes it time period.
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
#define PERIODIC_TIMER_PERIOD   (pdMS_TO_TICKS(500))

xTimerHandle TimerPeriodChange_Handle;
BaseType_t TimerPeriodChange_Started;

static void TimerCallBack(TimerHandle_t Timer)
{
    TickType_t CurrentTime;
    uint32_t Execution_Count;

    //Calculating the number of time the execution for a particular timer has occured and changing the timer ID as well
    Execution_Count = (uint32_t) pvTimerGetTimerID(Timer);
    Execution_Count++;
    vTimerSetTimerID(Timer,(void*)Execution_Count);

    CurrentTime = xTaskGetTickCount();

    if(Timer == TimerPeriodChange_Handle)
    {
        if(Execution_Count < 10)
        {
            ESP_LOGI(RTOS,"Periodic timer callback routine is executing before changing the timer period at time %d.\r\n",CurrentTime);
        }
        else if(Execution_Count > 10)
        {
            ESP_LOGI(RTOS,"Periodic timer callback routine is executing after changing the timer period at time %d.\r\n",CurrentTime);            
        }

        //Changing the timer period after it has executed 10th time with a period of 500ms
        if(Execution_Count == 10)
        {
            xTimerChangePeriod(Timer,pdMS_TO_TICKS(250),pdMS_TO_TICKS(100));
        }
    }
}

void app_main(void)
{
    //Creating periodic timer
    TimerPeriodChange_Handle = xTimerCreate("TimerPeriodChange",PERIODIC_TIMER_PERIOD,pdTRUE,0,TimerCallBack);

    //Validating whether timer is created or not
    if(TimerPeriodChange_Handle != NULL)
    {
        //Starting the periodic timer
        TimerPeriodChange_Started = xTimerStart(TimerPeriodChange_Handle,0);

        //Validating whether the timer have started or not
        if(TimerPeriodChange_Started == pdTRUE)
        {
            printf("Periodic timer is started!\r\n");
            printf("Scheduler is starting....................\r\n");           
        }
    }
}