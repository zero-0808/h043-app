#include "buzzer.h"
//#include "lpc177x_8x_pinsel.h"
//#include "lpc177x_8x_gpio.h"
#include "timer.h"
#include "cfg.h"
#include "gpio.h"


#define  BUZZER_TIME	(300*MS)

static void bz_init(void);
static void bz_act(uint8_t flag);

static TimerHandle_t timer1;
static TimerHandle_t timer2;
static uint8_t cyc_flag = 0;


bz_device bz_dev =
{
	bz_init,
	bz_act,
	0,
	0,

};



static void buzzer_timer1(TimerHandle_t timer)
{
	if(bz_dev.flag1 == 0)
	{
		bz_dev.act(0);
	}
	bz_dev.flag2 = 0;

	if(cyc_flag)
	{
		if(timer2)
		{
			xTimerStart(timer2,0);
		}
	}
}


static void buzzer_timer2(TimerHandle_t timer)
{
	bz_dev.act(1);
	bz_dev.flag2 = 1;

	if(timer1)
	{
		xTimerStart(timer1,0);
	}
}




static void bz_init(void)
{
//	PINSEL_ConfigPin(0,26,0);
//	GPIO_SetDir(0, 1<<26,GPIO_DIRECTION_OUTPUT);
//	GPIO_OutputValue(0, 1<<26, 1);
	L_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_SET);

	timer1 = xTimerCreate((const char *) "bz_timer1",
							(TickType_t)BUZZER_TIME,
							(UBaseType_t)pdFALSE,
							(void*	)NULL,
							(TimerCallbackFunction_t)buzzer_timer1);

	timer2 = xTimerCreate((const char *) "bz_timer2",
									(TickType_t)700*MS,
									(UBaseType_t)pdFALSE,
									(void*	)NULL,
									(TimerCallbackFunction_t)buzzer_timer2);


}


static void bz_act(uint8_t flag)
{
	if(flag)
	{
		//GPIO_OutputValue(0, 1<<26, 0);
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_RESET);
	}
	else
	{
		//GPIO_OutputValue(0, 1<<26, 1);
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_SET);
	}

}
/*
* unitms ??????????
*/
void set_buzzer_mode(uint8_t mode , uint32_t unitms)
{
	switch(mode)
	{
		case 0:
		{
			xTimerStop(timer1, 0);
			xTimerStop(timer2, 0);
			if(bz_dev.flag1 == 0)
			{
				bz_dev.act(0);
			}
			bz_dev.flag2 = 0;
			cyc_flag = 0;
		}
		break;

		case 1:
		{
			xTimerStop(timer1, 0);
			xTimerStop(timer2, 0);
			xTimerChangePeriod(timer2,700*MS, 0);
			xTimerReset(timer1, 0);
			bz_dev.act(1);
			bz_dev.flag2 = 1;
			cyc_flag = 1;
		}
		break;
		case 2:
		{
			xTimerStop(timer1, 0);
			xTimerStop(timer2, 0);
			xTimerChangePeriod(timer2,9700*MS, 0);
			xTimerReset(timer1, 0);
			xTimerReset(timer2, 0);
			bz_dev.act(1);
			bz_dev.flag2 = 1;
			cyc_flag = 1;
		}
		break;
		case 3:
		{
			xTimerStop(timer1, 0);
			xTimerStop(timer2, 0);
			xTimerChangePeriod(timer2,119700*MS, 0);
			xTimerReset(timer1, 0);
			xTimerReset(timer2, 0);
			bz_dev.act(1);
			bz_dev.flag2 = 1;
			cyc_flag = 1;
		}
		break;
		case 4:
		{
			xTimerStop(timer1, 0);
			xTimerStop(timer2, 0);
			bz_dev.act(1);
			bz_dev.flag2 = 1;
			cyc_flag = 0;

		}
		break;
		case 5:
		{
			xTimerStop(timer1, 0);
			xTimerStop(timer2, 0);
			bz_dev.act(1);
			bz_dev.flag2 = 1;
			cyc_flag = 0;
			xTimerReset(timer1, 0);
		}
		break;
		case 6:
		{//by liuzhiheng 2021-01-25
			xTimerStop(timer1, 0);
			xTimerStop(timer2, 0);
			xTimerChangePeriod(timer2,unitms*MS, 0);
			xTimerReset(timer1, 0);
			bz_dev.act(1);
			bz_dev.flag2 = 1;
			cyc_flag = 1;
		}
		break;
	}
}








