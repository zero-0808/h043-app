
/***************************************************************************************
****************************************************************************************
* Project      : 20180525_HMI
* FILE         : keyboard.c
* Description  : 
*                      
* Copyright (c) 2018 by ShenZhen JingHua Displays Co.,Ltd... All Rights Reserved.
* 
* History:
* Version		name			Date			Description
* 0.01			Qu Bingbing		2018/05/29		Initial Version
* 
****************************************************************************************
****************************************************************************************/
#include "keyboard.h"
#include "cfg.h"
#include "display.h"
#include "btn.h"
#include "textbox.h"

//#define HSMALL_KEYBOARD_WIDTH         (320)
//#define HSMALL_KEYBOARD_HEIGHT        (240)
//#define VSMALL_KEYBOARD_WIDTH         (240)
//#define VSMALL_KEYBOARD_HEIGHT        (320)



keyboard  small_keyboard,full_keyboard;
keyboard_info kb_info;
extern uint8_t shift_value;
extern uint8_t engch_value;





void system_keyboard_init(void)
{
    small_keyboard.active = 0;
    full_keyboard.active = 0;
    kb_info.sem = xSemaphoreCreateBinary();

}

static uint8_t pop_keyboard_msg(void *p, t_msg * p_msg)
{
    return 1;
}

static void pop_keyboard_release( void *p_ctrl )
{
    pop_keyboard * pboard = (pop_keyboard *) p_ctrl;

    if(pboard->image.p )
    {
        my_free(pboard->image.p);
    }
    
    my_free(p_ctrl);
}


static void pop_keyboard_act(base_touch * p_btn, uint8_t flag)
{
    return;
}


static void keyboard_draw_screen(base_touch * tp)
{
    if(tp->type == 0xFF)
    {
        dc ddc;
        pop_keyboard * pboard = (pop_keyboard *) tp; 
        uint16_t x,y;
        t_rect rect;
        x = pboard->tp_rect.left;
	    y = pboard->tp_rect.top;
	    rect.left = 0;
	    rect.right = pboard->image.width;
	    rect.top = 0;
		rect.bottom = pboard->image.height;

		 if(pboard->image.p == NULL)
        {
            pboard->image.p = (uint8_t *)my_malloc(pboard->image.width * pboard->image.height * disp_dev.bpp);
            get_image(pboard->image,0,0, pboard->pic_id);
            
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
    	
		vblt(ddc,x,y,rect,pboard->image);
    }
}





uint8_t * screen_touch_addrss(uint16_t screen)
{
	uint8_t * p;
	uint16_t pic_id;
	uint16_t len;

	
	
	p = src_cfg.p_touch;

	while(1)
	{
		len = (p[0] << 8) | p[1];
		if(len == 0)
		{
			break;
		}
		pic_id = (p[4] << 8) | p[5];
		

		if(pic_id == screen)
		{
			return p;
		}

		if(pic_id > screen )
		{
			return NULL;
		}

		p += len;

		
	}
	
	return NULL;
}


static void keyboard_touch_create(uint16_t x,uint16_t y,uint16_t screen)
{
	
	uint8_t *p = NULL;
	uint8_t  type;
	uint16_t pic_id;
	uint16_t len;
	

	p = screen_touch_addrss(screen);
	
	if(p == NULL)
	{
		return;
	}
	
	while(1)
	{
	
		len = (p[0] << 8) | p[1];
		
		if(len == 0)
		{
			return;
		}
		
		pic_id = (p[4] << 8) | p[5];
		
		if(pic_id != screen)
		{
			return;
		}
		
		type = p[8];

		if(type == CFG_BTN)
		{	
			t_btn * pbtn;
			uint32_t id;
			
		    keyboard_btn_create(x, y, &p[2]);
			id = (p[2]<<24) | (p[3]<<16) | (p[6]<<8) | (p[7]);
			pbtn = (t_btn *) get_ctrl(id);
			
			if(pbtn->use == switch_btn)
			{
				btn_switch * psw = (btn_switch *) pbtn->tp_use;
				psw->status = 0;
				psw->old_status = 0;
			}
			
			
		}
		else if(type == CFG_TEX)
		{
		    uint32_t id;
		    textbox * pbox;
		    keyboard_textbox_create(x, y,&p[2]);
		    id = (p[2]<<24) | (p[3]<<16) | (p[6]<<8) | (p[7]);
		    pbox = (textbox*)get_ctrl(id);
			if(old_active_box->text_show == 0)
			{
				pbox->text.font.password = old_active_box->text.font.password;
				
			}
			
		    if(old_active_box->input.keyboard.init_value)
		    {
				taskENTER_CRITICAL();
		        memcpy(pbox->code,old_active_box->code,old_active_box->code_num);
				taskEXIT_CRITICAL();
				pbox->code_num = old_active_box->code_num;
		        
		    }

		    
			pbox->code_update = 1;
			pbox->act((base_touch *)pbox,0);
			
			
		    
		    
		}

		
		

		p += len;
		
	}
	
}


void hide_keyboard(uint8_t flag)
{
    if((small_keyboard.active == 0) && (full_keyboard.active == 0))
    {
        return;
    }

    if(flag)
    {
        copy_to_box(old_active_box,active_box->code,active_box->code_num);
    }

    old_active_box->code_update = 1;
	old_active_box->status = 0;
	old_active_box->screen_update = 1;
	old_active_box = NULL;
	active_box = NULL;
	
    if(small_keyboard.active)
    {
        small_keyboard.active = 0;		
		ctrl_release_screen(small_keyboard.screen_id);
		
    }
    else
    {
        full_keyboard.active = 0;
        ctrl_release_screen(full_keyboard.screen_id);
    }

    active_ctrl();
    
	engch_value = 0;
	shift_value = 0;


    
}






/*
void pop_up_keyboard(uint16_t x,uint16_t y,uint16_t flag)
{
    if(flag != 1)
    {
        return;
    }
	
	uint16_t width,height;

    uint16_t screen_id;

    deactive_ctrl();

    

    pop_keyboard * pboard = NULL;

    pboard = my_malloc(sizeof(pop_keyboard));

    if(pboard == NULL)
    {
        return ;
    }
    memset(pboard,0,sizeof(pop_keyboard));

    deactive_ctrl();


    if(disp_dev.dir == 0)
    {
        if(src_cfg.ptscreen->HV_switch)
        {
            screen_id = src_cfg.ptscreen->screen_num - 2;
        }
        else
        {
            screen_id = src_cfg.ptscreen->screen_num - 1;
        }
		
		width = src_cfg.ptscreen->pkb_size->shwkb;
		height = src_cfg.ptscreen->pkb_size->shhkb;
    }
    else
    {
        screen_id = src_cfg.ptscreen->screen_num - 1;
		width = src_cfg.ptscreen->pkb_size->svwkb;
		height = src_cfg.ptscreen->pkb_size->svhkb;
    }
	
	if(x + width > disp_dev.width)
	{
		x = disp_dev.width - width;
	}
	if(y + height > disp_dev.height)
	{
		y = disp_dev.height - height;
	}


    small_keyboard.screen_id = screen_id;   
    small_keyboard.active = 1;
    pboard->ctrl.id = (screen_id << 16) | 0xFFFF;
    pboard->ctrl.func_ctrl_procmsg  = pop_keyboard_msg;
    pboard->ctrl.func_ctrl_release  = pop_keyboard_release;
    pboard->screen_update = 1;

    pboard->type = 0xff;
    pboard->tp_rect.left = x;
    pboard->tp_rect.top = y;
    pboard->pic_id = get_pic_num(screen_id);
    if(disp_dev.dir == 0)
    {
        pboard->tp_rect.right = x + src_cfg.ptscreen->pkb_size->shwkb - 1;
        pboard->tp_rect.bottom = y + src_cfg.ptscreen->pkb_size->shhkb - 1;
        pboard->image.height = src_cfg.ptscreen->pkb_size->shhkb;
        pboard->image.width =src_cfg.ptscreen->pkb_size->shwkb;
    }
    else
    {
        pboard->tp_rect.right = x + src_cfg.ptscreen->pkb_size->svwkb - 1;
        pboard->tp_rect.bottom = y + src_cfg.ptscreen->pkb_size->svhkb - 1;
        pboard->image.height = src_cfg.ptscreen->pkb_size->svhkb;
        pboard->image.width = src_cfg.ptscreen->pkb_size->svwkb;
    }
	small_keyboard.height = pboard->image.height;
	small_keyboard.width = pboard->image.width;
    pboard->pic_on   = -1;
    pboard->act = pop_keyboard_act;
    pboard->draw_screen = keyboard_draw_screen;
    pboard->image.p = NULL;

   
    
    ctrl_init((t_ctrl *)pboard);
    keyboard_touch_create(x,y,pboard->pic_id);
	
   

    
}

*/










void pop_up_keyboard(uint16_t x,uint16_t y,uint16_t flag)
{
    if((flag != 1) && (flag != 2))
    {
        return;
    }


    uint16_t width,height;

    uint16_t screen_id;

    deactive_ctrl();

    

    pop_keyboard * pboard = NULL;

    pboard = my_malloc(sizeof(pop_keyboard));

    if(pboard == NULL)
    {
        return ;
    }
    memset(pboard,0,sizeof(pop_keyboard));

    deactive_ctrl();

    if(flag == 1)
    {
        if(disp_dev.dir == 0)
        {
    		screen_id = src_cfg.ptscreen->pkb_size->sh_num;
    		width = src_cfg.ptscreen->pkb_size->shwkb;
    		height = src_cfg.ptscreen->pkb_size->shhkb;
            
        }
        else
        {
            screen_id = src_cfg.ptscreen->pkb_size->sv_num;
		    width = src_cfg.ptscreen->pkb_size->svwkb;
		    height = src_cfg.ptscreen->pkb_size->svhkb;
        }

        if(x + width > disp_dev.width)
    	{
    		x = disp_dev.width - width;
    	}
    	if(y + height > disp_dev.height)
    	{
    		y = disp_dev.height - height;
    	}

    	small_keyboard.screen_id = screen_id;   
        small_keyboard.active = 1;
        pboard->ctrl.id = (screen_id << 16) | 0xFFFF;
        pboard->ctrl.func_ctrl_procmsg  = pop_keyboard_msg;
        pboard->ctrl.func_ctrl_release  = pop_keyboard_release;
        pboard->screen_update = 1;

        pboard->type = 0xff;
        pboard->tp_rect.left = x;
        pboard->tp_rect.top = y;
        pboard->pic_id = get_pic_num(screen_id);



		pboard->tp_rect.right = x + width - 1;
		pboard->tp_rect.bottom = y + height - 1;
		pboard->image.height = height;
		pboard->image.width = width;

        pboard->pic_on   = -1;
        pboard->act = pop_keyboard_act;
        pboard->draw_screen = keyboard_draw_screen;
        pboard->image.p = NULL;

       
        
        ctrl_init((t_ctrl *)pboard);
        keyboard_touch_create(x,y,pboard->pic_id);

        
    }
    else
    {

        if(disp_dev.dir == 0)
        {
    		screen_id = src_cfg.ptscreen->pkb_size->mh_num;
    		width = src_cfg.ptscreen->pkb_size->mhwkb;
    		height = src_cfg.ptscreen->pkb_size->mhhkb;
            
        }
        else
        {
            screen_id = src_cfg.ptscreen->pkb_size->mv_num;
		    width = src_cfg.ptscreen->pkb_size->mvwkb;
		    height = src_cfg.ptscreen->pkb_size->mvhkb;
        }

        x = (disp_dev.width - width) / 2;
        y = (disp_dev.height - height) / 2;

        full_keyboard.screen_id = screen_id;   
        full_keyboard.active = 1;
        pboard->ctrl.id = (screen_id << 16) | 0xFFFF;
        pboard->ctrl.func_ctrl_procmsg  = pop_keyboard_msg;
        pboard->ctrl.func_ctrl_release  = pop_keyboard_release;
        pboard->screen_update = 1;

        pboard->type = 0xff;
        pboard->tp_rect.left = x;
        pboard->tp_rect.top = y;
        pboard->pic_id = get_pic_num(screen_id);

        pboard->tp_rect.right = x + width - 1;
		pboard->tp_rect.bottom = y + height - 1;
		pboard->image.height = height;
		pboard->image.width = width;
		
        pboard->pic_on   = -1;
        pboard->act = pop_keyboard_act;
        pboard->draw_screen = keyboard_draw_screen;
        pboard->image.p = NULL;

       
        
        ctrl_init((t_ctrl *)pboard);
        keyboard_touch_create(x,y,pboard->pic_id);
        
    }

    
}






void keyboard_task(void *pvParameters)
{
    while(1)
    {
		
        xSemaphoreTake(kb_info.sem,portMAX_DELAY);

        if(kb_info.status)
        {
           pop_up_keyboard(kb_info.x,kb_info.y,kb_info.keyboard_num);    
        }
        else
        {
            hide_keyboard(kb_info.flag);
        }
    }
}







/***************************************************************************************
* End of File: keyboard.c

****************************************************************************************/
