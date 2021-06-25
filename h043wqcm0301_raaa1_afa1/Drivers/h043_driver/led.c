

#include "led.h"


#if   LED_SUPPORT


//#include "lpc177x_8x_pinsel.h"
//#include "lpc177x_8x_gpio.h"
#include "gpio.h"
#include "FreeRTOS.h"
//#include "task.h"
//#include "semphr.h"
#include "timers.h"
#include "cfg.h"



#define LED_G_PORT          GPIOG
#define LED_G_PIN           GPIO_PIN_14

#define LED_R_PORT          GPIOE
#define LED_R_PIN           GPIO_PIN_3

#define LED_GREEN       {HAL_GPIO_WritePin(LED_R_PORT,GPIO_PIN_3,GPIO_PIN_RESET);HAL_GPIO_WritePin(GPIOG,GPIO_PIN_14,GPIO_PIN_SET);}
#define LED_RED			{HAL_GPIO_WritePin(LED_R_PORT,GPIO_PIN_3,GPIO_PIN_RESET);HAL_GPIO_WritePin(GPIOG,GPIO_PIN_14,GPIO_PIN_SET);}
#define LED_YELLOW		{HAL_GPIO_WritePin(LED_R_PORT,GPIO_PIN_3,GPIO_PIN_SET);HAL_GPIO_WritePin(GPIOG,GPIO_PIN_14,GPIO_PIN_RESET);}
#define LED_OFF			{HAL_GPIO_WritePin(LED_R_PORT,GPIO_PIN_3,GPIO_PIN_SET);HAL_GPIO_WritePin(GPIOG,GPIO_PIN_14,GPIO_PIN_SET);}





typedef struct
{
	uint8_t  mode;		//模式
	uint8_t  flag;  	//标记led状态
	TimerHandle_t timer;
}led_control_t;


static led_control_t  tled;





static void led_mode_manage(void)
{
	switch(tled.mode)
	{
		case 0:
			xTimerStop(tled.timer,2);
			LED_OFF;
			tled.flag = 0;
			break;
		case 1:
			xTimerStop(tled.timer,2);
			LED_GREEN;
			tled.flag = 1;
			break;
		case 2:
			xTimerStop(tled.timer,2);
			LED_RED;
			tled.flag = 1;
			break;
		case 3:
			xTimerStop(tled.timer,2);
			LED_YELLOW;
			tled.flag = 1;
			break;
		case 4:
			xTimerStop(tled.timer,2);
			xTimerReset(tled.timer,2);
			xTimerStart(tled.timer,2);
			LED_RED;
			tled.flag = 1;
			break;
		case 5:
			xTimerStop(tled.timer,2);
			xTimerReset(tled.timer,2);
			xTimerStart(tled.timer,2);
			LED_YELLOW;
			tled.flag = 1;
			break;
		default:
			break;
			
	}
}





uint8_t set_led_mode(uint8_t mode)
{
	if(mode < 6)
	{
		tled.mode = mode;
		led_mode_manage();
		return 0;
	}
	else
	{
		return 1;
	}
}






static void led_timer(TimerHandle_t timer)
{	
	switch(tled.mode)
	{
		case 4:
			{
				if(tled.flag)
				{
					LED_OFF;
					tled.flag = 0;
				}
				else
				{
					LED_RED;
					tled.flag = 1;
				}
				
			}
			break;
		case 5:
			{
				if(tled.flag)
				{
					LED_OFF;
					tled.flag = 0;
				}
				else
				{
					LED_YELLOW;
					tled.flag = 1;
				}
			}
	}
	
}











void   led_init(void)
{

	LED_OFF;
	tled.timer = xTimerCreate((const char *) "led",
								(TickType_t)1000*MS,
								(UBaseType_t)pdTRUE,
								(void*	)NULL,
								(TimerCallbackFunction_t)led_timer); 

}







#endif






