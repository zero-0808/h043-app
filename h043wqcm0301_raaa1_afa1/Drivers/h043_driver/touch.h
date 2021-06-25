#ifndef _TOUCH_H_
#define _TOUCH_H_


#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"



typedef struct tp_device
{
	void (*init)(void);
	uint8_t (*get_pos)(void);
	uint32_t (*get_seamphore)(uint32_t);
	SemaphoreHandle_t  tp_press;
	TimerHandle_t timer;
	uint8_t * pdate;
	uint16_t x;
	uint16_t y;
	uint16_t oldx;
	uint16_t oldy;
	uint8_t sta;
	uint8_t old_sta;
	
	
}tp_device;

extern tp_device tp_dev;





#endif

