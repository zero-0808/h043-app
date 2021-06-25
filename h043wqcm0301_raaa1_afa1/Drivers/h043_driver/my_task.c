
/***************************************************************************************
****************************************************************************************
* Project      : 20180525_HMI
* FILE         : my_task.c
* Description  : 
*                      
* Copyright (c) 2018 by ShenZhen JingHua Displays Co.,Ltd... All Rights Reserved.
* 
* History:
* Version		name			Date			Description
* 0.01			Qu Bingbing		2018/05/25		Initial Version
* 
****************************************************************************************
****************************************************************************************/
#include "my_task.h"
#include "mem_my.h"
#include "cmd.h"
#include "message.h"
#include "cfg.h"
#include "buzzer.h"
#include "display.h"
#include "ctrl.h"
#include "usart.h"
#include "keyboard.h"
#include "timer.h"
//#include "lpc177x_8x_gpio.h"
#include "btn.h"
#include "gpio.h"


TaskHandle_t StartTask_Handler;
TaskHandle_t TOUCHTask_Handler;
TaskHandle_t MESSAGETask_Handler;
TaskHandle_t USARTTask_Handler;
TaskHandle_t UPDATE_SCREENTask_Handler;
TaskHandle_t EXTERNAL_CMDTask_Handler;
TaskHandle_t CMD_PRASETask_Handler;
TaskHandle_t KB_Task_Handler;
TaskHandle_t RTC_Task_Handler;
TaskHandle_t WIN_Task_Handler;
TaskHandle_t USBMSC_Task_Handler;
TaskHandle_t WATCHDOG_Task_Handler;
#if (DIR_SWITCH_SUPPORT)
TaskHandle_t LIS3DH_Task_Handler;
#endif









uint8_t switch_screen_flag = 0;   //�л������־
//static uint16_t screen_id;      //��¼��ǰ����ID
static void update_screen_task(void *pvParameters);


#if (DIR_SWITCH_SUPPORT)

extern SemaphoreHandle_t screen_mutex_sem;

#endif





void cmd_nortify_reset(void)
{
	uint8_t * buf = (uint8_t *)my_malloc(7);

    buf[0] = 0xee;
    buf[1] = 0x07;
    buf[2] = 0xff;
    buf[3] = 0xfc;
    buf[4] = 0xff;
    buf[5] = 0xff;
	buf[6] = 0x3b;
	
    cmd_dev.notify_cmd(buf,7);

    my_free(buf);
}



static void upload_screen(void)
{
	//uint8_t screen_id;
	uint8_t * buf = (uint8_t *)my_malloc(10);
	
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
	
}





static void bl_nortify(uint8_t flag)
{
	uint8_t * buf = (uint8_t *)my_malloc(8);
	
	buf[0] = 0xee;
	buf[1] = 0x60;
	buf[2] = flag ? 1 : 0;
	buf[3] = 0xff;
	buf[4] = 0xfc;
	buf[5] = 0xff;
	buf[6] = 0xff;
	buf[7] = 0x3b;
	cmd_dev.notify_cmd(buf,8);
    my_free(buf);
}











static uint8_t touch_act(t_msg msg)
{
	base_touch * p;
//	uint16_t num;
	uint32_t id;

	p = NULL;
	
	if(msg.msg != MSG_TOUCH)
	{
		return 1;
	}

	id = (msg.a << 16) | (msg.b);
	
	p = (base_touch *) get_ctrl(id);

	if((switch_screen_flag)|| (p == NULL))
	{
		return 1;
	}

	if(p->act)
	{
		xSemaphoreTake(p->ctrl.mutex_sem,portMAX_DELAY);
		
	    p->act(p,msg.c);
		
		xSemaphoreGive(p->ctrl.mutex_sem);
	

	}

	
	

	return 0;	
	
}



//void put_pixel(uint16_t * dp,uint16_t * sp)
//{
//	
//	uint16_t x,y;
//	
//	for(y=0;y<disp_dev.height;y++)
//	{
//		for(x=0;x<disp_dev.width;x++)
//		{
//			*(dp++) = *(sp++);
//		}
//	}
//	
//}

/*
static void v2h(void)
{
	
	uint16_t * sp, *dp;
	uint16_t i,j;
	
	
	if(disp_dev.flag) 
	{
		dp = (uint16_t *)disp_dev.disp[0].p;
	}
	else
	{
		dp = (uint16_t *)disp_dev.disp[1].p;
	}

	for(i=0;i<disp_dev.height0;i++)
	{
		sp = (uint16_t *)((uint16_t *)disp_dev.disp[3].p + (disp_dev.height0 - i - 1));
		//sp = (uint16_t * ) ((uint16_t *)disp_dev.disp[3].p + i);
		for(j=0;j<disp_dev.width0;j++)
		{
			*dp = *sp;
			dp++;
			sp += disp_dev.height0;
		}
	}
	
}

*/


static void v2h(void)
{
	
	uint16_t * sp, *dp,*tdp;
	uint16_t i,j;
	
	
	if(disp_dev.flag) 
	{
		tdp = (uint16_t *)disp_dev.disp[0].p;
	}
	else
	{
		tdp = (uint16_t *)disp_dev.disp[1].p;
	}

	for(i=0;i<disp_dev.height0;i++)
	{
		sp = (uint16_t *)((uint16_t *)disp_dev.disp[3].p + i);
		dp = tdp + ((i+1)*disp_dev.width0) - 1;
		for(j=0;j<disp_dev.width0;j++)
		{
			*dp = *sp;
			dp--;
			sp += disp_dev.height0;
		}
	}
	
}



static uint8_t update_screen(void)
{
	uint16_t x,y,h,w;
	uint8_t *dp;
	uint8_t *sp;
	base_touch * p;
	
	uint16_t i;
	
	uint16_t num;

	
	num = get_pic_num(src_cfg.current_screen);

	


	ctrl_release();
	btn_key_delete();
	get_pic(num);
	
	sp = disp_dev.get_addr(2);

	if(disp_dev.dir == 0)
	{
    	if(disp_dev.flag == 0)
    	{
    		dp = disp_dev.get_addr(1);
    	}
    	else
    	{
    		dp = disp_dev.get_addr(0);
    	}
	}
	else if(disp_dev.dir == 1)
	{
	    dp = disp_dev.disp[3].p;
	}
	x = 0;
	y = 0;
	w = disp_dev.width;
	h = disp_dev.height;
	
	
	blt(dp,x,y,w,h,sp);

	
	
	
	parse_cfg();
	
	i=0;

	while(1)
	{
		 p = (base_touch *)get_ctrl_num(i);
		 
		if(p)
		{
			if(p->draw_screen)
			{
			    p->draw_screen(p);
			}
		}
		else
		{
			break;
		}

		i++;
	}


	if(disp_dev.dir == 1)
	{
	    
	    v2h();
	    
	}
	
    
	
	disp_dev.switch_base_addr();

	
	return 0;	
	
}






void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();
#if 1 
	cmd_nortify_reset();
   
    xTaskCreate((TaskFunction_t )touch_task,     	
                (const char*    )"touch_task",   	
                (uint16_t       )TOUCH_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )TOUCH_TASK_PRIO,	
                (TaskHandle_t*  )&TOUCHTask_Handler);
				
    
								
	xTaskCreate((TaskFunction_t )message_task,     
                (const char*    )"message_task",   
                (uint16_t       )MESSAGE_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )MESSAGE_TASK_PRIO,
                (TaskHandle_t*  )&MESSAGETask_Handler);
	
	xTaskCreate((TaskFunction_t )usart_task,     
                (const char*    )"usart_task",   
                (uint16_t       )USART_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )USART_TASK_PRIO,
                (TaskHandle_t*  )&USARTTask_Handler);
				
				
	xTaskCreate((TaskFunction_t )update_screen_task,     
                (const char*    )"update_screen_task",   
                (uint16_t       )UPDATE_SCREEN_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )UPDATE_SCREEN_TASK_PRIO,
                (TaskHandle_t*  )&UPDATE_SCREENTask_Handler);
				
	xTaskCreate((TaskFunction_t )external_cmd_task,     
                (const char*    )"external_cmd_task",   
                (uint16_t       )EXTERNAL_CMD_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )EXTERNAL_CMD_TASK_PRIO,
                (TaskHandle_t*  )&EXTERNAL_CMDTask_Handler);
               
	xTaskCreate((TaskFunction_t )cmd_prase_task,     
                (const char*    )"cmd_prase_task",   
                (uint16_t       )CMD_PRASE_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )CMD_PRASE_TASK_PRIO,
                (TaskHandle_t*  )&CMD_PRASETask_Handler);			


    xTaskCreate((TaskFunction_t )keyboard_task,     
                (const char*    )"keyboard_task",   
                (uint16_t       )KB_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )KB_TASK_PRIO,
                (TaskHandle_t*  )&KB_Task_Handler);
				
	xTaskCreate((TaskFunction_t )win_task,     
                (const char*    )"win_task",   
                (uint16_t       )WIN_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )WIN_TASK_PRIO,
                (TaskHandle_t*  )&WIN_Task_Handler);


    xTaskCreate((TaskFunction_t )update_rtc_task,     
                (const char*    )"update_rtc_task",   
                (uint16_t       )RTC_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )RTC_TASK_PRIO,
                (TaskHandle_t*  )&RTC_Task_Handler);
 
#if   (DIR_SWITCH_SUPPORT)				
				
	if(src_cfg.ptscreen->HV_switch)
	{
		xTaskCreate((TaskFunction_t )lis3dh_task,     
                (const char*    )"lis3dh_task",   
                (uint16_t       )LIS3DH_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )LIS3DH_TASK_PRIO,
                (TaskHandle_t*  )&LIS3DH_Task_Handler);
	}
	
#endif   
#endif
//#if   (SRC_DOWNLOAD_TYPE == SRC_DOWNLOAD_VUSB)
//    xTaskCreate((TaskFunction_t )usbmsc_task,     
//                (const char*    )"usbmsc_task",   
//                (uint16_t       )USBMSC_STK_SIZE, 
//                (void*          )NULL,
//                (UBaseType_t    )USBMSC_TASK_PRIO,
//                (TaskHandle_t*  )&USBMSC_Task_Handler);      
//#endif
//#define   HMI_APP_DEBUG            
#ifndef	HMI_APP_DEBUG	                
    xTaskCreate((TaskFunction_t )watchdog_task,     
                (const char*    )"watchdog_task",   
                (uint16_t       )WATCHDOG_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )WATCHDOG_TASK_PRIO,
                (TaskHandle_t*  )&WATCHDOG_Task_Handler); 
#endif                

				
	
								
								
    vTaskDelete(StartTask_Handler); 
								
								
    taskEXIT_CRITICAL();            
}

uint8_t  update_screen_key = 0;
//uint8_t  switch_screen_num = 0;


void check_mem_info(void)
{
	uint32_t num,max,size;
	size = get_mem_info(&num,&max);
	PRINT("num = %d,max = %d, size = %d\r\n",num,max,size);
}



static void update_screen_task(void *pvParameters)
{
	uint16_t i;
	base_touch * p;

	i = 0;
	
	
	
#if   (DIR_SWITCH_SUPPORT)
	
	if(src_cfg.ptscreen->HV_switch)
	{
		delay_xms(100);
		adjust_screen_dir(g_project->start_screen);
	}
	
#endif
	
	

	
	while(1)
	{
		if(src_cfg.new_screen != src_cfg.current_screen)
		{
#if (DIR_SWITCH_SUPPORT)
			xSemaphoreTake(screen_mutex_sem,portMAX_DELAY);   //���л���Ļʱ��������ת��Ļ���ʼӻ�����
#endif
			switch_screen_flag = 1;
			src_cfg.current_screen = src_cfg.new_screen;
			update_screen();
			switch_screen_flag = 0;
#if (DIR_SWITCH_SUPPORT)
			xSemaphoreGive(screen_mutex_sem);
#endif
			if(g_project->switch_nortify)
			{
				upload_screen();
			}

			//switch_screen_num = 0;
		
			
		}
		
		
		
		
		if(g_project->old_light != g_project->light)
		{
			disp_dev.set_light(g_project->light);
			g_project->old_light = g_project->light;
			if(g_project->light)
			{
				delay_ms(50);
				disp_dev.open_backlight();
				
			}
			else 
			{
				disp_dev.close_backlight();
			}
			if(g_project->bl_nortify)
			{
				uint8_t flag;
				flag = (g_project->light == g_project->active_light) ? 1 : 0;
				bl_nortify(flag);
			}

		}

		
		
		
		if(switch_screen_flag)
		{
			continue;
		}
		
		i = 0;
		
		while(1)
		{
			
			if((src_cfg.new_screen != src_cfg.current_screen))
			{
				break;
			}
			
			
			p = (base_touch *)get_ctrl_num(i);
			
			if(p)
			{
				
				if((p->screen_update || update_screen_key))
				{
					uint8_t *dp,*sp;
					uint16_t x,y,h,w;
					sp = disp_dev.disp[2].p;
					update_screen_key = 0;
					if(disp_dev.dir == 0)
					{
    					if(disp_dev.flag == 0)
    					{
    						dp = disp_dev.disp[1].p;
    					}
    					else
    					{
    						dp = disp_dev.disp[0].p;
    					}
					}
					else
					{
					    dp = disp_dev.disp[3].p;
					}
					x = 0;
					y = 0;
					w = disp_dev.width;
					h = disp_dev.height;
					blt(dp,x,y,w,h,sp);
					
					i = 0;
					
					while(1)
					{
						p = (base_touch *)get_ctrl_num(i);
						
						if(p)
						{
							
							if(p->draw_screen)
							{
								if(p->screen_update)
								{
									p->screen_update -= 1;
								}
							    xSemaphoreTake(p->ctrl.mutex_sem,portMAX_DELAY);
								p->draw_screen(p);
								xSemaphoreGive(p->ctrl.mutex_sem);
							}
							
							
							
						}
						else
						{
						    if(disp_dev.dir == 1)
                        	{
                        	    v2h();
                        	    
                        	}
                        	
							disp_dev.switch_base_addr();
							delay_ms(7);
							break;
						}
						
						i++;
						
					}
				
//					check_mem_info();
					
				}
				
				
				
				i++;

			}
			else
			{
				delay_ms(3);
				break;
			}

		}

	}
	
}




static void buzzer_off_timer(TimerHandle_t timer)
{
	if(bz_dev.flag2 == 0)
	{
		bz_dev.act(0);
	}
	bz_dev.flag1 = 0;
	
}





void message_task(void *pvParameters)
{

	t_msg msg;
	
	uint8_t res = 1;
	uint16_t dx,dy,ux,uy;
	TimerHandle_t timer = NULL;
	//disp_dev.set_light(g_project->light);
	dx = 0xffff;
	dy = 0xffff;
	
	
	if(g_project->buzzer)
	{
		timer =  xTimerCreate((const char *) "buzzer",
								(TickType_t)50 * MS,
								(UBaseType_t)pdFALSE,
								(void*	)NULL,
								(TimerCallbackFunction_t)buzzer_off_timer);
	}
	
	
	
	
	
	while(1)
	{

		msg_dev.read_message((void *)&msg,portMAX_DELAY );
		

		switch (msg.msg)
		{
			case MSG_TOUCH:
				touch_act(msg);
				break;
			case MSG_TP:
				res = 1;
				res = ctrl_msg(&msg);
				
				if(msg.c == 1)
				{
					
					if(g_project->buzzer == 1)
					{
						if(bz_dev.flag1)
						{
//							xTimerReset(timer,3);
							
						}
						else
						{
							bz_dev.act(1);
							bz_dev.flag1 = 1;
							xTimerStart(timer,3);
							
						}
					}
					else if((g_project->buzzer == 2) && (res == 0))
					{
						if(bz_dev.flag1)
						{
//							xTimerReset(timer,3);
//							xTimerStart(timer,3);
							
							
						}
						else
						{
							bz_dev.act(1);
							bz_dev.flag1 = 1;
							xTimerStart(timer,3);
							
						}
					}
					
					
					
					
					if(res /*&& (small_keyboard.active == 0) && (full_keyboard.active == 0) */) 
					{
						dx = msg.a;   //��¼����ʱ����
						dy = msg.b;
					}
				}
				else if(msg.c == 0)
				{
					ux = msg.a;     //��¼̧��ʱ����
					uy = msg.b;
					
					if(dx != 0xffff && dy != 0xffff)
					{
						if(g_project->slider_switch == 1)
						{
							if(ux > dx)
							{
								if((ux - dx) > (disp_dev.width / 5))
								{
									
									if(src_cfg.current_screen > 0)
									{
										src_cfg.new_screen = src_cfg.current_screen - 1;      //??????
										//src_cfg.pic_now = get_pic_num(screen_id);
									}
									
								}
							}
							else
							{
								if((dx - ux) > (disp_dev.width / 5))
								{
									if(src_cfg.current_screen < src_cfg.ptscreen->screen_num - 3)
									{
										src_cfg.new_screen = src_cfg.current_screen + 1;    // ??????
										//src_cfg.pic_now = get_pic_num(screen_id);
									}
								}
							}
						}
						else if(g_project->slider_switch == 2)
						{
							if(uy > dy)
							{
								if((uy - dy) > disp_dev.height / 5)
								{
									if(src_cfg.current_screen > 0)
									{
										src_cfg.new_screen = src_cfg.current_screen - 1; 
										//src_cfg.pic_now = get_pic_num(screen_id);
									}
								}
							}
							else
							{
								if((dy - uy) > disp_dev.height / 5)
								{
									if(src_cfg.current_screen < src_cfg.ptscreen->screen_num - 3)
									{
										src_cfg.new_screen += 1;
										//src_cfg.pic_now = get_pic_num(screen_id);
									}
								}
							}
								
						}
					}
					dx = 0xffff;
					dy = 0xffff;
				}
				break;
			default:
				break;
		}
		
		
		

	}
}





/***************************************************************************************
* End of File: my_task.c

****************************************************************************************/
