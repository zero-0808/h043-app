#ifndef __BUZZER_H__
#define __BUZZER_H__

//#include "lpc_types.h"
#include "stm32h7xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"

typedef struct buzzer_device
{
	void (*init)(void);
	void (*act)(uint8_t);
	uint8_t flag1;
	uint8_t flag2;

}bz_device;



extern bz_device bz_dev;

void set_buzzer_mode(uint8_t mode , uint32_t unitms);





#endif
