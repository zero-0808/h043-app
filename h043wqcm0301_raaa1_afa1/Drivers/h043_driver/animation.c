
/***************************************************************************************
****************************************************************************************
* Project      : 20180525_HMI
* FILE         : animation.c
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
#include "animation.h"
#include "mem_my.h"
#include "cfg.h"
#include "display.h"
#include "cmd.h"
#include "string.h"


    static uint16_t x1,y1_1,x2,y2;


    static uint8_t animation_msg( void * p, t_msg * p_msg )
    {
        uint8_t f = 1;
        uint16_t x,y,flag;
        t_msg msg;
    
        if(p_msg->msg != MSG_TP)
        {
            return f;
        }
    
        animation * panimation = (animation *)p;
    
        if(panimation->touch_act == 0 || panimation->ctrl.active == 0)
        {
            return f;
        }
    
        x = p_msg->a;
        y = p_msg->b;
        flag = p_msg->c;
    
        if((flag != 1) && !GETBIT( panimation->state, BTN_STATE_ACTIVE))
        {
            return f;
        }
    
        if(pt_in_rect(&panimation->tp_rect, x, y))
        {
            f = 0;
        }
    
        switch(flag)
        {
            case 1:
                if(!f)
                {
                    SETBIT( panimation->state, BTN_STATE_ACTIVE);
                    msg.msg = MSG_TOUCH;
                                msg.a   = (panimation->ctrl.id >> 16) & (0xffff);
                                msg.b   = (panimation->ctrl.id) & (0xffff);
                                msg.c   = 1;
                                msg_dev.write_message((void *)&msg,0);
    
                    x1 = p_msg->a;
                    y1_1 = p_msg->b;
                                
                }
                break;
            case 2:
                msg.msg = MSG_TOUCH;
                msg.a   = (panimation->ctrl.id >> 16) & (0xffff);
                msg.b   = (panimation->ctrl.id) & (0xffff);
                msg.c   = 2;
                x2 = p_msg->a;
                y2 = p_msg->b;
                msg_dev.write_message((void *)&msg,0);
                break;
            case 0:
                msg.msg = MSG_TOUCH;
                msg.a   = (panimation->ctrl.id >> 16) & (0xffff);
                msg.b   = (panimation->ctrl.id) & (0xffff);
                msg.c   = 0;
                x2 = p_msg->a;
                y2 = p_msg->b;
                msg_dev.write_message((void *)&msg,0);
                CLRBIT( panimation->state, BTN_STATE_ACTIVE);
                break;
        }
      
    
        return f;
        
    }

    
    
    
    
    static void animation_release( void* pCtrl )
    {
    
        animation * panimation = (animation *) pCtrl;
    
        if(panimation->animation_icon.p)
        {
            my_free((void *)panimation->animation_icon.p);
        }
        
        if(panimation->timer)
        {
            xTimerDelete(panimation->timer,10);
        }
    
        my_free((void *)panimation);
        
    }



    
    
    
    
    static void animation_act(base_touch *p,uint8_t flag)
    {
    
        uint16_t frame;
    
        if(p->type != CFG_ANI || p->ctrl.active == 0)
        {
            return;
        }
    
    
        animation * panimation = (animation *) p;
    
        if(flag == 2 || flag == 0)
        {
            frame = (panimation->frame_num[0] << 8) | (panimation->frame_num[1]);
            
            if(panimation->touch_act == 2)
            {
                if(x1 > x2)
                {
                    if((x1 - x2) > panimation->step_length)
                    {
                        frame += 1;
    
                        if(frame >= panimation->animation_icon.num)
                        {
                            frame = 0;
                        }
    
                        panimation->frame_num[0] = (frame >> 8);
                        panimation->frame_num[1] = frame;
                        panimation->screen_update = 1;
                        x1 = x2;
                        y1_1 = y2;
                    }
                }
                else
                {
                    if((x2 - x1) >= panimation->step_length)
                    {
                        if(frame == 0)
                        {
                            frame = panimation->animation_icon.num;
                        }
                        frame -= 1;
                        panimation->frame_num[0] = (frame >> 8);
                        panimation->frame_num[1] = frame;
                        panimation->screen_update = 1;
                        x1 = x2;
                        y1_1 = y2;
                        
                    }
                    
                }
            }
			else if(panimation->touch_act == 3)
			{
				if(y1_1 > y2)
				{
					if((y1_1 - y2) > panimation->step_length)
					{
						frame += 1;
    
						if(frame >= panimation->animation_icon.num)
						{
							frame = 0;
						}
    
						panimation->frame_num[0] = (frame >> 8);
						panimation->frame_num[1] = frame;
						panimation->screen_update = 1;
						x1 = x2;
						y1_1 = y2;
					}
               
                
				}
				else
				{
    
					if((y2 - y1_1) >= panimation->step_length)
                    {
                        if(frame == 0)
                        {
                            frame = panimation->animation_icon.num;
                        }
                        frame -= 1;
                        panimation->frame_num[0] = (frame >> 8);
                        panimation->frame_num[1] = frame;
                        panimation->screen_update = 1;
                        x1 = x2;
                        y1_1 = y2;
                        
                    }
                
				}
			}
    
        }
        
        
    }


    
    
    static void upload_animation(uint32_t id)
    {
        uint8_t  * buf = (uint8_t *) my_malloc(12);
        
        buf[0] = 0xee;
        buf[1] = 0xb1;
        
        buf[2] = 0x27;
        buf[3] = id >> 24;
        buf[4] = id >> 16;
        buf[5] = id >> 8;
        
        buf[6] = id;
        buf[7] = 0xff;
        buf[8] = 0xfc;
        buf[9] = 0xff;
        buf[10] = 0xff;
        
        buf[11] = 0x3b;
        
        cmd_dev.notify_cmd(buf,12);
    
        my_free((void *) buf);
    }


    
    
    
    
    static void animation_draw_screen(animation * panimation)
    {
    
        if(panimation->type != CFG_ANI)
        {
            return;
        }
    
    

        
        if(*(panimation->init_show) == 0)
        {
            panimation->ctrl.active = 0;

            return;
        }
    
        
        
        uint16_t times = (panimation->times[0] << 8) | (panimation->times[1]);
    
    
        if((panimation->play_times != 0)&& (times >= panimation->play_times))
        {
            panimation->times[0] = 0;
            panimation->times[1] = 0;
            //panimation->frame_num[0] = 0;
            //panimation->frame_num[1] = 0;
            panimation->screen_update = 0;
            *(panimation->auto_play) = 0;
            xTimerStop(panimation->timer,10);
            
            if(panimation->play_end == 1)
            {
                upload_animation(panimation->ctrl.id);
            }
            else if(panimation->play_end == 2)
            {
                //uint16_t screen_id;
				
				//screen_id = src_cfg.screen_id +1;
                //screen_id = get_screen_id(src_cfg.pic_now);
                //screen_id += 1;
                if(src_cfg.current_screen < src_cfg.ptscreen->screen_num - 3)
                {
                    //src_cfg.pic_now = get_pic_num(screen_id);
					src_cfg.new_screen = src_cfg.current_screen + 1;
                }
            }
            
        }
    
    
        uint16_t frame = (panimation->frame_num[0] << 8) | (panimation->frame_num[1]);
    
        if(frame >=  panimation->animation_icon.num)
        {
            frame = 0;
            panimation->frame_num[0] = 0;
            panimation->frame_num[1] = 0;
        }
    
        //vblt_bdc(disp_dev.disp[1],panimation->back_image);
    
        if(get_icon_image(&(panimation->animation_icon), frame) == 0)
        {
            uint8_t  * dp;


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
            show_icon_image_mix(panimation->animation_icon,panimation->tp_rect,dp);
        }
    
        
        
    }



    
    
    
    /*
    
    static void animation_draw_screen(animation * p)
    {
        
        if(p->type != CFG_ANI)
        {
            return;      
        }
    
        animation * panimation = (animation *) p;
    
        if(*(panimation->init_show))
        {
            uint16_t frame;
            uint8_t res;
            uint8_t * dp;
    
            frame = (panimation->frame_num[0] << 8) | (panimation->frame_num[1]);
    
            res = get_icon_image(&(panimation->animation_icon),frame);
    
            if(res == 0)
            {
                if(disp_dev.flag)
                {
                    dp = disp_dev.disp[0].p;
                }
                else
                {
                    dp = disp_dev.disp[1].p;
                }
                show_icon_image_mix(panimation->animation_icon,panimation->tp_rect,dp);
            }
    
            
    
            
        }
        else
        {
            panimation->ctrl.active = 0;
        }
          
        
    }

    */


    
    
    
    
    
    
    static void animation_timer(TimerHandle_t timer)
    {
        extern uint8_t switch_screen_flag;
        if(switch_screen_flag == 0)
        {
            animation * panimation =  (animation *) pvTimerGetTimerID(timer);
    
            uint16_t  frame = (panimation->frame_num[0] << 8) |  (panimation->frame_num[1]);
    
            frame += 1;
            
            if(frame >= panimation->animation_icon.num)
            {
                uint16_t times = (panimation->times[0] << 8) | (panimation->times[1]);
                times  += 1;
                panimation->times[0] = times >> 8;
                panimation->times[1] = times;
                if((times < panimation->play_times) || panimation->play_times == 0)
                {
                    frame = 0;
                 }
                else
                {
                    frame = panimation->animation_icon.num - 1;
                }
                
            }
    
            panimation->frame_num[0] = frame >> 8;
            panimation->frame_num[1] = frame;
    
            panimation->screen_update = 1;
    
            
    
            
        }
    }
    
    
    
    
    uint8_t animation_create(uint8_t * buffer)
    {
        uint32_t addr;
        
        animation * panimation = (animation *)my_malloc(sizeof(animation));
    
        if(panimation == NULL)
        {
            return 1;
        }
    
        memset((void*)panimation,0,sizeof(animation));
        panimation->ctrl.id = ((buffer[0] << 24) | (buffer[1] << 16) | (buffer[4] << 8) | (buffer[5] << 0));
        panimation->ctrl.func_ctrl_procmsg  = animation_msg;
        panimation->ctrl.func_ctrl_release  = animation_release;
        panimation->type = buffer[6];
        panimation->tp_rect.left = ((buffer[7] << 8) | (buffer[8]));
        panimation->tp_rect.top= ((buffer[9] << 8) | buffer[10]);
        panimation->tp_rect.right =((buffer[11] << 8) | (buffer[12]));
        panimation->tp_rect.bottom = ((buffer[13] << 8) | (buffer[14]));
        panimation->pic_id   = (buffer[2] << 8) | (buffer[3]);
        panimation->pic_on   = ((buffer[15] << 8) | (buffer[16]));
    
        panimation->act = animation_act;
        panimation->draw_screen = animation_draw_screen;
        //panimation->update_screen = animation_update_screen;
        //panimation->cmd_update_status = cmd_update_animation_status;
        //panimation->cmd_get_status = cmd_get_animation_status;
    
        //panimation->back_image.x = panimation->tp_rect.left;
        //panimation->back_image.y = panimation->tp_rect.top;
        //panimation->back_image.width = panimation->tp_rect.right - panimation->tp_rect.left + 1;
        //panimation->back_image.height = panimation->tp_rect.bottom - panimation->tp_rect.top + 1;
        //panimation->back_image.vdc = disp_dev.disp[2];
        
        panimation->init_show = &buffer[17];
        panimation->auto_play = &buffer[18];
        panimation->frame_time = (buffer[19]<<8) | (buffer[20]);
        panimation->play_times = (buffer[21]<<8) | (buffer[22]);
        panimation->times = &buffer[23];
        panimation->play_end = buffer[25];
        panimation->touch_act = buffer[26];
        panimation->step_length = (buffer[27]<<8) | (buffer[28]);
        panimation->frame_num = &buffer[29];
        addr = (buffer[31]<<24) | (buffer[32]<<16) | (buffer[33]<<8) | (buffer[34]);
        panimation->state = 0;
    
        panimation->timer =  xTimerCreate((const char *) "animation",
                                    (TickType_t)panimation->frame_time,
                                    (UBaseType_t)pdTRUE,
                                    (void*  )panimation,
                                    (TimerCallbackFunction_t)animation_timer);
    
    
            
    
        if(*(panimation->init_show) && *(panimation->auto_play) == 1)
        {
             xTimerStart(panimation->timer,10);
             //*(panimation->play_flag) = 1;
             //*(panimation->auto_play) = 0;
        }
        
        init_icon(&(panimation->animation_icon), addr);
        ctrl_init((t_ctrl*) panimation);
        
        if((*(panimation->init_show))==0)
        {
            panimation->ctrl.active = 0;
        }
        
        return 0;
        
    }



    
    
    
    
    
    
    
    
    t_ctrl * sanimation_create(uint8_t * buffer)
    {
        uint32_t addr;
        
        animation * panimation = (animation *)my_malloc(sizeof(animation));
    
        if(panimation == NULL)
        {
            return NULL;
        }
    
        memset((void*)panimation,0,sizeof(animation));
        panimation->ctrl.id = ((buffer[0] << 24) | (buffer[1] << 16) | (buffer[4] << 8) | (buffer[5] << 0));
        panimation->ctrl.func_ctrl_procmsg  = animation_msg;
        panimation->ctrl.func_ctrl_release  = animation_release;
        panimation->type = buffer[6];
        panimation->tp_rect.left = 0xffff;
        panimation->tp_rect.top= 0xffff;
        panimation->tp_rect.right =0xffff;
        panimation->tp_rect.bottom = 0xffff;
        //panimation->pic_id   = (buffer[2] << 8) | (buffer[3]);
        //panimation->pic_on   = ((buffer[15] << 8) | (buffer[16]));
    
        //panimation->act = animation_act;
        //panimation->init_screen = animation_init_screen;
        //panimation->update_screen = animation_update_screen;
        //panimation->cmd_update_status = cmd_update_animation_status;
        //panimation->cmd_get_status = cmd_get_animation_status;
    
        //panimation->back_image.x = panimation->tp_rect.left;
        //panimation->back_image.y = panimation->tp_rect.top;
        //panimation->back_image.width = panimation->tp_rect.right - panimation->tp_rect.left + 1;
        //panimation->back_image.height = panimation->tp_rect.bottom - panimation->tp_rect.top + 1;
        //panimation->back_image.vdc = disp_dev.disp[2];
        
        panimation->init_show = &buffer[17];
        panimation->auto_play = &buffer[18];
        //panimation->frame_time = (buffer[19]<<8) | (buffer[20]);
        //panimation->play_times = (buffer[21]<<8) | (buffer[22]);
        panimation->times = &buffer[23];
        //panimation->play_end = buffer[25];
        //panimation->touch_act = buffer[26];
        //panimation->step_length = (buffer[27]<<8) | (buffer[28]);
        panimation->frame_num = &buffer[29];
        addr = (buffer[31]<<24) | (buffer[32]<<16) | (buffer[33]<<8) | (buffer[34]);
        //panimation->state = 0;
        /*
        panimation->timer =  xTimerCreate((const char *) "animation",
                                    (TickType_t)panimation->frame_time,
                                    (UBaseType_t)pdTRUE,
                                    (void*  )panimation,
                                    (TimerCallbackFunction_t)animation_timer);
    */      
    /*
        if(*(panimation->init_show) && *(panimation->auto_play))
        {
             xTimerStart(panimation->timer,10);
             *(panimation->play_flag) = 1;
             *(panimation->auto_play) = 0;
        }
    
        */
    
        init_icon(&(panimation->animation_icon), addr);
        ctrl_init((t_ctrl*) panimation);
        panimation->ctrl.active = 0;
        
        return (t_ctrl *) panimation;
        
    }


/***************************************************************************************
* End of File: animation.c

****************************************************************************************/
