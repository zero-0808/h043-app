
/***************************************************************************************
****************************************************************************************
* Project      : 171124_HMI
* FILE         : cmd_func.c
* Description  : 
*                      
* Copyright (c) 2017 by ShenZhen JingHua Displays Co.,Ltd... All Rights Reserved.
* 
* History:
* Version		name			Date			Description
* 0.01			Qu Bingbing		2017/11/25		Initial Version
* 
****************************************************************************************
****************************************************************************************/
#include "cmd.h"
#include "cmd_func.h"
#include "cfg.h"
#include "mem_my.h"
#include "gui.h"
#include "btn.h"
#include "progress.h"
#include "rtc.h"
//#include "sys.h"
//#include "keyboard.h"
#include "slider.h"
#include "cmd_func.h"

#include "string.h"
#include "textbox.h"
#include "animation.h"
#include "icon_touch.h"
#include "menu.h"
#include "graph.h"
#include "language.h"
#include "buzzer.h"
#include "timer.h"
#include "select.h"
#include "display.h"
#include "pop_win.h"
#include "record.h"
#include "orientation.h"
#include "led.h"












static uint8_t cmd_handshake(uint8_t *p,uint16_t len);
static uint8_t cmd_reset(uint8_t *p,uint16_t len);
static uint8_t cmd_ctrl(uint8_t *p,uint16_t len);
static uint8_t ctrl_switch_screen(uint8_t * p,uint16_t len);
static uint8_t ctrl_get_screen(uint8_t *p,uint16_t len);
static uint8_t ctrl_change_status(uint8_t * p,uint16_t len);
static uint8_t ctrl_get_status(uint8_t *p,uint16_t len);
static uint8_t ctrl_start_rtc(uint8_t * p, uint16_t len);
static uint8_t ctrl_set_rtc(uint8_t *p,uint16_t len);
static uint8_t ctrl_stop_rtc(uint8_t *p,uint16_t len);
static uint8_t ctrl_clear_rtc(uint8_t * p, uint16_t len);
static uint8_t cmd_engch(uint8_t *p,uint16_t len);
static uint8_t cmd_shift(uint8_t *p,uint16_t len);
static t_ctrl * create_ctrl(uint32_t id);
static uint8_t cmd_set_rtc(uint8_t *p,uint16_t len);
static uint8_t cmd_get_rtc(uint8_t *p,uint16_t len);
static uint8_t start_play_animation(uint8_t * p,uint16_t len);
static uint8_t stop_play_animation(uint8_t * p,uint16_t len);
static uint8_t pause_play_animation(uint8_t * p,uint16_t len);
static uint8_t last_frame(uint8_t * p,uint16_t len);
static uint8_t next_frame(uint8_t * p,uint16_t len);
static uint8_t num_frame(uint8_t *p,uint16_t len);
//static uint8_t cmd_change_language(uint8_t *p,uint16_t len);
static uint8_t ctrl_menu(uint8_t *p,uint16_t len);
static uint8_t add_graph_channel(uint8_t *p,uint16_t len);
static uint8_t del_graph_channel(uint8_t *p,uint16_t len);
static uint8_t insert_data_tail(uint8_t *p,uint16_t len);
static uint8_t clear_graph_channel(uint8_t *p,uint16_t len);
static uint8_t insert_data_head(uint8_t *p,uint16_t len);
static uint8_t cmd_set_language(uint8_t *p,uint16_t len);
static uint8_t change_strid(uint8_t *p,uint16_t len);
static uint8_t cmd_backlight_adjust(uint8_t *p,uint16_t len);
static uint8_t cmd_get_dir(uint8_t *p,uint16_t len);
static uint8_t cmd_buzzer_mode(uint8_t *p,uint16_t len);
static uint8_t cmd_tp_test(uint8_t *p,uint16_t len);
static uint8_t ctrl_batch(uint8_t *p,uint16_t len);
static uint8_t cmd_set_light(uint8_t *p,uint16_t len);
static uint8_t ctrl_pop_win(uint8_t *p,uint16_t len);
static uint8_t ctrl_hide_win(uint8_t *p,uint16_t len);
static uint8_t record_trigger_warn(uint8_t *p,uint16_t len);
static uint8_t ctrl_add_record(uint8_t *p,uint16_t len);
static uint8_t record_relieve_warn(uint8_t *p,uint16_t len);
static uint8_t record_clear(uint8_t *p,uint16_t len);
static uint8_t record_set_offset(uint8_t *p,uint16_t len);
static uint8_t get_record_num(uint8_t *p,uint16_t len);
static uint8_t cmd_bk_auto(uint8_t *p,uint16_t len);

#if LED_SUPPORT
static uint8_t cmd_led_mode(uint8_t *p,uint16_t len);
#endif

#if DIR_SWITCH_SUPPORT
static uint8_t cmd_screen_mode(uint8_t * p,uint16_t len);
#endif






extern uint8_t switch_screen_flag;
extern uint8_t shift_value;
extern uint8_t engch_value;
extern void open_backlight(void);
extern void close_backlight(void);





static  const cmd_id_func cmd_func_table[MAX_CMD_NUM] = 
{
    {0X04,cmd_handshake},
	{0X07,cmd_reset},
	{0x60,cmd_set_light},
	{0X81,cmd_set_rtc},
	{0X82,cmd_get_rtc},
	{0X83,cmd_set_language},
	{0XA0,cmd_engch},
	{0XA1,cmd_shift},
	{0XB1,cmd_ctrl},
	{0x61,cmd_backlight_adjust},
	{0x62,cmd_get_dir},
	{0x65,cmd_buzzer_mode},
	{0x63,cmd_tp_test},
	{0x64,cmd_bk_auto},     //自动背光调节--》显示可休眠
#if LED_SUPPORT
	{0x48,cmd_led_mode},
#endif
	
#if DIR_SWITCH_SUPPORT
	{0x49,cmd_screen_mode},	
#endif

};




static const cmd_id_func  ctrl_func_table[MAX_CTRL_NUM] = 
{
    {0X00,ctrl_switch_screen},
	{0X01,ctrl_get_screen},
	{0X10,ctrl_change_status},
	{0X11,ctrl_get_status},
	{0x12,ctrl_batch},
	{0x13,ctrl_menu},
	{0X20,start_play_animation},
	{0X21,stop_play_animation},
	{0X22,pause_play_animation},
    {0X23,num_frame},
	{0X24,last_frame},
	{0X25,next_frame},
	{0X30,add_graph_channel},
	{0X31,del_graph_channel},
    {0X32,insert_data_tail},
	{0X33,clear_graph_channel},
	{0X35,insert_data_head},
	{0X37,change_strid},
	{0X40,ctrl_set_rtc},
    {0X42,ctrl_clear_rtc},
	{0X41,ctrl_start_rtc},
	{0X44,ctrl_stop_rtc},
	{0x50,record_trigger_warn},
	{0x51,record_relieve_warn},
    {0x52,ctrl_add_record},
	{0x53,record_clear},
	{0x54,record_set_offset},
	{0x55,get_record_num},
	{0x80,ctrl_pop_win},
	{0x81,ctrl_hide_win},
};



static uint8_t cmd_backlight_adjust(uint8_t *p,uint16_t len)
{
	if(len != 1)
	{
		return 1;
	}
	
	switch(p[0])
	{
		case 0:
			if(g_project->light > 10)
			{
				g_project->light -= 15;
			}
			break;
		case 1:
			if(g_project->light < 100)
			{
				g_project->light += 15;
			}
			break;
		case 2:
			disp_dev.close_backlight();
			delay_ms(1000);
			g_project->light = 100;
			disp_dev.open_backlight();
			break;
		default:
			return 1;
			
	}
	
	return 0;
}



static uint8_t ctrl_add_record(uint8_t *p,uint16_t len)
{
    if(len < 4)
    {
        return 1;
    }
    

    uint32_t id;
	uint8_t flag = 0;
    

    id = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | (p[3]);
    

    data_record * ptrecord = (data_record *) get_ctrl(id);

    if(ptrecord == NULL)
    {
		flag = 1;
		ptrecord = (data_record *) create_ctrl(id);
    }

    if(ptrecord)
    {
		if(ptrecord->type == CFG_REC)
		{
			ptrecord->cmd_handler_add(ptrecord,&p[4],len-4);
		}
		if(flag)
		{
			release_ctrl((t_ctrl *) ptrecord);
			return 1;
		}
		
    }
    else
    {
        return 1;
    }

    return 0;
}




static uint8_t record_trigger_warn(uint8_t *p,uint16_t len)
{
    if(len != 6 && len != 14)
    {
        return 1;
    }

    uint32_t id;
	uint8_t flag = 0;
    

    id = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | (p[3]);
    

    data_record * ptrecord = (data_record *) get_ctrl(id);

    if(ptrecord == NULL)
    {
	   flag = 1;
       ptrecord = (data_record *)create_ctrl(id);
    }

    if(ptrecord)
    {
		if(ptrecord->type == CFG_REC)
		{
			ptrecord->cmd_handler_add(ptrecord,&p[4],len-4);
		}
		if(flag)
		{
			release_ctrl((t_ctrl *) ptrecord);
			return 1;
		}
    }
    else
    {
        return 1;
    }

    return 0;
}


static uint8_t record_clear(uint8_t *p,uint16_t len)
{
    if(len != 4)
    {
        return 1;
    }

    uint32_t id;
	uint8_t flag = 0;
    

    id = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | (p[3]);
    

    data_record * ptrecord = (data_record *) get_ctrl(id);

    if(ptrecord == NULL)
    {
	   flag = 1;
       ptrecord = (data_record *)create_ctrl(id);
    }

    if(ptrecord)
    {
		if(ptrecord->type == CFG_REC)
		{
			ptrecord->cmd_handler_clear(ptrecord,&p[4],len-4);
		}
		if(flag)
		{
			release_ctrl((t_ctrl *) ptrecord);
			return 1;
		}
    }
    else
    {
        return 1;
    }

    return 0;
}


static uint8_t get_record_num(uint8_t *p,uint16_t len)
{
    if(len != 4)
    {
        return 1;
    }

    uint32_t id;
	uint8_t  flag = 0;
    

    id = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | (p[3]);
    

    data_record * ptrecord = (data_record *) get_ctrl(id);

    if(ptrecord == NULL)
    {
		flag = 1;
		ptrecord = (data_record *)create_ctrl(id);
    }

    if(ptrecord)
    {
		if(ptrecord->type == CFG_REC)
		{
			ptrecord->cmd_handler_get(ptrecord,&p[4],len-4);
		}
		if(flag)
		{
			release_ctrl((t_ctrl *) ptrecord);
			return 1;
		}
    }
    else
    {
        return 1;
    }

    return 0;
}



static uint8_t record_set_offset(uint8_t *p,uint16_t len)
{
    if(len != 6)
    {
        return 1;
    }

    uint32_t id;
	uint8_t flag = 0;
    

    id = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | (p[3]);
    

    data_record * ptrecord = (data_record *) get_ctrl(id);

    if(ptrecord == NULL)
    {
		flag = 1;
		ptrecord = (data_record *)create_ctrl(id);
    }

    if(ptrecord)
    {
		if(ptrecord->type == CFG_REC)
		{
			ptrecord->cmd_handler_offset(ptrecord,&p[4],len-4);
		}
		if(flag)
		{
			release_ctrl((t_ctrl *) ptrecord);
			return 1;
		}
    }
    else
    {
        return 1;
    }

    return 0;
}


static uint8_t record_relieve_warn(uint8_t *p,uint16_t len)
{
    if(len != 6 && len != 14)
    {
        return 1;
    }

    uint32_t id;
	uint8_t flag = 0;
    

    id = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | (p[3]);
    

    data_record * ptrecord = (data_record *) get_ctrl(id);

    if(ptrecord == NULL)
    {
		flag = 1;
		ptrecord = (data_record *)create_ctrl(id);
    }

    if(ptrecord)
    {
		if(ptrecord->type == CFG_REC)
		{
			if(ptrecord->record_type == 2)
			{
				ptrecord->cmd_handler_relieve(ptrecord,&p[4],len-4);
			}
			else
			{
				if(flag)
				{
					release_ctrl((t_ctrl *) ptrecord);
				}
				return 1;
				
			}
		}
		if(flag)
		{
			release_ctrl((t_ctrl *) ptrecord);
			return 1;
		}
		
        
    }
    else
    {
        return 1;
    }

    return 0;
}








static uint8_t cmd_get_dir(uint8_t *p,uint16_t len)
{
	
    uint8_t *buf;
    
    if(len)
    {
        return 1;
    }

    buf = (uint8_t *)my_malloc(8);
    buf[0] = 0xee;
    buf[1] = 0x62;
    buf[2] = disp_dev.dir;
    buf[3] = 0xff;
    buf[4] = 0xfc;
    buf[5] = 0xff;
    buf[6] = 0xff;
    buf[7] = 0x3b;

    cmd_dev.notify_cmd(buf,8);

    my_free((void *) buf);
		
	return 0;
}

static uint8_t cmd_buzzer_mode(uint8_t *p,uint16_t len)
{
	uint32_t uinitms = 0;
	if((len != 1) && (len != 5))
	{
		return 1;
	}

	if(len == 5)
	{
		uinitms = p[1];
		uinitms = (uinitms<< 8) | p[2];
		uinitms = (uinitms<< 8) | p[3];
		uinitms = (uinitms<< 8) | p[4];
		if(uinitms < 300)
		{
			uinitms = 300;
		}
	}
	set_buzzer_mode(p[0] , uinitms);

	return 0;
}

#if LED_SUPPORT



static uint8_t cmd_led_mode(uint8_t *p,uint16_t len)
{
	if(len != 1)
	{
		return 1;
	}

	set_led_mode(p[0]);
	return 0;
	
}


#endif


#if DIR_SWITCH_SUPPORT


static uint8_t cmd_screen_mode(uint8_t *p,uint16_t len)
{
	if(len != 1)
	{
		return 1;
	}

	if(src_cfg.ptscreen->screen_mode != p[0])
    {
        set_screen_mode(p[0]);
    }
	
	return 0;
}


#endif

static uint8_t cmd_tp_test(uint8_t *p,uint16_t len)
{
	if(len)
	{
		return 1;
	}
	extern void tp_test(void);
	tp_test();
	return 0;
}



/*==================================================================
* Function     : cmd_ctrl
* Description  : 
* Input Para   : 
* Output Para  : 
* Return Value : 
==================================================================*/
static uint8_t cmd_ctrl(uint8_t *p,uint16_t len)
{

    uint8_t * buf = NULL;
    short num;
    uint8_t res = 1;

    if(len < 1)
    {
        return 1;
    }

    for(num = 0;num < MAX_CTRL_NUM; num++)
    {
        if(p[0] == ctrl_func_table[num].cmd)//查找指令
        {
            break;
        }
    }

    if(num < MAX_CTRL_NUM)
    {
        if(ctrl_func_table[num].func)
        {
            if(len > 1)
            {
                buf = (uint8_t *)my_malloc(len-1);
                mem_copy(buf,&p[1],len-1);
            }

            res = ctrl_func_table[num].func(buf,len-1);

            if(buf)
            {
                my_free(buf);
            }
        }
    }

    return res;
    
}



/*==================================================================
* Function     : ctrl_set_rtc
* Description  : 
* Input Para   : 
* Output Para  : 
* Return Value : 
==================================================================*/
static uint8_t ctrl_set_rtc(uint8_t *p,uint16_t len)
{
	/*
    uint32_t id,value;
    base_touch * tp;
    rtc_tp * prtc;
    
    if(len != 8)
    {
       return 1; 
    }

    id = (p[0]<<24)|(p[1]<<16)|(p[2]<<8)|(p[3]);
    value = (p[4]<<24)|(p[5]<<16)|(p[6]<<8)|(p[7]);

    tp = (base_touch *)get_ctrl(id);

    if(tp == NULL)
    {
        return 1;
    }

    if(tp->type != CFG_RTC)
    {
        return 1;
    }

    prtc = (rtc_tp *) tp;

    if(prtc->format < 5 || prtc->format > 7)
    {
        return 1;
    }

    if(prtc->rtc_type.counter.start)
    {
        return 1;
    }

    prtc->rtc_type.counter.counter = value;
    prtc->rtc_type.counter.counter_value = 0;
    prtc->update_screen(tp,0);
	*/
    return 0;

    
}


/*==================================================================
* Function     : ctrl_stop_rtc
* Description  : 
* Input Para   : 
* Output Para  : 
* Return Value : 
==================================================================*/
static uint8_t ctrl_stop_rtc(uint8_t *p,uint16_t len)
{
	/*
    uint32_t id;
    
    base_touch * tp;
    rtc_tp * prtc;
    
    if(len != 4)
    {
       return 1; 
    }

    id = (p[0]<<24)|(p[1]<<16)|(p[2]<<8)|(p[3]);

    tp = (base_touch *)get_ctrl(id);

    if(tp == NULL)
    {
        return 1;
    }

    if(tp->type != CFG_RTC)
    {
        return 1;
    }

    prtc = (rtc_tp *) tp;

    if(prtc->format < 5 || prtc->format > 7)
    {
        return 1;
    }

    prtc->rtc_type.counter.start = 0;
	*/
	
    return 0;
    
}

/*==================================================================
* Function     : ctrl_clear_rtc
* Description  : 
* Input Para   : 
* Output Para  : 
* Return Value : 
==================================================================*/
static uint8_t ctrl_clear_rtc(uint8_t * p, uint16_t len)
{
   /* 
	uint32_t id;
    
    base_touch * tp;
    rtc_tp * prtc;
    
    if(len != 4)
    {
       return 1; 
    }

    id = (p[0]<<24)|(p[1]<<16)|(p[2]<<8)|(p[3]);

    tp = (base_touch *)get_ctrl(id);

    if(tp == NULL)
    {
        return 1;
    }

    if(tp->type != CFG_RTC)
    {
        return 1;
    }

    prtc = (rtc_tp *) tp;

    
    if(prtc->format < 5 || prtc->format > 7)
    {
        return 1;
    }

    prtc->rtc_type.counter.counter_value = 0;
    prtc->rtc_type.counter.start = 0;
	prtc->screen_update = 1;
	*/
    return 0;
    
}
/*==================================================================
* Function     : ctrl_start_rtc
* Description  : 
* Input Para   : 
* Output Para  : 
* Return Value : 
==================================================================*/
static uint8_t ctrl_start_rtc(uint8_t * p, uint16_t len)
{
	/*
    uint32_t id;
    
    base_touch * tp;
    rtc_tp * prtc;
    
    if(len != 4)
    {
       return 1; 
    }

    id = (p[0]<<24)|(p[1]<<16)|(p[2]<<8)|(p[3]);

    tp = (base_touch *)get_ctrl(id);

    if(tp == NULL)
    {
        return 1;
    }

    if(tp->type != CFG_RTC)
    {
        return 1;
    }

    prtc = (rtc_tp *) tp;

    if(prtc->format < 5 || prtc->format > 7)
    {
        return 1;
    }

    if(prtc->rtc_type.counter.start)
    {
        return 1;
    }

    prtc->rtc_type.counter.start = 1;
	//prtc->rtc_type.counter.counter_value = 0;
	*/
    return 0;
    
    
}

/*==================================================================
* Function     : ctrl_get_status
* Description  : 
* Input Para   : 
* Output Para  : 
* Return Value : 
==================================================================*/
static uint8_t ctrl_get_status(uint8_t *p,uint16_t len)
{
	
    uint32_t id;
    
    base_touch * tp;
	uint8_t flag = 0;

    

    if(len != 4)
    {
        return 1;
    }

    id = (p[0]<<24)|(p[1]<<16)|(p[2]<<8)|(p[3]);

    tp = (base_touch *)get_ctrl(id);

    if(tp == NULL)
    {
        tp = (base_touch *)create_ctrl(id);
		flag = 1;
    }

    if(tp == NULL)
    {
        return 1;
    } 

    if(tp->type == CFG_BTN)
    {
		t_btn * pbtn = (t_btn *) tp;
		xSemaphoreTake(pbtn->ctrl.mutex_sem,portMAX_DELAY);
		if(pbtn->cmd_get_status)
		{
			pbtn->cmd_get_status(pbtn);
		}
		xSemaphoreGive(pbtn->ctrl.mutex_sem);
    }
	else if(tp->type == CFG_TEX)
    {
		textbox * pbox = (textbox *) tp;
		xSemaphoreTake(pbox->ctrl.mutex_sem,portMAX_DELAY);
		if(pbox->cmd_get_status)
		{
			pbox->cmd_get_status(pbox);
		}
		xSemaphoreGive(pbox->ctrl.mutex_sem);
    }
	else if(tp->type == CFG_BAR)
    {
		prog_bar * pbar = (prog_bar *) tp;
		xSemaphoreTake(pbar->ctrl.mutex_sem,portMAX_DELAY);
		if(pbar->cmd_get_status)
		{
			pbar->cmd_get_status(pbar);
		}
		xSemaphoreGive(pbar->ctrl.mutex_sem);
    }
	else if(tp->type == CFG_SLI)
    {
		slider * pslider = (slider *) tp;
		xSemaphoreTake(pslider->ctrl.mutex_sem,portMAX_DELAY);
		if(pslider->cmd_get_status)
		{
			pslider->cmd_get_status(pslider);
		}
		xSemaphoreGive(pslider->ctrl.mutex_sem);
    }

    if(flag)
    {
        release_ctrl((t_ctrl *)  tp);
		flag = 0;
    }
        
      

    return 0;
}


static uint8_t add_graph_channel(uint8_t *p,uint16_t len)
{
    if(len != 7)
    {
        return 1;
    }
    uint32_t id;
    tgraph * pgraph = NULL;
    uint8_t flag = 0;

    id = (p[0]<<24)|(p[1]<<16)|(p[2]<<8)|(p[3]);
    pgraph = (tgraph *) get_ctrl(id);
    
    if(pgraph == NULL)
    {
        flag = 1;
        pgraph = (tgraph *)create_ctrl(id);
    }
    if(pgraph == NULL)
    {
        return 1;
    }

    if(pgraph->tp_type != CFG_GRA || p[4] > 7)
    {
        if(flag)
		{
			release_ctrl((t_ctrl *) pgraph);
		}
		return 1;
        
    }

   

    pgraph->pgraph->channel_sta |= (1 << p[4]);
    pgraph->pgraph->channel[p[4]].len  = 0;
    pgraph->pgraph->channel[p[4]].color = (p[5] << 8) | (p[6]);
    if(flag == 0)
    {
        pgraph->screen_update = 1;
    }
    if(flag == 1)
    {
        release_ctrl((t_ctrl *) pgraph);
    }
    return 0;
}




static uint8_t del_graph_channel(uint8_t *p,uint16_t len)
{
    if(len != 5)
    {
        return 1;
    }
    uint32_t id;
    tgraph * pgraph = NULL;
    uint8_t flag = 0;

    id = (p[0]<<24)|(p[1]<<16)|(p[2]<<8)|(p[3]);
    pgraph = (tgraph *) get_ctrl(id);
    
    if(pgraph == NULL)
    {
        flag = 1;
        pgraph = (tgraph *)create_ctrl(id);
    }
    if(pgraph == NULL)
    {
        return 1;
    }

    if(pgraph->tp_type != CFG_GRA || p[4] > 7)
    {
        if(flag)
		{
			release_ctrl((t_ctrl *) pgraph);
		}
		return 1;
        
    }

   
    pgraph->pgraph->channel_sta &= ~(1 << p[4]);
    if(pgraph->pgraph->channel[p[4]].len != 0 && flag == 0)
    {
        pgraph->screen_update = 1;
    }
    
    if(flag == 1)
    {
        release_ctrl((t_ctrl *) pgraph);
    }
    return 0;
}




static uint8_t insert_data_tail(uint8_t *p,uint16_t len)
{
    if(len < 8)
    {
        return 1;
    }
    uint32_t id,size,size1;
    tgraph * pgraph = NULL;
    uint8_t flag = 0,byte;
    uint8_t * buf;
    uint16_t len1;

    id = (p[0]<<24)|(p[1]<<16)|(p[2]<<8)|(p[3]);
    pgraph = (tgraph *) get_ctrl(id);
    
    if(pgraph == NULL)
    {
        flag = 1;
        pgraph = (tgraph *)create_ctrl(id);
    }
    if(pgraph == NULL)
    {
        return 1;
    }

    if(pgraph->tp_type != CFG_GRA || p[4] > 7 || (pgraph->pgraph->channel_sta&(1 << p[4])) == 0)
    {
        if(flag)
		{
			release_ctrl((t_ctrl *) pgraph);
		}
		return 1;
        
    }
	
	if(pgraph->pgraph->sample_depth == 0)
	{
		byte = 1;
	}
	else
	{
		byte = 2;
	}

    len1 = (p[5] << 8) | p[6];
	
	if(len - len1 != 7)
	{
		if(flag)
		{
			release_ctrl((t_ctrl *) pgraph);
		}
		return 1;
        
	}
	
    size1 = byte * pgraph->pgraph->channel[p[4]].len; 
    size =  size1 + len1;
    buf = my_malloc(size);
    memcpy(buf,pgraph->pgraph->channel[p[4]].sample,size1);
    memcpy(&buf[size1],&p[7],len1);

    len1 = size / byte;

    if(len1 > pgraph->pgraph->sample_num)
    {
		size = byte*pgraph->pgraph->sample_num;
        len1 = len1 - pgraph->pgraph->sample_num;
        memcpy(pgraph->pgraph->channel[p[4]].sample,&buf[len1*byte],size);
        pgraph->pgraph->channel[p[4]].len = pgraph->pgraph->sample_num;
    }
    else
    {
        memcpy(pgraph->pgraph->channel[p[4]].sample,buf,size);
        pgraph->pgraph->channel[p[4]].len = len1;
    }

    
    pgraph->screen_update = 1;

    my_free(buf);

    if(flag == 1)
    {
        release_ctrl((t_ctrl *) pgraph);
    }
    return 0;
}





static uint8_t clear_graph_channel(uint8_t *p,uint16_t len)
{
    if(len != 5)
    {
        return 1;
    }
    uint32_t id;
    tgraph * pgraph = NULL;
    uint8_t flag = 0;

    id = (p[0]<<24)|(p[1]<<16)|(p[2]<<8)|(p[3]);
    pgraph = (tgraph *) get_ctrl(id);
    
    if(pgraph == NULL)
    {
        flag = 1;
        pgraph = (tgraph *)create_ctrl(id);
    }
    if(pgraph == NULL)
    {
        return 1;
    }

    if(pgraph->tp_type != CFG_GRA || p[4] > 7 || (pgraph->pgraph->channel_sta & (1 << p[4])) == 0)
    {
        if(flag)
		{
			release_ctrl((t_ctrl *) pgraph);
		}
		return 1;
        
    }

   

    
    pgraph->pgraph->channel[p[4]].len  = 0;
    
    if(flag == 0)
    {
        pgraph->screen_update = 1;
    }
    if(flag == 1)
    {
        release_ctrl((t_ctrl *) pgraph);
    }
    return 0;
}








static uint8_t insert_data_head(uint8_t *p,uint16_t len)
{
    if(len < 8)
    {
        return 1;
    }
    uint32_t id,size,size1;
    tgraph * pgraph = NULL;
    uint8_t flag = 0,byte = 0;
    uint8_t * buf;
    uint16_t len1;

    id = (p[0]<<24)|(p[1]<<16)|(p[2]<<8)|(p[3]);
    pgraph = (tgraph *) get_ctrl(id);
    
    if(pgraph == NULL)
    {
        flag = 1;
        pgraph = (tgraph *)create_ctrl(id);
    }
    if(pgraph == NULL)
    {
        return 1;
    }
	
	if(pgraph->pgraph->sample_depth == 0)
	{
		byte = 1;
	}
	else
	{
		byte = 2;
	}

    if(pgraph->tp_type != CFG_GRA || p[4] > 7 || (pgraph->pgraph->channel_sta & (1 << p[4])) == 0)
    {
        if(flag)
		{
			release_ctrl((t_ctrl *) pgraph);
		}
		return 1;
        
    }
	
    len1 = (p[5] << 8) | p[6];
	if(len - len1 != 7)
	{
		if(flag)
		{
			release_ctrl((t_ctrl *) pgraph);
		}
		return 1;
        
	}
	
    size1 = byte * pgraph->pgraph->channel[p[4]].len; 
    size =  size1 + len1;
    buf = my_malloc(size);
    
    memcpy(buf,&p[7],len1);
    memcpy(&buf[len1],pgraph->pgraph->channel[p[4]].sample,size1);
    

    len1 = size / byte;
	
	xSemaphoreTake(pgraph->ctrl.mutex_sem,portMAX_DELAY);

    if(len1 > pgraph->pgraph->sample_num)
    {
        size = byte * pgraph->pgraph->sample_num * byte;
        len1 = len1 - pgraph->pgraph->sample_num;
        memcpy(pgraph->pgraph->channel[p[4]].sample,buf,size);
        pgraph->pgraph->channel[p[4]].len = pgraph->pgraph->sample_num;
    }
    else
    {
        memcpy(pgraph->pgraph->channel[p[4]].sample,buf,size);
        pgraph->pgraph->channel[p[4]].len = len1;
    }
	
	xSemaphoreGive(pgraph->ctrl.mutex_sem);

    my_free(buf);

    if(flag == 1)
    {
        release_ctrl((t_ctrl *) pgraph);
    }
	else
	{
		pgraph->screen_update = 1;
	}
	
    return 0;
}









static uint8_t change_strid(uint8_t *p,uint16_t len)
{
    if(len != 6)
    {
        return 1;
    }

    uint32_t id;
    str_touch * pstr = NULL;
    uint8_t flag = 0;
    
    id = (p[0]<<24)|(p[1]<<16)|(p[2]<<8)|(p[3]);
    pstr = (str_touch *) get_ctrl(id);
    
    if(pstr == NULL)
    {
        flag = 1;
        pstr = (str_touch *)create_ctrl(id);
    }
    if(pstr == NULL)
    {
        return 1;
    }

    if(pstr->tp_type != CFG_STR)
    {
        if(flag)
		{
			release_ctrl((t_ctrl *) pstr);
		}
		return 1;
        
    }

    
    pstr->pstr_id[0] = p[4];
    pstr->pstr_id[1] = p[5];

    if(flag == 1)
    {
        release_ctrl((t_ctrl *) pstr);
    }
    else
    {
        pstr->screen_update = 1;
    }
    return 0;
}


static uint8_t start_play_animation(uint8_t * p,uint16_t len)
{
	
    if(len != 4)
    {
        return  1;
    }

    uint32_t id  = (p[0]<<24)|(p[1]<<16)|(p[2]<<8)|(p[3]);
	uint8_t flag = 0;

    animation * panimation = (animation *) get_ctrl(id);
	
	if(panimation == NULL)
	{
		flag = 1;
		
		panimation = (animation *)create_ctrl(id);
	}
	
	if(panimation == NULL)
	{
		return 1;
	}
	
	if(panimation->type != CFG_ANI)
    {
		if(flag)
		{
			release_ctrl((t_ctrl *) panimation);
		}
		return 1;
    }
	
	if(*(panimation->auto_play) == 1)
    {
		return 0;
    }
	
	*(panimation->auto_play) = 1;

	if(*(panimation->init_show)&&(flag==0))
	{
		xTimerStart(panimation->timer,10);
	}
			
			
    
	
	 if(flag)
	 {
		 release_ctrl((t_ctrl *) panimation);
	 }
	 
	 
	 return 0;


}


static uint8_t stop_play_animation(uint8_t * p,uint16_t len)
{
	
    if(len != 4)
    {
        return 1;
    }

    uint32_t id  = (p[0]<<24)|(p[1]<<16)|(p[2]<<8)|(p[3]);
	uint8_t flag = 0;

    animation * panimation = (animation *) get_ctrl(id);
	
	if(panimation == NULL)
	{
		flag = 1;
		panimation = (animation *) create_ctrl(id);
	}
	
	if(panimation == NULL)
	{
		return 1;
	}
	
	if(panimation->type != CFG_ANI)
	{
		if(flag)
		{
			release_ctrl((t_ctrl *) panimation);
		}
		return 1;
	}
	
	panimation->frame_num[0] = 0;
    panimation->frame_num[1] = 0;
	
	panimation->times[0] = 0;
	panimation->times[1] = 0;   //停止复位播放次数
	
	if(*(panimation->init_show)&&(!flag))
    {
		panimation->screen_update = 1;
		
		if(*(panimation->auto_play)==1)
		{
			xTimerStop(panimation->timer,2);
		}
    }
	
//	*(panimation->auto_play) = (1 << 1);
	
	*(panimation->auto_play) = 0;
	
	
	
	panimation->screen_update = 1;
	
	if(flag)
	{
		release_ctrl((t_ctrl *) panimation);
	}
	
	
	return 0;

    
           
	
			
}




static uint8_t  pause_play_animation(uint8_t * p,uint16_t len)
{
	
    if(len != 4)
    {
        return 1;
    }

    uint32_t id  = (p[0]<<24)|(p[1]<<16)|(p[2]<<8)|(p[3]);
	uint8_t flag = 0;

    animation * panimation = (animation *) get_ctrl(id);
	
	if(panimation == NULL)
	{
		flag = 1;
		panimation = (animation *)create_ctrl(id);
	}
	
	if(panimation == NULL)
	{
		return 1;
	}
	
	if(panimation->type != CFG_ANI)
	{
		if(flag)
		{
			release_ctrl((t_ctrl *) panimation);
		}
		return 1;
	}
	if((*(panimation->auto_play))==1)
	{
		*(panimation->auto_play) = 0;
		
		if(*(panimation->init_show)&&(!flag))
		{
			 xTimerStop(panimation->timer,2);
		}
	}
	if(flag)
	{
		release_ctrl((t_ctrl *) panimation);
	}
	
	
	return 0;
	
	


    
}






static uint8_t next_frame(uint8_t * p,uint16_t len)
{

	
    
    if(len != 4)
    {
        return 1;
    }

    uint32_t id  = (p[0]<<24)|(p[1]<<16)|(p[2]<<8)|(p[3]);
	uint8_t flag = 0;
	uint16_t frame;

    base_touch * pt = (base_touch *) get_ctrl(id);
	
	if(pt == NULL)
	{
		flag = 1;
		pt = (base_touch *)create_ctrl(id);
	}
	
	if(pt == NULL)
	{
		return 1;
	}
	
	if((pt->type != CFG_ANI) && (pt->type != CFG_ICO))
	{
		if(flag)
		{
			release_ctrl((t_ctrl *) pt);
		}
		
		return 1;
	}
	
	if(pt->type == CFG_ANI)
	{
		animation * panimation = (animation *) pt;
		
		frame = (panimation->frame_num[0] << 8) | (panimation->frame_num[1]);
		frame += 1;
		if(frame >= panimation->animation_icon.num)
		{
			frame = 0;
		}
	
		panimation->frame_num[0] = frame >> 8;
		panimation->frame_num[1] = frame;
	
		if(*(panimation->init_show) && (!flag))
		{
			panimation->screen_update = 1;
		}
	}
	else if(pt->type == CFG_ICO)
	{
		icon_touch * picon = (icon_touch *) pt;
		
		frame = (picon->frame_num[0] << 8) | (picon->frame_num[1]);
		frame += 1;
		if(frame >= picon->touch_icon.num)
		{
			frame = 0;
		}
	
		picon->frame_num[0] = frame >> 8;
		picon->frame_num[1] = frame;
	
		if(*(picon->init_show) && (!flag))
		{
			picon->screen_update = 1;
		}
	}
	
	if(flag)
	{
		release_ctrl((t_ctrl *) pt);
	}
	
	
	
	return 0;

     
    
}






static uint8_t last_frame(uint8_t * p,uint16_t len)
{
	
    
    if(len != 4)
    {
        return 1;
    }

    uint32_t id  = (p[0]<<24)|(p[1]<<16)|(p[2]<<8)|(p[3]);

    base_touch * pt = (base_touch *) get_ctrl(id);
	uint8_t flag = 0;
	uint16_t frame;
	
	if(pt == NULL)
	{
		flag = 1;
		pt = (base_touch *)create_ctrl(id);
	}
	
	if(pt == NULL)
	{
		return 1;
	}
	
	if((pt->type != CFG_ANI) && (pt->type != CFG_ICO))
	{
		if(flag)
		{
			release_ctrl((t_ctrl *) pt);
		}
		
		return 1;
	}
	
	
	if(pt->type == CFG_ANI)
	{
		animation * panimation = (animation *) pt;
		frame = (panimation->frame_num[0] << 8) | (panimation->frame_num[1]);
	
		if(frame == 0)
		{
			frame = panimation->animation_icon.num;
		}
	
		frame -= 1;
		panimation->frame_num[0] = frame >> 8;
		panimation->frame_num[1] = frame;
	
		if(*(panimation->init_show) && (!flag))
		{
			panimation->screen_update = 1;
		}
	}
	else if(pt->type == CFG_ICO)
	{
		icon_touch * picon = (icon_touch *) pt;
		frame = (picon->frame_num[0] << 8) | (picon->frame_num[1]);
		
		if(frame == 0)
		{
			frame = picon->touch_icon.num;
		}
		
		frame -= 1;
		picon->frame_num[0] = frame >> 8;
		picon->frame_num[1] = frame;
		
		if(*(picon->init_show) && (!flag))
		{
			picon->screen_update = 1;
		}
	}
	
	if(flag)
	{
		release_ctrl((t_ctrl *) pt);
	}
	
	
	
	return 0;   
}



static uint8_t  num_frame(uint8_t *p,uint16_t len)
{
	
	
    if(len != 6)
    {
        return 1;
    }

    uint32_t id  = (p[0]<<24)|(p[1]<<16)|(p[2]<<8)|(p[3]);
	uint8_t flag = 0;
    uint16_t frame = (p[4]<<8)|(p[5]);
	base_touch * pt = (base_touch *) get_ctrl(id);
	
	if(pt == NULL)
	{
		flag = 1;
		pt = (base_touch *)create_ctrl(id);
		
	}
	
	if(pt == NULL)
	{
		return 1;
	}
	
	if((pt->type != CFG_ANI) && (pt->type != CFG_ICO))
	{
		if(flag)
		{
			release_ctrl((t_ctrl *) pt);
		}
		
		return 1;
	}
	
	
	if(pt->type == CFG_ANI)
	{
		animation * panimation = (animation *) pt;
		if(frame > (panimation->animation_icon.num - 1) )
		{
			frame = panimation->animation_icon.num - 1;
		}
	
		panimation->frame_num[0] = frame >> 8;
		panimation->frame_num[1] = frame;
	
		if(*(panimation->init_show) && (!flag))
		{
			panimation->screen_update = 1;
		}
	}
	else if(pt->type == CFG_ICO)
	{
		
		icon_touch * picon = (icon_touch *) pt;
		
		if(frame > (picon->touch_icon.num - 1))
		{
			frame = picon->touch_icon.num - 1;
		}
		
		picon->frame_num[0] = frame >> 8;
		picon->frame_num[1] = frame;
		
		if(*(picon->init_show) && (!flag))
		{
			picon->screen_update = 1;
		}
		
		
	}
	
	if(flag)
	{
		
		release_ctrl((t_ctrl *) pt);
		
	}
	
	return 0;
	
	

}

static uint8_t ctrl_menu(uint8_t *p,uint16_t len)
{
    if(len != 7)
    {
        return 1;
    }

    uint32_t menu_id;
    uint8_t pop;

    menu_id = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | (p[3]);
    pop = p[4];

    menu * pmenu = (menu*) get_ctrl(menu_id);

    if(pop)
    {
        if(pmenu)
        {
            return 1;
        }

        create_ctrl(menu_id);

        pmenu = (menu*) get_ctrl(menu_id);
		
		if(pmenu == NULL)
		{
			return 1;
		}

        pmenu->text_id = (p[5] << 8) | (p[6]);

        pmenu->screen_update = 1;
        
    }
    else
    {
        if(pmenu == NULL)
        {
            return 1;
        }

        release_ctrl((t_ctrl *) pmenu);
		extern uint8_t update_screen_key;
		update_screen_key = 1;
    }
	
	return 0;
}



static uint8_t ctrl_pop_win(uint8_t *p,uint16_t len)
{
    if(len != 8)
    {
        return 1;
    }

    
	g_win.id = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | (p[3]);
	g_win.x = (p[4] << 8) | (p[5]);
	g_win.y = (p[6] << 8) | (p[7]);
	g_win.opt = 1;
	xSemaphoreGive(g_win.sem);
	    
	
	return 0;
}


static uint8_t ctrl_hide_win(uint8_t *p,uint16_t len)
{
    if(len != 4)
    {
        return 1;
    }

    
	g_win.id = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | (p[3]);
	g_win.opt = 0;
	xSemaphoreGive(g_win.sem);
	    
	
	return 0;
}



/*==================================================================
* Function     : ctrl_change_status
* Description  : 
* Input Para   : 
* Output Para  : 
* Return Value : 
==================================================================*/
static uint8_t ctrl_change_status(uint8_t * p,uint16_t len)
{
    uint8_t * buf;
    uint32_t id;
    base_touch * tp;
	uint8_t flag = 0;

    buf = p;
	
    if(len < 0x04)
    {
        return 1;
    }

    id = (buf[0]<<24)|(buf[1]<<16)|(buf[2]<<8)|(buf[3]);
	
    tp = (base_touch *)get_ctrl(id);

    if(tp == NULL)
    {
        tp = (base_touch *)create_ctrl(id);
		flag = 1;
    }

    if(tp == NULL)
    {
        return 1;
    }
    
    if( tp->type == CFG_BTN)
    {
		t_btn *pbtn = (t_btn *)tp;
		xSemaphoreTake(pbtn->ctrl.mutex_sem,portMAX_DELAY);
		if(pbtn->cmd_update_status)
		{
			pbtn->cmd_update_status(pbtn,&buf[4],len-4);
		}
		xSemaphoreGive(pbtn->ctrl.mutex_sem);
    }
	else if(tp->type == CFG_TEX)
	{
		textbox *pbox = (textbox *)tp;
		xSemaphoreTake(pbox->ctrl.mutex_sem,portMAX_DELAY);
		if(pbox->cmd_update_status)
		{
			pbox->cmd_update_status(pbox,&buf[4],len-4);
		}
		xSemaphoreGive(pbox->ctrl.mutex_sem);
	}
	else if(tp->type == CFG_BAR)
	{
		prog_bar *pbar = (prog_bar *)tp;
		xSemaphoreTake(pbar->ctrl.mutex_sem,portMAX_DELAY);
		if(pbar->cmd_update_status)
		{
			pbar->cmd_update_status(pbar,&buf[4],len-4);
		}
		xSemaphoreGive(pbar->ctrl.mutex_sem);
	}
	else if(tp->type == CFG_SLI)
	{
		slider *pslider = (slider *)tp;
		xSemaphoreTake(pslider->ctrl.mutex_sem,portMAX_DELAY);
		if(pslider->cmd_update_status)
		{
			pslider->cmd_update_status(pslider,&buf[4],len-4);
		}
		xSemaphoreGive(pslider->ctrl.mutex_sem);
	}
	else if(tp->type == CFG_SEL)
	{
	    if(len == 5)
	    {
	        select_touch * pselect = (select_touch *) tp;
	        if(pselect->use == 0)
	        {
	            *(pselect->sel_use.sel_slide.option) = buf[4];
	            
	            if(flag == 0)
	            {
	                extern void init_options_infmation(select_touch * pselect);
	                init_options_infmation(pselect);
	                pselect->screen_update = 1;
	            }
	            
	            
	        }
	        else
	        {
	            *(pselect->sel_use.sel_show.option) = buf[4];
				pselect->screen_update = 1;
	        }
	    }
	    else if(flag)
	    {
	        flag = 0;
            release_ctrl((t_ctrl *) tp);
            return 1;
	    }
	}
	
	
    if(flag )
    {
		flag = 0;
        release_ctrl((t_ctrl *) tp);
    }

    return 0;

    
}


/*==================================================================
* Function     : ctrl_get_screen
* Description  : 
* Input Para   : 
* Output Para  : 
* Return Value : 
==================================================================*/
static uint8_t ctrl_get_screen(uint8_t *p,uint16_t len)
{

    //uint16_t screen_id;
    uint8_t *buf;
    
    if(len)
    {
        return 1;
    }

    buf = (uint8_t *)my_malloc(10);
    //screen_id = get_screen_id(src_cfg.pic_now);
    buf[0] = 0xee;
    buf[1] = 0xb1;
    buf[2] = 0x01;
    buf[3] = (src_cfg.current_screen >> 8)&0x00FF;
    buf[4] = src_cfg.current_screen&0x00ff;
    buf[5] = 0xff;
    buf[6] = 0xfc;
    buf[7] = 0xff;
    buf[8] = 0xff;
    buf[9] = 0x3b;

    cmd_dev.notify_cmd(buf,10);

    my_free((void *) buf);
		
		return 0;
    
}


/*==================================================================
* Function     : ctrl_switch_screen
* Description  : 
* Input Para   : 
* Output Para  : 
* Return Value : 
==================================================================*/
static uint8_t ctrl_switch_screen(uint8_t * p,uint16_t len)
{
    uint16_t screen_id;
    //short pic;
    
    
    if(len != 2)
    {
        return 1;
    }

    screen_id = p[0]<<8|p[1];

    //pic = get_pic_num(screen_id);
    
    if(screen_id >= src_cfg.ptscreen->screen_num)
    {
        return 1;
    }

#if   (DIR_SWITCH_SUPPORT)
	
	if(src_cfg.ptscreen->HV_switch == 0)
	{
		src_cfg.new_screen = screen_id;
	}
	else
	{
		adjust_screen_dir(screen_id);
	}

#else
    src_cfg.new_screen = screen_id;
#endif
	
	

    //src_cfg.pic_now = pic;

    return 0;    
}

/*==================================================================
* Function     : cmd_reset
* Description  : 
* Input Para   : 
* Output Para  : 
* Return Value : 
==================================================================*/
static uint8_t cmd_reset(uint8_t *p,uint16_t len)
{
    //uint8_t * buf;
    

    if(len != 4)
    {
        return 1;
    }

    if(p[0]!=0x35||p[1]!=0x5a||p[2]!=0x53||p[3]!=0xa5)
    {
        return 1;
    }
	
	NVIC_SystemReset();

    

    return 0;
}


static uint8_t cmd_shift(uint8_t *p,uint16_t len)
{
	if(len != 1)
	{
		return 1;
	}
	
	if(p[0])
	{
		shift_value = 1;
	}
	else
	{
		shift_value = 0;
	}
	
	return 0;
}

static uint8_t cmd_engch(uint8_t *p,uint16_t len)
{
	
	if(len != 1)
	{
		return 1;
	}
	
	if(p[0])
	{
		engch_value = 1;
	}
	else
	{
		engch_value = 0;
	}
	
	
	return 0;
}


//static uint8_t cmd_update(uint8_t *p,uint16_t len)
//{
//	
//	if(len != 0)
//    {
//        return 1;
//    }
//	
//	update_app();
//	
//	
//	return 0;
//}

/*==================================================================
* Function     : cmd_handshake
* Description  : 
* Input Para   : 
* Output Para  : 
* Return Value : 
==================================================================*/
static uint8_t cmd_handshake(uint8_t *p,uint16_t len)
{
	
    uint8_t * buf;
	
    
    if(len != 0)
    {
        return 1;
    }
	
	extern uint8_t  * version;
	
	uint16_t len1;
	
	len1 = strlen((char*)version);

    buf = (uint8_t *)my_malloc(7+len1);

    buf[0] = 0xee;
    buf[1] = 0x55;
	memcpy(&buf[2],version,len1);
    buf[2+len1] = 0xff;
    buf[3+len1] = 0xfc;
    buf[4+len1] = 0xff;
    buf[5+len1] = 0xff;
	buf[6+len1] = 0x3b;
    cmd_dev.notify_cmd(buf,7+len1);

    my_free(buf);

    return 0;
    
}


static uint8_t cmd_set_language(uint8_t *p,uint16_t len)
{
	
	if(len != 1)
	{
		return 1;
	}
	
	if(p[0] >= src_cfg.lang_info->lang_num)
	{
		return 1;
	}
	
	base_touch * tp;
	uint16_t i;
	
	set_lang(p[0]);
	i = 0;
	while(1)
	{
		tp = (base_touch *)get_ctrl_num(i);
		if(tp)
		{
			if(tp->type == CFG_STR || tp->type == CFG_BTN || 
			   tp->type == CFG_MEN || tp->type == CFG_SEL)
			{
				tp->screen_update = 1;
				break;
			}
		}
		else
		{
			break;
		}
		
		i++;
	}
	
	
	
	
	
	return 0;
}

//自动背光调节--》显示可休眠
static uint8_t cmd_bk_auto(uint8_t *p,uint16_t len)
{
	
	if(len != 1)
	{
		return 1;
	}
	
//	g_project->light = p[0];
//	
//	if(g_project->light > 100)
//	{
//		g_project->light = 100;
//	}
	
	
	
	if(p[0])
	{
		g_project->auto_bl  = 1;
		xTimerReset(g_project->timer,10);
		xTimerStart(g_project->timer,10);
	}
	else
	{
		g_project->auto_bl = 0;
		xTimerStop(g_project->timer,10);
		g_project->light = g_project->active_light;
		
	}
	
	return 0;
}



static uint8_t cmd_set_light(uint8_t *p,uint16_t len)
{
	
	if(len != 1)
	{
		return 1;
	}
	
	
	
		
	
	if(p[0] > 100)
	{
		p[0] = 100;
	}
	else if(p[0]  < 10)
	{
		p[0] = 50;
	}
	g_project->light = p[0];
	
	g_project->active_light = g_project->light;
	
	return 0;
}




static uint8_t cmd_set_rtc(uint8_t *p,uint16_t len)
{
	
	if(len != 8)
	{
		return 1;
	}
	
	//rtc_time time;
	
	
	
	rtc_dev.time.sec = p[0];
	rtc_dev.time.min = p[1];
	rtc_dev.time.hour = p[2];
	rtc_dev.time.day = p[3];
	rtc_dev.time.week = p[4];
	rtc_dev.time.month = p[5];
	rtc_dev.time.year = (p[6]<<8) | p[7];
	
	rtc_dev.set_time(rtc_dev.time);
	
	
	return 0;
}




/*
static uint8_t cmd_change_language(uint8_t *p,uint16_t len)
{
	if(len != 2)
	{
		return 1;
	}

	uint8_t language_num = p[0];
	
    if((language_num + 0XC1) != p[1])
    {
        return 1;
    }
	
	
	uint8_t * index = src_cfg.p_touch;
	uint16_t len1;
	uint32_t id;
	

	while(1)
	{
	    len1 = (index[0] << 8) | (index[1]);
	    if(len1 == 0 || len1 > 0X1000)
	    {
	        break;
	    }
		
		if(index[8] == CFG_MEN)
		{
			menu * pmenu = NULL;
			
			id = ((index[2] << 24) | (index[3] << 16) | (index[6] << 8) | (index[7] << 0));
			pmenu = (menu *)get_ctrl(id);
			if(pmenu)
			{
				if(language_num < pmenu->language_num)
				{
					if(*(pmenu->planguage_num) != language_num)
					{
						*(pmenu->planguage_num) = language_num;
						pmenu->screen_update = 1;
					}
				}
			}
			else
			{
				pmenu = (menu *)create_ctrl(id);
				if(pmenu)
				{
					if(language_num < pmenu->language_num)
					{
						*(pmenu->planguage_num) = language_num;
						
					}
					
					release_ctrl((t_ctrl *)pmenu);
				}
			}
		}

	    if(index[8] == CFG_STE)
	    {
			text * ptext = NULL;
	        id = ((index[2] << 24) | (index[3] << 16) | (index[6] << 8) | (index[7] << 0));
            ptext = (text *)get_ctrl(id);
            if(ptext)
            {
				if(language_num < ptext->language_num)
				{
					if(*(ptext->pshow_num) != language_num)
					{	
						*(ptext->pshow_num) = language_num;
						ptext->screen_update = 1;
					}
				}
              
            }
            else
            {
                ptext = (text *)create_ctrl(id);
                if(ptext)
                {
					if(language_num < ptext->language_num)
					{
						*(ptext->pshow_num) = language_num;
						release_ctrl((t_ctrl *)ptext);
					}
                }
            }
	    }
		if(index[8] == CFG_SEL)
		{
			select_touch * ptslect = NULL;
			id = ((index[2] << 24) | (index[3] << 16) | (index[6] << 8) | (index[7] << 0));
            ptslect = (select_touch *)get_ctrl(id);
			if(ptslect)
			{
				if(language_num < ptslect->language_num)
				{
					if(*(ptslect->plang) != language_num)
					{
						*(ptslect->plang) = language_num;
						ptslect->screen_update = 1;
					}
				}
			}
			else
			{
				ptslect = (select_touch *)create_ctrl(id);
                if(ptslect)
                {
					if(language_num < ptslect->language_num)
					{
						*(ptslect->plang) = language_num;
						release_ctrl((t_ctrl *)ptslect);
					}
                }
				
			}
		}
		
		index += len1;
	}
	
	
	return 0;
}

*/


static uint8_t cmd_get_rtc(uint8_t *p,uint16_t len)
{
	
	
	if(len != 0)
	{
		return 1;
	}
	uint8_t * buf;
	
	buf = (uint8_t *)my_malloc(15);
	
	
	
	buf[0] = 0xee;
	buf[1] = 0xf7;
	buf[2] = rtc_dev.time.year  >> 8;
	buf[3] = rtc_dev.time.year & 0xff;
	buf[4] = rtc_dev.time.month;
	buf[5] = rtc_dev.time.week;
	buf[6] = rtc_dev.time.day;
	buf[7] = rtc_dev.time.hour;
	buf[8] = rtc_dev.time.min;
	buf[9] = rtc_dev.time.sec;
	buf[10] = 0xff;
	buf[11] = 0xfc;
	buf[12] = 0xff;
	buf[13] = 0xff;
	buf[14] = 0x3b;
	
	
	cmd_dev.notify_cmd(buf,15);
	
	my_free(buf);
	
	
	return 0;
}



//static uint8_t ctrl_get_screen(uint8_t *p,uint16_t len)
//{
//    uint8_t * buf;
//    
//    if(len != 0)
//    {
//        return 1;
//    }

//    buf = (uint8_t *)my_malloc(7);

//    buf[0] = 0xee;
//    buf[1] = 0x07;
//    buf[2] = 0xff;
//    buf[3] = 0xfc;
//    buf[4] = 0xff;
//    buf[5] = 0xff;
//		buf[6] = 0x3b;
//    cmd_dev.notify_cmd(buf,7);

//    my_free(buf);

//    return 0;
//    
//}






/*==================================================================
* Function     : cmd_handle_func
* Description  : 内部指令处理入口函数
* Input Para   : p 去除了头尾的指令字符串；len 指令长度
* Output Para  : 
* Return Value : 是否处理成功 0：成功   1：失败
==================================================================*/
uint8_t cmd_handle_func(uint8_t * p, uint16_t len)
{
    uint8_t * buf = NULL;
    uint8_t res = 1;
    short num;
    

    for(num=0;num<MAX_CMD_NUM;num++)
    {
        if(p[0] == cmd_func_table[num].cmd)
        {
            break;
        }
    }
    
    if(num < MAX_CMD_NUM)
    {
        if(cmd_func_table[num].func)
        {
            if(len > 1)
            {
                buf = (uint8_t *) my_malloc(len - 1);
		        mem_copy(buf, &p[1],len-1);
            }

            res = cmd_func_table[num].func(buf,len-1);

            if(buf)
            {
                my_free(buf);
            }
            
        }
    }

    return res;

    
}



static uint8_t * find_touch_screen(uint16_t screen)
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

//批量更新控件指令
static uint8_t ctrl_batch(uint8_t *p,uint16_t len)
{
	if(len < 5)
	{
		return 1;
	}
	
	uint16_t frame,len0,screen,screen_id,index = 2;
	uint32_t id;
	
	screen = (p[0] << 8) | (p[1]);
	screen_id = src_cfg.current_screen;
	base_touch * tp;
	textbox *pbox;
	slider * psli;
	prog_bar * pbar;
	t_btn * pbtn;
	icon_touch * pico;
	
	if(screen == screen_id)
	{
		while(index < len)
		{
			id = (p[0] << 24) | (p[1] << 16) | (p[index] << 8) | (p[index+1]);
			index += 2;
			tp = (base_touch *)get_ctrl(id);
			len0 = (p[index] << 8) | (p[index+1]);
			index += 2;
			if(index + len0 > len)
			{
				break;
			}
			if(tp)
			{
				switch(tp->type)
				{
					case CFG_TEX:
						pbox = (textbox *)tp;
						xSemaphoreTake(pbox->ctrl.mutex_sem,portMAX_DELAY);
						if(pbox->cmd_update_status)
						{
							pbox->cmd_update_status(pbox,&p[index],len0);
						}
						xSemaphoreGive(pbox->ctrl.mutex_sem);
						break;
					case CFG_SLI:
						psli = (slider *)tp;
						xSemaphoreTake(psli->ctrl.mutex_sem,portMAX_DELAY);
						if(psli->cmd_update_status)
						{
							psli->cmd_update_status(psli,&p[index],len0);
						}
						xSemaphoreGive(psli->ctrl.mutex_sem);
						break;
					case CFG_BAR:
						pbar = (prog_bar *)tp;
						xSemaphoreTake(pbar->ctrl.mutex_sem,portMAX_DELAY);
						if(pbar->cmd_update_status)
						{
							pbar->cmd_update_status(pbar,&p[index],len0);
						}
						xSemaphoreGive(pbar->ctrl.mutex_sem);
						break;
					case CFG_BTN:
						pbtn = (t_btn *)tp;
						xSemaphoreTake(pbtn->ctrl.mutex_sem,portMAX_DELAY);
						if(pbtn->cmd_update_status)
						{
							pbtn->cmd_update_status(pbtn,&p[index],len0);
						}
						xSemaphoreGive(pbtn->ctrl.mutex_sem);
						break;
					case CFG_ICO:
						
						if(len0 != 2)
						{
							break;
						}
						frame = (p[index] << 8) | p[index+1];
						pico = (icon_touch *) tp;
						if(frame > (pico->touch_icon.num - 1))
						{
							frame = pico->touch_icon.num - 1;
						}
						pico->frame_num[0] = frame >> 8;
						pico->frame_num[1] = frame;
						break;
					default:
						break;
										
						
				}
			}
			
			index += len0;
			
			
			
		}
	}
	else
	{
		while(index < len)
		{
			id = (p[0] << 24) | (p[1] << 16) | (p[index] << 8) | (p[index+1]);
			index += 2;
			tp = (base_touch *)create_ctrl(id);
			len0 = (p[index] << 8) | (p[index+1]);//控件字节长度，2字节
			index += 2;
			if(index + len0 > len)
			{
                if(tp)
                {
                    release_ctrl((t_ctrl *) tp);
                }
				break;
			}
			
			if(tp)
			{
				switch(tp->type)
				{
					case CFG_TEX:
						pbox = (textbox *)tp;
						xSemaphoreTake(pbox->ctrl.mutex_sem,portMAX_DELAY);
						if(pbox->cmd_update_status)
						{
							pbox->cmd_update_status(pbox,&p[index],len0);
						}
						xSemaphoreGive(pbox->ctrl.mutex_sem);
						break;
					case CFG_SLI:
						psli = (slider *)tp;
						xSemaphoreTake(psli->ctrl.mutex_sem,portMAX_DELAY);
						if(psli->cmd_update_status)
						{
							psli->cmd_update_status(psli,&p[index],len0);
						}
						xSemaphoreGive(psli->ctrl.mutex_sem);
						break;
					case CFG_BAR:
						pbar = (prog_bar *)tp;
						xSemaphoreTake(pbar->ctrl.mutex_sem,portMAX_DELAY);
						if(pbar->cmd_update_status)
						{
							pbar->cmd_update_status(pbar,&p[index],len0);
						}
						xSemaphoreGive(pbar->ctrl.mutex_sem);
						break;
					case CFG_BTN:
						pbtn = (t_btn *)tp;
						xSemaphoreTake(pbtn->ctrl.mutex_sem,portMAX_DELAY);
						if(pbtn->cmd_update_status)
						{
							pbtn->cmd_update_status(pbtn,&p[index],len0);
						}
						xSemaphoreGive(pbtn->ctrl.mutex_sem);
						break;
					case CFG_ICO:
						
						if(len0 != 2)
						{
							break;
						}
						frame = (p[index] << 8) | p[index+1];
						pico = (icon_touch *) tp;
						if(frame > (pico->touch_icon.num - 1))
						{
							frame = pico->touch_icon.num - 1;
						}
						pico->frame_num[0] = frame >> 8;
						pico->frame_num[1] = frame;
						break;
					default:
						break;
											
				}
				
				release_ctrl((t_ctrl *) tp);
			}
			
			index += len0;
			
		}
	}
	
	
	
	return 0;
}




static t_ctrl * create_ctrl(uint32_t id)
{
    uint16_t screen_id = (id >> 16);
    uint8_t * p;
    uint16_t pic_id ,len;
    uint32_t temp_id;
    uint8_t type;
    t_ctrl * tp;
    
    
    p = find_touch_screen(src_cfg.ptscreen->screen_pic[screen_id]);

    if(p == NULL)
    {
        return NULL;
    }

    while(1)
	{
	
		len = (p[0] << 8) | p[1];
		
		if(len == 0)
		{
			return NULL;
		}
		pic_id = (p[4] << 8) | p[5];
		
		if(pic_id != src_cfg.ptscreen->screen_pic[screen_id])
		{
			return NULL;
		}

		temp_id = (p[2] << 24) | (p[3] << 16) | (p[6] << 8) | (p[7]);

		if(id == temp_id)
		{
		
		    type = p[8];

		    switch(type)
		    {
			    case CFG_BTN:
				    sbtn_create(&p[2]);
				    break;
		        case CFG_BAR:
				    sprog_bar_create(&p[2]);
				    break;
			    case CFG_SLI:
				    sslider_create(&p[2]);
				    break;
			    case CFG_TEX:
				    stextbox_create(&p[2]);
				    break;
				case CFG_ANI:
				    sanimation_create(&p[2]);
					break;
				case CFG_ICO:
					sicon_create(&p[2]);
					break;
//			    case CFG_STE:
//				    stext_create(&p[2]);
//				    break;
			    case CFG_MEN:
			        menu_create(&p[2]);
			        break;
			    case CFG_GRA:
			        graph_create(&p[2]);
			        break;
			    case CFG_STR:
			        sstr_touch_create(&p[2]);
			        break;
			    case CFG_SEL:
			        sselect_create(&p[2]);
					break;
				case CFG_REC:
					srecord_create(&p[2]);
					break;
			    default:
				    break;	
		    }

		    tp = get_ctrl(id);
			if(tp && type != CFG_MEN)
			{	
				tp->active = 0;
			}
		    return tp;
	    }
	    

		p += len;
		
	}

	
}









/***************************************************************************************
* End of File: cmd_func.c

****************************************************************************************/
