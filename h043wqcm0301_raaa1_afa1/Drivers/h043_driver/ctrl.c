#include "ctrl.h"


static t_ctrl * g_pctrl_last = NULL;
static t_ctrl * g_pctrl_head = NULL;
t_ctrl * g_pctrl_rtc = NULL;
static t_ctrl * g_pctrl_focus = NULL;




void T_CtrlGlobleDataInit( void )
{
	g_pctrl_last = NULL;
}



void ctrl_init( t_ctrl * p_ctrl )
{
	t_ctrl * p = p_ctrl;
	p->active = 1;
	p->mutex_sem = xSemaphoreCreateMutex();
	
	if(g_pctrl_last)
	{
		g_pctrl_last->p_next = p;
		p->p_up = g_pctrl_last;
		p->p_next = NULL;
	}
	else
	{
		p->p_up= NULL;
		p->p_next= NULL;
		g_pctrl_head = p;
	}
	g_pctrl_last = p;
}


void ctrl_release( void )
{
	t_ctrl *p = g_pctrl_last;
	SemaphoreHandle_t mutex_sem;
	while(p)
	{
		mutex_sem =  p->mutex_sem;
		g_pctrl_last = p->p_up;
//		vTaskSuspendAll();
		xSemaphoreTake(mutex_sem,portMAX_DELAY);
		(*p->func_ctrl_release)( (void*)p );
		xSemaphoreGive(mutex_sem);
		vSemaphoreDelete(mutex_sem);
		p = g_pctrl_last;
//		xTaskResumeAll();
	}

	if(g_pctrl_last == NULL)
	{
	    g_pctrl_head = NULL;
	}

	g_pctrl_focus = NULL;
}


void deactive_ctrl(void)
{
    t_ctrl * p = g_pctrl_last;

    while(p)
    {
        p->active = 0;
        p = p->p_up;
    }
}

void active_ctrl(void)
{
    t_ctrl * p = g_pctrl_last;

    while(p)
    {
        p->active = 1;
        p = p->p_up;
    }
}



static void clear_focus_ctrl(t_ctrl *pctrl)
{
	
	if(pctrl)
	{
		if(pctrl->func_ctrl_procmsg)
		{	t_msg msg;
			msg.msg = MSG_TP;
			msg.a = 0;   
			msg.b = 0;
			msg.c = 0;

			xSemaphoreTake(pctrl->mutex_sem,portMAX_DELAY);
			(*pctrl->func_ctrl_procmsg)( (void *)pctrl, &msg );
			xSemaphoreGive(pctrl->mutex_sem);
		}
	}
}

uint8_t   ctrl_msg( t_msg * p_msg )
{
	uint8_t f = 1;
	t_ctrl *p = g_pctrl_last;

	if(p_msg->msg == MSG_TP)
	{
		if(p_msg->c == 1)
		{
			if(g_pctrl_focus)
			{
				clear_focus_ctrl(g_pctrl_focus);
				g_pctrl_focus = NULL;
			}
		}
		while(p)
		{
			if(p->func_ctrl_procmsg)
			{
				xSemaphoreTake(p->mutex_sem,portMAX_DELAY);
				f = (*p->func_ctrl_procmsg)( (void *)p, p_msg );
				xSemaphoreGive(p->mutex_sem);
			}
			if(!f)
			{
				break;
			}
			p = p->p_up;
		}

		if(p_msg->c == 1)
		{
			g_pctrl_focus = p;
		}
	}
	return f;
}

t_ctrl * get_ctrl( uint32_t id )
{
	t_ctrl * p = g_pctrl_last;
	while(p)
	{
		if(p->id == id)
		{
			return p;
		}
		p = p->p_up;
	}
	return NULL;
}

//得到第num个控件(从头部开始)

t_ctrl * get_ctrl_num(uint16_t num)
{
	t_ctrl * p = g_pctrl_head;
	
	while(num && p)
	{
		p = p->p_next;
		num--;
	}

	return p;
}


uint8_t release_ctrl( t_ctrl * p_ctrl )
{
	t_ctrl * p = p_ctrl;
	if( !p )
	{
		return 0;
	}

	if(p == g_pctrl_focus)
	{
		g_pctrl_focus = NULL;
	}
	
	SemaphoreHandle_t mutex_sem;

	if( p->p_up)
	{
		p->p_up->p_next = p->p_next;
		if(p->p_next)
		{
			p->p_next->p_up= p->p_up;
		}
	}
	else if(p->p_next)
	{
		p->p_next->p_up = NULL;
	}
	if( g_pctrl_last == p )
	{
		g_pctrl_last = p->p_up;

		if(g_pctrl_last == NULL)
		{
		    g_pctrl_head = NULL;
		}
	}
	mutex_sem =  p->mutex_sem;
	xSemaphoreTake(mutex_sem,portMAX_DELAY);
	(*p->func_ctrl_release)( (void*)p );
	xSemaphoreGive(mutex_sem);
	vSemaphoreDelete(mutex_sem);
	return 1;
}


void ctrl_release_screen( short screen_id )
{
    short screen;
	t_ctrl *p = g_pctrl_last;
	t_ctrl *temp;
	
	
	while(p)
	{
		screen = p->id >> 16;
		temp = p->p_up;
		
		if(screen == screen_id)
		{   
		    release_ctrl(p);
		   
		}

		 p = temp;
	}
}












