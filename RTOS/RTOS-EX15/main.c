/**
 * @file main.c
 * @author Tushar Uttekar
 * 
 * @brief 
 * 
 * In this example we are covering how the timer can be reset using which we are simulating a keypad + backlight application.
 * 
 * Resetting a software timer means to re-start the timer, the timer’s expiry time is recalculated to be relative to when the
 * timer was reset, rather than when the timer was originally started.
 * 
 * The timer that has a period of 3000 ms being started, then reset it at a interval of 100 ms in the keypad task,
 * before eventually expiring and executing its callback function.
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
#define PERIODIC_TIMER_PERIOD   (pdMS_TO_TICKS(3000))

TaskHandle_t Keypad_Handle;
TimerHandle_t Backlight_Timer_Handle;
BaseType_t Backlight_Timer_Started, KeyPressed_t = pdTRUE;

static void BacklightCallBack(TimerHandle_t Timer)
{
    TickType_t CurrentTime;

    CurrentTime = xTaskGetTickCount();

    //Simulating the key not pressed action when the timer tick count expire
    KeyPressed_t = pdFALSE;

    ESP_LOGI(RTOS,"Timer has expired and is reloading..... at %d",CurrentTime);
}

void KeyPad_Task(void* pvParameters)
{
    TickType_t CurrentTime;

    for(;;)
    {
        CurrentTime = xTaskGetTickCount();

        //When the key pressed action is simulated
        if(KeyPressed_t == pdTRUE)
        {
            ESP_LOGI(RTOS,"One of the keys is pressed on the keypad during %d.\r\n",CurrentTime);
            xTimerReset(Backlight_Timer_Handle,pdMS_TO_TICKS(100));
        }
        else    //When non of the key pressed action is simulated
        {
            ESP_LOGI(RTOS,"Non of the keys is pressed on the keypad until %d.\r\n",CurrentTime);
        }   

        vTaskDelay(100);
    }
}

void app_main(void)
{
    //Creating timer for backlight function
    Backlight_Timer_Handle = xTimerCreate("Backlight_Timer",PERIODIC_TIMER_PERIOD,pdTRUE,0,BacklightCallBack);

    //Creating task for virtual keypad
    xTaskCreate(KeyPad_Task,"Keypad",2048,NULL,1,NULL);

    //Validating whether timer is created or not
    if(Backlight_Timer_Handle != NULL)
    {
        //Starting the back light timer
        Backlight_Timer_Started = xTimerStart(Backlight_Timer_Handle,0);

        //Validating whether timer has started or not
        if(Backlight_Timer_Started == pdTRUE)
        {
            printf("Backlight timer is started!\r\n");
            printf("Scheduler is starting....................\r\n"); 
        }
    }
}