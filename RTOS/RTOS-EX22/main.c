/**
 * @file main.c
 * @author Tushar Uttekar
 * 
 * @brief 
 * 
 * In this example we have demonstrated how the EventGroup feature of FREERTOS can we implemented and used for Blocking and
 * Unblocking of the task based on the event bits which basically represents the events in the real application.
 * 
 * There are three tasks used in this example:
 * 1) To Write the Event bits to the Event Group
 * 2) To Read the Event bits from the Event Group
 * 3) To Periodically generate interrupt for the ISR event bit.
 * 
 * ->The deferred task prints the input string from the ISR routine.
 * 
 * The output of the example code can be changed by setting the Waitforallbits to either True/False.
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
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "freertos/portmacro.h"
#include "freertos/xtensa_api.h"
#include "xtensa/core-macros.h"
#include "esp_intr_alloc.h"
#include "esp_log.h"
#include "esp_system.h"

#define SW_ISR_LEVEL_3      29
#define FIRST_BIT_SET       (1 << 0)
#define SECOND_BIT_SET      (1 << 1)
#define ISR_BIT_SET         (1 << 2)

EventGroupHandle_t EventGroup_Handle;

static void Interrupt_Generator(void* pvParameters)
{
    for(;;)
    {
        xt_set_intset(1 << SW_ISR_LEVEL_3);
        vTaskDelay(pdMS_TO_TICKS(300));
    }
}

static void vDeferredHandlingFunction( void *pvParameter1, uint32_t ulParameter2 )
{
    char* Print_String;

    Print_String = (char*) pvParameter1;
    
    /* Process the event - in this case just print out a message received in pvParameter1. ulParameter2 is not used in this example. */
    printf("%s",Print_String);
}

static void EventBitRead_Task(void* pvParameters)
{
    EventBits_t EventGroupBit;
    const EventBits_t WaitingBits = (FIRST_BIT_SET|SECOND_BIT_SET|ISR_BIT_SET);

    for(;;)
    {
        //Checking whether any particular event has occured using the event group variable
        EventGroupBit = xEventGroupWaitBits(EventGroup_Handle,WaitingBits,pdTRUE,pdTRUE,portMAX_DELAY);

        //Checking for the First Event Bit
        if((EventGroupBit & FIRST_BIT_SET) != NULL)
        {
            printf("Bit 1st was set-\t From the Bit Reading Task.\r\n");
        }
        //Checking for the Second Event Bit
        if((EventGroupBit & SECOND_BIT_SET) != NULL)
        {
            printf("Bit 2nd was set-\t From the Bit Reading Task.\r\n");
        }
        //Checking for the ISR Event Bit
        if((EventGroupBit & ISR_BIT_SET) != NULL)
        {
            printf("Bit From ISR was set-\t From the Bit Reading Task.\r\n");
        }
    }
}

static void EventBitWrite_Task(void* pvParameters)
{
    for(;;)
    {
        vTaskDelay(pdMS_TO_TICKS(200));

        //Setting First Bit in the event group
        printf("Setting the BIT 1-\t From the Bit Writting Task.\r\n");
        xEventGroupSetBits(EventGroup_Handle,FIRST_BIT_SET);

        vTaskDelay(pdMS_TO_TICKS(200));

        //Setting Second Bit in the event group
        printf("Setting the BIT 2-\t From the Bit Writting Task.\r\n");
        xEventGroupSetBits(EventGroup_Handle,SECOND_BIT_SET);
    }
}

static void Interrupt_Handler(void* arg)
{
    BaseType_t xHigherPriorityTaskWoken;
    static const char* string = "Setting the ISR BIT-\t From the ISR routine.\r\n";

    xt_set_intclear(1 << SW_ISR_LEVEL_3);

    //Setting the variable to FALSE for the first time when interrupt occurs as it will TRUE if context switch is required
    xHigherPriorityTaskWoken = pdFALSE;

    //Timer pending function will be called for performing the deferred processing in the daemon task in which the function
    //parameter and function to execute is passed as arguments
    xTimerPendFunctionCallFromISR(vDeferredHandlingFunction,(void*)string,0,&xHigherPriorityTaskWoken);

    //Setting ISR Bit in the event group
    xEventGroupSetBitsFromISR(EventGroup_Handle,ISR_BIT_SET,&xHigherPriorityTaskWoken);

    //To Switch context to the higher priority task for deferring work to be done
    portYIELD_FROM_ISR();
}

void app_main(void)
{
    //Create the event groupt
    EventGroup_Handle = xEventGroupCreate();

    //Create the task for setting, reading the event group bits and generating software interuppt periodically 
    xTaskCreate(EventBitWrite_Task,"Event_Write",2048,NULL,1,NULL);
    xTaskCreate(EventBitRead_Task,"Event_Read",2048,NULL,2,NULL);
    xTaskCreate(Interrupt_Generator,"Interrupt_Trigger",2048,NULL,3,NULL);

    //Setting up interrupt handler based on the xtensa port function
    esp_intr_alloc(ETS_INTERNAL_SW1_INTR_SOURCE,0,Interrupt_Handler,NULL,NULL);

    while(1);
}