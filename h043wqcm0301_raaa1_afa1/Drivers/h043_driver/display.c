#include "display.h"
#include "mem_my.h"
#include "cfg.h"
#include "string.h"
#include "ltdc.h"
#include "dma2d.h"
#include "tim.h"
#include "main.h"




static void TFT_init(void);
static uint8_t * lcd_get_addr(uint8_t flag);
static void lcd_draw_cross(uint16_t x, uint16_t y,uint16_t rad, uint32_t color);
static void lcd_fill_rect(uint32_t sx, uint32_t ex,uint32_t sy, uint32_t ey, uint32_t color);
static void lcd_put_pixel(uint32_t x, uint32_t y, uint32_t color);
static void lcd_put_pixel_buffer(uint32_t x, uint32_t y, uint32_t color,uint8_t * buffer);
static void set_back_light(uint8_t light);
static uint16_t lcd_read_pixel_buffer(uint32_t x,uint32_t y,uint8_t * buffer);
static void switch_base_addr(void);
void open_backlight(void);
void close_backlight(void);
static void lcd_fill_sceen(uint32_t color);



static void set_back_light(uint8_t light);
void drv_bl_pwm_out(void);








disp_device disp_dev =
{
	TFT_init,
	lcd_get_addr,
	lcd_put_pixel,
	lcd_put_pixel_buffer,
	lcd_read_pixel_buffer,
	lcd_fill_rect,
	lcd_fill_sceen,
	lcd_draw_cross,
	switch_base_addr,
	set_back_light,
	open_backlight,
	close_backlight,
	
	0,
	0,
	0,
	0,
	0,
	
};



static void lcd_draw_cross(uint16_t x, uint16_t y,uint16_t rad, uint32_t color)
{
#if 0
	//lpc
	LCD_Draw_Cross(LCD_PANEL_UPPER,x,y,rad,color);
#endif
}

static uint16_t lcd_read_pixel_buffer(uint32_t x,uint32_t y,uint8_t * buffer)
{
	uint16_t * buf = (uint16_t *)buffer;
	
	if(x<disp_dev.width && y<disp_dev.height)
	{
		return buf[y*disp_dev.width + x];
	}
	else
	{
		return 0;
	}
}



static void lcd_fill_rect(uint32_t sx, uint32_t ex,uint32_t sy, uint32_t ey, uint32_t color)
{
	
	
	uint16_t i,height,width;

	if(ex > disp_dev.width)
	{
		ex = disp_dev.width;
		
	}
	if(ey > disp_dev.height)
	{
		ey = disp_dev.height;
	}

	
	if(ex < sx || ey < sy)
	{
		return;
	}

	width = ex - sx + 1;
	height = ey - sy + 1;
	for(i = 0; i < height; i++)
	{
		mem_set2((uint16_t * ) &disp_dev.disp[0].p[((i+sy)* disp_dev.width + sx) * disp_dev.bpp],color,width);
	}
}


static void lcd_fill_sceen(uint32_t color)
{
	uint16_t i,j;
	uint16_t * p = (uint16_t *) disp_dev.disp[0].p;
	for(i=0;i<disp_dev.height0;i++)
	{
		for(j=0;j<disp_dev.width0;j++)
		{
			*p++ = color;
		}
	}
	
}



static void lcd_put_pixel(uint32_t x, uint32_t y, uint32_t color)
{
	
	
	uint16_t * p;

	if(disp_dev.dir == 0)
	{

        if(disp_dev.flag==0)
        {
            p = (uint16_t *) disp_dev.disp[1].p;
        }
        else if(disp_dev.flag==1)
        {
            p = (uint16_t *) disp_dev.disp[0].p;
        }
    }
    else if(disp_dev.dir == 1)
    {
        p = (uint16_t *)disp_dev.disp[3].p;
    }
    
	
	
	if(x >= disp_dev.width || y >= disp_dev.height || p == NULL)
	{
		return;
	}

	p[x+y*disp_dev.width] = color;
}


static void lcd_put_pixel_buffer(uint32_t x, uint32_t y, uint32_t color,uint8_t * buffer)
{
	
	uint16_t * p;

    p = (uint16_t *) buffer;
	
	if(x >= disp_dev.width || y >= disp_dev.height)
	{
		return;
	}

	p[x+y*disp_dev.width] = color;
}




static uint8_t * lcd_get_addr(uint8_t flag)
{

	if(flag==0)
	{
	    return (uint8_t *)disp_dev.disp[0].p;
	}
	else if(flag == 1)
	{
	    return (uint8_t *)disp_dev.disp[1].p;
	}
	else if(flag == 2)
	{
	    return (uint8_t *)disp_dev.disp[2].p;
	}
	return NULL;
	
}

static void back_light_init(void)
{
#if 0 //st
    //uint8_t pwmChannel, channelVal;
    PWM_TIMERCFG_Type PWMCfgDat;
    PWM_MATCHCFG_Type PWMMatchCfgDat;
	
	
	PINSEL_ConfigPin(0,18,0);
	GPIO_SetDir(0, 1<<18,GPIO_DIRECTION_OUTPUT);
	GPIO_OutputValue(0, 1<<18, 0);

    
    PWMCfgDat.PrescaleOption = PWM_TIMER_PRESCALE_TICKVAL;
    PWMCfgDat.PrescaleValue = 24;
    PWM_Init(1, PWM_MODE_TIMER, (void *) &PWMCfgDat);
    PINSEL_ConfigPin (2, 0, 1);//PWM1[1]
    PWM_MatchUpdate(1, 0, 100, PWM_MATCH_UPDATE_NOW);


    PWMMatchCfgDat.IntOnMatch = DISABLE;
    PWMMatchCfgDat.MatchChannel = 0;
    PWMMatchCfgDat.ResetOnMatch = ENABLE;
    PWMMatchCfgDat.StopOnMatch = DISABLE;
    PWM_ConfigMatch(1, &PWMMatchCfgDat);
    
    PWM_ChannelConfig(1, 1, PWM_CHANNEL_SINGLE_EDGE);
    PWM_MatchUpdate(1, 1,80, PWM_MATCH_UPDATE_NOW);
    PWMMatchCfgDat.IntOnMatch = DISABLE;
    PWMMatchCfgDat.MatchChannel = 1;
    PWMMatchCfgDat.ResetOnMatch = DISABLE;
    PWMMatchCfgDat.StopOnMatch = DISABLE;
    PWM_ConfigMatch(1, &PWMMatchCfgDat);
    PWM_ChannelCmd(1, 1, ENABLE);
    
    PWM_ResetCounter(1);
    PWM_CounterCmd(1, ENABLE);
    PWM_Cmd(1, ENABLE);
	delay_us(40);
	set_back_light(0);
#endif

	drv_bl_pwm_out();
	delay_us(40);
	set_back_light(0);
		 
}
void drv_bl_pwm_out(void)
{
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);

}


static void set_back_light(uint8_t light)
{
#if 0 //st
	uint8_t duty = 100 - light;
	duty = duty * (89/100.0);
	PWM_MatchUpdate(1, 1,duty, PWM_MATCH_UPDATE_NOW);
#endif

	uint32_t conver = 0;
	if(light > 100)
	{
		return;
	}

	conver = light;
	conver = (100 - conver) * 80  / 100;
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2 ,conver);//
}

/* USER CODE BEGIN 1 */
//在指定区域内填充单个颜色
//LTDC,层颜窗口设置,窗口以LCD面板坐标系为基准
//注意:此函数必须在LTDC_Layer_Parameter_Config之后再设�??.
//layerx:层�??,0/1.
//sx,sy:起始坐标
//width,height:宽度和高�??
void LTDC_Layer_Window_Config(u8 layerx,u16 sx,u16 sy,u16 width,u16 height)
{
    HAL_LTDC_SetWindowPosition(&hltdc,sx,sy,layerx);  //设置窗口的位�??
    HAL_LTDC_SetWindowSize(&hltdc,width,height,layerx);//设置窗口大小
}

//LCD初始化函�??
void LTDC_Init(void)
{
	#if(UPDN_SHLR_GPIO_STATE == 1)
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_RESET);
	#endif
	//open vlcd power
	HAL_GPIO_WritePin(TFT_ENT_GPIO_Port,TFT_ENT_Pin  , 1);
	HAL_Delay(10);
	LTDC_Layer_Window_Config(0 , 0 , 0 , LCD_W_SIZE , LCD_H_SIZE);
	disp_dev.width0 = LCD_W_SIZE;
	disp_dev.height0 = LCD_H_SIZE;
}

static void TFT_init(void)
{
#if 0 //st
	uint32_t * lcd_addr_up = NULL;
	uint32_t * lcd_addr_low = NULL;
	uint32_t * lcd_addr  = NULL;
	
	
//	memset((void *)SDRAM_BASE_ADDR,0,LCD_W_SIZE*LCD_H_SIZE*4);

	PINSEL_ConfigPin(0,15,0);
	GPIO_SetDir(0, 1<<15,GPIO_DIRECTION_OUTPUT);
	GPIO_OutputValue(0, 1<<15, 1);
	
	lcd_config.big_endian_byte = 0;
	lcd_config.big_endian_pixel = 0;
	lcd_config.hConfig.hbp = LCD_W_BACK_PORCH;
	lcd_config.hConfig.hfp = LCD_W_FRONT_PORCH;
	lcd_config.hConfig.hsw = LCD_W_PULSE;
	lcd_config.hConfig.ppl = LCD_W_SIZE;
	lcd_config.vConfig.lpp = LCD_H_SIZE;
	lcd_config.vConfig.vbp = LCD_H_BACK_PORCH;
	lcd_config.vConfig.vfp = LCD_H_FRONT_PORCH;
	lcd_config.vConfig.vsw = LCD_H_PULSE;
	lcd_config.panel_clk   = LCD_PIX_CLK;
	lcd_config.polarity.active_high = 1;
	lcd_config.polarity.cpl = LCD_W_SIZE;
	lcd_config.polarity.invert_hsync = 1;
	lcd_config.polarity.invert_vsync = 1;
	lcd_config.polarity.invert_panel_clock = 0;
	lcd_addr_up = (uint32_t *) SDRAM_BASE_ADDR;
	lcd_addr_low = (uint32_t *)(SDRAM_BASE_ADDR + LCD_W_SIZE*LCD_H_SIZE*2);
	lcd_addr  = (uint32_t *)(SDRAM_BASE_ADDR + 2*LCD_W_SIZE*LCD_H_SIZE*2);

	lcd_config.lcd_panel_upper = (uint32_t)lcd_addr_up;
	lcd_config.lcd_bpp = LCD_BPP_16;
	//lcd_config.lcd_bpp = LCD_BPP_16_565Mode;
	lcd_config.lcd_type = LCD_TFT;
	lcd_config.lcd_bgr = TRUE;
#endif
	uint32_t * lcd_addr_up = NULL;
	uint32_t * lcd_addr_low = NULL;
	uint32_t * lcd_addr  = NULL;

	lcd_addr_up = (uint32_t *) SDRAM_BASE_ADDR;
	lcd_addr_low = (uint32_t *)(SDRAM_BASE_ADDR + LCD_W_SIZE*LCD_H_SIZE*2);
	lcd_addr  = (uint32_t *)(SDRAM_BASE_ADDR + 2*LCD_W_SIZE*LCD_H_SIZE*2);

	LTDC_Init ();

	back_light_init();

	disp_dev.flag = 0;
	disp_dev.width0 = LCD_W_SIZE;
	disp_dev.height0 = LCD_H_SIZE;
	disp_dev.bpp = 2;

	
	disp_dev.disp[0].width = disp_dev.width0;
	disp_dev.disp[0].height = disp_dev.height0;
	disp_dev.disp[0].p = (uint8_t*)lcd_addr_up;


	disp_dev.disp[1].width = disp_dev.width0;
	disp_dev.disp[1].height = disp_dev.height0;
	disp_dev.disp[1].p = (uint8_t*)lcd_addr_low;


	disp_dev.disp[2].width = disp_dev.width0;
	disp_dev.disp[2].height = disp_dev.height0;
	disp_dev.disp[2].p = (uint8_t*)lcd_addr;

	disp_dev.disp[3].width = 0;
    disp_dev.disp[3].height = 0;
    disp_dev.disp[3].p = NULL;
	*(volatile uint32_t  *)(0x400fc188) = 0xc09;
}





void LCD_SetBaseAddress(LCD_PANEL panel, uint32_t pAddress)
{
	LTDC_LayerCfgTypeDef pLayerCfg = {0};

    if(panel == LCD_PANEL_UPPER)
    	pLayerCfg.FBStartAdress = pAddress & ~7UL ;
    else
    	pLayerCfg.FBStartAdress = pAddress & ~7UL ;
}







static void switch_base_addr(void)
{

    if(disp_dev.flag == 1)
    {
        LCD_SetBaseAddress(LCD_PANEL_UPPER,(uint32_t)disp_dev.disp[0].p);
        disp_dev.flag = 0;
		
    }
    else if(disp_dev.flag == 0)
    {
        LCD_SetBaseAddress(LCD_PANEL_UPPER,(uint32_t)disp_dev.disp[1].p);
        disp_dev.flag = 1;
		
    }

}



 void open_backlight(void)
{
#if 0
	//st
	GPIO_OutputValue(0, 1<<18, 1); 
#endif
	HAL_GPIO_WritePin(BL_EN_GPIO_Port,BL_EN_Pin,GPIO_PIN_SET);
}


void close_backlight(void)
{
#if 0
	//st
	GPIO_OutputValue(0, 1<<18, 0);
#endif
	HAL_GPIO_WritePin(BL_EN_GPIO_Port,BL_EN_Pin,GPIO_PIN_RESET);
}
























