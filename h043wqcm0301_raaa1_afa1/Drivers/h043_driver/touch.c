/***************************************************************************************
2021-06-18 移植
****************************************************************************************/
#include "touch.h"
//#include "lpc177x_8x_ssp.h"
//#include "lpc177x_8x_pinsel.h"
//#include "lpc177x_8x_gpio.h"
//#include "lpc177x_8x_i2c.h"
#include "cmd.h"
#include "message.h"
#include "cfg.h"
#include "timer.h"
#include "mem_my.h"
#include "display.h"
#include "hmi_config.h"
#include "gpio.h"
#include "i2c.h"
#include "orientation.h"

#define CTP_RST0_PORT GPIOA
#define CTP_RST0_Pin GPIO_PIN_12

//#define TP_INT_IN           ( GPIO_ReadValue(0) & (1<<13) )
#if TP_TYPE_DRIVER == TP_TYPE_ST1633I
#define SLAVE_ADDR          (0x55)
#elif  TP_TYPE_DRIVER == TP_TYPE_FT5426
#define SLAVE_ADDR          (0x38)
#endif

#define I2CDEV1             (1)



static void touch_init(void);
static uint8_t get_pos(void);
static uint32_t get_tp_seamphore(uint32_t time);
static uint8_t tp_err_cnt=0;
//static short cur_screen=0;
static uint8_t bkl_flag=0;

static int ST1633I_i2c2_read(uint16_t addr, uint8_t  reg, uint8_t *rbuf, uint16_t len);
static int ST1633I_i2c2_write(uint16_t addr, uint8_t *data, uint16_t len);

tp_device tp_dev =
{
    touch_init,
    get_pos,
    get_tp_seamphore,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,

};





void GPIO_IRQHandler(void)
{
#if 0
	//lpc
	BaseType_t temp;


	if(GPIO_GetIntStatus(0, 13, 1))
	{
		GPIO_ClearInt(0, 1<<13);
		LPC_GPIOINT->IO0IntEnF &= ~(1 << 13);
		xSemaphoreGiveFromISR(tp_dev.tp_press, &temp);

	}
#if  (DIR_SWITCH_SUPPORT)

	extern SemaphoreHandle_t  lis3dh_sem;
	if(GPIO_GetIntStatus(0, 23, 1))
	{
		GPIO_ClearInt(0, 1<<23);
		LPC_GPIOINT->IO0IntEnF &= ~(1 << 23);
		xSemaphoreGiveFromISR(lis3dh_sem, &temp);
	}
#endif
#endif



}


static void upload_pos(void)
{
	uint8_t  * buf = (uint8_t *) my_malloc(11);

	buf[0] = 0xee;
	buf[1] = 0x01;

	buf[2] = (tp_dev.x >> 8);
	buf[3] = (tp_dev.x & 0xff);
	buf[4] = (tp_dev.y >> 8);
	buf[5] = (tp_dev.y & 0xff);

	buf[6] = 0xff;
	buf[7] = 0xfc;
	buf[8] = 0xff;
	buf[9] = 0xff;

	buf[10] = 0x3b;

	cmd_dev.notify_cmd(buf,11);

    my_free((void *) buf);
}



static void upload_pos_timer(TimerHandle_t timer)
{
	upload_pos();
}


static void standby_act_timer(TimerHandle_t timer)
{
    uint16_t screen_id;
	g_project->light = g_project->standby_light;
	bkl_flag = 1;
	if(g_project->standby_screen != 0xffff)  //上位机配置了待机画面
	{
        //cur_screen = src_cfg.current_screen;        //wxy
		src_cfg.new_screen = g_project->standby_screen;
#if  (DIR_SWITCH_SUPPORT)
		if((src_cfg.ptscreen != NULL) && (src_cfg.ptscreen->HV_switch == 1))
		{//支持横竖屏切换
	        if(get_screen_HV(src_cfg.new_screen)==0)//横屏
	        {
	            if(disp_dev.dir==1)//竖屏
	            {
	                screen_id = get_Vscreen(src_cfg.new_screen);
	                src_cfg.new_screen = screen_id;
	            }
	        }
	        else if(get_screen_HV(src_cfg.new_screen)==0)//竖屏
	        {
	            if(disp_dev.dir==0)//横屏
	            {
	                screen_id = get_Hscreen(src_cfg.new_screen);
	                src_cfg.new_screen = screen_id;
	            }
	        }
		}
#endif
	}
}


static uint32_t get_tp_seamphore(uint32_t time)
{
	return xSemaphoreTake(tp_dev.tp_press,time);
}





#if (TP_TYPE_DRIVER == TP_TYPE_ST1633I)


static void touch_init(void)
{
#if 0
	//lpc
    /*i2c init*/
    PINSEL_ConfigPin (0, 19, 3);
    PINSEL_ConfigPin (0, 20, 3);
    PINSEL_SetOpenDrainMode(0, 19, ENABLE);
    PINSEL_SetOpenDrainMode(0, 20, ENABLE);
    PINSEL_SetPinMode(0, 19, PINSEL_BASICMODE_PLAINOUT);
    PINSEL_SetPinMode(0, 20, PINSEL_BASICMODE_PLAINOUT);
    I2C_Init((en_I2C_unitId)I2CDEV1, 400000);
    I2C_Cmd((en_I2C_unitId)I2CDEV1, I2C_MASTER_MODE, ENABLE);


    /*reset gpio*/
    PINSEL_ConfigPin(0, 16, 0);
    GPIO_SetDir(0, (1<<16), 1);
	GPIO_OutputValue(0, (1<<16), 1);


	GPIO_OutputValue(0, (1<<16), 0);
	delay_us(2);
	GPIO_OutputValue(0, (1<<16), 1);
	delay_xms(50);
#endif

	HAL_GPIO_WritePin( CTP_RST0_PORT , CTP_RST0_Pin , RESET);
	delay_us(2);
	HAL_GPIO_WritePin(  CTP_RST0_PORT , CTP_RST0_Pin , SET);
	delay_xms(50);
}






uint8_t get_pos(void)
{
#if 0
	//lpc
	uint8_t res = 0;
	uint8_t pos[5];

    I2C_M_SETUP_Type transferMCfg;
	transferMCfg.sl_addr7bit = SLAVE_ADDR;
    transferMCfg.tx_data = NULL;
    transferMCfg.tx_length = 0;
    transferMCfg.rx_data = pos;
    transferMCfg.rx_length = 5;
    transferMCfg.retransmissions_max = 3;
	//taskENTER_CRITICAL();
    if (I2C_MasterTransferData((en_I2C_unitId)I2CDEV1, &transferMCfg, I2C_TRANSFER_POLLING) == SUCCESS)
    {
        tp_err_cnt=0;
        res = 0;
		tp_dev.x = (((pos[2] & 0x70) >> 4)<<8) | pos[3];
		tp_dev.y = ((pos[2] & 0x7)<<8) | pos[4];
        tp_dev.x = disp_dev.width0 - tp_dev.x - 1;
		tp_dev.y = disp_dev.height0 - tp_dev.y -  1;

//#ifndef HMI_APP_DEBUG
//
//		tp_dev.x = disp_dev.width0 - tp_dev.x - 1;
//		tp_dev.y = disp_dev.height0 - tp_dev.y -  1;
//#endif

		if(disp_dev.dir == 1) //竖屏情况下，进行坐标装换
		{
		    uint16_t x,y;

		    x = tp_dev.x;
		    y = tp_dev.y;

		    tp_dev.x = y;
		    tp_dev.y = disp_dev.width0 - x - 1;

		}

    }
    else
    {
        tp_err_cnt++;
        if(tp_err_cnt>2)
        {
            tp_err_cnt = 0;
            touch_init();
        }
        res = 1;
    }
	//taskEXIT_CRITICAL();

	return res;
#endif
	uint8_t res = 0;
	uint8_t pos[5];
	uint8_t i2c_res = 0;

	i2c_res  = ST1633I_i2c2_read(SLAVE_ADDR,0,pos,5);

	if(i2c_res == 0)
	{
	    tp_err_cnt=0;
		res = 0;
		tp_dev.x = (((pos[2] & 0x70) >> 4)<<8) | pos[3];
		tp_dev.y = ((pos[2] & 0x7)<<8) | pos[4];
		tp_dev.x = disp_dev.width0 - tp_dev.x - 1;
		tp_dev.y = disp_dev.height0 - tp_dev.y -  1;
		if(disp_dev.dir == 1) //竖屏情况下，进行坐标装换
			{
			    uint16_t x,y;

			    x = tp_dev.x;
			    y = tp_dev.y;

			    tp_dev.x = y;
			    tp_dev.y = disp_dev.width0 - x - 1;

			}
	}
	else
	{
		 tp_err_cnt++;
		if(tp_err_cnt>2)
		{
			tp_err_cnt = 0;
			touch_init();
		}
		res = 1;

	}

}


void touch_task(void *pvParameters)
{

	uint8_t res;
	t_msg msg;
	uint8_t flag = 0;
	TimerHandle_t timer0 = NULL;
	uint32_t count,result;



	tp_dev.tp_press = xSemaphoreCreateBinary();




	 /*int*/
#if 0
	//lpc
	PINSEL_ConfigPin(0, 13, 0);
	GPIO_SetDir(0, (1<<13), 0);
	GPIO_IntCmd(0,(1<<13), 1);
	NVIC_SetPriority(GPIO_IRQn, 15);
	NVIC_EnableIRQ(GPIO_IRQn);
#endif
	HAL_NVIC_EnableIRQ(EXTI3_IRQn);

	g_project->timer =  xTimerCreate((const char *) "bl",
							(TickType_t)g_project->standby_time*1000*MS,
							(UBaseType_t)pdFALSE,
							(void*	)NULL,
							(TimerCallbackFunction_t)standby_act_timer);   //创建自动调节背光时钟
	if(g_project->auto_bl)
	{
		xTimerStart(g_project->timer,10); 
	}


	if(g_project->touch_nortify == 2)
	{
		timer0 =  xTimerCreate((const char *) "touch",
								(TickType_t)100,
								(UBaseType_t)pdTRUE,
								(void*	)NULL,
								(TimerCallbackFunction_t)upload_pos_timer);
		 xTimerStart(timer0,10);
	}

	count = portMAX_DELAY;

	while(1)
	{
		result = tp_dev.get_seamphore(count);

		if(result == pdTRUE)
		{

    		res = tp_dev.get_pos();

    		if(g_project->auto_bl)
    		{
    			if(bkl_flag==1)
					{
							bkl_flag =0;
//                    if(g_project->light == g_project->active_light)
//                    {
//                        xTimerReset(g_project->timer,10);
//                    }
//                    else //if(g_project->light == g_project->standby_light)
//                    {
									g_project->light = g_project->active_light;
									g_project->old_light = 0xFF;
									xTimerStart(g_project->timer,10);
									//src_cfg.new_screen = cur_screen;    //wxy,回到原来的界面
									flag = 1;  //从待机到正常模式的的按下触摸坐标不上报处理，
							//}
					}
					else
					{
							g_project->light = g_project->active_light;
							g_project->old_light = 0xFF;
							xTimerReset(g_project->timer,10);
					}

    		}
				else
				{
					g_project->light = g_project->active_light;
					g_project->old_light = 0xFF;
				}


    		if(res == 0)
    		{

    			if(tp_dev.x != tp_dev.oldx || tp_dev.y != tp_dev.oldy || tp_dev.old_sta != 2)
    			{
    				tp_dev.sta = tp_dev.old_sta ? 2 : 1;
    				msg.msg = MSG_TP;
    				msg.a = tp_dev.x;    //记录按下时的坐标
    				msg.b = tp_dev.y;

    				msg.c = tp_dev.sta;

    				if(flag == 0)
    				{
    					msg_dev.write_message((const void *) &msg,0);

    					if(msg.c == 1)
    					{
    						if(g_project->touch_nortify == 0 ||
    						   g_project->touch_nortify == 2 ||
    						   g_project->touch_nortify == 3)
    						{
    							upload_pos();
    						}
    						if(g_project->touch_nortify == 2)
    						{
    							xTimerReset(timer0,10);
    							xTimerStart(timer0,10);
    						}
    					}
    				}
    				else
    				{
    					flag = 0;
    				}
    				tp_dev.oldx = tp_dev.x;
    				tp_dev.oldy = tp_dev.y;
    				tp_dev.old_sta = tp_dev.sta;
    			}

    			count = 20;


    		}
		}
		else /*if((tp_dev.old_sta != 0) && (tp_dev.sta == 0) )*/
		{
			tp_dev.sta = 0;

			msg.msg = MSG_TP;
			msg.a = tp_dev.oldx;
			msg.b = tp_dev.oldy;
			msg.c = tp_dev.sta;



			msg_dev.write_message((const void *) &msg,0);
			if(g_project->touch_nortify == 1 || g_project->touch_nortify ==3)
			{
				upload_pos();
			}
			else if(g_project->touch_nortify == 2)
			{
				xTimerStop(timer0,10);
			}
//			tp_dev.oldx = tp_dev.x;
//			tp_dev.oldy = tp_dev.y;
			tp_dev.old_sta = tp_dev.sta;
			count = portMAX_DELAY;
			delay_ms(10);

		}

		delay_ms(25);
		HAL_NVIC_EnableIRQ(EXTI3_IRQn);


	}

}



#elif (TP_TYPE_DRIVER == TP_TYPE_FT5426)

uint8_t touch_down_flag = 0;


static void touch_init(void)
{
    /*i2c init*/
    PINSEL_ConfigPin (0, 19, 3);
    PINSEL_ConfigPin (0, 20, 3);
    PINSEL_SetOpenDrainMode(0, 19, ENABLE);
    PINSEL_SetOpenDrainMode(0, 20, ENABLE);
    PINSEL_SetPinMode(0, 19, PINSEL_BASICMODE_PLAINOUT);
    PINSEL_SetPinMode(0, 20, PINSEL_BASICMODE_PLAINOUT);
    I2C_Init((en_I2C_unitId)I2CDEV1, 100000);
    I2C_Cmd((en_I2C_unitId)I2CDEV1, I2C_MASTER_MODE, ENABLE);


    /*reset gpio*/
    PINSEL_ConfigPin(1, 19, 0);
    GPIO_SetDir(1, (1<<19), 1);
	GPIO_OutputValue(1, (1<<19), 1);


	GPIO_OutputValue(1, (1<<19), 0);
	delay_us(1100);
	GPIO_OutputValue(1, (1<<19), 1);
	delay_xms(50);
}










uint8_t get_pos(void)
{
	uint8_t res = 0,adr = 0x03;
	uint8_t pos[4];
	uint8_t flag,index;

    I2C_M_SETUP_Type transferMCfg;
	transferMCfg.sl_addr7bit = SLAVE_ADDR;
    transferMCfg.tx_data = &adr;
    transferMCfg.tx_length = 1;
    transferMCfg.rx_data = pos;
    transferMCfg.rx_length = 4;
    transferMCfg.retransmissions_max = 3;
    if (I2C_MasterTransferData((en_I2C_unitId)I2CDEV1, &transferMCfg, I2C_TRANSFER_POLLING) == SUCCESS)
    {
        flag = (pos[0] >> 6);
		index = pos[2] >> 4;

		if(index == 0)
		{

			switch(flag)
			{
				case 0:
					tp_dev.sta = 1;
					touch_down_flag = 1;
					break;
				case 1:
					tp_dev.sta = 0;
					touch_down_flag = 0;
					break;
				case 2:
					tp_dev.sta = 2;
					break;
				default:
					res = 1;
					break;

			}

			if(flag != 3)
			{
				tp_dev.x = ((pos[0] & 0x0F)<<8) | pos[1];
				tp_dev.y = ((pos[2] & 0x0F)<<8) | pos[3];
				if(disp_dev.dir == 1) //竖屏情况下，进行坐标装换
				{
					uint16_t x,y;

					x = tp_dev.x;
					y = tp_dev.y;

					tp_dev.x = disp_dev.height0 - y - 1;
					tp_dev.y = x;

				}

			}
		}
		else
		{
			res = 1;
		}
    }
    else
    {
        touch_init();
        res = 1;
    }

	if(res)
	{
		if(touch_down_flag)
		{
			res = 0;
			touch_down_flag = 0;
			tp_dev.sta = 0;

		}
	}
	return res;
}













void touch_task(void *pvParameters)
{

	uint8_t res;
	t_msg msg;
	uint8_t flag = 0;
	TimerHandle_t timer0 = NULL;


	tp_dev.tp_press = xSemaphoreCreateBinary();


	 /*int*/
	PINSEL_ConfigPin(0, 13, 0);
	GPIO_SetDir(0, (1<<13), 0);
	GPIO_IntCmd(0, 1<<13, 1);
	NVIC_SetPriority(GPIO_IRQn, 9);
	NVIC_EnableIRQ(GPIO_IRQn);







	g_project->timer =  xTimerCreate((const char *) "bl",
							(TickType_t)g_project->standby_time*1000*MS,
							(UBaseType_t)pdFALSE,
							(void*	)NULL,
							(TimerCallbackFunction_t)standby_act_timer);   //创建自动调节背光时钟
	if(g_project->auto_bl)
	{
		xTimerStart(g_project->timer,10);
	}


	if(g_project->touch_nortify == 2)
	{
		timer0 =  xTimerCreate((const char *) "touch",
								(TickType_t)100,
								(UBaseType_t)pdTRUE,
								(void*	)NULL,
								(TimerCallbackFunction_t)upload_pos_timer);
		 xTimerStart(timer0,10);
	}



	while(1)
	{
		tp_dev.get_seamphore(portMAX_DELAY);


		res = tp_dev.get_pos();

		if(g_project->auto_bl)
		{

			if(g_project->light == g_project->active_light)
			{
				xTimerReset(g_project->timer,10);
			}
			else
			{
				g_project->light = g_project->active_light;
				xTimerStart(g_project->timer,10);
				flag = 1;  //从待机到正常模式的的按下触摸坐标不上报处理，
			}

		}


		if(res == 0)
		{

			if(tp_dev.x != tp_dev.oldx || tp_dev.y != tp_dev.oldy || tp_dev.sta != tp_dev.old_sta)
			{

				msg.msg = MSG_TP;
				msg.a = tp_dev.x;    //记录按下时的坐标
				msg.b = tp_dev.y;

				msg.c = tp_dev.sta;

				if(flag == 0)
				{
					msg_dev.write_message((const void *) &msg,0);

					if(msg.c == 1)
					{
						if(g_project->touch_nortify == 0 ||
						   g_project->touch_nortify == 2 ||
						   g_project->touch_nortify == 3)
						{
							upload_pos();
						}
						if(g_project->touch_nortify == 2)
						{
							xTimerReset(timer0,10);
							xTimerStart(timer0,10);
						}
					}
					else if(msg.c == 0)
					{
						if(g_project->touch_nortify == 1 || g_project->touch_nortify ==3)
						{
							upload_pos();
						}
						else if(g_project->touch_nortify == 2)
						{
							xTimerStop(timer0,10);
						}

					}
				}
				else
				{
					flag = 0;
				}
				tp_dev.oldx = tp_dev.x;
				tp_dev.oldy = tp_dev.y;
				tp_dev.old_sta = tp_dev.sta;
			}

		}



		delay_ms(5);

		LPC_GPIOINT->IO0IntEnF |= (1 << 13);


	}

}




#endif


static int ST1633I_i2c2_read(uint16_t addr, uint8_t  reg, uint8_t *rbuf, uint16_t len)
{
	if( HAL_I2C_Mem_Read( &hi2c2, addr , reg , I2C_MEMADD_SIZE_8BIT, rbuf, len, 1000 ) == HAL_OK )

		return 0;
	else
		return -1;
}

static int ST1633I_i2c2_write(uint16_t addr, uint8_t *data, uint16_t len)
{
	if( HAL_I2C_Mem_Write( &hi2c2, addr , data[0], I2C_MEMADD_SIZE_8BIT, data + 1, (len - 1) , 1000 ) == HAL_OK )
		return 0;
	else
		return -1;

}



