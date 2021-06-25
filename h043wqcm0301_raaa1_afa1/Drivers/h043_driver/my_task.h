#ifndef _MY_TASK_H_
#define _MY_TASK_H_


#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "hmi_config.h"


#define START_TASK_PRIO		1
	
#define START_STK_SIZE 		128  

extern TaskHandle_t StartTask_Handler;

extern void start_task(void *pvParameters);




#define TOUCH_TASK_PRIO		29
	
#define TOUCH_STK_SIZE 		200

extern TaskHandle_t TOUCHTask_Handler;

extern void touch_task(void *pvParameters);








#define MESSAGE_TASK_PRIO		26
	
#define MESSAGE_STK_SIZE 		1024

extern TaskHandle_t MESSAGETask_Handler;

void message_task(void *pvParameters);


#define USART_TASK_PRIO		28
	
#define USART_STK_SIZE 		512

extern TaskHandle_t USARTTask_Handler;

extern void usart_task(void *pvParameters);


#define UPDATE_SCREEN_TASK_PRIO		10 
	
#define UPDATE_SCREEN_STK_SIZE 		512

extern TaskHandle_t UPDATE_SCREENTask_Handler;



#define EXTERNAL_CMD_TASK_PRIO		28
	
#define EXTERNAL_CMD_STK_SIZE 		512

extern TaskHandle_t EXTERNAL_CMDTask_Handler;

extern void external_cmd_task(void *pvParameters);



#define CMD_PRASE_TASK_PRIO		28
	
#define CMD_PRASE_STK_SIZE 		512

extern TaskHandle_t CMD_PRASETask_Handler;

extern void cmd_prase_task(void *pvParameters);




#define KB_TASK_PRIO		27
	
#define KB_STK_SIZE 		100

extern TaskHandle_t KB_Task_Handler;

extern void keyboard_task(void *pvParameters);



#define RTC_TASK_PRIO		27
	
#define RTC_STK_SIZE 		200

extern TaskHandle_t RTC_Task_Handler;

extern void update_rtc_task(void *pvParameters);



#define WIN_TASK_PRIO		27
	
#define WIN_STK_SIZE 		100

extern TaskHandle_t WIN_Task_Handler;

extern void win_task(void *pvParameters);

//#define USBMSC_TASK_PRIO		28
//	
//#define USBMSC_STK_SIZE 		100

//extern TaskHandle_t USBMSC_Task_Handler;

//extern void usbmsc_task(void *pvParameters);



#define WATCHDOG_TASK_PRIO		2
	
#define WATCHDOG_STK_SIZE 		100

extern TaskHandle_t WATCHDOG_Task_Handler;

extern void watchdog_task(void *pvParameters);


#if   (DIR_SWITCH_SUPPORT)

#define LIS3DH_TASK_PRIO		9
	
#define LIS3DH_STK_SIZE 		50

extern TaskHandle_t LIS3DH_Task_Handler;

extern void lis3dh_task(void *pvParameters);

#endif











#endif /*_MY_TASK_H_*/

