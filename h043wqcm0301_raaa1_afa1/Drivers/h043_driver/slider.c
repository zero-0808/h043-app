
/***************************************************************************************
****************************************************************************************
* Project      : 20180525_HMI
* FILE         : slider.c
* Description  : 
*                      
* Copyright (c) 2018 by ShenZhen JingHua Displays Co.,Ltd... All Rights Reserved.
* 
* History:
* Version		name			Date			Description
* 0.01			Qu Bingbing		2018/05/30		Initial Version
* 
****************************************************************************************
****************************************************************************************/
#include "slider.h"
#include "mem_my.h"
#include "string.h"
#include "cmd.h"
#include "display.h"
#include "cfg.h"

uint16_t g_x, g_y;


static  float count_slider_value(uint16_t start,uint16_t end,uint16_t t)
{
    float value;

    if(start < end)
    {
        if(t < start)
        {
            value = 0;
        }
        else if(t > end)
        {
            value = 1;
        }
        else
        {
            value = (float)(t-start)/(float)(end-start);
        }
    }
    else
    {
        if(t > start)
        {
            value = 0;
        }
        else if(t<end)
        {
            value = 1;
        }
        else
        {
            value = (float)(start-t)/(float)(start-end);
        }
    }
		
		return value;
}



static void update_slider_value(slider * pslider, uint16_t x, uint16_t y)
{
    float value;
    float dec;
	uint8_t * p;

	if(pslider->slider_dir == 0)
	{
	   value = 
	   count_slider_value(pslider->tp_rect.left+pslider->start_margin,pslider->tp_rect.right-pslider->end_margin,x);
	}
	else
	{
	    value=count_slider_value(pslider->tp_rect.bottom-pslider->start_margin,pslider->tp_rect.top+pslider->end_margin,y
); 
	}

	value = pslider->start_value + value*(pslider->end_value-pslider->start_value);
	pslider->value = value;

	dec = value - pslider->value;
	if(dec > 0.5)
	{
	    pslider->value += 1;
	}
	if(pslider->value > pslider->end_value)
	{
	    pslider->value = pslider->end_value;
	}
	else if(pslider->value < pslider->start_value)
	{
	    pslider->value = pslider->start_value;
	}
	p=pslider->pvalue;

	p[0]=(pslider->value >> 24) & 0xff;
	p[1]=(pslider->value >> 16) & 0xff;
	p[2]=(pslider->value >> 8) & 0xff;
	p[3]=(pslider->value >> 0) & 0xff;

	
    
}



static uint8_t slider_msg( void * p, t_msg * p_msg )
{
    uint16_t x,y,flag,f=1;
		t_msg msg;
    
    if(p_msg->msg != MSG_TP)
    {
        return f;
    }
    
    
    slider * pslider = (slider *) p;
    
    if(pslider->rect == touch_null || pslider->ctrl.active == 0)
    {
        return f;
    }
    x = p_msg->a;
    y = p_msg->b;
    flag = p_msg->c;

    if((flag != 1) && !GETBIT( pslider->state, BTN_STATE_ACTIVE))
	{
		return f;
	}

	if(pt_in_rect(&pslider->touch, x, y))
	{
	    f = 0;
	}

	switch(flag)
	{
	    case 1:
	    {
	        if(!f)
	        {
	            SETBIT( pslider->state, BTN_STATE_ACTIVE);
							msg.msg = MSG_TOUCH;
			    msg.a   = (pslider->ctrl.id >> 16) & (0xffff);
			    msg.b   = (pslider->ctrl.id) & (0xffff);
			    msg.c   = 1;
			    msg_dev.write_message((void *)&msg,0);
			    g_x = x;
			    g_y = y;
			    xTimerReset(pslider->timer,3);
			    xTimerStart(pslider->timer,3);
	        }
	        break;
	    }
	    case 2:
	    {
	       msg.msg = MSG_TOUCH;
		   msg.a   = (pslider->ctrl.id >> 16) & (0xffff);
		   msg.b   = (pslider->ctrl.id) & (0xffff);
		   msg.c   = 2;
		   msg_dev.write_message((void *)&msg,0); 
		   g_x = x;
		   g_y = y;
		   break;
	    }
	    case 0:
	    {
           msg.msg = MSG_TOUCH;
		   msg.a   = (pslider->ctrl.id >> 16) & (0xffff);
		   msg.b   = (pslider->ctrl.id) & (0xffff);
		   msg.c   = 0;
		   msg_dev.write_message((void *)&msg,0); 
            g_x = x;
            g_y = y;
		   update_slider_value(pslider,x,y); 
		   xTimerStop(pslider->timer,3);
	       CLRBIT( pslider->state, BTN_STATE_ACTIVE);
	       pslider->screen_update = 1;
	       break;
	    }

	    
	}
    
    return  f;
}



static void slider_release( void* pCtrl )
{

    slider * pslider = (slider *) pCtrl;
	
	

    if(pslider->slider_icon.p)
    {
        my_free((void *)pslider->slider_icon.p);
    }

    if(pslider->timer)
    {
         xTimerDelete(pslider->timer,3);
    }
    my_free((void *)pslider);
	
    
}




static void slider_notify(slider * pslider)
{
     uint8_t *p;

    p = (uint8_t *)my_malloc((uint32_t)17);

    p[0]=0xee;
    p[1]=0xb1;
    p[2]=0x11;
    p[3]=(pslider->ctrl.id>>24);
    p[4]=(pslider->ctrl.id>>16)&0xff;
    p[5]=(pslider->ctrl.id>>8)&0xff;
    p[6]=(pslider->ctrl.id)&0xff;
    p[7]=pslider->tp_type;
    p[8]=(pslider->value>>24);
    p[9]=(pslider->value>>16)&0xff;
    p[10]=(pslider->value>>8)&0xff;
    p[11]=(pslider->value)&0xff;
    p[12]=0xff;
    p[13]=0xfc;
    p[14]=0xff;
    p[15]=0xff;
    p[16]=0x3b;

    cmd_dev.notify_cmd(p,17);

    my_free((void *) p);
}






static void slider_act(base_touch *p,uint8_t flag)
{
    if(p->type != CFG_SLI)
    {
        return ;
    }

    slider * pslider = (slider *) p;
    
    if(flag == 0)
    {
        if(pslider->notify == 1)
        {
            slider_notify(pslider);
        }
    }
    else
    {
        if(pslider->notify == 0)
        {
            slider_notify(pslider);
        }
    }
    
    
    
}








static void slider_update_touch(slider * pslider,uint16_t pos)
{

    if(pslider->rect == touch_vernier)
    {
        if(pslider->slider_dir == 0)
        {
            if(pslider->slider_class==0)
            {
                pslider->touch.top = pslider->tp_rect.top + pslider->vernier_offset;
                pslider->touch.bottom = pslider->tp_rect.top + pslider->vernier_height+pslider->vernier_offset;
            }
            else
            {
                pslider->touch.top = pslider->tp_rect.top + ((pslider->tp_rect.bottom - pslider->tp_rect.top) - 
                pslider->vernier_height)/2;

                pslider->touch.bottom = pslider->tp_rect.top +((pslider->tp_rect.bottom - 
                pslider->tp_rect.top)+pslider->vernier_height)/2;
            }

            pslider->touch.left = pos - pslider->vernier_width / 2;
            pslider->touch.right = pos + pslider->vernier_width / 2;
        }
        else
        {
            if(pslider->slider_class == 0)  
            {
                pslider->touch.left = pslider->tp_rect.left + pslider->vernier_offset;
                pslider->touch.right = pslider->tp_rect.left + pslider->vernier_offset + pslider->vernier_height;
            }
            else
            {
                pslider->touch.left = pslider->tp_rect.left+((pslider->tp_rect.right-pslider->tp_rect.left) - 
                pslider->vernier_height)/2;
                pslider->touch.right = 
                pslider->tp_rect.left+((pslider->tp_rect.right-pslider->tp_rect.left)+pslider->vernier_height)/2;
            }
            pslider->touch.top = pos - pslider->vernier_width / 2;
            pslider->touch.bottom = pos + pslider->vernier_width /2;
        
        }
    }
    
    
}





static uint16_t count_slider_pos(slider * pslider,uint8_t flag)
{
    float value;
    uint16_t pos;

    if(flag)
    {
        value = (float)(pslider->value - pslider->start_value) / (float)(pslider->end_value - pslider->start_value);
    }
    else
    {
        value = (float)(pslider->old_value - pslider->start_value) / (float)(pslider->end_value - pslider->start_value
);
    }
    if(value < 0)
    {
        value = 0;
    }
    else if(value > 1)
    {
        value = 1;
    }

    if(pslider->slider_dir == 0)
    {
        pos = (uint16_t)((pslider->tp_rect.left + 
        pslider->start_margin)+value*((pslider->tp_rect.right-pslider->end_margin)-(pslider->tp_rect.left+pslider->start_margin)));
    }
    else if(pslider->slider_dir == 1)
    {
        pos = (uint16_t)((pslider->tp_rect.bottom-pslider->start_margin)-value*((pslider->tp_rect.bottom-pslider->start_margin)-(pslider->tp_rect.top+pslider->end_margin)));
    }

    return pos;
}






static void slider_draw_screen(base_touch * p)
{
    uint16_t pos;
    t_rect rect;

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
    
    
    if(p->type != CFG_SLI)
    {
        return;
    }

    
    slider * pslider = (slider *) p;
    pos = count_slider_pos(pslider, 1);
    //vblt_bdc(disp_dev.disp[1],pslider->back_image);

    
    if(pslider->slider_dir == 0)
    {
        rect.left = pos - pslider->vernier_width / 2;
        
        if(pslider->slider_class == 0)
        {
            rect.top = pslider->tp_rect.top + pslider->vernier_offset;
        }
        else
        {
            rect.top = pslider->tp_rect.top + ((pslider->tp_rect.bottom - pslider->tp_rect.top) - 
                pslider->vernier_height)/2;
        }
        
    }
    else
    {
        if(pslider->slider_class == 0)
        {
            rect.left = pslider->tp_rect.left + pslider->vernier_offset;
        }
        else
        {
            rect.left = pslider->tp_rect.left+((pslider->tp_rect.right-pslider->tp_rect.left) - 
                    pslider->vernier_height)/2;
        }
        rect.top = pos - pslider->vernier_height / 2;
    }
    rect.right = rect.left + pslider->vernier_width - 1;
    rect.bottom = rect.top + pslider->vernier_width - 1;
	
	get_icon_image(&(pslider->slider_icon),0);
    show_icon_image_mix(pslider->slider_icon,rect,dp);
    
		
	if(pslider->rect == touch_vernier)
    {
        slider_update_touch(pslider,pos);
    }
    

    
    
}





static void cmd_update_slider_status(struct slider * pslider,uint8_t * buf,uint16_t len)
{
    if((pslider->tp_type==CFG_SLI)&&(len==4))
    {
        uint8_t * pv;
        uint32_t value;
      
        value = (buf[0]<<24)|(buf[1]<<16)|(buf[2]<<8)|(buf[3]);
        if(value < pslider->start_value)
        {
            pslider->value = pslider->start_value;
        }
        else if(value > pslider->end_value)
        {
            pslider->value = pslider->end_value;
        }
        else
        {
            pslider->value = value;
        }
        pv = pslider->pvalue;

        pv[0] = buf[0];
        pv[1] = buf[1];
        pv[2] = buf[2];
        pv[3] = buf[3];
        pslider->screen_update = 1;
        
        
    }
    
}







static void cmd_get_slider_status(struct slider * pslider)
{
    if(pslider->tp_type == CFG_SLI)
    {
        uint8_t * buf;
       
        
        buf = (uint8_t *)my_malloc(17);

        buf[0] = 0xee;
        buf[1] = 0xb1;
        buf[2] = 0x11;
        buf[3] = (pslider->ctrl.id >> 24)&(0XFF);
        buf[4] = (pslider->ctrl.id >> 16)&(0XFF);
        buf[5] = (pslider->ctrl.id >> 8)&(0XFF);
        buf[6] = (pslider->ctrl.id)&(0XFF);
        buf[7] = pslider->tp_type;
        buf[8] = (pslider->value>>24)&0xff;
        buf[9] = (pslider->value>>16)&0xff;
        buf[10] = (pslider->value>>8)&0xff;
        buf[11] = (pslider->value)&0xff;
        buf[12] = 0xff;
        buf[13] = 0xfc;
        buf[14] = 0xff;
        buf[15] = 0xff;
        buf[16] = 0x3b;
        cmd_dev.notify_cmd(buf,17);
        my_free(buf);
    }
}


static void slider_touch_back(TimerHandle_t timer)
{
	slider * pslider = pvTimerGetTimerID(timer);
	update_slider_value(pslider,g_x, g_y);
	pslider->screen_update = 1; //更新界面

	

}










uint8_t slider_create(uint8_t * buffer)
{
    slider * pslider = (slider *) my_malloc(sizeof(slider));

    if(pslider == NULL)
    {
        return 1;
    }
    
    uint32_t addr;
    memset((void*)pslider,0,sizeof(slider));
    pslider->ctrl.id = ((buffer[0]<<24)|(buffer[1]<<16)|(buffer[4]<<8)|(buffer[5])<<0);
    pslider->ctrl.func_ctrl_procmsg = slider_msg;
    pslider->ctrl.func_ctrl_release = slider_release;
    

    pslider->tp_type = buffer[6];
    pslider->tp_rect.left = (buffer[7]<<8) | (buffer[8]);
    pslider->tp_rect.top = (buffer[9]<<8) | (buffer[10]);
    pslider->tp_rect.right = (buffer[11]<<8) | (buffer[12]);
    pslider->tp_rect.bottom = (buffer[13]<<8) | (buffer[14]);
    pslider->pic_id = (buffer[2]<<8) | (buffer[3]);
    pslider->pic_on = (buffer[15]<<8) | (buffer[16]);

    pslider->act = slider_act;
    pslider->draw_screen = slider_draw_screen;
    //pslider->update_screen = slider_update_screen;
    pslider->cmd_update_status = cmd_update_slider_status;
    pslider->cmd_get_status = cmd_get_slider_status;
    pslider->screen_update = 0;

    pslider->rect = (touch_rect)buffer[17];
    pslider->notify = buffer[18];
    pslider->slider_dir = buffer[19];
    pslider->slider_class = buffer[20];
    pslider->start_margin = (buffer[21]<<8)|(buffer[22]);
    pslider->end_margin = (buffer[23]<<8) | (buffer[24]);
    pslider->vernier_offset = (buffer[25]<<8) | (buffer[26]);
    pslider->vernier_width = (buffer[27]<<8) | (buffer[28]);
    pslider->vernier_height = (buffer[29]<<8) | (buffer[30]);

    

    //pslider->back_image.x = pslider->tp_rect.left;
    //pslider->back_image.y = pslider->tp_rect.top;
    //pslider->back_image.width = pslider->vernier_width;
    //pslider->back_image.height = pslider->vernier_height;
    //pslider->back_image.vdc = disp_dev.disp[2];

   
    pslider->start_value = (buffer[31]<<24)|(buffer[32]<<16)|(buffer[33]<<8)|(buffer[34]);
    pslider->end_value = (buffer[35]<<24)|(buffer[36]<<16)|(buffer[37]<<8)|(buffer[38]);
    pslider->value = (buffer[39]<<24)|(buffer[40]<<16)|(buffer[41]<<8)|(buffer[42]);
    pslider->pvalue = &buffer[39];

    if(pslider->rect == touch_scale)
    {
    
        if(pslider->slider_dir == 0)
        {
            if(pslider->slider_class==0)
            {
                pslider->touch.top = pslider->tp_rect.top + pslider->vernier_offset;
                pslider->touch.bottom = pslider->tp_rect.top + pslider->vernier_height+pslider->vernier_offset;
             }
             else
             {
                pslider->touch.top = pslider->tp_rect.top + ((pslider->tp_rect.bottom - pslider->tp_rect.top) - 
                pslider->vernier_height)/2;
    
                pslider->touch.bottom = pslider->tp_rect.top +((pslider->tp_rect.bottom - 
                pslider->tp_rect.top)+pslider->vernier_height)/2;
              }
    
              pslider->touch.left = pslider->tp_rect.left + pslider->start_margin;
              pslider->touch.right = pslider->tp_rect.right - pslider->end_margin;
          }
          else
          {
                if(pslider->slider_class == 0)  
                {
                    pslider->touch.left = pslider->tp_rect.left + pslider->vernier_offset;
                    pslider->touch.right = pslider->tp_rect.left + pslider->vernier_offset + pslider->vernier_height;
                }
                else
                {
                    pslider->touch.left = pslider->tp_rect.left+((pslider->tp_rect.right-pslider->tp_rect.left) - 
                    pslider->vernier_height)/2;
                    pslider->touch.right = 
                    pslider->tp_rect.left+((pslider->tp_rect.right-pslider->tp_rect.left)+pslider->vernier_height)/2;
                }
                pslider->touch.top = pslider->tp_rect.top + pslider->start_margin;
                pslider->touch.bottom = pslider->tp_rect.bottom - pslider->end_margin;
            
          }
            
        }
        
        addr = (buffer[43]<<24)|(buffer[44]<<16)|(buffer[45]<<8)|(buffer[46]);
        init_icon(&pslider->slider_icon, addr);

        pslider->timer = xTimerCreate((const char *) "slider",
								(TickType_t)50*MS,
								(UBaseType_t)pdTRUE,
								(void*	)pslider,
								(TimerCallbackFunction_t)slider_touch_back);
								

		
		ctrl_init((t_ctrl*) pslider);
    
    return 0;
    
    
}






t_ctrl * sslider_create(uint8_t * buffer)
{
    slider * pslider = (slider *) my_malloc(sizeof(slider));

    if(pslider == NULL)
    {
        return NULL;
    }

    memset((void*)pslider,0,sizeof(slider));
    pslider->ctrl.id = ((buffer[0]<<24)|(buffer[1]<<16)|(buffer[4]<<8)|(buffer[5])<<0);
    //pslider->ctrl.func_ctrl_procmsg = slider_msg;
    pslider->ctrl.func_ctrl_release = slider_release;
    

    pslider->tp_type = buffer[6];
    pslider->tp_rect.left = 0xffff;
    pslider->tp_rect.top = 0xffff;
    pslider->tp_rect.right = 0xffff;
    pslider->tp_rect.bottom = 0xffff;
    pslider->pic_id = (buffer[2]<<8) | (buffer[3]);
    pslider->pic_on = 0xffff;

    //pslider->act = slider_act;
    //pslider->init_screen = slider_init_screen;
    //pslider->update_screen = slider_update_screen;
    pslider->cmd_update_status = cmd_update_slider_status;
    pslider->cmd_get_status = cmd_get_slider_status;
    //pslider->update_screen = 0;

    //pslider->rect = (touch_rect)buffer[17];
    //pslider->notify = buffer[18];
    //pslider->slider_dir = buffer[19];
    //pslider->slider_class = buffer[20];
    //pslider->start_margin = (buffer[21]<<8)|(buffer[22]);
    //pslider->end_margin = (buffer[23]<<8) | (buffer[24]);
    //pslider->vernier_offset = (buffer[25]<<8) | (buffer[26]);
    //pslider->vernier_width = (buffer[27]<<8) | (buffer[28]);
    //pslider->vernier_height = (buffer[29]<<8) | (buffer[30]);

    //pslider->back_image.x = pslider->tp_rect.left;
    //pslider->back_image.y = pslider->tp_rect.top;
    //pslider->back_image.width = pslider->tp_rect.right - pslider->tp_rect.left+1;
    //pslider->back_image.height = pslider->tp_rect.bottom - pslider->tp_rect.top+1;
    //pslider->back_image.vdc = disp_dev.disp[2];

    /*

    if(pslider->slider_dir == 0)
    {
        pslider->vernier_image.width = pslider->vernier_width;
        pslider->vernier_image.height = pslider->tp_rect.bottom - pslider->tp_rect.top + 1;
        pslider->vernier_image.x = pslider->tp_rect.left + pslider->start_margin - pslider->vernier_width/2;
        pslider->vernier_image.y = pslider->tp_rect.top;
       
    }
    else if(pslider->slider_dir)
    {
        pslider->vernier_image.width = pslider->tp_rect.right - pslider->tp_rect.left;
        pslider->vernier_image.height = pslider->vernier_width;
        pslider->vernier_image.x = pslider->tp_rect.left;
        pslider->vernier_image.y = pslider->tp_rect.bottom - pslider->start_margin -  pslider->vernier_width / 2;
    }

     pslider->vernier_image.p = (uint8_t *)my_malloc(pslider->vernier_image.height*pslider->vernier_image.width*2);
     taskENTER_CRITICAL(); 
     get_touch_image(pslider->vernier_image, (base_touch *) pslider, 0);
     taskEXIT_CRITICAL();
     */
    

    pslider->start_value = (buffer[31]<<24)|(buffer[32]<<16)|(buffer[33]<<8)|(buffer[34]);
    pslider->end_value = (buffer[35]<<24)|(buffer[36]<<16)|(buffer[37]<<8)|(buffer[38]);
    pslider->value = (buffer[39]<<24)|(buffer[40]<<16)|(buffer[41]<<8)|(buffer[42]);
    pslider->pvalue = &buffer[39];

    /*
    if(pslider->rect == touch_scale)
    {
    
        if(pslider->slider_dir == 0)
        {
            if(pslider->slider_class==0)
            {
                pslider->touch.top = pslider->tp_rect.top + pslider->vernier_offset;
                pslider->touch.bottom = pslider->tp_rect.top + pslider->vernier_height+pslider->vernier_offset;
             }
             else
             {
                pslider->touch.top = pslider->tp_rect.top + ((pslider->tp_rect.bottom - pslider->tp_rect.top) - 
                pslider->vernier_height)/2;
    
                pslider->touch.bottom = pslider->tp_rect.top +((pslider->tp_rect.bottom - 
                pslider->tp_rect.top)+pslider->vernier_height)/2;
              }
    
              pslider->touch.left = pslider->tp_rect.left + pslider->start_margin;
              pslider->touch.right = pslider->tp_rect.right - pslider->end_margin;
          }
          else
          {
                if(pslider->slider_class == 0)  
                {
                    pslider->touch.left = pslider->tp_rect.left + pslider->vernier_offset;
                    pslider->touch.right = pslider->tp_rect.left + pslider->vernier_offset + pslider->vernier_height;
                }
                else
                {
                    pslider->touch.left = pslider->tp_rect.left+((pslider->tp_rect.right-pslider->tp_rect.left) - 
                    pslider->vernier_height)/2;
                    pslider->touch.right = 
                    pslider->tp_rect.left+((pslider->tp_rect.right-pslider->tp_rect.left)+pslider->vernier_height)/2;
                }
                pslider->touch.top = pslider->tp_rect.top + pslider->start_margin;
                pslider->touch.bottom = pslider->tp_rect.bottom - pslider->end_margin;
            
          }
            
        }
        */
		
//    
	ctrl_init((t_ctrl*) pslider);
    
    return (t_ctrl *) pslider;
    
    
}




/***************************************************************************************
* End of File: slider.c

****************************************************************************************/
