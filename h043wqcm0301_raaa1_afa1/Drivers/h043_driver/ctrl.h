#ifndef __CTRL_H__
#define __CTRL_H__
//#include "lpc_types.h"
#include "message.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"



#define SETBIT(x,n) x = ( (x) |  (1<<(n)) )
#define CLRBIT(x,n) x = ( (x) & ~(1<<(n)) )
#define GETBIT(x,n) ( (x) & (1<<(n)) )












typedef struct ctrl 
{
    uint32_t id;
	uint8_t   (*func_ctrl_procmsg)( void* pCtrl, t_msg * pMsg );
	void (*func_ctrl_release)( void* pCtrl );

	struct ctrl *p_up;
	struct ctrl *p_next;
	uint8_t active;
	
	SemaphoreHandle_t mutex_sem;

	uint32_t user_data;
}t_ctrl;






void ctrl_globle_data_init( void );
void ctrl_init( t_ctrl * p_ctrl );
void ctrl_release( void );
uint8_t ctrl_msg( t_msg * p_msg );

t_ctrl* get_ctrl( uint32_t id );
t_ctrl * get_ctrl_num(uint16_t num);


uint8_t release_ctrl( t_ctrl * p_ctrl );
void ctrl_release_screen( short screen_id );
void active_ctrl(void);
void deactive_ctrl(void);




#endif




