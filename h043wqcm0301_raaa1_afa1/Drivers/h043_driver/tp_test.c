#include "tp_test.h"
#include "mem_my.h"
#include "string.h"
#include "display.h"


static uint8_t tp_touch_msg(void *p, t_msg * p_msg)
{
	
	uint16_t x,y,flag,f=1;
	t_msg msg;
	
	
	if(p_msg->msg != MSG_TP)
	{
		return f;
	}
	
	tp_touch  * ptp = (tp_touch *) p;
	
	if(ptp->ctrl.active == 0)
	{
		return f;
	}

	x = p_msg->a;
	y = p_msg->b;
	flag = p_msg->c;
	
	
	if((flag != 1) && !GETBIT( ptp->state, BTN_STATE_ACTIVE))
	{
		return f;
	}
	
	if(pt_in_rect(&ptp->tp_rect, x, y))
	{
		f = 0;
	}
	
	switch(flag)
	{	
		case 1:
			if(!f)
			{
				SETBIT( ptp->state, BTN_STATE_ACTIVE);
				msg.msg = MSG_TOUCH;
				msg.a   = (ptp->ctrl.id >> 16) & (0xffff);
				msg.b   = (ptp->ctrl.id) & (0xffff);
				msg.c   = 1;//????
				ptp->x = x;
				ptp->y = y;
				msg_dev.write_message((void *)&msg,0);
					
			}

			break;
		case 2:
			msg.msg = MSG_TOUCH;
			msg.a   = (ptp->ctrl.id >> 16) & (0xffff);
			msg.b   = (ptp->ctrl.id) & (0xffff);
			msg.c   = 2;//????
			ptp->x = x;
			ptp->y = y;
			msg_dev.write_message((void *)&msg,0);
			break;
		case 0:
			msg.msg = MSG_TOUCH;
			msg.a   = (ptp->ctrl.id >> 16) & (0xffff);
			msg.b   = (ptp->ctrl.id) & (0xffff);
			msg.c   = 0;//????
			ptp->x = 0xFFFF;
			ptp->y = 0xFFFF;
			msg_dev.write_message((void *)&msg,0);
			CLRBIT( ptp->state, BTN_STATE_ACTIVE);
			break;
	}

	return f;
	
   
}


static void tp_touch_release( void* pCtrl )
{


	my_free(pCtrl);
   
}


static void tp_touch_act(base_touch *p,uint8_t flag)
{

   p->screen_update = 1;
	
}


static void tp_draw_screen(base_touch * p)
{
	tp_touch * ptp = (tp_touch *)p;
	
	if(ptp->tp_type == 0xFF)
	{
		if(ptp->x < disp_dev.width && ptp->y < disp_dev.height)
		{
			draw_Vline(ptp->x,ptp->tp_rect.top,ptp->tp_rect.bottom - ptp->tp_rect.top + 1, 0);
			draw_Hline(ptp->tp_rect.left,ptp->y,ptp->tp_rect.right - ptp->tp_rect.left + 1, 0);
		}
	}
	
}




uint8_t tp_touch_create(void)
{
	tp_touch * ptp = (tp_touch *) my_malloc(sizeof(tp_touch));
   
	
	if(ptp == NULL)
	{
		return 1;
	}
	memset((void *)ptp,0,sizeof(tp_touch));

	ptp->ctrl.id = 0xFFFFFFFF;
	ptp->ctrl.func_ctrl_procmsg = tp_touch_msg;
	ptp->ctrl.func_ctrl_release = tp_touch_release;
	
	ptp->tp_type = 0xff;
	ptp->tp_rect.left = 0;
	ptp->tp_rect.right = disp_dev.width - 1;
	ptp->tp_rect.top = 0;
	ptp->tp_rect.bottom = disp_dev.height - 1;
	
	ptp->act = tp_touch_act;
	ptp->draw_screen = tp_draw_screen;
	ctrl_init((t_ctrl *) ptp);
	
	return 0;
}



void tp_test(void)
{
	base_touch * p;
	ctrl_release();
	tp_touch_create();
	extern void parse_cfg(void);
	parse_cfg();
	p = (base_touch *)get_ctrl_num(0);
	
	p->screen_update = 1;
}
