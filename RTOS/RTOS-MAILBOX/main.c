/**
 * @file main.c
 * @author Tushar Uttekar
 * 
 * @brief 
 * 
 * A mailbox is used to hold data that can be read by any task, or any interrupt service routine.
 * The data does not pass through the mailbox, but instead remains in the mailbox until it is overwritten.
 * The sender overwrites the value in the mailbox.
 * The receiver reads the value from the mailbox, but does not remove the value from the mailbox.
 * 
 * We have created two functions one for updating the mailbox data and another one for reading the data from the mailbox.
 * 
 * Our task first updates the value which it the passes on to the function which updates the mailbox and then we read the data
 * from the mail box for validating whether the value has been updated succesfully or not.
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

xQueueHandle xMailBox;

typedef struct xMailBox
{
    TickType_t TimeStamp;
    int32_t Data_Value;
}MailBox_t;

void Create_MailBox(void)
{
    xMailBox = xQueueCreate(1,sizeof(MailBox_t));
}

void Update_MailBox(int32_t Updated_Value)
{
    MailBox_t Update_Box;

    Update_Box.Data_Value = Updated_Value;
    Update_Box.TimeStamp = xTaskGetTickCount();

    xQueueOverwrite(xMailBox,&Update_Box);
}

BaseType_t Read_MailBox(MailBox_t* Read)
{
    TickType_t Previous_TimeStamp;
    BaseType_t Updated_Data;

    //Storing the timestamp before it is updated by another call
    Previous_TimeStamp = Read->TimeStamp;

    xQueuePeek(xMailBox,Read,pdMS_TO_TICKS(100));

    //The function will return true only if the data is updated since the last call of this function
    if(Read->TimeStamp > Previous_TimeStamp)
    {
        Updated_Data = pdTRUE;
    }
    else
    {
        Updated_Data = pdFALSE;
    }

    return Updated_Data;
}

void Task_Function(void* pvParameters)
{
    int32_t Updating_Value = 0;
    MailBox_t Task_MailBox;
    BaseType_t Validate_Queue_Read;

    for(;;)
    {
        //Value which is to be updated in the mailbox
        Updating_Value++;

        //Function which updates the data in mailbox
        Update_MailBox(Updating_Value);

        //Function for reading the data from the mailbox
        Validate_Queue_Read = Read_MailBox(&Task_MailBox);

        //Validating whether data is updated or not
        if(Validate_Queue_Read == pdTRUE)
        {
            printf("The Updated Value is %d at time %d.\r\n",Task_MailBox.Data_Value,Task_MailBox.TimeStamp);
        }
        else
        {
            printf("Value is not Updated yet!!!!.\r\n");
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void app_main(void)
{
    //Creating the MailBox
    Create_MailBox();

    //Task for updating the mailbox value and also validating the updation of mailbox
    xTaskCreate(Task_Function,"MailBox-Task",2048,NULL,1,NULL);

    while(1);
}