
/***************************************************************************************
****************************************************************************************
* Project      : 20180525_HMI
* FILE         : rtc.c
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
#include "rtc.h"
//#include "lpc177x_8x_rtc.h"
#include "rtc.h"
#include "mem_my.h"
#include "cfg.h"
#include "string.h"
#include "cmd.h"



extern RTC_HandleTypeDef hrtc;

static void rtc_init(void);
static void rtc_set_time(rtc_time time);
static void rtc_get_time(rtc_time * time);





static const char * rtc_format_string[8] =
{
    "%h:%m:%s",
    "%y-%n-%d",
    "%y-%n-%d %h:%m:%s",
    "%y-%n-%d %h:%m:%s %w",
    "%w",
    "%h:%m:%s",
    "%m:%s",
    "%s"
};


static uint8_t rtc_week_ch[7][3] =
{
	{'S','U','N'},
    {'M','O','N'},
    {'T','U','E'},
    {'W','E','D'},
    {'T','H','U'},
    {'F','R','I'},
    {'S','A','T'},
    
};



static uint8_t rtc_week_hz[14][6] =
{
	{0XD0,0XC7,0XC6,0XDA,0XC8,0XD5},
    {0XD0,0XC7,0XC6,0XDA,0XD2,0XBB},
    {0XD0,0XC7,0XC6,0XDA,0XB6,0XFE},
    {0XD0,0XC7,0XC6,0XDA,0XC8,0XFD},
    {0XD0,0XC7,0XC6,0XDA,0XCB,0XC4},
    {0XD0,0XC7,0XC6,0XDA,0XCE,0XE5},
    {0XD0,0XC7,0XC6,0XDA,0XC1,0XF9},
    {0X66,0X1F,0X67,0X1F,0X59,0X29},
    {0X66,0X1F,0X67,0X1F,0X4E,0X00},
    {0X66,0X1F,0X67,0X1F,0X4E,0X8C},
    {0X66,0X1F,0X67,0X1F,0X4E,0X09},
    {0X66,0X1F,0X67,0X1F,0X56,0XDB},
    {0X66,0X1F,0X67,0X1F,0X4E,0X94},
    {0X66,0X1F,0X67,0X1F,0X51,0X6D},

};




rtc_device rtc_dev =
{
	rtc_init,
	rtc_set_time,
	rtc_get_time,
};





void RTC_IRQHandler(void)
{
	BaseType_t temp;
	
//	if (RTC_GetIntPending(LPC_RTC, RTC_INT_COUNTER_INCREASE))
//	{
//		RTC_ClearIntPending(LPC_RTC, RTC_INT_COUNTER_INCREASE);
//		rtc_dev.get_time(&rtc_dev.time);
//		//rtc_dev.update_flag = 1;
//		xSemaphoreGiveFromISR(rtc_dev.sem, &temp);
//	}
	if(__HAL_RTC_WAKEUPTIMER_GET_FLAG(&hrtc,RTC_FLAG_WUTF))
	{
		 __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&hrtc, RTC_FLAG_WUTF);
		 rtc_dev.get_time(&rtc_dev.time);

		xSemaphoreGiveFromISR(rtc_dev.sem, &temp);	
	}
	
	
	
}




static void upload_rtc_counter(rtc_tp * prtc)
{
	uint8_t * p = my_malloc(12);
	
	p[0] = 0xee;
	p[1] = 0x43;
	p[2] = prtc->ctrl.id >> 24;
	p[3] = prtc->ctrl.id >> 16;
	p[4] = prtc->ctrl.id >> 8;
	p[5] = prtc->ctrl.id & 0xff;
	p[6] = 0x17;
	p[7] = 0xff;
	p[8] = 0xfc;
	p[9] = 0xff;
	p[10] = 0xff;
	p[11] = 0x3b;
	
	cmd_dev.notify_cmd(p,12);
	
	
	
	my_free(p);
	
}



void update_rtc_task(void *pvParameters)
{	
    uint16_t i;
	base_touch * p;
	rtc_tp * prtc;
	
	i = 0;
	
	while(1)
	{
		xSemaphoreTake(rtc_dev.sem,portMAX_DELAY);

		while(1)
		{
		
    		p = (base_touch *)get_ctrl_num(i);

    		if(p)
    		{
    			if(p->type == CFG_RTC)
    			{
    				prtc = (rtc_tp *) p;
    		            
    				if(prtc->format > 4 && prtc->format < 8)
    				{
    					if(prtc->rtc_type.counter.start)
    					{
    						if(prtc->rtc_type.counter.counter_value < prtc->rtc_type.counter.counter)
    						{
    							prtc->rtc_type.counter.counter_value += 1;
    							
    							if(prtc->rtc_type.counter.counter_value == prtc->rtc_type.counter.counter)
    							{
    								upload_rtc_counter(prtc);
    							}
							
    							
    						}
    						else
    						{
    							prtc->rtc_type.counter.start = 0;
    							prtc->rtc_type.counter.counter_value = 0;
    						}
    						
    						if(prtc->ctrl.id == src_cfg.current_screen)
    						{
    							if(src_cfg.current_screen == src_cfg.new_screen)
    							{
    								p->screen_update = 1;
    							}
    						}
    						
    						
        				}
    		                
    				}
    				else
    				{
    					prtc->rtc_type.clock.time = rtc_dev.time;
    					p->screen_update = 1;
    						
    					
    							
    				}
    		            
    			}
				
				i++;
			
    		}			
    		else
    		{
				i = 0;
    			break;
    		}

    		
	    }
	}
	
	
}




static void rtc_init(void)
{
#if 0
	//st
//	RTC_TIME_Type RTCFullTime;
	RTC_Init(LPC_RTC);
	NVIC_DisableIRQ(RTC_IRQn);
	NVIC_SetPriority(RTC_IRQn, 15);

	RTC_ResetClockTickCounter(LPC_RTC);   
	RTC_Cmd(LPC_RTC, ENABLE); 
	RTC_CalibCounterCmd(LPC_RTC, DISABLE);
	

//	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_SECOND, 00);    
//	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_MINUTE, 59);    
//	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_HOUR, 23);    
//	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_MONTH, 2);    
//	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_YEAR, 2017);   
//	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_DAYOFMONTH, 28);
//	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_DAYOFWEEK, 0);

//	RTC_SetAlarmTime (LPC_RTC, RTC_TIMETYPE_SECOND, 1);
	RTC_CntIncrIntConfig (LPC_RTC, RTC_TIMETYPE_SECOND, ENABLE);
	RTC_AlarmIntConfig (LPC_RTC, RTC_TIMETYPE_SECOND, DISABLE);
	
//	rtc_dev.timer = xTimerCreate((const char *) "rtc",
//								(TickType_t)1000,
//								(UBaseType_t)pdTRUE,
//								(void*	)NULL,
//								(TimerCallbackFunction_t)update_rtc);
//	xTimerStart(rtc_dev.timer,10);

    rtc_dev.sem = xSemaphoreCreateBinary();
	NVIC_EnableIRQ(RTC_IRQn);
	
#endif
	
}







static void rtc_set_time(rtc_time time)
{
#if 0
	LPC_RTC->SEC   = time.sec & RTC_SEC_MASK;
	LPC_RTC->MIN   = time.min & RTC_MIN_MASK;
	LPC_RTC->HOUR  = time.hour & RTC_HOUR_MASK;
	LPC_RTC->DOW   = time.week & RTC_DOW_MASK;
	LPC_RTC->DOM   = time.day  & RTC_DOM_MASK;
	LPC_RTC->MONTH = time.month & RTC_MONTH_MASK;
	LPC_RTC->YEAR  = time.year & RTC_YEAR_MASK;
#endif

	RTC_TimeTypeDef RTC_TimeStructure;

	RTC_TimeStructure.Hours= time.hour ;
	RTC_TimeStructure.Minutes=time.min;
	RTC_TimeStructure.Seconds=time.sec;
	RTC_TimeStructure.TimeFormat=RTC_HOURFORMAT12_AM;
	RTC_TimeStructure.DayLightSaving=RTC_DAYLIGHTSAVING_NONE;
	RTC_TimeStructure.StoreOperation=RTC_STOREOPERATION_RESET;


	RTC_DateTypeDef RTC_DateStructure;

	RTC_DateStructure.Date=time.day;
	RTC_DateStructure.Month=time.month;
	RTC_DateStructure.WeekDay= time.week;
	RTC_DateStructure.Year=time.year;

}








static void rtc_get_time(rtc_time * time)
{
#if 0
	//lpc
	time->sec	= LPC_RTC->SEC & RTC_SEC_MASK;
	time->min   = LPC_RTC->MIN & RTC_MIN_MASK;
	time->hour  = LPC_RTC->HOUR & RTC_HOUR_MASK;
	time->week  = LPC_RTC->DOW & RTC_DOW_MASK;
	time->day   = LPC_RTC->DOM & RTC_DOM_MASK;   
	time->month = LPC_RTC->MONTH & RTC_MONTH_MASK; 
	time->year  = LPC_RTC->YEAR & RTC_YEAR_MASK;	
#endif
	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_DateTypeDef RTC_DateStruct;

	HAL_RTC_GetTime(&hrtc,&RTC_TimeStruct,RTC_FORMAT_BIN);

	HAL_RTC_GetDate(&hrtc, &RTC_DateStruct, RTC_FORMAT_BIN);
	time->day = RTC_DateStruct.Date;
	time->month = RTC_DateStruct.Month;
	time->week = RTC_DateStruct.WeekDay;
	time->year = RTC_DateStruct.Year;
	time->hour = RTC_TimeStruct.Hours;
	time->min = RTC_TimeStruct.Minutes;
	time->sec = RTC_TimeStruct.Seconds;
}









static uint8_t rtc_msg( void * p, t_msg * p_msg )
{
	
    return 1;
}



static void rtc_release( void* pCtrl )
{
    rtc_tp * prtc = (rtc_tp *)pCtrl;

    if(prtc->format > 4 && prtc->format < 8)
    {
        if(prtc->rtc_type.counter.code)
        {
            my_free((void *)prtc->rtc_type.counter.code);
        }
    }
    else
    {
        if(prtc->rtc_type.clock.code)
        {
            my_free((void *)prtc->rtc_type.clock.code);
        }
    }

    my_free((void *) prtc);
    
}





static void rtc_act(base_touch *p,uint8_t flag)
{
    if(p->type != CFG_RTC)
    {
        return;
    }
}





static void rtc_draw_screen(base_touch * tp)
{
    if(tp->type != CFG_RTC)
    {
        return;
    }
	
    uint8_t num,i,week;
    uint16_t x,y,width;
    uint8_t ch;
    uint16_t hz,color;
    uint8_t * p;
    font_type  *pfont,*hz_font;
	font_type *ch_font;
    rtc_tp * prtc = (rtc_tp *) tp;

    x = prtc->tp_rect.left;
    y = prtc->tp_rect.top;
    
    if((prtc->format > 4)&&(prtc->format < 8))
    {
        num = prtc->rtc_type.counter.code_num;
        p = prtc->rtc_type.counter.code;
		if(prtc->rtc_type.counter.ptfont == NULL)
		{
			return;
			
		}
		pfont = prtc->rtc_type.counter.ptfont;
        ch_font = hz_to_ch(pfont);
		if(ch_font == NULL)
		{
			return;
		}
        color = prtc->rtc_type.counter.color;
        
    }
    else
    {
        num = prtc->rtc_type.clock.code_num;
        p = prtc->rtc_type.clock.code;
		if(prtc->rtc_type.clock.ptfont == NULL)
		{
			return;
		}
		pfont = prtc->rtc_type.clock.ptfont;
        ch_font = hz_to_ch(pfont);
		if(ch_font == NULL)
		{
			return;
		}
        color = prtc->rtc_type.clock.color;
    }
    
    width = get_font_width(*ch_font);
    hz_font = ch_to_hz(pfont);

    if((prtc->format < 5)||(prtc->format == 8))
    {
    
        for(i=0;i<num;i++)
        {
            if(p[i]!='%')
            {
                ch = p[i];
                show_charn_font(x,y,color,ch,*ch_font);
                x += width;
            }
            else
            {
                switch(p[i+1])
                {
                    case 'y':
                        ch = prtc->rtc_type.clock.time.year / 1000 + '0';
                        show_charn_font(x,y,color,ch,*ch_font);
                        x += width;
                        ch = prtc->rtc_type.clock.time.year % 1000 / 100 + '0';
                        show_charn_font(x,y,color,ch,*ch_font);
                        x += width;
                        ch = prtc->rtc_type.clock.time.year % 100 / 10 + '0';
                        show_charn_font(x,y,color,ch,*ch_font);
                        x += width;
                        ch = prtc->rtc_type.clock.time.year % 10 + '0';
                        show_charn_font(x,y,color,ch,*ch_font);
                        x += width;
                        i++;
                        break;
                   case 'n':
                        ch = prtc->rtc_type.clock.time.month / 10 + '0';
                        show_charn_font(x,y,color,ch,*ch_font);
                        x += width;
                        ch = prtc->rtc_type.clock.time.month % 10+'0';
                        show_charn_font(x,y,color,ch,*ch_font);
                        x += width;
                        i++;
                        break;
                   case 'd':
                        ch = prtc->rtc_type.clock.time.day / 10 + '0';
                        show_charn_font(x,y,color,ch,*ch_font);
                        x += width;
                        ch = prtc->rtc_type.clock.time.day % 10 + '0';
                        show_charn_font(x,y,color,ch,*ch_font);
                        x += width;
                        i++;
                        break;
                   case 'h':
                        ch = prtc->rtc_type.clock.time.hour / 10 + '0';
                        show_charn_font(x,y,color,ch,*ch_font);
                        x += width;
                        ch = prtc->rtc_type.clock.time.hour % 10 + '0';
                        show_charn_font(x,y,color,ch,*ch_font);
                        x += width;
                        i++;
                        break;
                  case 'm':
                        ch = prtc->rtc_type.clock.time.min / 10 + '0';
                        show_charn_font(x,y,color,ch,*ch_font);
                        x += width;
                        ch = prtc->rtc_type.clock.time.min % 10 + '0';
                        show_charn_font(x,y,color,ch,*ch_font);
                        x += width;
                        i++;
                        break; 
                  case 's':
                        ch = prtc->rtc_type.clock.time.sec / 10 + '0';
                        show_charn_font(x,y,color,ch,*ch_font);
                        x += width;
                        ch = prtc->rtc_type.clock.time.sec % 10 + '0';
                        show_charn_font(x,y,color,ch,*ch_font);
                        x += width;
                        i++;
                        break; 
				  case 'w':
                 
                    week = prtc->rtc_type.clock.time.week;

                    if(prtc->format == 4)
                    {
                        ch = rtc_week_ch[week][0];
                        show_charn_font(x,y,color,ch,*ch_font);
                        x += width;
                        ch = rtc_week_ch[week][1];
                        show_charn_font(x,y,color,ch,*ch_font);
                        x += width;
                        ch = rtc_week_ch[week][2];
                        show_charn_font(x,y,color,ch,*ch_font);
                        x += 2*width;

                        
						if(hz_font)
						{
						    if(hz_font->code < 3)
						    {
						        hz = (rtc_week_hz[week][0] << 8) | (rtc_week_hz[week][1]);
						    }
						    else
						    {
						        hz = (rtc_week_hz[week+7][0] << 8) | (rtc_week_hz[week+7][1]);
						    }
							show_nfont(x,y,color,hz,*hz_font);
						}
                        x += 2*width;
                        
                        if(hz_font)
						{
						    if(hz_font->code < 3)
						    {
						        hz = (rtc_week_hz[week][2] << 8) | (rtc_week_hz[week][3]);
						    }
						    else
						    {
						        hz = (rtc_week_hz[week+7][2] << 8) | (rtc_week_hz[week+7][3]);
						    }
							show_nfont(x,y,color,hz,*hz_font);
						}
                        x += 2*width;
                        
                        if(hz_font)
						{
						    if(hz_font->code < 3)
						    {
						        hz = (rtc_week_hz[week][4] << 8) | (rtc_week_hz[week][5]);
						    }
						    else
						    {
						        hz = (rtc_week_hz[week+7][4] << 8) | (rtc_week_hz[week+7][5]);
						    }
							show_nfont(x,y,color,hz,*hz_font);
						}
                        x += 2*width;
						//prtc->tp_rect.right = x;
                    }
                    else
                    {
                    
                        if(prtc->rtc_type.clock.language == 0)
                        {
                            if(hz_font)
							{
							    if(hz_font->code < 3)
							    {
							        hz = (rtc_week_hz[week][0] << 8) | (rtc_week_hz[week][1]);
							    }
							    else
							    {
							        hz = (rtc_week_hz[week+7][0] << 8) | (rtc_week_hz[week+7][1]);
							    }
								show_nfont(x,y,color,hz,*hz_font);
							}
                            x += 2*width;
                            
                            if(hz_font)
							{
							    if(hz_font->code < 3)
							    {
							        hz = (rtc_week_hz[week][2] << 8) | (rtc_week_hz[week][3]);
							    }
							    else
							    {
							        hz = (rtc_week_hz[week+7][2] << 8) | (rtc_week_hz[week+7][3]);
							    }
								show_nfont(x,y,color,hz,*hz_font);
							}
                            x += 2*width;
                            
                            if(hz_font)
							{
							    if(hz_font->code < 3)
							    {
							        hz = (rtc_week_hz[week][4] << 8) | (rtc_week_hz[week][5]);
							    }
							    else
							    {
							        hz = (rtc_week_hz[week+7][4] << 8) | (rtc_week_hz[week+7][5]);
							    }
								show_nfont(x,y,color,hz,*hz_font);
							}
                            x += 2*width;
                        
                        
                        }
                        else
                        {
                            ch = rtc_week_ch[week][0];
                            show_charn_font(x,y,color,ch,*pfont);
                            x += width;
                            ch = rtc_week_ch[week][1];
                            show_charn_font(x,y,color,ch,*pfont);
                            x += width;
                            ch = rtc_week_ch[week][2];
                            show_charn_font(x,y,color,ch,*pfont);
                            x += width;
                        }
                    }
                    i++;
                    break;
                 default:
                    ch = p[i];
                    show_charn_font(x,y,color,ch,*pfont);
                    x += width;
                    break;
                    
                        
                        
                        
                }
            }
         }
    }
    else
    {
        uint32_t count;

		
        if(prtc->rtc_type.counter.count_type == 0)
        {
            count = prtc->rtc_type.counter.counter - prtc->rtc_type.counter.counter_value;
            
        }
        else
        {
            count = prtc->rtc_type.counter.counter_value;
        }
        if(1/*prtc->format != 7*/)
        {
            uint8_t h,m,s;

            s = count % 60;
            m = count / 60 % 60;
            h = count / 60 / 60 % 60;

            if(prtc->format == 5)
            {
                ch = h / 10 + '0';
                show_charn_font(x,y,color,ch,*ch_font);
                x += width;
                ch = h % 10 + '0';
                show_charn_font(x,y,color,ch,*ch_font);
                x += width;
                ch = ':';
                show_charn_font(x,y,color,ch,*ch_font);
                x += width;
            }
            if(prtc->format == 5 || prtc->format == 6)
            {
                ch = m / 10 + '0';
                show_charn_font(x,y,color,ch,*ch_font);
                x += width;
                ch = m % 10 + '0';
                show_charn_font(x,y,color,ch,*ch_font);
                x += width;
                ch = ':';
                show_charn_font(x,y,color,ch,*ch_font);
                x += width;
             }
            ch = s / 10 + '0';
            show_charn_font(x,y,color,ch,*ch_font);
            x += width;
            ch = s % 10 + '0';
            show_charn_font(x,y,color,ch,*ch_font);
            x += width;
            
        }
        else
        {
            
        }
    }
	
	prtc->tp_rect.right = x;
	//prtc->back_image.width  = prtc->tp_rect.right - prtc->tp_rect.left + 1;
	//prtc->screen_update = 0;
       
}









		

			
    
	













uint8_t rtc_create(uint8_t * buffer)
{
    rtc_tp  * prtc = (rtc_tp *) my_malloc(sizeof(rtc_tp));
	
	extern t_ctrl * g_pctrl_rtc;
   

    if(prtc == NULL)
    {
        return 1;
    }
	
	g_pctrl_rtc = (t_ctrl *) prtc;

    memset((void *)prtc,0,sizeof(rtc_tp));

    prtc->ctrl.id = ((buffer[0]<<24)|(buffer[1]<<16)|(buffer[4]<<8)|(buffer[5])<<0);
    prtc->ctrl.func_ctrl_procmsg = rtc_msg;
    prtc->ctrl.func_ctrl_release = rtc_release;
    

    prtc->tp_type = buffer[6];
    prtc->tp_rect.left = (buffer[7]<<8) | (buffer[8]);
    prtc->tp_rect.top = (buffer[9]<<8) | (buffer[10]);
    prtc->tp_rect.right = (buffer[11]<<8) | (buffer[12]);
    prtc->tp_rect.bottom = (buffer[13]<<8) | (buffer[14]);
    prtc->pic_id = (buffer[2]<<8) | (buffer[3]);
    prtc->pic_on = (buffer[15]<<8) | (buffer[16]);

    prtc->act = rtc_act;
    prtc->draw_screen = rtc_draw_screen;
    //prtc->update_screen = rtc_update_screen;
    //prtc->cmd_update_status = cmd_update_rtc_status;
    //prtc->cmd_get_status = cmd_get_rtc_status;
	prtc->active = 1;
    
    //prtc->back_image.x = prtc->tp_rect.left;
    //prtc->back_image.y = prtc->tp_rect.top;
    //prtc->back_image.width = prtc->tp_rect.right - prtc->tp_rect.left + 1;
    //prtc->back_image.height = prtc->tp_rect.bottom - prtc->tp_rect.top + 1;
    //prtc->back_image.vdc = disp_dev.disp[2];
    prtc->format = buffer[17];


    if(prtc->format < 5)
    {
        prtc->rtc_type.clock.language = buffer[18];
        prtc->rtc_type.clock.time_lag = buffer[19];
        prtc->rtc_type.clock.ptfont = get_font_type(buffer[20]);
        prtc->rtc_type.clock.color = (buffer[21] << 8) | (buffer[22]);
        prtc->rtc_type.clock.set_type = buffer[23];
        prtc->rtc_type.clock.code_num = strlen(rtc_format_string[prtc->format]);
        prtc->rtc_type.clock.code = (uint8_t *)my_malloc(prtc->rtc_type.clock.code_num);
        memcpy(prtc->rtc_type.clock.code,rtc_format_string[prtc->format],prtc->rtc_type.clock.code_num);
         
    }
    else if(prtc->format < 8)
    {	
        prtc->rtc_type.counter.counter = (buffer[18]<<24) | (buffer[19]<<16) | (buffer[20]<<8) | (buffer[21]);
        prtc->rtc_type.counter.count_type = buffer[22];
        prtc->rtc_type.counter.ptfont = get_font_type(buffer[23]);
        prtc->rtc_type.counter.color = (buffer[24]<<8)|(buffer[25]);
        prtc->rtc_type.counter.code_num = strlen(rtc_format_string[prtc->format]);
        prtc->rtc_type.counter.code = (uint8_t *)(uint8_t *)my_malloc( prtc->rtc_type.counter.code_num);
        memcpy(prtc->rtc_type.counter.code,rtc_format_string[prtc->format],prtc->rtc_type.counter.code_num);
        prtc->rtc_type.counter.counter_value = 0;
        prtc->rtc_type.counter.start = 0;
    }
       
    else
    {

		
		prtc->rtc_type.clock.language = buffer[18];
        prtc->rtc_type.clock.time_lag = buffer[19];
        prtc->rtc_type.clock.ptfont = get_font_type(buffer[20]);
        prtc->rtc_type.clock.color = (buffer[21] << 8) | (buffer[22]);
        prtc->rtc_type.clock.set_type = buffer[23];
		prtc->rtc_type.clock.code_num = buffer[24];
		prtc->rtc_type.clock.code = (uint8_t *)my_malloc(prtc->rtc_type.clock.code_num);
		memcpy(prtc->rtc_type.clock.code,&buffer[25],prtc->rtc_type.clock.code_num);
        
    }
    if(prtc->format < 5 || prtc->format == 8)
    { 
		  prtc->rtc_type.clock.time  = rtc_dev.time;    
        
    }
    prtc->state = 0;
	
	ctrl_init((t_ctrl*) prtc);
	
	return 0;
    
    
}




void create_rtc_touch(void)
{
	
	uint8_t *p;
	uint8_t  type;
	uint16_t len;

	p = src_cfg.p_touch;
	
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
		
		
		type = p[8];

		if(type == 0xff)
		{
		    return;
		}

        if(type == CFG_RTC)
		{
	        rtc_create(&p[2]);

	    }
					
		
		p += len;
		
	}
	
}




/***************************************************************************************
* End of File: rtc.c

****************************************************************************************/
