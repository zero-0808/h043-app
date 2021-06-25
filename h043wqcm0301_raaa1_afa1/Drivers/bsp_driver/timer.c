#include "FreeRTOS.h"						  
#include "task.h"
#include "stm32h7xx_hal.h"
#include "timer.h"

extern void xPortSysTickHandler(void);
static uint8_t  fac_us=0;							//us��ʱ������			   
static uint16_t fac_ms=0;	

//void SysTick_Handler(void)
//{
//	if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED)
//	{
//		xPortSysTickHandler();
//	}
//}



void delay_init(void)
{
	
	uint32_t reload;
	fac_us=SystemCoreClock/1000000;
	reload=SystemCoreClock/1000000;
	fac_ms=1000/configTICK_RATE_HZ;	
	reload*=1000000/configTICK_RATE_HZ;
	SysTick_Config(reload);
}

void delay_us(uint32_t nus)
{
	uint32_t  ticks;
	uint32_t  told,tnow,tcnt=0;
	uint32_t  reload=SysTick->LOAD;
	ticks=nus*fac_us;
	tcnt=0;
	told=SysTick->VAL;
	while(1)
	{
		tnow=SysTick->VAL;
		if(tnow!=told)
		{
			if(tnow<told)
			{
				tcnt+=told-tnow;
			}
			else
			{
				tcnt+=reload-tnow+told;
			}
			told=tnow;
			if(tcnt>=ticks)
				break;
			
		}
	}
}

void delay_ms(uint32_t nms)
{
	if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED)
	{
		if(nms>=fac_ms)		
		{
			vTaskDelay(nms/fac_ms);	
		}
		nms%=fac_ms;
	}
	delay_us((uint32_t)(nms*1000));	
}

void delay_xms(uint32_t nms)
{
	uint32_t i;
	for(i=0;i<nms;i++) delay_us(1000);
}
