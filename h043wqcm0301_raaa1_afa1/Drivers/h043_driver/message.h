#ifndef __MESSAGE_H__
#define __MESSAGE_H__

//#include"lpc_types.h"
#include "FreeRTOS.h"
#include "queue.h"


#define MESSAGE_QUEUE_LEN   (64)


#define MSG_TP         1
#define MSG_TOUCH      2





typedef struct msg
{
	uint16_t msg;
	uint16_t a;
	uint16_t b;
	uint16_t c;
	uint8_t *p;
}t_msg;


typedef struct msg_device
{
	void (*message_init) (void);
	uint32_t (*write_message)(const void *, uint32_t);
	uint32_t (*read_message)(void * buffer,uint32_t);
	QueueHandle_t message_queue;
		
}msg_device;


extern msg_device msg_dev;


#endif


