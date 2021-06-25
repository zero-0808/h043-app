#ifndef __USART_H__
#define __USART_H__


//#include "lpc_types.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

void usart_init(uint8_t baud_flag);
uint8_t  usart_send_str(uint8_t * str,uint16_t len);




#endif

