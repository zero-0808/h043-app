
/***************************************************************************************
****************************************************************************************
* Project      : 20180525_HMI
* FILE         : btn.c
* Description  : 
*                      
* Copyright (c) 2018 by ShenZhen JingHua Displays Co.,Ltd... All Rights Reserved.
* 
* History:
* Version		name			Date			Description
* 0.01			Qu Bingbing		2018/05/26		Initial Version
* 
****************************************************************************************
****************************************************************************************/
#include "btn.h"
#include "display.h"
#include "cfg.h"
#include "cmd.h"
#include "textbox.h"
#include "keyboard.h"
#include "orientation.h"

extern textbox * active_box;
extern uint8_t shift_value;
extern uint8_t engch_value;



static uint8_t btn_msg(void *p, t_msg * p_msg)
{
	uint16_t x,y,flag,f=1;
	t_msg msg;
	
	
	if(p_msg->msg != MSG_TP)
	{
		return f;
	}
	
	t_btn * p_btn = (t_btn * )p;
	
	if(p_btn->ctrl.active == 0)
	{
		return f;
	}

	x = p_msg->a;
	y = p_msg->b;
	flag = p_msg->c;
	
	

	if((flag != 1) && !GETBIT( p_btn->state, BTN_STATE_ACTIVE))
	{
		return f;
	}
	
	

	if(pt_in_rect(&p_btn->tp_rect, x, y))
	{
		f = 0;
	}

	switch(flag)
	{
		case 1:
		{
			if(!f)
			{
				SETBIT( p_btn->state, BTN_STATE_ACTIVE);
				msg.msg = MSG_TOUCH;
				msg.a   = (p_btn->ctrl.id >> 16) & (0xffff);
				msg.b   = (p_btn->ctrl.id) & (0xffff);
				msg.c   = 1;//����
				msg_dev.write_message((void *)&msg,0);
					
			}

			break;
		}
		case 2:
		{
			msg.msg = MSG_TOUCH;
			msg.a   = (p_btn->ctrl.id >> 16) & (0xffff);
			msg.b   = (p_btn->ctrl.id) & (0xffff);
			msg.c   = 2;//����
			msg_dev.write_message((void *)&msg,0);
//			if(!f)
//			{
//				
//			}
//			else
//			{
//				
//			}
			break;
		}
		case 0:
		{
			
			//if(f || (p_btn->pic_next == -1))
			msg.msg = MSG_TOUCH;
			msg.a   = (p_btn->ctrl.id >> 16) & (0xffff);
			msg.b   = (p_btn->ctrl.id) & (0xffff);
			msg.c   = 0;//̧��
			msg_dev.write_message((void *)&msg,0);
				
		
			//if(!f)//��̧��ʱ���ڴ��ط�Χ��
			//{
			/*
				switch(p_btn->use)
				{

					case switch_pic:
						msg.msg = MSG_PIC;
						msg.a =* ((uint16_t *)p_btn->tp_use);
						msg_dev.write_message((void *)&msg,0);
						break;
					case custom_key:
						break;
					default:
						break;
					
				}

				*/
				
			//}
			
			CLRBIT( p_btn->state, BTN_STATE_ACTIVE);
			
			break;
		}
	}

	return f;
	
}




static BaseType_t btn_switch_timer_delete(btn_switch * p)
{
    if(p->timer1)
    {
	    xTimerDelete(p->timer1,3);
	 }
	if(p->timer2)
	{
	    xTimerDelete(p->timer2,3);
	}

	return 0;
}






static void btn_release( void *p_ctrl )
{
	t_btn *p_btn = (t_btn *)p_ctrl;
	
	
	if( p_btn )
	{
		if(p_btn->pic_on >= 0)
		{
			if(p_btn->depress.p)
			{
				my_free(p_btn->depress.p);
			}
			
		}
		if(p_btn->tp_use)
		{
			if(p_btn->use == switch_btn)
			{
				btn_switch * p = (btn_switch *) p_btn->tp_use;
				
				if(p->timer1)
				{
					btn_switch_timer_delete(p);
				}
			}
			my_free(p_btn->tp_use);
		}
		if(p_btn->pdraw)
		{
		    my_free(p_btn->pdraw);
		}
	
		my_free( p_btn );
	}
	
	
}






static void btn_switch_screen(btn_screen * p,uint8_t flag)
{
	uint16_t id;
	
	p->status = flag;
	
	if(flag == 0)
	{
#if   (DIR_SWITCH_SUPPORT)
		id = get_screen_id(p->screen);
		
		if(src_cfg.ptscreen->HV_switch == 0)
		{
			src_cfg.new_screen = id;
		}
		else
		{
			adjust_screen_dir(id);
		}
#else
        src_cfg.new_screen = get_screen_id(p->screen);
        
#endif

	}

}

static BaseType_t btn_switch_timer_reset(TimerHandle_t timer)
{
	return xTimerReset(timer,5);
}




static BaseType_t btn_switch_timer_stop(TimerHandle_t timer)
{
	return xTimerStop(timer,5 );
	
} 







static BaseType_t btn_switch_timer_start(TimerHandle_t timer)
{
	return xTimerStart(timer,5 );
	
}


static void external_cmd_call_back(TimerHandle_t timer)
{
	
	btn_cmd_info * cmd = (btn_cmd_info *) pvTimerGetTimerID(timer);
	cmd_dev.get_cmd(cmd->addr,cmd->len,1);

	my_free(cmd);

	xTimerDelete(timer,0);
	
	
}








static void internal_cmd_call_back(TimerHandle_t timer)
{
	

	btn_cmd_info * cmd = (btn_cmd_info *) pvTimerGetTimerID(timer);

    cmd_dev.get_cmd(cmd->addr,cmd->len,0);

	my_free(cmd);

	xTimerDelete(timer,0);
	
	
}


static void btn_pop_menu(btn_list * p,uint8_t flag)
{
	
	p->status = flag;

	uint16_t screen_id;
	
	
	if(flag == 0)
	{
	    uint8_t * buf;

	    buf = my_malloc(14);
		
		//screen_id = get_screen_id(src_cfg.pic_now);
		
		screen_id = src_cfg.current_screen;

	    buf[0] = 0xEE;
        buf[1] = 0xB1;
        buf[2] = 0x13;
	    buf[3] = screen_id >> 8;
	    buf[4] = screen_id & 0xFF;
	    buf[5] = p->menu >> 8;
	    buf[6] = (p->menu) & 0xFF;
	    buf[7] = 0x01;
	    buf[8] = p->text >> 8;
	    buf[9] = (p->text) & 0xFF;
	    buf[10] = 0xFF;
        buf[11] = 0xFC;
        buf[12] = 0xFF;
        buf[13] = 0xFF;

        cmd_dev.usart_cmd(buf,14);

        my_free(buf);
	    
	}

}




static void btn_cmd_handle(uint32_t addr,uint16_t len,uint16_t time,uint8_t flag)
{
    uint16_t num,i,t;
    uint32_t ad = addr;
    void ** p = NULL;
    btn_cmd_info ** pinfo = NULL;

    num = get_cmd_num(ad, len);

    p = (void **)my_malloc((num)*4);
    pinfo = (btn_cmd_info **) my_malloc((num)*4);

    for(i=0;i<num;i++)
    {
        pinfo[i] = my_malloc(sizeof(btn_cmd_info));
		pinfo[i]->addr = ad;
        ad = get_next_cmd_addr(ad);
        pinfo[i]->len = ad - pinfo[i]->addr;
        
        
        if(i == 0)
        {
            cmd_dev.get_cmd(pinfo[i]->addr,pinfo[i]->len,flag);
            my_free((void *)pinfo[i]);  
        }
        else
        {
           t = time * i;
        
           if(flag == 0)
           {
                p[i]=xTimerCreate((const char *) "cmd",
								(TickType_t)t,
								(UBaseType_t)pdFALSE,
								(void*	)pinfo[i],
								(TimerCallbackFunction_t)internal_cmd_call_back);
			}
			else
			{
		        p[i]=xTimerCreate((const char *) "cmd",
								(TickType_t)t,
								(UBaseType_t)pdFALSE,
								(void*	)pinfo[i],
								(TimerCallbackFunction_t)external_cmd_call_back);
		    }
	       xTimerStart(p[i],5);
	    }
    }

    
    my_free((void*)p);
    my_free((void*)pinfo);
    
    
}





static void btn_switch_btn(btn_switch * use, uint8_t flag)
{
	uint32_t addr1, addr2;
	uint16_t len1=0;
	uint16_t len2=0;
	
	if(flag == 0)
	{
	
		if(use->style == transient || use->style == sustain)
		{
		
			use->status = !(use->status);
			*(use->pstatus) = use->status;
			
			if(use->status)
			{
				addr1 = use->down_in_addr;
				len1 = use->down_in_len;
				addr2 = use->down_out_addr;
				len2 = use->down_out_len;
			}
			else
			{
				addr1 = use->up_in_addr;
				len1 = use->up_in_len;
				addr2 = use->up_out_addr;
				len2 = use->up_out_len;
			}
		}
		if(use->style == sustain)
		{
		    if(use->timer1)
		    {
			    btn_switch_timer_reset(use->timer1);
			    btn_switch_timer_stop(use->timer1);
			}
			if(use->timer2)
			{
			    btn_switch_timer_reset(use->timer2);
			    btn_switch_timer_stop(use->timer2);
			}
		}
	}
	else if(flag == 1)
	{
		if(use->style == transient || use->style == sustain || use->style == on_off)
		{
			use->status = !(use->status);
			*(use->pstatus) = use->status;

			if(use->status)
			{

				addr1 = use->down_in_addr;
				len1 = use->down_in_len;
				addr2 = use->down_out_addr;
				len2 = use->down_out_len;
				
			}
			else
			{
				addr1 = use->up_in_addr;
				len1 = use->up_in_len;
				addr2 = use->up_out_addr;
				len2 = use->up_out_len;
			}

			if(use->style == sustain)
			{
			    if(use->timer1)
			    {
				    btn_switch_timer_start(use->timer1);
				}
				if(use->timer2)
				{
				    btn_switch_timer_start(use->timer2);
				}
			}
			
		}
		else if(use->style == turn_off)
		{
			if(use->status)
			{
				use->status = 0;
				*(use->pstatus) = use->status;
				
				addr1 = use->up_in_addr;
				len1 = use->up_in_len;
				addr2 = use->up_out_addr;
				len2 = use->up_out_len;
			}
		}
		else if(use->style==turn_on)
		{
			if(use->status == 0)
			{
				use->status = 1;
				*(use->pstatus) = use->status;
				addr1 = use->down_in_addr;
				len1 = use->down_in_len;
				addr2 = use->down_out_addr;
				len2 = use->down_out_len;
			}
		}
		
	}
	else if(flag == 2)
	{
		
	}
	
	
	if(len1)
	{
		btn_cmd_handle(addr1,len1,use->times,0);
	}
	if(len2)
	{
		btn_cmd_handle(addr2,len2,use->times,1);
	}
	

	
	
	
}





static void btn_custom_cmd(btn_cmd * p,uint8_t flag)
{
	uint32_t addr;
	uint16_t len = 0;

	p->status = flag;
	
	if(flag)
	{
		len = p->down_len;
		addr = p->down_addr;
	}
	else
	{
		len = p->up_len;
		addr = p->up_addr;
	}
	if(len)
	{
		btn_cmd_handle(addr,len,p->times,1);
	}
}




static void btn_notify(t_btn *pbtn)
{

	uint8_t * p;
	

	p = (uint8_t *)my_malloc(15);

	p[0] = 0xee;
	p[1] = 0xb1;
	p[2] = 0x11;
	p[3] = pbtn->ctrl.id >> 24;
	p[4] = (pbtn->ctrl.id >> 16)&(0XFF);
	p[5] = (pbtn->ctrl.id >> 8)&(0XFF);
	p[6] = (pbtn->ctrl.id)&(0XFF);
	p[7] = pbtn->tp_type;
	p[8] = pbtn->use;
	if(pbtn->use == custom_key )
	{
		uint8_t * value;
		value = (uint8_t *) pbtn->tp_use;
		p[9] = *value;
	}
	else if(pbtn->use == switch_btn)
	{
		btn_switch * sw;
		sw = (btn_switch *)pbtn->tp_use;
		p[9] = sw->status;
	}
	else
	{
		p[9] = 0;
	}
	p[10] = 0xff;
	p[11] = 0xfc;
	p[12] = 0xff;
	p[13] = 0xff;
	p[14] = ';';

	cmd_dev.notify_cmd(p,15);
	
	my_free(p);

	
}



static void draw_virtual_frame(t_btn * pbtn)
{
	uint16_t xs,ys,xe,ye;
	
	xs = pbtn->tp_rect.left - 1;
	ys = pbtn->tp_rect.top - 1;
	xe = pbtn->tp_rect.right + 1;
	ye = pbtn->tp_rect.bottom + 1;

	draw_virtual_hline(xs, ys, xe);
	draw_virtual_vline(xe, ys, ye);
	draw_virtual_hline(xe, ye, xs);
	draw_virtual_vline(xs, ye, ys);

}




/*
static void recover_frame(t_btn * pbtn)
{
	uint16_t xs,ys,xe,ye;
	uint16_t * p = (uint16_t *)pbtn->fame;
	uint16_t index;
	
	xs = pbtn->tp_rect.left - 1;
	ys = pbtn->tp_rect.top - 1;
	xe = pbtn->tp_rect.right + 1;
	ye = pbtn->tp_rect.bottom + 1;

	index = 0;

	index = 0;
	recover_hline(xs,ys,xe,&p[index]);
	
	index += (xe - xs);
	recover_vline(xe, ys, ye,&p[index]);

	index += (ye - ys);
	recover_hline(xe,ye,xs,&p[index]);

	index += (xe - xs);
	recover_vline(xs, ye, ys, &p[index]);
}
*/




/*
static void save_virtual_frame(t_btn * pbtn)
{
	uint16_t xs,ys,xe,ye;
	uint16_t * p = (uint16_t *)pbtn->fame;
	uint16_t index;
	
	xs = pbtn->tp_rect.left - 1;
	ys = pbtn->tp_rect.top - 1;
	xe = pbtn->tp_rect.right + 1;
	ye = pbtn->tp_rect.bottom + 1;

	index = 0;
	save_virtual_hline(xs,ys,xe,&p[index]);
	
	index += (xe - xs);
	save_virtual_vline(xe, ys, ye,&p[index]);

	index += (ye - ys);
	save_virtual_hline(xe,ye,xs,&p[index]);

	index += (xe - xs);
	save_virtual_vline(xs, ye, ys, &p[index]);

	
}
*/



static void btn_sustain_call_back2(TimerHandle_t timer)
{
	uint16_t len1;

	uint32_t addr1;
	
	btn_switch * p = pvTimerGetTimerID(timer);

	if(p->status)
	{
		len1 = p->down_out_len;
		addr1 = p->down_out_addr;
		
	}
	else
	{
		len1 = p->up_out_len;
		addr1 = p->up_out_addr;
		
	}
	
	if(len1)
	{
		btn_cmd_handle(addr1,len1,p->times,1);
	}

}



static void btn_sustain_call_back1(TimerHandle_t timer)
{
	uint16_t len1;

	uint32_t addr1;
	
	btn_switch * p = pvTimerGetTimerID(timer);

	if(p->status)
	{
		len1 = p->down_in_len;
		addr1 = p->down_in_addr;
		
	}
	else
	{
		len1 = p->up_in_len;
		addr1 = p->up_in_addr;
		
	}
	
	if(len1)
	{
		btn_cmd_handle(addr1,len1,p->times,0);
	}

}







/*==================================================================
* Function     : btn_switch_timer_create
* Description  : 创建按钮开关软件定时器
* Input Para   : p 按钮开关指针 flag 0:对内指令定时器  flag 1:对外指令定时器
* Output Para  : 
* Return Value : 定时器指针
==================================================================*/
static TimerHandle_t btn_switch_timer_create(btn_switch * p,uint8_t flag)
{
	uint16_t len1,num,time;
	uint32_t addr1;
	

	if(p->status)
	{
	    if(flag)
	    {
	        len1 = p->up_out_len;
		    addr1 = p->up_out_addr;
	    }
	    else
	    {
		    len1 = p->up_in_len;
		    addr1 = p->up_in_addr;
		 }
		
	}
	else
	{
        if(flag)
        {
            len1 = p->down_out_len;
		    addr1 = p->down_out_addr;
		    
		 }
		 else
		 {
		    len1 = p->down_in_len;
		    addr1 = p->down_in_addr;
		 }
		 
		
	}

	num = get_cmd_num(addr1,len1);
	

	if(num == 0)
	{
	    return NULL;
	}

	time = p->times*num;

	if(flag)
	{
	
	    return   xTimerCreate((const char *) "btn_swich",
								(TickType_t)time,
								(UBaseType_t)pdTRUE,
								(void*	)p,
								(TimerCallbackFunction_t)btn_sustain_call_back2);
    }
    else
    {
        return   xTimerCreate((const char *) "btn_swich",
								(TickType_t)time,
								(UBaseType_t)pdTRUE,
								(void*	)p,
								(TimerCallbackFunction_t)btn_sustain_call_back1);
    }

    
}



	

static btn_switch * btn_switch_create(uint8_t * buffer)
{
	btn_switch * p;

	p = (btn_switch *) my_malloc(sizeof(btn_switch));
	if(p == NULL)
	{
		return p;
	}
	memset((uint8_t *)p,0,sizeof(btn_switch));
	p->status = buffer[0];
	p->style = (btn_switch_style)buffer[1];
	p->old_status = p->status;
	

	
	p->down_in_addr = (buffer[10]<<24)|(buffer[11]<<16)|(buffer[12]<<8)|(buffer[13]);
	p->down_in_len = (buffer[8]<<8)|(buffer[9]);
	
	p->up_in_addr = (buffer[4]<<24)|(buffer[5]<<16)|(buffer[6] << 8)|(buffer[7]);
	p->up_in_len  = (buffer[2]<<8)|(buffer[3]);

	

	p->up_out_addr = (buffer[16]<<24)|(buffer[17]<<16)|(buffer[18]<<8)|(buffer[19]);
	p->up_out_len = (buffer[14]<<8)|(buffer[15]);

	p->down_out_addr = (buffer[22]<<24)|(buffer[23]<<16)|(buffer[24]<<8)|(buffer[25]);
	p->down_out_len  = (buffer[20]<<8)|(buffer[21]);
	p->times = (buffer[26] << 8)|(buffer[27]);
	
	p->pstatus = &buffer[0];
	
	if(p->style == sustain)
	{
		p->timer1 = btn_switch_timer_create(p,0);
		p->timer2 = btn_switch_timer_create(p,1);
	}
		
	return p;
}






static void btn_custom_key(btn_key * key,uint8_t flag)
{
    key->status = flag;
		
    if((active_box == NULL)|| (active_box->input_way != 2) ||(flag == 0))
    {		
        return ;			
    }

    if(key->type == key_char)
    {
        if(active_box->code_num   < active_box->maxmum_code)
        {
            uint8_t *p;
            uint8_t num;

            p = active_box->code;
            num = active_box->code_num;
            p[num] = key->value;
            
			if(shift_value)
			{
				if((p[num] >= 'a') && (p[num] <= 'z') )
				{
					p[num] -= 32;
				}
			}
			
            active_box->code_num += 1;
            active_box->code_update = 1;
            active_box->screen_update = 1;
        }
    }
    else if(key->type == key_enter)
    {
        
		if((((active_box->ctrl.id) >> 16) == small_keyboard.screen_id) || (((active_box->ctrl.id) >> 16) == full_keyboard.screen_id))
		{
			kb_info.status = 0;
			kb_info.flag = 1;
			xSemaphoreGive(kb_info.sem);
			return;
		}
		else
		{
			active_box->status = 0;
            active_box->screen_update = 1;
            active_box = NULL;
		}
		
    }
    else if(key->type == key_clear)
    {
        if(active_box->tp_type > 0)
        {
            active_box->code_num = 0;
            active_box->code_update = 1;
            active_box->screen_update = 1;
         }
    }
    else if(key->type == key_backspace)
    {
        if(active_box->code_num > 0)
        {
            active_box->code_num -= 1;
            
			if(active_box->code_num > 0)
			{
				if(active_box->code[active_box->code_num - 1] >= 0x80)
				{
					active_box->code_num -= 1;
				}
			}
			
			active_box->code_update = 1;
            active_box->screen_update = 1;
         }
    }
    else if(key->type == key_space)
    {
        if(active_box->code_num   < active_box->maxmum_code)
        {
            uint8_t *p = active_box->code;
            uint8_t num = active_box->code_num;
            p[num] = ' ';
            active_box->code_num += 1;
            active_box->code_update = 1;
            active_box->screen_update = 1;
         }
    }
	else if(key->type == key_pm)
	{
		uint8_t * p0,*p1;
		uint8_t num;
		p0 = active_box->code;
		num = active_box->code_num;
		if(num)
		{
			p1 = (uint8_t *)my_malloc(num);
			memcpy((void *)p1,(const void *)p0,num);
			if(p0[0] == '-')
			{
				memcpy(p0,&p1[1],num -1);
				active_box->code_num = num - 1;
			}
			else
			{
				p0[0] = '-';
				
				if(num < active_box->maxmum_code)
				{
					memcpy(&p0[1],p1,num);
					active_box->code_num = num + 1;
				}
				else
				{
					memcpy(&p0[1],p1,num-1);
				}
			}
		}
		else
		{
			p0[0] = '-'; 
			active_box->code_num = 1;
		}
		
		active_box->code_update = 1;
		active_box->screen_update = 1;
	}
	else if(key->type == key_esc)
	{
		
		
		if((((active_box->ctrl.id) >> 16) == small_keyboard.screen_id) || (((active_box->ctrl.id) >> 16) == full_keyboard.screen_id))
		{
			kb_info.status = 0;
			kb_info.flag = 0;
			xSemaphoreGive(kb_info.sem);
			return;
		}
		else
		{
		    active_box->status = 0;
		    active_box = NULL;
		}
		
		
	}	
	else 
	{
		//PRINT("key_type = %d \r\n",key->type);
	}		
	

    if(key->type != key_enter && key->type != key_esc)
	{
		*(active_box->pcode_addr) &= 0X00FFFFFF;
		*(active_box->pcode_addr) |= (active_box->code_num << 24);
		if((((active_box->ctrl.id) >> 16) != small_keyboard.screen_id) && (((active_box->ctrl.id) >> 16) != full_keyboard.screen_id))
		{
			active_box->cmd_get_status(active_box);
		}
	}
    
    
}





static volatile TimerHandle_t timer = NULL;
static volatile uint8_t key_flag = 0;



static void key_del_timer1(TimerHandle_t timer)
{
	if(key_flag < 4)
	{
		key_flag++;
	}
	else
	{
		btn_key * p = (btn_key *) pvTimerGetTimerID(timer);
		btn_custom_key(p,1);
	}
}


static void btn_act(base_touch * p_btn, uint8_t flag)
{
	
	t_btn * pbtn  = (t_btn *)p_btn;
	btn_key * key;
	
	
	
		
	if(flag == 1)  
	{
		
		pbtn->screen_update = 1;

		
		
		switch(pbtn->use)
		{
			case switch_screen:
			     btn_switch_screen((btn_screen*)pbtn->tp_use,1);
				break;
			case custom_key:
			    btn_custom_key((btn_key *)pbtn->tp_use,1);
			    
			    key = (btn_key *) pbtn->tp_use;
			
			    if( (key->type == key_char) || (key->type == key_backspace) )
			    {
					timer =  xTimerCreate((const char *) "key_backspace",
							(TickType_t)150,
							(UBaseType_t)pdTRUE,
							(void*	)key,
							(TimerCallbackFunction_t)key_del_timer1);
							xTimerStart(timer,10);
			    }
			    
				break;
			case switch_btn:
				btn_switch_btn((btn_switch *)pbtn->tp_use,1);
				break;
			case custom_cmd:
				btn_custom_cmd((btn_cmd *)pbtn->tp_use,1);
				break;
			case popup_list:
			    btn_pop_menu((btn_list *)pbtn->tp_use , 1);
				break;
		}
		
		
		
	}
	else if(flag==2) 
	{
	}
	else 
	{
		pbtn->screen_update = 1;
		
		

		switch(pbtn->use)
		{
			case switch_screen:
				btn_switch_screen((btn_screen*)pbtn->tp_use,0);
				break;
			case custom_key:
			    
			    btn_custom_key((btn_key *)pbtn->tp_use,0);
			   
				if(timer)
				{
					key_flag = 0;
					xTimerDelete(timer,3);
					timer = NULL;
				}
                
			    
				break;
			case switch_btn:
				btn_switch_btn((btn_switch*)pbtn->tp_use,0);
				break;
			case custom_cmd:
				btn_custom_cmd((btn_cmd *)pbtn->tp_use,0);
				break;
			case popup_list:
			    btn_pop_menu((btn_list *)pbtn->tp_use , 0);
				break;
					
		}
        
		
//		if(pbtn->virtual_fame)
//		{
//			recover_frame(pbtn);
//		}
		
			
			
		if(pbtn->notify)
		{	
			btn_notify(pbtn);
				
		}
	}
	
	
	
	
	
}


void btn_key_delete(void)
{
	if(timer)
	{
		key_flag = 0;
		xTimerDelete(timer,3);
		timer = NULL;
	}
}


static void cmd_update_btn_status (t_btn * pbtn,uint8_t * buf,uint16_t len)
{
    if((pbtn->tp_type == CFG_BTN)&&(len == 1))
    {
        

         if(pbtn->use != switch_btn)
         {
             return;
         }

         btn_switch * btn_use = (btn_switch *)pbtn->tp_use;

         btn_use->status = buf[0]?1:0;

         *(btn_use->pstatus) = btn_use->status;
		 
		 pbtn->screen_update = 1;
    }
    
}



 
static void cmd_get_btn_status (t_btn * pbtn)
{
    if(pbtn->tp_type == CFG_BTN)
    {
        
        uint8_t * buf;

        if(pbtn->use != switch_btn)
        {
            return;
        }
        buf = (uint8_t *)my_malloc(15);
        btn_switch * btn_use = (btn_switch *) pbtn->tp_use;
        buf[0] = 0xee;
        buf[1] = 0xb1;
        buf[2] = 0x11;
        buf[3] = (pbtn->ctrl.id>>24)&(0xff);
        buf[4] = (pbtn->ctrl.id>>16)&(0xff);
        buf[5] = (pbtn->ctrl.id>>8)&(0xff);
        buf[6] = (pbtn->ctrl.id)&(0xff);
        buf[7] = pbtn->tp_type;
        buf[8] = pbtn->use;
        buf[9] = btn_use->status;
        buf[10] = 0xff;
        buf[11] = 0xfc;
        buf[12] = 0xff;
        buf[13] = 0xff;
        buf[14] = 0x3b;
        cmd_dev.notify_cmd(buf,15);
        my_free((void *) buf);
       
    }
    
}



    
    
    
    
    
    
    
static void update_draw_text(draw_text * pdraw,uint16_t str_id, font_type * ptfont)
{
	if(pdraw == NULL || ptfont == NULL)
	{
		return;
	}
	
	pdraw->line = 0;

   
	if(pdraw->line_num == 0 || pdraw->col_num == 0)
	{
		return;
	}

	uint16_t w;
	uint8_t line,col,i,j,k,len;
	uint8_t * code = get_str_code(str_id,&len);

	if(code == NULL || len == 0)
	{
		return;
	}
	
	

	if(ptfont->code == 0)
	{
		w = ptfont->width;
	}
	else
	{
		w = ptfont->width / 2;
	}
    
	line = pdraw->line_num;
	col = pdraw->col_num;

	pdraw->line = 1;
	
	if(ptfont->code < 3)
	{
		for(j=0,i=0,k=0;(j<len) && (i<line);j++)
		{
		   
			if(code[j] < 0x80)
			{
				if(code[j] != '\r')
				{
				    pdraw->pline[i].pcode[k] = code[j];
					k++;
					if(k >= col || (j + 1) == len)
					{
						pdraw->pline[i].len = k;
						pdraw->pline[i].width = k * w;
						k = 0;
						i++;
					}
                            
				}
				else if((j + 1) < len)
				{
					if(code[j+1] == '\n')
					{
						j++;
						pdraw->pline[i].len = k;
						pdraw->pline[i].width = k * w;
						k = 0;
						i++;
					}
					else
					{
					    pdraw->pline[i].pcode[k] = code[j];
						k++;
						if(k >= col)
						{
							pdraw->pline[i].len = k;
							pdraw->pline[i].width = k * w;
							k = 0;
							i++;
						}
					}
				}
				else
				{
					pdraw->pline[i].pcode[k] = code[j];
					k++;
					if(k >= col || (j + 1) == len)
					{
						pdraw->pline[i].len = k;
						pdraw->pline[i].width = k * w;
						k = 0;
						i++;
					}
				}
			}
			else
			{
				if((j + 1) < len)
				{
					if(k+1 < col)
					{
						pdraw->pline[i].pcode[k] = code[j];
						k++;
						j++;
						pdraw->pline[i].pcode[k] = code[j];
						k++;
						if(k >= col || (j + 1) >= len)
						{
							pdraw->pline[i].len = k;
							pdraw->pline[i].width = k * w;
							k = 0;
							i++;
						}
					}
					else
					{
						j--;
					    pdraw->pline[i].len = k;
					    pdraw->pline[i].width = k * w;
						k = 0;
						i++;
					}
				}
				else
				{
					pdraw->pline[i].pcode[k] = 0;
					k++;
					pdraw->pline[i].len = k;
					pdraw->pline[i].width = k * w;
					k = 0;
					i++;
				}
			}
		   
		}
	    pdraw->line = i;
	}
	else //unicode
	{
		uint16_t width;
		width = pdraw->col_num / 2 * w;
		pdraw->pline[0].width = 0;
		for(j=0,i=0,k=0;(j<len) && (i<line);j++)
		{        
			if((code[j]==0) && (code[j+1]<0x80))
			{
				if(code[j+1] != '\r')
				{
					pdraw->pline[i].pcode[k] = code[j];
					k++;
					j++;
					pdraw->pline[i].pcode[k] = code[j];
					k++;
					pdraw->pline[i].width += w;

					if(pdraw->pline[i].width + w > width || (j+2) >= len)
					{
						pdraw->pline[i].len = k;
						k = 0;
						i++;
						if(i < line)
						{
							pdraw->pline[i].width = 0;
						}
					}
				}
				else if((j + 3) < len)
				{
					if(code[j+2]==0 && code[j+3]=='\n' )
					{
						j+=3;
					    pdraw->pline[i].len = k;
						k = 0;
						i++;
						if(i < line)
						{
							pdraw->pline[i].width = 0;
						}
					}
					else
					{
						pdraw->pline[i].pcode[k] = code[j];
						k++;
						j++;
						pdraw->pline[i].pcode[k] = code[j];
						k++;
						pdraw->pline[i].width += w;

						if(pdraw->pline[i].width + w > width || (j+2) >= len)
						{
							pdraw->pline[i].len = k;
							k = 0;
							i++;
							if(i < line)
							{
								pdraw->pline[i].width = 0;
							}
						}
								
					}
				}
				else
				{
					pdraw->pline[i].pcode[k] = code[j];
					k++;
					j++;
					pdraw->pline[i].pcode[k] = code[j];
					k++;
					pdraw->pline[i].width += w;

					if(pdraw->pline[i].width + w > width || (j+2) >= len)
					{
						pdraw->pline[i].len = k;
						k = 0;
						i++;
						if(i < line)
						{
							pdraw->pline[i].width = 0;
						}
					}
				}
			}
			else
			{
				if(pdraw->pline[i].width + 2*w <=  width)
				{
					pdraw->pline[i].pcode[k] = code[j];
					k++;
					j++;
					pdraw->pline[i].pcode[k] = code[j];
					k++;
					pdraw->pline[i].width += 2*w;

					if(pdraw->pline[i].width + w > width || (j+2) >= len)
					{
						pdraw->pline[i].len = k;
						k = 0;
						i++;
						if(i < line)
						{
							pdraw->pline[i].width = 0;
						}
					}
				}
				else
				{
					j--;
					pdraw->pline[i].len = k;
					k = 0;
					i++;
					if(i < line)
					{
						pdraw->pline[i].width = 0;
					}
				}
			}
		 
		}

		pdraw->line = i;
	}

}





static void btn_show_font(t_btn * pbtn)
{
    if(pbtn->ptfont)
    {
        uint16_t str_id,color,width,height,htemp,hz;
        font_type * ch_font, *hz_font;
        uint8_t line,i,code_index;
        uint16_t x,y;
        uint8_t * code;

        if(GETBIT(pbtn->state,BTN_STATE_ACTIVE))
        {
            str_id = pbtn->pstr_id;
            color = pbtn->pcolor;
        }
        else
        {
            str_id = pbtn->rstr_id;
            color = pbtn->rcolor;
        }

        ch_font = hz_to_ch(pbtn->ptfont);
		hz_font = ch_to_hz(pbtn->ptfont);
		
		
		if(pbtn->ptfont->code == 0)
		{
			width = pbtn->ptfont->width;
		}
		else
		{
			width = pbtn->ptfont->width / 2;
		}

		height = get_font_height(*(pbtn->ptfont));

		update_draw_text(pbtn->pdraw,str_id,pbtn->ptfont);

		line = pbtn->pdraw->line;
		
		htemp = line * height;

		y = pbtn->tp_rect.top + (pbtn->tp_rect.bottom - pbtn->tp_rect.top + 1 - htemp) / 2;

		for(i = 0;i<line;i++)
		{
		    x = pbtn->tp_rect.left + (pbtn->tp_rect.right - pbtn->tp_rect.left + 1 - pbtn->pdraw->pline[i].width) / 2;
		    code = pbtn->pdraw->pline[i].pcode;
		    if(pbtn->ptfont->code < 3)
			{
				for(code_index=0;code_index<pbtn->pdraw->pline[i].len;code_index++)
				{
					if(code[code_index] < 0x80)
					{
						show_charn_font(x,y,color, code[code_index],*ch_font);
						x += width;
					}
					else
					{
						hz = code[code_index] << 8 | code[code_index + 1];
						show_nfont(x,y,color, hz,*hz_font);
						code_index++;
						x += 2*width;
					}

						
				}
    
			}
			else
			{
			    for(code_index=0;code_index<pbtn->pdraw->pline[i].len;code_index++)
				{
					if(code[code_index] == 0 && code[code_index+1] < 0x80)
					{
						show_charn_font(x,y,color, code[code_index + 1],*ch_font);
						x += width;
					}
					else
					{
						hz = code[code_index] << 8 | code[code_index + 1];
						show_nfont(x,y,color, hz,*hz_font);
						x += 2*width;
					}
					code_index++;
				}
			}

			if(i+1 < line)
			{
				y += height;
			}
		}

        
        
    }
}




    
    
    
    
    
    
static draw_text  * init_draw_text(t_rect rect,font_type * ptfont)
{

	draw_text * pdraw = NULL;
	uint32_t size;
	uint8_t i;
	uint16_t w,h;
	uint8_t line,col;

	if(ptfont == NULL)
	{
		return NULL;
	}
	
	
	

	if(ptfont->code == 0)
	{
		w = ptfont->width;
	}
	else
	{
		w = ptfont->width / 2;
	}

	h = get_font_height(*ptfont);

	line = (rect.bottom - rect.top + 1) / h;
	col = (rect.right - rect.left +  1) / w;
		
	if(col == 0 || line == 0)
	{
		return NULL;
	}

	if(ptfont->code == 3)
	{
		col = 2 * col;
	}
	size = sizeof(draw_text) + line * (sizeof(line_text) + col);
	pdraw = (draw_text *) my_malloc(size);
	pdraw->line_num = line;
	pdraw->col_num = col;

	pdraw->pline = (line_text *) ((uint8_t *) pdraw + sizeof(draw_text)); 

	for(i = 0; i < line ; i++)
	{
		pdraw->pline[i].pcode = (uint8_t *) ((uint8_t *) pdraw + sizeof(draw_text) + line * sizeof(line_text) + i * col);
	}

	return pdraw;
		
 
	

}





static void btn_draw_screen(base_touch * pbtn)
{

    dc ddc;
    uint16_t x,y;
	t_rect rect;
    if(pbtn->type != CFG_BTN)
    {
        return;
    }
    t_btn * p_btn = (t_btn *)pbtn;
	

    if(disp_dev.dir == 0)
    {

    	if(disp_dev.flag)
    	{
    	    ddc = disp_dev.disp[0];
    	}
    	else
    	{
    	    ddc = disp_dev.disp[1];
    	}
	}
	else if(disp_dev.dir == 1)
	{
	    ddc = disp_dev.disp[3];
	}
	
	if(p_btn->virtual_fame)
	{
		if(GETBIT(p_btn->state,BTN_STATE_ACTIVE))
		{
			draw_virtual_frame(p_btn);
		}
	}

	

	if(p_btn->pic_on >= 0)
	{
        x = p_btn->tp_rect.left;
	    y = p_btn->tp_rect.top;
	    rect.left = 0;
	    rect.right = p_btn->depress.width;
	    rect.top = 0;
		rect.bottom = p_btn->depress.height;

		
		if(p_btn->use == switch_screen)
		{
			btn_screen * p = (btn_screen *) p_btn->tp_use;
		    if(p->status)
		    {     
				vblt(ddc,x,y,rect,p_btn->depress);
		    }
			
		}
		else if(p_btn->use == switch_btn)
		{
			btn_switch * p = (btn_switch *) p_btn->tp_use;
			if(p->status)
		    {  
				vblt(ddc,x,y,rect,p_btn->depress);
		    }
			
		}
		else if(p_btn->use == custom_key)
		{
			btn_key * p = (btn_key *) p_btn->tp_use;
		    if(p->status)
		    {  
				vblt(ddc,x,y,rect,p_btn->depress);
		    }	
		}
		else if(p_btn->use == custom_cmd)
		{
			btn_cmd * p = (btn_cmd *) p_btn->tp_use;
			if(p->status)
		    {  
				vblt(ddc,x,y,rect,p_btn->depress);
		    }
		}
		else if(p_btn->use == popup_list)
		{
			btn_list * p = (btn_list *) p_btn->tp_use;
			if(p->status)
		    {  
				vblt(ddc,x,y,rect,p_btn->depress);
		    }
		}
		
		
	
	}

	if(p_btn->pdraw)
	{
	    btn_show_font(p_btn);
	}
	

	
}







static btn_cmd * btn_cmd_create(uint8_t * buffer)
{
	btn_cmd  *p;
	//uint16_t offset = 0;
	
	

	p = (btn_cmd*) my_malloc(sizeof(btn_cmd));
	if(p == NULL)
	{
		return p;
	}
	
	memset(p,0,sizeof(btn_cmd));

	p->up_addr = (buffer[2]<<24)|(buffer[3]<<16)|(buffer[4]<<8)|(buffer[5]);
	p->up_len = (buffer[0]<<8)|(buffer[1]);
	

	p->down_addr = (buffer[8]<<24)|(buffer[9]<<16)|(buffer[10]<<8)|(buffer[11]);
	p->down_len = (buffer[6]<<8)|(buffer[7]);

	p->times = (buffer[12]<<8)|(buffer[13]);
	
	

	return p;
}




static btn_list * btn_list_create(uint8_t * buffer)
{

	btn_list * p;
	p = my_malloc(sizeof(btn_list));
	if(p == NULL)
	{
		return p;
	}

	p->menu = (buffer[0] << 8) | (buffer[1]);
	p->text = (buffer[2] << 8) | (buffer[3]);
	
	p->status = 0;
	p->old_status = 0;

	return p;
}



	

static btn_switch * sbtn_switch_create(uint8_t * buffer)
{
	btn_switch * p;

	p = (btn_switch *) my_malloc(sizeof(btn_switch));
	if(p == NULL)
	{
		return p;
	}
	memset((uint8_t *)p,0,sizeof(btn_switch));
	p->status = buffer[0];
	p->old_status = p->status;
	p->pstatus = &buffer[0];
	
		
	return p;
}













uint8_t btn_create(uint8_t * buffer)
{
	btn_screen * tp_use;
	btn_key * key;
	t_btn * p_btn = my_malloc(sizeof(t_btn));
//	uint32_t size;
	if(p_btn == NULL)
	{
		return 1;
	}
	memset(p_btn,0,sizeof(t_btn));
	p_btn->ctrl.id = ((buffer[0] << 24) | (buffer[1] << 16) | (buffer[4] << 8) | (buffer[5] << 0));
	p_btn->ctrl.func_ctrl_procmsg  = btn_msg;
	p_btn->ctrl.func_ctrl_release  = btn_release;
	p_btn->screen_update = 0;

	p_btn->tp_type = buffer[6];
	p_btn->tp_rect.left = ((buffer[7] << 8) | (buffer[8]));
	p_btn->tp_rect.top= ((buffer[9] << 8) | buffer[10]);
	p_btn->tp_rect.right =((buffer[11] << 8) | (buffer[12]));
	p_btn->tp_rect.bottom = ((buffer[13] << 8) | (buffer[14]));
	p_btn->pic_id   = (buffer[2] << 8) | (buffer[3]);
	p_btn->pic_on   = ((buffer[15] << 8) | (buffer[16]));
	p_btn->virtual_fame = buffer[17];
	p_btn->notify = buffer[18];
	p_btn->ptfont = get_font_type(buffer[19]);
	p_btn->rcolor = (buffer[20] << 8) | (buffer[21]);
	p_btn->pcolor = (buffer[22] << 8) | (buffer[23]);
	p_btn->rstr_id = (buffer[24] << 8) | (buffer[25]);
	p_btn->pstr_id = (buffer[26] << 8) | (buffer[27]);

	if(p_btn->ptfont)
	{
	    p_btn->pdraw = init_draw_text(p_btn->tp_rect,p_btn->ptfont);
	}
	
	p_btn->use = (btn_use)buffer[28];
	p_btn->act = btn_act;
	p_btn->draw_screen = btn_draw_screen;
	p_btn->cmd_update_status = cmd_update_btn_status;
	p_btn->cmd_get_status = cmd_get_btn_status;
	//p_btn->init_screen = btn_init_screen;
    //p_btn->update_screen = btn_update_screen;
	

	if(p_btn->virtual_fame)
	{
		if((p_btn->tp_rect.left<1)||(p_btn->tp_rect.right > disp_dev.width-2)||(p_btn->tp_rect.top<1)||(p_btn->tp_rect.bottom>disp_dev.height-2) )
		{
			p_btn->virtual_fame = 0;
		}
	
	}
	
	if(p_btn->pic_on >= 0)
	{

		p_btn->depress.width = p_btn->tp_rect.right - p_btn->tp_rect.left + 1;
		p_btn->depress.height = p_btn->tp_rect.bottom - p_btn->tp_rect.top + 1;
		p_btn->depress.p = (uint8_t * ) my_malloc(p_btn->depress.width * p_btn->depress.height * disp_dev.bpp);
		//taskENTER_CRITICAL(); 
        get_touch_image(p_btn->depress, (base_touch *) p_btn, 0);
        //taskEXIT_CRITICAL();

        //p_btn->release.x = p_btn->tp_rect.left;
        //p_btn->release.y = p_btn->tp_rect.top;
		//p_btn->release.width  = p_btn->depress.width;
		//p_btn->release.height = p_btn->depress.height;
		//p_btn->release.vdc      = disp_dev.disp[2];
		
	}
	
	
	
	switch(p_btn->use)
	{
		case switch_screen:
			
			tp_use = (btn_screen *)my_malloc(sizeof(sizeof(btn_screen)));
			if(tp_use== NULL)
			{
				if(p_btn->pic_on >= 0)
				{
					my_free(p_btn->depress.p);
					
				}
				my_free(p_btn);
				return 1;
			}
			tp_use->old_status = 0;
			tp_use->status = 0;
			tp_use->screen = (buffer[29] << 8) | (buffer[30]);
			p_btn->tp_use = (void *)tp_use;
			break;
		case custom_key:
			
			key = (btn_key*)my_malloc(sizeof(uint8_t));
			if(key== NULL)
			{
				if(p_btn->pic_on >= 0)
				{
					my_free(p_btn->depress.p);
					
				}
				my_free(p_btn);
				return 1;
			}
			key->type = (key_type)buffer[29];
			key->value = buffer[30];
			key->status = 0;
			key->old_status = 0;
			p_btn->tp_use = (void *)key;
			break;
		case switch_btn:
			
			p_btn->tp_use = (void *)btn_switch_create(&buffer[29]);
			
			
			if(p_btn->tp_use== NULL)
			{
				if(p_btn->pic_on >= 0)
				{
					my_free(p_btn->depress.p);
					
				}
				my_free(p_btn);
				return 1;
			}
			break;
		case custom_cmd:
			p_btn->tp_use = (void *)btn_cmd_create(&buffer[29]);

			if(p_btn->tp_use== NULL)
			{
				if(p_btn->pic_on >= 0)
				{
					my_free(p_btn->depress.p);
					
				}
				my_free(p_btn);
				return 1;
			}
			break;
		case popup_list:

			p_btn->tp_use = (void *)btn_list_create(&buffer[29]);
			if(p_btn->tp_use== NULL)
			{
				if(p_btn->pic_on >= 0)
				{
					my_free(p_btn->depress.p);
					
				}
				my_free(p_btn);
				return 1;
			}
			break;
			
		default:
	
			return 1;
		
	}
	p_btn->state = 0;
	
	ctrl_init((t_ctrl*) p_btn);
	
	
	
	return 0;

	
}






//简单的创建控件，当收到串口指令改变该控件状态时，而该控件在
//当前界面没有被创建，调用该函数

t_ctrl * sbtn_create(uint8_t * buffer)
{
	btn_screen * tp_use;
	btn_key * key;
	t_btn * p_btn = my_malloc(sizeof(t_btn));
	
	if(p_btn == NULL)
	{
		return (t_ctrl *)p_btn;
	}
	memset(p_btn,0,sizeof(t_btn));
	p_btn->ctrl.id = ((buffer[0] << 24) | (buffer[1] << 16) | (buffer[4] << 8) | (buffer[5] << 0));
	//p_btn->ctrl.func_ctrl_procmsg  = btn_msg;
	p_btn->ctrl.func_ctrl_release  = btn_release;
	
	p_btn->screen_update = 0;

	p_btn->tp_type = buffer[6];
	p_btn->tp_rect.left = 0xffff;
	p_btn->tp_rect.top= 0xffff;
	p_btn->tp_rect.right =0xffff;
	p_btn->tp_rect.bottom = 0xffff;
	p_btn->pic_id   = (buffer[2] << 8) | (buffer[3]);
	p_btn->pic_on   = 0xffff;
	p_btn->use = (btn_use)buffer[28];
	p_btn->cmd_update_status = cmd_update_btn_status;
	p_btn->cmd_get_status = cmd_get_btn_status;
	
	
	
	switch(p_btn->use)
	{
		case switch_screen:
			
			tp_use = (btn_screen *)my_malloc(sizeof(sizeof(btn_screen)));
			if(tp_use== NULL)
			{
			   
				
				my_free(p_btn);
				
				return NULL;
			}
			tp_use->old_status = 0;
			tp_use->status = 0;
			tp_use->screen = (buffer[29] << 8) | (buffer[30]);
			p_btn->tp_use = (void *)tp_use;
			break;
		case custom_key:
			
			key = (btn_key*)my_malloc(sizeof(uint8_t));
			if(key== NULL)
			{
			   
				my_free(p_btn);
				return NULL;
			}
			key->type = (key_type)buffer[29];
			key->value = buffer[30];
			key->status = 0;
			key->old_status = 0;
			p_btn->tp_use = (void *)key;
			break;
		case switch_btn:
			
			p_btn->tp_use = (void *)sbtn_switch_create(&buffer[29]);
			
			
			if(p_btn->tp_use== NULL)
			{   
				
				my_free(p_btn);
				
				return NULL;
			}
			break;
		case custom_cmd:
			p_btn->tp_use = (void *)btn_cmd_create(&buffer[29]);

			if(p_btn->tp_use== NULL)
			{
			   
				my_free(p_btn);
				return NULL;
			}
			break;
		case popup_list:

			p_btn->tp_use = (void *)btn_list_create(&buffer[29]);
			if(p_btn->tp_use== NULL)
			{
			   
				my_free(p_btn);
				return NULL;
			}
			break;
			
		default:
	
			return NULL;
		
	}
	p_btn->state = 0;
	ctrl_init((t_ctrl*) p_btn);
	return (t_ctrl *)p_btn;

	
}









uint8_t keyboard_btn_create(uint16_t x, uint16_t y,uint8_t * buffer)
{
	btn_screen * tp_use;
	btn_key * key;
	t_btn * p_btn = my_malloc(sizeof(t_btn));
//	uint32_t size;
	if(p_btn == NULL)
	{
		return 1;
	}
	memset(p_btn,0,sizeof(t_btn));
	p_btn->ctrl.id = ((buffer[0] << 24) | (buffer[1] << 16) | (buffer[4] << 8) | (buffer[5] << 0));
	p_btn->ctrl.func_ctrl_procmsg  = btn_msg;
	p_btn->ctrl.func_ctrl_release  = btn_release;
//	ctrl_init((t_ctrl*) p_btn);
	p_btn->screen_update = 0;

	p_btn->tp_type = buffer[6];
	p_btn->tp_rect.left = ((buffer[7] << 8) | (buffer[8]));
	p_btn->tp_rect.top= ((buffer[9] << 8) | buffer[10]);
	p_btn->tp_rect.right =((buffer[11] << 8) | (buffer[12]));
	p_btn->tp_rect.bottom = ((buffer[13] << 8) | (buffer[14]));
	p_btn->pic_id   = (buffer[2] << 8) | (buffer[3]);
	p_btn->pic_on   = ((buffer[15] << 8) | (buffer[16]));
	p_btn->virtual_fame = buffer[17];
	p_btn->notify = buffer[18];
	
	p_btn->ptfont = get_font_type(buffer[19]);
	p_btn->rcolor = (buffer[20] << 8) | (buffer[21]);
	p_btn->pcolor = (buffer[22] << 8) | (buffer[23]);
	p_btn->rstr_id = (buffer[24] << 8) | (buffer[25]);
	p_btn->pstr_id = (buffer[26] << 8) | (buffer[27]);
	if(p_btn->ptfont)
	{
	    p_btn->pdraw = init_draw_text(p_btn->tp_rect,p_btn->ptfont);
	}
	
	p_btn->use = (btn_use)buffer[28];
	p_btn->act = btn_act;
	//p_btn->cmd_update_status = cmd_update_btn_status;
	//p_btn->cmd_get_status = cmd_get_btn_status;
	p_btn->draw_screen = btn_draw_screen;
    //p_btn->update_screen = btn_update_screen;
	

    if(p_btn->pic_on >= 0)
	{

	  
		p_btn->depress.width = p_btn->tp_rect.right - p_btn->tp_rect.left + 1;
		p_btn->depress.height = p_btn->tp_rect.bottom - p_btn->tp_rect.top + 1;
		p_btn->depress.p = (uint8_t * ) my_malloc(p_btn->depress.width * p_btn->depress.height * disp_dev.bpp);

	}
	

	if(p_btn->virtual_fame)
	{
		if((p_btn->tp_rect.left<1)||(p_btn->tp_rect.right > disp_dev.width-2)||(p_btn->tp_rect.top<1)||(p_btn->tp_rect.bottom>disp_dev.height-2) )
		{
			p_btn->virtual_fame = 0;
		}
	
	}
	
	
	
	switch(p_btn->use)
	{
		case switch_screen:
			
			tp_use = (btn_screen *)my_malloc(sizeof(sizeof(btn_screen)));
			if(tp_use== NULL)
			{
				if(p_btn->pic_on >= 0)
				{
					my_free(p_btn->depress.p);
					
				}
				//release_ctrl( (t_ctrl *) p_btn);
				my_free(p_btn);
				return 1;
			}
			tp_use->old_status = 0;
			tp_use->status = 0;
			tp_use->screen = (buffer[29] << 8) | (buffer[30]);
			p_btn->tp_use = (void *)tp_use;
			break;
		case custom_key:
			
			key = (btn_key*)my_malloc(sizeof(uint8_t));
			if(key== NULL)
			{
				if(p_btn->pic_on >= 0)
				{
					my_free(p_btn->depress.p);
					
				}
				//release_ctrl( (t_ctrl *) p_btn);
				my_free(p_btn);
				return 1;
			}
			key->type = (key_type)buffer[29];
			key->value = buffer[30];
			key->status = 0;
			key->old_status = 0;
			p_btn->tp_use = (void *)key;
			break;
		case switch_btn:
			
			p_btn->tp_use = (void *)btn_switch_create(&buffer[29]);
			
			
			if(p_btn->tp_use== NULL)
			{
				if(p_btn->pic_on >= 0)
				{
					my_free(p_btn->depress.p);
				}
				//release_ctrl( (t_ctrl *) p_btn);
				my_free(p_btn);
				return 1;
			}
			break;
		case custom_cmd:
			p_btn->tp_use = (void *)btn_cmd_create(&buffer[29]);

			if(p_btn->tp_use== NULL)
			{
				if(p_btn->pic_on >= 0)
				{
					my_free(p_btn->depress.p);
				}

				//release_ctrl( (t_ctrl *) p_btn);
				my_free(p_btn);
				return 1;
			}
			break;
		case popup_list:

			p_btn->tp_use = (void *)btn_list_create(&buffer[29]);
			if(p_btn->tp_use== NULL)
			{
				if(p_btn->pic_on >= 0)
				{
					my_free(p_btn->depress.p);
				}
				//release_ctrl( (t_ctrl *) p_btn);
				my_free(p_btn);
				return 1;
			}
			break;
			
		default:
	
			return 1;
		
	}
	p_btn->state = 0;
	
	if(p_btn->depress.p)
	{
		//taskENTER_CRITICAL(); 
		get_touch_image(p_btn->depress, (base_touch *) p_btn, 0);
		//taskEXIT_CRITICAL();
	}
	
	
	
	p_btn->tp_rect.left += x;
	p_btn->tp_rect.right += x;
	p_btn->tp_rect.top += y;
	p_btn->tp_rect.bottom += y;
	
	ctrl_init((t_ctrl*) p_btn);
	
	
	
	return 0;

	
}





/***************************************************************************************
* End of File: btn.c

****************************************************************************************/
