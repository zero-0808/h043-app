
/***************************************************************************************
****************************************************************************************
* Project      : 20180525_HMI
* FILE         : progress.c
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

#include "progress.h"
#include "mem_my.h"
#include "cmd.h"
#include "cfg.h"
#include "display.h"
#include "string.h"


static  float count_bar_value(uint16_t start,uint16_t end,uint16_t t)
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




static void update_bar_value(prog_bar * p_bar,uint16_t x,uint16_t y)
{
    float value;
    float dec;
	uint8_t * p;
    
    switch(p_bar->dir)
    {
        case h_lelft:
            value = count_bar_value(p_bar->tp_rect.left,p_bar->tp_rect.right,x);
            break;
        case h_right:
            value = count_bar_value(p_bar->tp_rect.right,p_bar->tp_rect.left,x);
            break;
        case v_top:
            value = count_bar_value(p_bar->tp_rect.top, p_bar->tp_rect.bottom,y);
            break;
        case v_bottom:
            value = count_bar_value(p_bar->tp_rect.bottom, p_bar->tp_rect.top,y);
            break;
       default:
            break;
    }
    value =(p_bar->end - p_bar->start)*value;
    p_bar->value = (uint32_t)value;
    dec = value - p_bar->value;
    if(dec > 0.5)
    {
        p_bar->value += 1;
    }
    p_bar->value += p_bar->start;
	p = (uint8_t *)p_bar->pvalue;
    p[0] = (p_bar->value >> 24)&0xff;
	p[1] = (p_bar->value >> 16)&0xff;
	p[2] = (p_bar->value >> 8)&0xff;
	p[3] = (p_bar->value)&0xff;
	p_bar->screen_update = 1;
}





static uint8_t prog_bar_msg( void * p, t_msg * p_msg )
{
  uint16_t x,y,flag,f=1;
  t_msg msg;

	if(p_msg->msg != MSG_TP)
	{
		return f;
	}

	
	prog_bar * p_bar = (prog_bar *)p;
	
	if(p_bar->notiy == no_notiy || p_bar->ctrl.active == 0)
	{
		return f;
	}

	x = p_msg->a;
	y = p_msg->b;
	flag = p_msg->c;

	if((flag != 1) && !GETBIT( p_bar->state, BTN_STATE_ACTIVE))
	{
		return f;
	}

	if(pt_in_rect(&p_bar->tp_rect, x, y))
	{
		f = 0;
	}

	switch(flag)
	{
	    case 1:
	        if(!f)
	        {
	            SETBIT( p_bar->state, BTN_STATE_ACTIVE);
	            msg.msg = MSG_TOUCH;
							msg.a   = (p_bar->ctrl.id >> 16) & (0xffff);
							msg.b   = (p_bar->ctrl.id) & (0xffff);
							msg.c   = 1;
							msg_dev.write_message((void *)&msg,0);
							update_bar_value(p_bar,x,y);
			 
			}
	        break;
	    case 2:
	    msg.msg = MSG_TOUCH;
			msg.a   = (p_bar->ctrl.id >> 16) & (0xffff);
			msg.b   = (p_bar->ctrl.id) & (0xffff);
			msg.c   = 2;
		  msg_dev.write_message((void *)&msg,0);
		  update_bar_value(p_bar,x,y);
	        break;
	   case 0:
	        msg.msg = MSG_TOUCH;
					msg.a   = (p_bar->ctrl.id >> 16) & (0xffff);
					msg.b   = (p_bar->ctrl.id) & (0xffff);
					msg.c   = 0;
					msg_dev.write_message((void *)&msg,0);
			
					update_bar_value(p_bar,x,y);
			
			
					CLRBIT( p_bar->state, BTN_STATE_ACTIVE);
	   
	}
	
	return f;
    
}




static void prog_bar_release( void* pCtrl )
{

    prog_bar * p_bar = ( prog_bar *) pCtrl;

    if(p_bar->fore_image.p)
    {
        my_free((void *)p_bar->fore_image.p);
    }
//    if(p_bar->back_image.p)
//    {
//        my_free((void *)p_bar->back_image.p);
//    }
    my_free(p_bar);
    
}


static void prog_bar_notify(prog_bar *pbar)
{
    uint8_t *p;

    p = (uint8_t *)my_malloc((uint32_t)17);

    p[0]=0xee;
    p[1]=0xb1;
    p[2]=0x11;
    p[3]=(pbar->ctrl.id>>24);
    p[4]=(pbar->ctrl.id>>16)&0xff;
    p[5]=(pbar->ctrl.id>>8)&0xff;
    p[6]=(pbar->ctrl.id)&0xff;
    p[7]=pbar->tp_type;
    p[8]=(pbar->value>>24);
    p[9]=(pbar->value>>16)&0xff;
    p[10]=(pbar->value>>8)&0xff;
    p[11]=(pbar->value)&0xff;
    p[12]=0xff;
    p[13]=0xfc;
    p[14]=0xff;
    p[15]=0xff;
    p[16]=0x3b;

    cmd_dev.notify_cmd(p,17);

    my_free((void *) p);
    
}



static void prog_bar_act(base_touch *p,uint8_t flag)
{
    if(p->type != CFG_BAR)
    {
        return;
    }
    prog_bar *pbar = (prog_bar *)  p;
    
    if(flag==1)
    {
        if(pbar->notiy == press_notiy)
        {
            prog_bar_notify(pbar);
        }   
        
    }
    else if(flag==2)
    {
        if(pbar->notiy == press_notiy)
        {
            prog_bar_notify(pbar);
        }   
            
    }
    else
    {
        if(pbar->notiy==release_notiy)
        {
             prog_bar_notify(pbar);
        }
    }
}



static uint16_t count_bar_pos(prog_bar *pbar,uint8_t flag)
{
    float value;
    uint16_t pos;
    if(flag)
    {
        value = (float)(pbar->value - pbar->start) / (float)(pbar->end - pbar->start);
    }
    else
    {
        value = (float)(pbar->old_value - pbar->start) / (float)(pbar->end - pbar->start);
    }
    if(value < 0)
    {
        value = 0;
    }
    if(value > 1)
    {
        value = 1;
    }
    switch(pbar->dir)
    {
        case h_lelft:
            pos = (uint16_t) pbar->tp_rect.left + value*(pbar->tp_rect.right - pbar->tp_rect.left); 
            break;
        case h_right:
            pos = (uint16_t) pbar->tp_rect.right - value*(pbar->tp_rect.right - pbar->tp_rect.left);
            break;
       case v_top:
            pos = (uint16_t) pbar->tp_rect.top + value*(pbar->tp_rect.bottom-pbar->tp_rect.top);
            break;
       case v_bottom:
            pos=(uint16_t)pbar->tp_rect.bottom-value*(pbar->tp_rect.bottom-pbar->tp_rect.top);
            break;
       default :
            pos = 0;
            break;
       
            
    }

    return pos;
    
    
}




static void init_progress(prog_bar *pbar)
{
    uint16_t pos;
    uint16_t x,y;
	t_rect rect;
	dc ddc;

   
    pos = count_bar_pos(pbar,1);

    x  = pbar->tp_rect.left;
    y  = pbar->tp_rect.top;


    if(pbar->dir == h_lelft || pbar->dir == h_right)
    {
        rect.top = 0;
		rect.bottom  = pbar->fore_image.height - 1;
        
        if(pbar->dir == h_lelft)
        {
            rect.left = 0;
            rect.right = pos - pbar->tp_rect.left;
        }
        else
        {
			x = pos;
            rect.left = pos - pbar->tp_rect.left;
            rect.right = pbar->fore_image.width - 1;
        }
    }

    else if(pbar->dir == v_bottom || pbar->dir == v_top)
    {
        rect.left = 0;
        rect.right = pbar->fore_image.width - 1;

        if(pbar->dir == v_top)
        {
			rect.top = 0;
            rect.bottom = pos - pbar->tp_rect.top;
        }
        else
        {
			y = pos;
            rect.top = pos - pbar->tp_rect.top;
            rect.bottom = pbar->fore_image.height - 1;
        }

        
    }

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


    vblt(ddc,x,y,rect,pbar->fore_image);

    
}



static uint8_t number_to_ascii(uint8_t num,uint8_t *p)
{
    if(num / 100)
    {
        p[0] = '1';
        p[1] = '0';
        p[2] = '0';
        return 3;
    }
    else if(num / 10)
    {
        p[0] = num / 10 + '0';
        p[1] = num % 10 + '0';
        return 2;
    }
    else
    {
        p[0] = num + '0';
        return 1;
    }
}








static void bar_show_font(prog_bar * pbar)
{
    uint32_t percent;
    float value,dec;
    uint8_t num,i;
    uint8_t width,height;
    uint16_t wds;
    uint16_t x,y;
	font_type * pfont, *ch_font;
	
    
	
	
	if(pbar->ptfont == NULL)
	{
		return;
	}
	
	pfont = pbar->ptfont;
	
	
    
    value = (float)(pbar->value-pbar->start)/(float)(pbar->end-pbar->start);
    
    if(value > 1)
    {
        value = 1;
    }
    value *= 100;
    percent = value;
    dec=value-percent;
    if(dec > 0.5)
    {
        percent += 1;
    }

    ch_font = hz_to_ch(pfont);
	
	
    
    if(ch_font)
    {
        uint8_t p[4];
		height = get_font_height(*ch_font);
		width = get_font_width(*ch_font);
        num = number_to_ascii(percent, p);
        if(pbar->percent)
        {
            p[num] = '%';
			num ++;
        }
        
        wds = num * width; 
        if(pbar->font_pos==show_center)
        {
            y=(uint16_t)((short)pbar->tp_rect.top + (short)(pbar->tp_rect.bottom - pbar->tp_rect.top)/2 - 
            (short)height/2);
            x=(uint16_t)((short)pbar->tp_rect.left + (short)(pbar->tp_rect.right-pbar->tp_rect.left)/2-(short)wds/2);
            
        }
        else if(pbar->font_pos==show_end)
        {
            uint16_t pos;
            pos = count_bar_pos(pbar,1);
            
            if(pbar->dir == h_lelft)
            {
               y = (uint16_t)((short)pbar->tp_rect.top + (short)(pbar->tp_rect.bottom - pbar->tp_rect.top)/2 - 
               (short)height/2);
               x = pos;
               if(x+wds > pbar->tp_rect.right)
               {
                    x = pbar->tp_rect.right - wds;
               }
            }
            else if(pbar->dir == h_right)
            {
                 y = (uint16_t)((short)pbar->tp_rect.top + (short)(pbar->tp_rect.bottom - pbar->tp_rect.top)/2 - (short)height/2);
                 if(pos - pbar->tp_rect.left < wds)
                 {
                    x = pbar->tp_rect.left;
                 }
                 else
                 {
                    x = pos - wds;
                 }
            }
            else if(pbar->dir == v_top)
            {
                x = (uint16_t)((short)pbar->tp_rect.left + (short)(pbar->tp_rect.right-pbar->tp_rect.left)/2-(short)
wds/2);
                y = pos;
                if(y+height > pbar->tp_rect.bottom)
                {
                    y = pbar->tp_rect.bottom - height;
                }
            }
            else if(pbar->dir == v_bottom) 
            {
                x = (uint16_t)((short)pbar->tp_rect.left + (short)(pbar->tp_rect.right-pbar->tp_rect.left)/2-(short)
wds/2);
                if(pos - pbar->tp_rect.top < height)
                {
                    y = pbar->tp_rect.top;
                }
                else
                {
                    y = pos - height;
                }
            }
        }

        for(i=0;i<num;i++)
        {
            show_charn_font(x+i*width,y,(uint16_t)pbar->font_color,p[i],*ch_font);
        }
    }
    
}




static void bar_draw_screen(base_touch * p)
{
    
    if(p->type != CFG_BAR)
    {
        return;
    }
    prog_bar * pbar = (prog_bar *)p;

    
    init_progress(pbar);
    
    if(pbar->font_pos != show_blank)
    {
        bar_show_font(pbar);

    }
      
    
}



static void cmd_update_bar_status(struct prog_bar * pbar,uint8_t * buf,uint16_t len)
{

	
    if((pbar->tp_type==CFG_BAR)&&(len==4))
    {
        uint8_t * pv;
        uint32_t value;
       
        value = (buf[0]<<24)|(buf[1]<<16)|(buf[2]<<8)|(buf[3]);
		
        if(value < pbar->start)
        {
            pbar->value = pbar->start;
        }
        else if(value > pbar->end)
        {
            pbar->value = pbar->end;
        }
        else
        {
            pbar->value = value;
        }
        pbar->screen_update = 1;
        pv = pbar->pvalue;
		
        pv[0] = buf[0];
        pv[1] = buf[1];
        pv[2] = buf[2];
        pv[3] = buf[3];
		
		
        
        
    }
    
}





static void cmd_get_bar_status(struct prog_bar * pbar)
{
	
    if(pbar->tp_type == CFG_BAR)
    {
        uint8_t * buf;
        
        
        buf = (uint8_t *)my_malloc(17);

        buf[0] = 0xee;
        buf[1] = 0xb1;
        buf[2] = 0x11;
        buf[3] = (pbar->ctrl.id >> 24)&(0XFF);
        buf[4] = (pbar->ctrl.id >> 16)&(0XFF);
        buf[5] = (pbar->ctrl.id >> 8)&(0XFF);
        buf[6] = (pbar->ctrl.id)&(0XFF);
        buf[7] = pbar->tp_type;
        buf[8] = (pbar->value>>24)&0xff;
        buf[9] = (pbar->value>>16)&0xff;
        buf[10] = (pbar->value>>8)&0xff;
        buf[11] = (pbar->value)&0xff;
        buf[12] = 0xff;
        buf[13] = 0xfc;
        buf[14] = 0xff;
        buf[15] = 0xff;
        buf[16] = 0x3b;
        cmd_dev.notify_cmd(buf,17);
        my_free(buf);
    }
	
}












uint8_t prog_bar_create(uint8_t * buffer)
{
  
	prog_bar * pbar  = (prog_bar *)my_malloc(sizeof(prog_bar));

	
	if(pbar == NULL)
	{
		return 1;
	}

//	p = (uint16_t *)disp_dev.get_addr(2);
	
	memset((void*)pbar,0,sizeof(prog_bar));
	pbar->ctrl.id = ((buffer[0] << 24) | (buffer[1] << 16) | (buffer[4] << 8) | (buffer[5] << 0));
	pbar->ctrl.func_ctrl_procmsg  = prog_bar_msg;
	pbar->ctrl.func_ctrl_release  = prog_bar_release;
//	ctrl_init((t_ctrl*) pbar);
	
	pbar->tp_type = buffer[6];
	pbar->tp_rect.left = ((buffer[7] << 8) | (buffer[8]));
	pbar->tp_rect.top= ((buffer[9] << 8) | buffer[10]);
	pbar->tp_rect.right =((buffer[11] << 8) | (buffer[12]));
	pbar->tp_rect.bottom = ((buffer[13] << 8) | (buffer[14]));
	pbar->pic_id   = (buffer[2] << 8) | (buffer[3]);
	pbar->pic_on   = ((buffer[15] << 8) | (buffer[16]));
	pbar->act = prog_bar_act;
	pbar->draw_screen = bar_draw_screen;
	pbar->screen_update = 0;
	pbar->cmd_update_status = cmd_update_bar_status;
	pbar->cmd_get_status = cmd_get_bar_status;
	//pbar->update_screen = 0;

	pbar->dir = (bar_dir)buffer[17];
	pbar->fore_color_type = (color_type)buffer[18];
	pbar->fore_color = (buffer[19] << 8)|(buffer[20]);
//	pbar->fore_image.x = pbar->tp_rect.left;
//	pbar->fore_image.y = pbar->tp_rect.top;
	pbar->fore_image.width = pbar->tp_rect.right - pbar->tp_rect.left + 1;
	pbar->fore_image.height = pbar->tp_rect.bottom - pbar->tp_rect.top + 1;
	pbar->fore_image.p = (uint8_t *)my_malloc(pbar->fore_image.width*pbar->fore_image.height*disp_dev.bpp);
	//taskENTER_CRITICAL(); 
	get_touch_image(pbar->fore_image, (base_touch *) pbar, 0);
	//taskEXIT_CRITICAL();

	pbar->back_color_type = (color_type)buffer[21];
	pbar->back_color = (buffer[22] << 8) | (buffer[23]);
	//pbar->back_image.x = pbar->tp_rect.left;
	//pbar->back_image.y = pbar->tp_rect.top;
	//pbar->back_image.width = pbar->tp_rect.right - pbar->tp_rect.left + 1;
	//pbar->back_image.height = pbar->tp_rect.bottom - pbar->tp_rect.top + 1;
	//pbar->back_image.vdc  = disp_dev.disp[2];
//	pbar->back_image.p = (uint8_t *)my_malloc(pbar->back_image.width*pbar->back_image.height*disp_dev.bpp);
//	taskENTER_CRITICAL(); 
//	get_touch_image(pbar->back_image, (base_touch *) pbar, 0);
//	taskEXIT_CRITICAL();
	

	pbar->notiy = (notiy_type)buffer[24];
	pbar->ptfont = get_font_type(buffer[25]);
	pbar->font_color = (buffer[26]<<8)|(buffer[27]);
	pbar->percent = buffer[28];
	pbar->font_pos = (show_type)buffer[29];

	

    if(pbar->ptfont && pbar->font_pos != show_blank)
    {
        uint16_t height,width,wds;
		font_type * pfont;
		
		pfont = pbar->ptfont;
        height = (uint16_t)get_font_height(*pfont);
        width = (uint16_t)get_font_width(*pfont);
		
		if(pfont->code > 0)
		{
			width /= 2;
		}

        if(pbar->percent)
        {
            wds = width * 4;
        }
        else
        {
            wds = width * 3;
        }

        if(pbar->dir==h_lelft || pbar->dir==h_right)
        {

            if(height>(pbar->tp_rect.bottom - pbar->tp_rect.top))//越界不显示字符
            {
                pbar->font_pos = show_blank;      
            }
            if(pbar->font_pos == show_end)
            {
                if(wds > (pbar->tp_rect.right - pbar->tp_rect.left))
                {
                
                    pbar->font_pos = show_blank;
                }
            }
       }
       else if(pbar->dir==v_bottom || pbar->dir == v_top)
       {
            if(wds>(pbar->tp_rect.right - pbar->tp_rect.left))
            {
                pbar->font_pos = show_blank; 
            }
            if(pbar->font_pos == show_end)
            {
                if(height > (pbar->tp_rect.bottom - pbar->tp_rect.top))
                {
                    pbar->font_pos = show_blank;
                }
            }
       }

     
    }
    
	

	
	pbar->start = (buffer[30]<<24)|(buffer[31]<<16)|(buffer[32]<<8)|(buffer[33]);
	pbar->end = (buffer[34]<<24)|(buffer[35]<<16)|(buffer[36]<<8)|(buffer[37]);
	pbar->value = (buffer[38]<<24)|(buffer[39]<<16)|(buffer[40]<<8)|(buffer[41]);
	pbar->old_value = pbar->value;
	pbar->pvalue = &buffer[38];
	
	ctrl_init((t_ctrl*) pbar);
	
	return 0;
	
	
}





t_ctrl * sprog_bar_create(uint8_t * buffer)
{
  
	prog_bar * pbar  = (prog_bar *)my_malloc(sizeof(prog_bar));

	
	if(pbar == NULL)
	{
		return NULL;
	}

//	p = (uint16_t *)disp_dev.get_addr(2);
	
	memset((void*)pbar,0,sizeof(prog_bar));
	pbar->ctrl.id = ((buffer[0] << 24) | (buffer[1] << 16) | (buffer[4] << 8) | (buffer[5] << 0));
	pbar->ctrl.func_ctrl_procmsg  = prog_bar_msg;
	pbar->ctrl.func_ctrl_release  = prog_bar_release;
//	ctrl_init((t_ctrl*) pbar);
	
	pbar->tp_type = buffer[6];
	//pbar->tp_rect.left = ((buffer[7] << 8) | (buffer[8]));
	//pbar->tp_rect.top= ((buffer[9] << 8) | buffer[10]);
	//pbar->tp_rect.right =((buffer[11] << 8) | (buffer[12]));
	//pbar->tp_rect.bottom = ((buffer[13] << 8) | (buffer[14]));
	pbar->pic_id   = (buffer[2] << 8) | (buffer[3]);
	pbar->pic_on   = ((buffer[15] << 8) | (buffer[16]));
	//pbar->act = prog_bar_act;
	//pbar->draw_screen = bar_draw_screen;
	pbar->screen_update = 0;
	pbar->cmd_update_status = cmd_update_bar_status;
	pbar->cmd_get_status = cmd_get_bar_status;
	//pbar->update_screen = 0;

	//pbar->dir = (bar_dir)buffer[17];
	//pbar->fore_color_type = (color_type)buffer[18];
	//pbar->fore_color = (buffer[19] << 8)|(buffer[20]);

	//pbar->fore_image.width = pbar->tp_rect.right - pbar->tp_rect.left + 1;
	//pbar->fore_image.height = pbar->tp_rect.bottom - pbar->tp_rect.top + 1;
	//pbar->fore_image.p = (uint8_t *)my_malloc(pbar->fore_image.width*pbar->fore_image.height*disp_dev.bpp);
	//taskENTER_CRITICAL(); 
	//get_touch_image(pbar->fore_image, (base_touch *) pbar, 0);
	//taskEXIT_CRITICAL();

	//pbar->back_color_type = (color_type)buffer[21];
	//pbar->back_color = (buffer[22] << 8) | (buffer[23]);
	//pbar->back_image.x = pbar->tp_rect.left;
	//pbar->back_image.y = pbar->tp_rect.top;
	//pbar->back_image.width = pbar->tp_rect.right - pbar->tp_rect.left + 1;
	//pbar->back_image.height = pbar->tp_rect.bottom - pbar->tp_rect.top + 1;
	//pbar->back_image.vdc  = disp_dev.disp[2];
//	pbar->back_image.p = (uint8_t *)my_malloc(pbar->back_image.width*pbar->back_image.height*disp_dev.bpp);
//	taskENTER_CRITICAL(); 
//	get_touch_image(pbar->back_image, (base_touch *) pbar, 0);
//	taskEXIT_CRITICAL();
	

	//pbar->notiy = (notiy_type)buffer[24];
	//pbar->font = (font_type)buffer[25];
	//pbar->font_color = (buffer[26]<<8)|(buffer[27]);
	//pbar->percent = buffer[28];
	//pbar->font_pos = (show_type)buffer[29];

	/*

    if(pbar->font <= gbk24 && pbar->font_pos != show_blank)
    {
        uint16_t height,width,wds;

        height = (uint16_t)get_font_height(pbar->font);
        width = (uint16_t)get_font_width(pbar->font);

        if(pbar->percent)
        {
            wds = width * 4;
        }
        else
        {
            wds = width * 3;
        }

        if(pbar->dir==h_lelft || pbar->dir==h_right)
        {

            if(height>(pbar->tp_rect.bottom - pbar->tp_rect.top))//越界不显示字符
            {
                pbar->font_pos = show_blank;      
            }
            if(pbar->font_pos == show_end)
            {
                if(wds > (pbar->tp_rect.right - pbar->tp_rect.left))
                {
                
                    pbar->font_pos = show_blank;
                }
            }
       }
       else if(pbar->dir==v_bottom || pbar->dir == v_top)
       {
            if(wds>(pbar->tp_rect.right - pbar->tp_rect.left))
            {
                pbar->font_pos = show_blank; 
            }
            if(pbar->font_pos == show_end)
            {
                if(height > (pbar->tp_rect.bottom - pbar->tp_rect.top))
                {
                    pbar->font_pos = show_blank;
                }
            }
       }

     
    }

    */
	

	
	pbar->start = (buffer[30]<<24)|(buffer[31]<<16)|(buffer[32]<<8)|(buffer[33]);
	pbar->end = (buffer[34]<<24)|(buffer[35]<<16)|(buffer[36]<<8)|(buffer[37]);
	pbar->value = (buffer[38]<<24)|(buffer[39]<<16)|(buffer[40]<<8)|(buffer[41]);
	pbar->old_value = pbar->value;
	pbar->pvalue = &buffer[38];
	
	ctrl_init((t_ctrl*) pbar);
	
	return ((t_ctrl *) pbar);
	
	
}




/***************************************************************************************
* End of File: progress.c

****************************************************************************************/
