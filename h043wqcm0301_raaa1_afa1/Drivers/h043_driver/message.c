#include "message.h"

static void message_queue_init(void);
static uint32_t  write_message_queue(const void * item,uint32_t time);
static uint32_t read_message_queue(void * buffer, uint32_t time);




msg_device msg_dev =
{
	message_queue_init,
	write_message_queue,
	read_message_queue,	
};



static void message_queue_init(void)
{
	msg_dev.message_queue = xQueueCreate(MESSAGE_QUEUE_LEN,sizeof(t_msg));
}


static uint32_t  write_message_queue(const void * item,uint32_t time)
{
	return xQueueSend(msg_dev.message_queue, item, time);
}


static uint32_t read_message_queue(void * buffer, uint32_t time)
{
	return xQueueReceive(msg_dev.message_queue, buffer, time);
}








