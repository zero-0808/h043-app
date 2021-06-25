#include "ff.h"
#include "bmp.h"
#include "mem_my.h"
#include "string.h"
#include "nand.h"
#include "monitor.h"
#include "display.h"
#include "cfg.h"
#include "gui.h"

#include "font.h"
#include "buzzer.h"
#include "timer.h"
#include "icon.h"

#include "usb_host.h"


extern const unsigned char g_ASCII_8_16[ 2048 ];

#if   (SRC_DOWNLOAD_TYPE == SRC_DOWNLOAD_USB)

#define TEMP_SIZE    (8192)


#elif (SRC_DOWNLOAD_TYPE == SRC_DOWNLOAD_SDIO)

#define TEMP_SIZE    (512)

#else
#define TEMP_SIZE    (512)    
#endif




uint32_t read_source(FIL file,uint8_t * buf,uint32_t size);




static short str_to_num(uint8_t * file_name)
{	uint8_t j,k;
	uint8_t i = 0;
	short num = 0;
	short temp;
	while(1)
	{
		if(!(file_name[i] < '0' || file_name[i] > '9'))
		{
			i++;
		}
		else
		{
			break;
		}
	}

	if(i==0)
	{
		return -1;
	}

	for(j=0;j<i;j++)
	{
		temp = file_name[j] - '0';
		for(k=0;k<i-j-1;k++)
		{
			temp *= 10;
		}

		num += temp;
	}

	return num;
}





static uint8_t update_bmp_source(DIR dir)
{
	
	FRESULT res;
	FILINFO tfileinfo;
	char * fn = NULL;
	char * pname = NULL;
	FIL file;
	BMP_HEAD head;
	uint32_t bw;
	uint8_t * buf = NULL;
	uint8_t * buf1 = NULL;
	uint8_t * buf2 = NULL;
	uint8_t * wbuf = NULL;
	uint8_t * p = NULL;
	short num;
	uint32_t screen_size,screen,size;
	uint32_t temp_size;
	uint32_t block,page;


	screen_size = disp_dev.width0 * disp_dev.height0 * disp_dev.bpp;
	
	

	
	
	while(1)
	{
		res = f_readdir(&dir,&tfileinfo);
		if((res != FR_OK) || (tfileinfo.fname[0] == 0))
		{
			break;
		}

		
		
		

		if(is_bmp_file((uint8_t *)tfileinfo.fname))
		{

			num = str_to_num((uint8_t *)tfileinfo.fname);
			if(num < 0)
			{
				continue;
			}
			if(pname == NULL)
			{
				pname = (char *)my_malloc(tfileinfo.fsize);
				
				if(pname == NULL)
				{
					f_closedir(&dir);
					return 1;
				}
			}

			fn = tfileinfo.fname;
			strcpy(pname,"0:/JHD_CFG/Bmp/");
			strcat(pname,fn);
			

			res = f_open(&file,pname,FA_READ);
			
			if(res != FR_OK)
			{
				f_closedir(&dir);
				my_free(pname);
				return 1;
			}
			
			res = f_read(&file,(void *)&head,sizeof(BMP_HEAD),&bw);
			if(res != FR_OK)
			{
				f_closedir(&dir);
				f_close(&file);
				my_free(pname);
				return 1;
			}
			size = disp_dev.width0 * disp_dev.height0 * head.pic_dip / 8;
			
			res = f_lseek(&file,(head.pic_data_address_h << 16) | head.pic_data_address_l);
			if(res != FR_OK)
			{
				f_closedir(&dir);
				f_close(&file);
				my_free(pname);
				return 1;
			}

			

			if(buf == NULL)
			{	
				
				buf = (uint8_t *) my_malloc(size);
				if(buf == NULL)
				{
					f_close(&file);
					f_closedir(&dir);
					my_free(pname);
					return 1;
				}
			}

			if(buf1 == NULL)
			{
				buf1 = (uint8_t *) my_malloc(screen_size);
				if(buf1 == NULL)
				{
					f_close(&file);
					f_closedir(&dir);
					my_free(pname);
					my_free(buf);
					return 1;
				}
			}

			if(buf2 == NULL)
			{
				buf2 = (uint8_t *) my_malloc(screen_size);
				if(buf2 == NULL)
				{
					f_close(&file);
					f_closedir(&dir);
					my_free(pname);
					my_free(buf);
					my_free(buf1);
					return 1;
				}
			}


			
			read_source(file,buf,size);

			

		
			if(head.pic_dip == 24)
			{
				bmp_3to2(buf1,buf);
				convert_bmp(buf2,buf1,(head.pic_w_h << 16) | head.pic_w_l,(head.pic_h_h << 16) | head.pic_h_l);
			}
			else if(head.pic_dip == 16)
			{
				convert_bmp(buf2,buf,(head.pic_w_h << 16) | head.pic_w_l,(head.pic_h_h << 16) | head.pic_h_l);
			}
			

			my_free(buf);
			buf = NULL;
			
			

			if(wbuf == NULL)
			{
				wbuf = (uint8_t *) my_malloc(nand_dev.page_totalsize);
				if(wbuf == NULL)
				{
					f_close(&file);
					f_closedir(&dir);
					my_free(pname);
					my_free(buf);
					my_free(buf1);
					my_free(buf2);
					return 1;
				}
			}

			mem_set(wbuf,0xff,nand_dev.page_totalsize);
			screen = 0;
			page = screen_size * num / nand_dev.page_mainsize;
			if(screen_size % nand_dev.page_mainsize)
			{
				page += num; 
			}
			block = page / nand_dev.block_pagenum;
			block += src_cfg.block_bmp_addr;
			page = page % nand_dev.block_pagenum;
			nand_dev.use_blocknum = block;
			
			while(screen < screen_size)
			{
				p = buf2 + screen;
				temp_size = ((screen_size - screen) > nand_dev.page_mainsize) ? nand_dev.page_mainsize:(screen_size - screen);
				screen += nand_dev.page_mainsize;
				mem_copy(wbuf,p,temp_size);
				wbuf[nand_dev.page_mainsize + 1] = 0xee;
			  	nand_dev.page_program( nand_dev.lut[block], page, wbuf,0);
				page++;
				if(page >= nand_dev.block_pagenum)
				{
					page = 0;
					nand_dev.use_blocknum += 1;
					block++;
					if(nand_dev.use_blocknum  >= nand_dev.good_blocknum)
					{
						f_close(&file);
						f_closedir(&dir);
						my_free(pname);
						my_free(buf);
						my_free(buf1);
						my_free(wbuf);
						return 1;
					}
					
				}
			}

			
			
			
		}
	}
	
	f_close(&file);
	f_closedir(&dir);
	my_free(pname);
	my_free(buf1);
	my_free(buf2);
	my_free(wbuf);

	
	return 0;
	
	
}


/*SDIO��*/


 uint32_t read_source(FIL file,uint8_t * buf,uint32_t size)
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
	

    while(size2 < size1)
    {
        p = buf + size2;
        temp_size = ((size1 - size2) > (TEMP_SIZE)) ? (TEMP_SIZE) : (size1 - size2);
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


/*USB��*/
//uint32_t read_source(FIL file,uint8_t * buf,uint32_t size)
//{
//    uint32_t bw;
//    

//    if(buf == NULL)
//    {
//        return 0;
//    }
//	f_read(&file,(void *)buf,size,&bw);
//	
//    return bw;
//    
//}




static uint8_t save_source(uint32_t addr,FIL file,uint32_t size)
{
	uint16_t block,page;
	uint32_t size1,size2,temp_size;
	uint8_t * buffer;
//	uint8_t * p;
	
	buffer = (uint8_t *) my_malloc(nand_dev.page_mainsize);
	
	if(buffer == NULL)
	{
		return 1;
	}



	//read_source(file, buffer,size);
	
	block = (addr >> 16) & 0XFFFF;
	page = (addr) & 0XFFFF;
	size1 = size;
	size2 = 0;
	
	while(size2 < size1)
	{
		//p = buffer + size2;
		temp_size = (size1 - size2)>(nand_dev.page_mainsize)?(nand_dev.page_mainsize):(size1-size2);
		read_source(file, buffer,temp_size);
		nand_dev.page_program(nand_dev.lut[block],page,buffer,0);
		size2 += temp_size;
		f_lseek(&file,size2);
		page ++;
		if(page >= nand_dev.block_pagenum)
		{
			page = 0;
			block ++;
		}
	}

	my_free(buffer);

	return 0;

	
}












static void update_tci_source(FIL file,uint16_t block)
{
    uint32_t size,addr;
    size = f_size(&file);
    addr = (block << 16) & (0XFFFF0000);
    save_source(addr,file, size);
}


void put_charn(uint16_t x,uint16_t y,uint16_t color,uint8_t *hzk,uint16_t size)
{
	uint16_t temp, t,t1;
	uint16_t y0 = y;
	uint16_t x0 = x;
	uint16_t csize=(size/8+((size%8)?1:0))*(2*size);
	uint16_t * p = (uint16_t *)disp_dev.disp[0].p;
	
	for(t=0;t<csize;t++)
	{
		temp=hzk[t];
		
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)
			{
				p[y0*disp_dev.width0 + x0] = color;
			}
			
			temp<<=1;
			x0++;
			
			if(x0 - x >= size)
			{
				x0 = x;
				y0++;
				break;
			}
		}
	}
		
}



void show_charn(uint16_t x,uint16_t y,uint16_t color,uint8_t code)
{
	
	uint8_t * p = (uint8_t *)(&g_ASCII_8_16[code * 16]);
	
	put_charn(x,y,color,p,8);
	
	
	

	
}



void show_progress(char * fname,uint16_t color)
{
	uint16_t sx,sy,height,width,i,len,x;
	
	
	height = 16;
	
	char tstr[100] = "The progress of:";
	strcat(tstr,fname);
	len = strlen(tstr);
	
	width = len * 8;
	
	sx = (disp_dev.width0 - width) / 2;
	sy = (disp_dev.height0 - height) / 2;
	
	disp_dev.fill_screen(0x1F);
	
	for(i = 0,x = sx;i<len;i++)
	{
		show_charn(x,sy,color,tstr[i]);
		x += 8;
	}
}





uint8_t update_source(void)
{
	uint8_t res;
	FATFS fs;
	DIR dir;
	FIL file;
	uint16_t temp[9],block_num;
	uint8_t flag = 2;
	uint32_t size;
	uint32_t block_size;
    delay_xms(1000);
#if   (SRC_DOWNLOAD_TYPE == SRC_DOWNLOAD_USB)	
	Host_Init();
    delay_xms(1000);
	Host_EnumDev();
#endif

  
	res = f_mount (&fs,"0:",1);
	
	
	
	if(res != FR_OK)
	{
		return 1;
	}

	
	
	res = f_opendir(&dir,"0:JHD_CFG/Bmp");

	if(res != FR_OK)
	{
		return 1;
	}
	
	disp_dev.fill_screen(0x1f);
	open_backlight();
	delay_xms(1000);

	res = f_open(&file,"0:/JHD_CFG/Bin/12.bin",FA_READ);

	if(res == FR_OK)
	{
		show_progress("12.bin",0x7C00);
		size = f_size(&file);
		
		if(size == 28)
		{
		    uint8_t * buf;
		    uint32_t  bw;

		    buf = (uint8_t *)my_malloc(28);
			
			res = f_read(&file,(void *)buf,size,&bw);
		/*	EEPROM_Erase(2); st没有e2p 暂时注释
			EEPROM_Write(0,2,(void*)buf,MODE_8_BIT,size);*/
			my_free(buf);
		   
		}
	}

	res = f_open(&file,"0:/JHD_CFG/Bin/Font.bin",FA_READ);
	
	if(res == FR_OK)
	{
	    flag = 1;
	}


	nand_dev.create_lut(flag);
	disp_dev.set_light(80);

	block_size = nand_dev.block_pagenum * nand_dev.page_mainsize;
    block_num = 0;
    
	if(res == FR_OK)
	{
		show_progress("font.bin",0x7C00);
	    src_cfg.block_font_addr = 0;
	    update_tci_source(file,src_cfg.block_font_addr);
	    size = f_size(&file);
	    block_num = size / block_size;
	    if(size % block_size)
	    {
	        block_num++;
	    }
	    f_close(&file);
		src_cfg.block_touch_addr = src_cfg.block_font_addr + block_num;
		block_num = 0;
	}

	
    //���������ļ�13.bin�Ĵ洢��ַ
	res = f_open(&file,"0:/JHD_CFG/Bin/13.bin",FA_READ);
	
	if(res == FR_OK)
	{
		show_progress("13.bin",0x7C00);
		update_tci_source(file,src_cfg.block_touch_addr);
		size = f_size(&file);
	    block_num = size / block_size;
	    if(size % block_size)
	    {
	        block_num++;
	    }
		f_close(&file);
	}
    //src_cfg.block_touch_size = block_num;
	
    src_cfg.block_cmd_addr = src_cfg.block_touch_addr + block_num;  
	block_num = 0;
	res = f_open(&file,"0:/JHD_CFG/Bin/14.bin",FA_READ);
	if(res == FR_OK)
	{
		show_progress("14.bin",0x7C00);
		update_tci_source(file,src_cfg.block_cmd_addr);
		size = f_size(&file);
	    block_num = size / block_size;
	    if(size % block_size)
	    {
	        block_num++;
	    }
		f_close(&file);
	}
    //src_cfg.block_cmd_size = block_num;
    
	src_cfg.block_lang_addr = src_cfg.block_cmd_addr + block_num;
	block_num = 0;

	res = f_open(&file,"0:/JHD_CFG/Bin/Str.bin",FA_READ);
        
    if(res == FR_OK)
    {
		show_progress("Str.bin",0x7C00);
        update_tci_source(file,src_cfg.block_lang_addr);
        size = f_size(&file);
	    block_num = size / block_size;
	    if(size % block_size)
	    {
	        block_num++;
	    }
        f_close(&file);
    }

    src_cfg.block_icon_addr = src_cfg.block_lang_addr + block_num;
    block_num = 0;

     res = f_open(&file,"0:/JHD_CFG/Bin/15.bin",FA_READ);
    
    if(res == FR_OK)
    {
		show_progress("15.bin",0x7C00);
        update_tci_source(file,src_cfg.block_icon_addr);
        size = f_size(&file);
	    block_num = size / block_size;
	    if(size % block_size)
	    {
	        block_num++;
	    }
        f_close(&file);
    }


    src_cfg.block_screen_addr = src_cfg.block_icon_addr + block_num;
	block_num = 0;

	res = f_open(&file,"0:/JHD_CFG/Bin/17.bin",FA_READ);

	if(res == FR_OK)
	{
		show_progress("17.bin",0x7C00);
	    update_tci_source(file,src_cfg.block_screen_addr);
        size = f_size(&file);
	    block_num = size / block_size;
	    if(size % block_size)
	    {
	        block_num++;
	    }
        f_close(&file);
	}

    
	show_progress("picture",0x7C00);
    src_cfg.block_bmp_addr = src_cfg.block_screen_addr + block_num;
	
	res = update_bmp_source(dir);
	
	

	temp[0] = src_cfg.block_font_addr;
	temp[1] = src_cfg.block_touch_addr;
	temp[2] = src_cfg.block_cmd_addr;
	temp[3] = src_cfg.block_lang_addr;
	temp[4] = src_cfg.block_icon_addr;
	temp[5] = src_cfg.block_screen_addr;
	temp[6] = src_cfg.block_bmp_addr;
    //temp[7] = src_cfg.block_touch_size;
    //temp[8] = src_cfg.block_cmd_size;

	/*EEPROM_Erase(1); st没有e2p 暂时注释
    EEPROM_Write(0,1,(void*)temp,MODE_16_BIT,7);*/

	
	
	disp_dev.fill_screen(0x03e0);
	
	bz_dev.act(1);
	delay_xms(70);
	bz_dev.act(0);
	res = f_mount (&fs,"0:",0);
	
	
	return res;
}






	

