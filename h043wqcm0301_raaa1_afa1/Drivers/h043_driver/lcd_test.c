
/***************************************************************************************
****************************************************************************************
* Project      : 20180525_HMI
* FILE         : lcd_test.c
* Description  : 
*                      
* Copyright (c) 2018 by ShenZhen JingHua Displays Co.,Ltd... All Rights Reserved.
* 
* History:
* Version		name			Date			Description
* 0.01			Qu Bingbing		2018/06/01		Initial Version
* 
****************************************************************************************
****************************************************************************************/
#include "bmp.h"
#include "display.h"
#include "gui.h"
#include "ff.h"
#include "string.h"
#include "mem_my.h"
#include "timer.h"


static uint32_t read_source1(FIL file,uint8_t * buf,uint32_t size)
{
    uint32_t size1,size2,temp_size,bw;
    uint8_t * p;
    uint8_t res;

    if(buf == NULL)
    {
        return 0;
    }

    size1 = size;
    size2 = 0;
	
	temp_size = f_size(&file);
	
	if(size > temp_size)
	{
		size = temp_size;
	}
	
//	res = f_read(&file,(void *)buf,size,&bw);
//	
//	return size;
	
	

    while(size2 < size1)
    {
        p = buf + size2;
        temp_size = ((size1 - size2) > (512)) ? (512) : (size1 - size2);
        res = f_read(&file,(void *)p,temp_size,&bw);
        if(res == FR_OK)
        {
		    size2 += temp_size;
		}
		else
		{
		    break;
		}
    }

    return size2;
    
}




extern uint32_t read_source(FIL file,uint8_t * buf,uint32_t size);




char bmp_file[30][30];


uint32_t get_bmp_name(char (*Files)[30],char * path)
{
    FRESULT res;
	DIR dir;
	FILINFO tfileinfo;
	uint32_t i = 0;
    
    res = f_opendir(&dir,path);

    if(res != FR_OK)
    {
        return i;
    }

    while(1)
    {
        
		res = f_readdir(&dir,&tfileinfo);
		
		if((res != FR_OK) || (tfileinfo.fname[0] == 0))
		{
			break;
		}

		
		if(is_bmp_file((uint8_t *)tfileinfo.fname))
		{
		    strcpy(Files[i],(const char *)tfileinfo.fname);
		    i++;
		}
    }


    return i;

    
}



void lcd_test(void)
{
    uint8_t res;
	FATFS fs;
	//DIR dir;
	FIL file;
	uint32_t num,i = 0;
	char name[30];
	BMP_HEAD head;
	uint32_t size,screen_size,bw;
	uint8_t * buf = NULL;
	uint8_t * buf1 = NULL;
	
    
    res = f_mount (&fs,"0:",1);

    if(res != FR_OK)
    {
        return;
    }

    screen_size = disp_dev.width * disp_dev.height * 2;
    size = disp_dev.width * disp_dev.height * 3;

    buf = (uint8_t *) my_malloc(screen_size);
    buf1 = (uint8_t *) my_malloc(size);
    

    num = get_bmp_name(bmp_file, "0:/Bmp");
    
	while(1)
	{
	    strcpy(name,"0:/Bmp/");
	    strcat(name,bmp_file[i]);
	    res = f_open(&file,name, FA_READ);
        res = f_read(&file,(void *)&head,sizeof(BMP_HEAD),&bw);
        res = f_lseek(&file,(head.pic_data_address_h << 16) | head.pic_data_address_l);
        read_source1(file,buf1,size);
        bmp_3to2(buf, buf1);
        blt(disp_dev.disp[0].p,0,0,disp_dev.width,disp_dev.height,buf);
		//delay_xms(1000);
        i++;
        if(i>=num)
        {
            i = 0;
        }
	}
}



void file_name_test(void)
{
    
	FRESULT res;
	FATFS fs;
	//DIR dir;
	FIL file;
//	uint32_t num,i = 0;
//	char name[30];
//	BMP_HEAD head;
//	uint32_t size,screen_size,bw;
//	uint8_t * buf = NULL;
//	uint8_t * buf1 = NULL;
	
    
    res = f_mount (&fs,"0:",1);

    if(res != FR_OK)
    {
        return;
    }
	
	while(1)
	{
		res = f_open(&file,"0:/file/unicode24.DZK",FA_READ);
		
		if(res != FR_OK)
		{
			xprintf("open err 1\r\n");
		}
		
		res = f_open(&file,"0:/file/uni24.DZK",FA_READ);
		
		if(res != FR_OK)
		{
			xprintf("open err 2\r\n");
		}
	}

//    screen_size = disp_dev.width * disp_dev.height * 2;
//    size = disp_dev.width * disp_dev.height * 3;

//    buf = (uint8_t *) my_malloc(screen_size);
//    buf1 = (uint8_t *) my_malloc(size);
//    

//    num = get_bmp_name(bmp_file, "0:/Bmp");
//    
//	while(1)
//	{
//	    strcpy(name,"0:/Bmp/");
//	    strcat(name,bmp_file[i]);
//	    res = f_open(&file,name, FA_READ);
//        res = f_read(&file,(void *)&head,sizeof(BMP_HEAD),&bw);
//        res = f_lseek(&file,(head.pic_data_address_h << 16) | head.pic_data_address_l);
//        read_source1(file,buf1,size);
//        bmp_3to2(buf, buf1);
//        blt(disp_dev.disp[0].p,0,0,disp_dev.width,disp_dev.height,buf);
//		delay_xms(1000);
//        i++;
//        if(i>=num)
//        {
//            i = 0;
//        }
//	}
}






/***************************************************************************************
* End of File: lcd_test.c

****************************************************************************************/
