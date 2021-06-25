#ifndef  __INSTRUCTIONS_H__
#define  __INSTRUCTIONS_H__


//#include "lpc_types.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"





typedef struct cmd_device
{
	void (*init)(void);
	void (*get_cmd)(uint32_t,uint16_t,uint8_t);
	void (*notify_cmd)(uint8_t *,uint16_t);
	void (*usart_cmd)(uint8_t *,uint16_t);
	uint32_t (*get_seamphore)(uint32_t,uint8_t);
	uint32_t (*give_seamphore)(uint8_t);
	
	SemaphoreHandle_t  cmd_sem;
	SemaphoreHandle_t  send_sem;
	
}cmd_device;


extern cmd_device cmd_dev;
uint32_t get_next_cmd_addr(uint32_t addr);
uint16_t get_cmd_num(uint32_t addr,uint16_t len);



//uint8_t cmd_prase(t_msg msg);
//uint16_t get_cmd_num(uint8_t flag);








#endif
