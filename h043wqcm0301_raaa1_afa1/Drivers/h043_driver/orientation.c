#include "mem_my.h"
#include "string.h"
#include "display.h"
#include "lis3dh_reg.h"

//#include "lpc177x_8x_pinsel.h"

#include "cfg.h"
#include "monitor.h"
#include "i2c.h"
#include "gpio.h"



//������

#if  (DIR_SWITCH_SUPPORT)






SemaphoreHandle_t  lis3dh_sem;
SemaphoreHandle_t screen_mutex_sem;
static uint8_t dir_cmd_flag = 0;
static uint8_t dir = 0;
static uint8_t mode;








static int32_t platform_read(uint8_t reg, uint8_t *bufp,uint16_t len);
static int32_t platform_write(uint8_t reg, uint8_t *bufp,uint16_t len);






//static TimerHandle_t lis3dh_timer = NULL;
static lis3dh_ctx_t dev_ctx;
//static uint8_t lis3dh_dir;






void  lis3dh_init(void)
{
#if 0
	//lpc
    uint8_t whoamI;
    dev_ctx.write_reg = platform_write;
    dev_ctx.read_reg = platform_read;

    PINSEL_ConfigPin (0, 27, 1);
    PINSEL_ConfigPin (0, 28, 1);
	LPC_IOCON->P0_27 &= ~0x1F;	/*  I2C I/O config */
	LPC_IOCON->P0_27 |= (0x01|(0x1<<10));  /* make it open-drain, I2C SDA */
	LPC_IOCON->P0_28 &= ~0x1F;	
	LPC_IOCON->P0_28 |= (0x01|(0x1<<10));  /* make it open-drain, I2C SCL */
    I2C_Init((en_I2C_unitId)0, 100000);
    I2C_Cmd((en_I2C_unitId)0, I2C_GENERAL_MODE, ENABLE);

    lis3dh_sem = xSemaphoreCreateBinary();
	screen_mutex_sem = xSemaphoreCreateMutex();
	
	 /*int*/
	PINSEL_ConfigPin(0, 23, 0);	  
	GPIO_SetDir(0, (1<<23), 0);
	GPIO_IntCmd(0, 1<<23, 1);
//	
//	NVIC_SetPriority(GPIO_IRQn, 15);	
//	NVIC_EnableIRQ(GPIO_IRQn);

    lis3dh_device_id_get(&dev_ctx, &whoamI);
    

    if (whoamI != LIS3DH_ID)
    {
        return;
    }
    



	uint8_t val;

	val = 0x37;
	platform_write(0x20,&val,1);
	val = 0x00;
	platform_write(0x21,&val,1);
	val = 0x40;
	platform_write(0x22,&val,1);
	val = 0x08;
	platform_write(0x23,&val,1);
	val = 0x0;
	platform_write(0x24,&val,1);
	val = 0x02;
	platform_write(0x25,&val,1);
	val = 0xFF;
	platform_write(0x30,&val,1);
	val = 0x60;
	platform_write(0x31,&val,1);
	val = 0x12;
	platform_write(0x32,&val,1);
#endif

    uint8_t whoamI;


	dev_ctx.write_reg = platform_write;
	dev_ctx.read_reg = platform_read;
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);


   lis3dh_device_id_get(&dev_ctx, &whoamI);
	if (whoamI != LIS3DH_ID)
	{
		return;
	}
	uint8_t val;

	val = 0x37;
	platform_write(0x20,&val,1);
	val = 0x00;
	platform_write(0x21,&val,1);
	val = 0x40;
	platform_write(0x22,&val,1);
	val = 0x08;
	platform_write(0x23,&val,1);
	val = 0x0;
	platform_write(0x24,&val,1);
	val = 0x02;
	platform_write(0x25,&val,1);
	val = 0xFF;
	platform_write(0x30,&val,1);
	val = 0x60;
	platform_write(0x31,&val,1);
	val = 0x12;
	platform_write(0x32,&val,1);


	 
}



int i2c_read(int addr, uint8_t  reg, unsigned char *rbuf, uint16_t len)
{
#if 0
	//lpc
    I2C_M_SETUP_Type transferMCfg;

   
    transferMCfg.sl_addr7bit = addr;
    transferMCfg.tx_data = &reg;
	transferMCfg.tx_length = 1;
	transferMCfg.rx_data = rbuf;
	transferMCfg.rx_length = len;
	transferMCfg.retransmissions_max = 3;

	if (I2C_MasterTransferData((en_I2C_unitId)0, &transferMCfg, I2C_TRANSFER_POLLING) == SUCCESS)
	{
	    return len;
	}
	else
	{
	    return 0;
	}
#endif
	if( HAL_I2C_Mem_Read( &hi2c1, (addr<<1), reg , I2C_MEMADD_SIZE_8BIT, rbuf, len, 1000 ) == HAL_OK )

	        return 0;
	    else
	        return -1;
}



int i2c_write(int addr, unsigned char *data, int len)
{
#if 0
	//lpc
    I2C_M_SETUP_Type transferMCfg;


    transferMCfg.sl_addr7bit = addr;
    transferMCfg.tx_data = data;
	transferMCfg.tx_length = len;
	transferMCfg.rx_data = NULL;
	transferMCfg.rx_length = 0;
	transferMCfg.retransmissions_max = 3;

	if (I2C_MasterTransferData((en_I2C_unitId)0, &transferMCfg, I2C_TRANSFER_POLLING) == SUCCESS)
	{
	    return len;
	}
	else
	{
	    return 0;
	}
#endif
	if( HAL_I2C_Mem_Write( &hi2c1, (addr << 1), data[0], I2C_MEMADD_SIZE_8BIT, data + 1, (len - 1) , 1000 ) == HAL_OK )
			return 0;
		else
			return -1;
}





static int32_t platform_read(uint8_t reg, uint8_t *bufp,uint16_t len)
{
    reg |= 0x80;

    i2c_read(LIS3DH_I2C_ADD_H,reg,bufp,len);
    
    return 0;
}








static int32_t platform_write(uint8_t reg, uint8_t *bufp,uint16_t len)
{
    uint32_t size;
    uint8_t * p;
    reg |= 0x80;
    size = len + 1;
    p = (uint8_t *)  my_malloc(size);
    p[0] = reg;
    memcpy((void *)&p[1],(void *)bufp,len);
    i2c_write(LIS3DH_I2C_ADD_H,p,size);
	my_free(p);
    return 0;
}


uint8_t get_lis3dh_dir(void)
{
	lis3dh_reg_t all_source;
	
	uint8_t dir = disp_dev.dir;
	
	lis3dh_int1_gen_source_get(&dev_ctx, &all_source.int1_src);
	
	switch(all_source.byte & 0x3f)
	{
		case 0x01:
        case 0x02:
			dir = 0;
			break;
		case 0x04:
		case 0x08:
			dir = 1;
			break;
		default:
			break;
	}
	
	return dir;
}



void lis3dh_task(void *pvParameters)
{

	
	lis3dh_reg_t all_source;
	uint16_t screen_id;
	
	
	
	
	while(1)
	{	
		
		xSemaphoreTake(lis3dh_sem,portMAX_DELAY);
		
		if(dir_cmd_flag == 0)
		{
			taskENTER_CRITICAL();
			lis3dh_int1_gen_source_get(&dev_ctx, &all_source.int1_src);
			taskEXIT_CRITICAL();
			switch(all_source.byte & 0x3f)
			{
				case 0x01:
				case 0x02:
					
				
				dir = 1;
				break;
					
					
				case 0x04:
				case 0x08:
					
					dir = 0;
					
					break;
				default:
					dir = disp_dev.dir;
					break;
			}
			if(dir != disp_dev.dir)
			{
				
				xSemaphoreTake(screen_mutex_sem,portMAX_DELAY);   //���л���Ļʱ��������ת��Ļ
				
				
				if(dir)     //����
				{
					screen_id = get_Vscreen(src_cfg.current_screen);
					if(screen_id != 0xFFFF)
					{
						taskENTER_CRITICAL();
						disp_dev.width = disp_dev.height0;
						disp_dev.height = disp_dev.width0;
						src_cfg.new_screen = screen_id;
						disp_dev.dir = dir;
						taskEXIT_CRITICAL();
					}
				}
				else    //����
				{
					screen_id = get_Hscreen(src_cfg.current_screen);
					if(screen_id != 0xFFFF)
					{
						taskENTER_CRITICAL();
						disp_dev.width = disp_dev.width0;
						disp_dev.height = disp_dev.height0;
						src_cfg.new_screen = screen_id;
						disp_dev.dir = dir;
						taskEXIT_CRITICAL();
					}
					
				}
				
				
				xSemaphoreGive(screen_mutex_sem);
				
			}
			
			//NVIC_EnableIRQ(GPIO_IRQn);
			//LPC_GPIOINT->IO0IntEnF |= (1 << 23);
			HAL_NVIC_EnableIRQ(EXTI0_IRQn);
		}
		else 
		{
			dir_cmd_flag = 0;
			
			uint8_t old_mode = src_cfg.ptscreen->screen_mode;
			src_cfg.ptscreen->screen_mode = mode;
			
			switch(mode)
			{
				case 0:
				case 1:
					if(old_mode == 2)
					{
						//LPC_GPIOINT->IO0IntEnF &= ~(1 << 23);
						HAL_NVIC_DisableIRQ(EXTI0_IRQn);
					}
					break;
				case 2:
					if(old_mode != 2)
					{

						//LPC_GPIOINT->IO0IntEnF |= (1 << 23);
						HAL_NVIC_EnableIRQ(EXTI0_IRQn);
					}
					break;    
			}

			if(old_mode != mode)
			{
				
				xSemaphoreTake(screen_mutex_sem,portMAX_DELAY);   //���л���Ļʱ��������ת��Ļ
				adjust_screen_dir(src_cfg.current_screen);
				xSemaphoreGive(screen_mutex_sem);
				
			}
		}
	
	}
}





uint8_t lis3dh_get_dir(void)
{
    lis3dh_reg_t all_source;

    lis3dh_int1_gen_source_get(&dev_ctx, &all_source.int1_src);

    switch(all_source.byte & 0x3f)
    {
        case 0x01:
    	case 0x02:
    	    return 1;
    	case 0x04:
    	case 0x08:
    	    return 0;
        default :
            return 0xFF;
	}
	    

}


void set_screen_mode(uint8_t new_mode)
{
	mode = new_mode;
	dir_cmd_flag = 1;
	xSemaphoreGive(lis3dh_sem);
    
   
}


//void set_screen_mode(uint8_t new_mode)
//{
//	
//	
//    uint8_t old_mode = src_cfg.ptscreen->screen_mode;
//    
//    src_cfg.ptscreen->screen_mode = mode;
//	

//    
//    switch(mode)
//    {
//        case 0:
//        case 1:
//            if(old_mode == 2)
//            {
//                LPC_GPIOINT->IO0IntEnF &= ~(1 << 23);
//                vTaskSuspend(LIS3DH_Task_Handler);  
//            }
//            break;
//        case 2:
//            if(old_mode != 2)
//            {
//                vTaskResume(LIS3DH_Task_Handler);
//                LPC_GPIOINT->IO0IntEnF |= (1 << 23);
//            }
//            break;    
//    }

//    if(old_mode != mode)
//    {
//        adjust_screen_dir(src_cfg.screen_id);
//    }
//}




#endif



