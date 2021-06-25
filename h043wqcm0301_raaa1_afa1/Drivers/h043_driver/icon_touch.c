
/***************************************************************************************
****************************************************************************************
* Project      : 20180525_HMI
* FILE         : icon_touch.c
* Description  : 
*                      
* Copyright (c) 2018 by ShenZhen JingHua Displays Co.,Ltd... All Rights Reserved.
* 
* History:
* Version		name			Date			Description
* 0.01			Qu Bingbing		2018/05/31		Initial Version
* 
****************************************************************************************
****************************************************************************************/
#include "icon_touch.h"
#include "mem_my.h"
#include "cfg.h"
#include "display.h"
#include "string.h"

static uint16_t x1,y1_2,x2,y2;


static uint8_t icon_msg( void * p, t_msg * p_msg )
{
    uint8_t f = 1;
    uint16_t x,y,flag;
	t_msg msg;

    if(p_msg->msg != MSG_TP)
	{
		return f;
	}

	icon_touch * picon = (icon_touch *)p;

	if(picon->touch_act == 0 || picon->ctrl.active == 0)
	{
		return f;
	}

	x = p_msg->a;
	y = p_msg->b;
	flag = p_msg->c;

	if((flag != 1) && !GETBIT( picon->state, BTN_STATE_ACTIVE))
	{
		return f;
	}

	if(pt_in_rect(&picon->tp_rect, x, y))
	{
		f = 0;
	}

	switch(flag)
	{
	    case 1:
	        if(!f)
	        {
	            SETBIT( picon->state, BTN_STATE_ACTIVE);
	            msg.msg = MSG_TOUCH;
							msg.a   = (picon->ctrl.id >> 16) & (0xffff);
							msg.b   = (picon->ctrl.id) & (0xffff);
							msg.c   = 1;
				msg_dev.write_message((void *)&msg,0);
			    x1 = p_msg->a;
			    y1_2 = p_msg->b;
	        }
	        break;
	    case 2:
	        msg.msg = MSG_TOUCH;
			msg.a   = (picon->ctrl.id >> 16) & (0xffff);
			msg.b   = (picon->ctrl.id) & (0xffff);
			msg.c   = 2;
		    msg_dev.write_message((void *)&msg,0);
		    x2 = p_msg->a;
			y2 = p_msg->b;
	        break;
	    case 0:
	        msg.msg = MSG_TOUCH;
			msg.a   = (picon->ctrl.id >> 16) & (0xffff);
			msg.b   = (picon->ctrl.id) & (0xffff);
			msg.c   = 0;
			msg_dev.write_message((void *)&msg,0);
			CLRBIT( picon->state, BTN_STATE_ACTIVE);
			x2 = p_msg->a;
			y2 = p_msg->b;
	        break;
	}
  

	return f;
    
}






static void icon_release( void* pCtrl )
{

    icon_touch * picon = (icon_touch *) pCtrl;

    if(picon->touch_icon.p)
    {
        my_free((void *)picon->touch_icon.p);
    }
	
    my_free((void *)picon);
    
}










static void icon_act(base_touch *p,uint8_t flag)
{

    uint16_t frame;

    if(p->type != CFG_ICO)
    {
        return;
    }


    icon_touch * picon = (icon_touch *) p;

    if(flag == 2 || flag == 0)
    {
        frame = (picon->frame_num[0] << 8) | (picon->frame_num[1]);
        
        if(picon->touch_act == 2)
        {
            if(x1 > x2)
            {
                if((x1 - x2) > picon->step_length)
                {
                    frame += 1;

                    if(frame >= picon->touch_icon.num)
                    {
                        frame = 0;
                    }

                    picon->frame_num[0] = (frame >> 8);
                    picon->frame_num[1] = frame;
                    picon->screen_update = 1;
					x1 = x2;
					y1_2 = y2;
                }
            }
            else
            {
                if((x2 - x1) >= picon->step_length)
                {
                    if(frame == 0)
                    {
                        frame = picon->touch_icon.num;
                    }
                    frame -= 1;
                    picon->frame_num[0] = (frame >> 8);
                    picon->frame_num[1] = frame;
                    picon->screen_update = 1;
					x1 = x2;
					y1_2 = y2;
                    
                }
                
            }
        }
		else if(picon->touch_act == 3)
		{
			if(y1_2 > y2)
			{
				if((y1_2 - y2) > picon->step_length)
				{
					frame += 1;

					if(frame >= picon->touch_icon.num)
					{
						frame = 0;
					}

					picon->frame_num[0] = (frame >> 8);
					picon->frame_num[1] = frame;
					picon->screen_update = 1;
					x1 = x2;
					y1_2 = y2;
				}
           
            
			}
			else
			{

				if((y2 - y1_2) >= picon->step_length)
				{
					if(frame == 0)
					{
						frame = picon->touch_icon.num;
					}
					frame -= 1;
					picon->frame_num[0] = (frame >> 8);
					picon->frame_num[1] = frame;
					picon->screen_update = 1;
					x1 = x2;
					y1_2 = y2;
                    
                }
            
			}
		}
    }
    

    
}















static void icon_draw_screen(base_touch * p)
{

    if(p->type != CFG_ICO)
    {
        return;
    }


    icon_touch * picon  = (icon_touch *) p;

   

    uint16_t frame = (picon->frame_num[0] << 8) | (picon->frame_num[1]);

    if(frame >=  picon->touch_icon.num)
    {
        frame = 0;
        picon->frame_num[0] = 0;
        picon->frame_num[1] = 0;
    }

    //vblt_bdc(disp_dev.disp[1],picon->back_image);

    if(get_icon_image(&(picon->touch_icon), frame) == 0)
    {
		uint8_t * dp;
		if(disp_dev.dir == 0)
		{
    		if(disp_dev.flag)
    		{
    			dp = disp_dev.disp[0].p;
    		}
    		else
    		{
    			dp = disp_dev.disp[1].p;
    		}
		}
		else if(disp_dev.dir == 1)
		{
		    dp = disp_dev.disp[3].p;
		}
        show_icon_image_mix(picon->touch_icon,picon->tp_rect,dp);
    }

    
    
}













uint8_t icon_create(uint8_t * buffer)
{
    uint32_t addr;
    
    icon_touch * picon = (icon_touch *)my_malloc(sizeof(icon_touch));

    if(picon == NULL)
    {
        return 1;
    }

    memset((void*)picon,0,sizeof(icon_touch));
	picon->ctrl.id = ((buffer[0] << 24) | (buffer[1] << 16) | (buffer[4] << 8) | (buffer[5] << 0));
	picon->ctrl.func_ctrl_procmsg  = icon_msg;
	picon->ctrl.func_ctrl_release  = icon_release;
	picon->type = buffer[6];
	picon->tp_rect.left = ((buffer[7] << 8) | (buffer[8]));
	picon->tp_rect.top= ((buffer[9] << 8) | buffer[10]);
	picon->tp_rect.right =((buffer[11] << 8) | (buffer[12]));
	picon->tp_rect.bottom = ((buffer[13] << 8) | (buffer[14]));
	picon->pic_id   = (buffer[2] << 8) | (buffer[3]);
	picon->pic_on   = ((buffer[15] << 8) | (buffer[16]));

	picon->act = icon_act;
	picon->draw_screen = icon_draw_screen;
	//picon->update_screen = icon_update_screen;
	//picon->cmd_update_status = NULL;
	//picon->cmd_get_status = NULL;

	//picon->back_image.x = picon->tp_rect.left;
	//picon->back_image.y = picon->tp_rect.top;
	//picon->back_image.width = picon->tp_rect.right - picon->tp_rect.left + 1;
	//picon->back_image.height = picon->tp_rect.bottom - picon->tp_rect.top + 1;
	//picon->back_image.vdc = disp_dev.disp[2];
	
	picon->init_show = &buffer[17];
	picon->touch_act = buffer[18];
	picon->step_length = (buffer[19]<<8) | (buffer[20]);
	picon->frame_num = &buffer[21];
	addr = (buffer[23]<<24) | (buffer[24]<<16) | (buffer[25]<<8) | (buffer[26]);
	picon->state = 0;
	
	init_icon(&(picon->touch_icon), addr);
	ctrl_init((t_ctrl*) picon);
	
	if((*(picon->init_show))==0 || picon->touch_act==0)
	{
		picon->ctrl.active = 0;
	}
	
	return 0;
	
}














t_ctrl * sicon_create(uint8_t * buffer)
{
    uint32_t addr;
    
    icon_touch * picon = (icon_touch *)my_malloc(sizeof(icon_touch));

    if(picon == NULL)
    {
        return NULL;
    }

    memset((void*)picon,0,sizeof(icon_touch));
	picon->ctrl.id = ((buffer[0] << 24) | (buffer[1] << 16) | (buffer[4] << 8) | (buffer[5] << 0));
	picon->ctrl.func_ctrl_procmsg  = icon_msg;
	picon->ctrl.func_ctrl_release  = icon_release;
	picon->type = buffer[6];
	//picon->tp_rect.left = ((buffer[7] << 8) | (buffer[8]));
	//picon->tp_rect.top= ((buffer[9] << 8) | buffer[10]);
	//picon->tp_rect.right =((buffer[11] << 8) | (buffer[12]));
	//picon->tp_rect.bottom = ((buffer[13] << 8) | (buffer[14]));
	//picon->pic_id   = (buffer[2] << 8) | (buffer[3]);
	//picon->pic_on   = ((buffer[15] << 8) | (buffer[16]));

	//picon->act = icon_act;
	//picon->init_screen = icon_init_screen;
	//picon->update_screen = icon_update_screen;
	//picon->cmd_update_status = NULL;
	//picon->cmd_get_status = NULL;

	//picon->back_image.x = picon->tp_rect.left;
	//picon->back_image.y = picon->tp_rect.top;
	//picon->back_image.width = picon->tp_rect.right - picon->tp_rect.left + 1;
	//picon->back_image.height = picon->tp_rect.bottom - picon->tp_rect.top + 1;
	//picon->back_image.vdc = disp_dev.disp[2];
	
	picon->init_show = &buffer[17];
	//picon->touch_act = buffer[18];
	//picon->step_length = (buffer[19]<<8) | (buffer[20]);
	picon->frame_num = &buffer[21];
	
	addr = (buffer[23]<<24) | (buffer[24]<<16) | (buffer[25]<<8) | (buffer[26]);
	picon->state = 0;
	
	init_icon(&(picon->touch_icon), addr);
	ctrl_init((t_ctrl*) picon);
	
	picon->ctrl.active = 0;
	
	
	return (t_ctrl *) picon;
	
	
}




/***************************************************************************************
* End of File: icon_touch.c

****************************************************************************************/
