#include "display.h"
#include "nand.h"
#include "cfg.h"
#include "mem_my.h"
#include "font.h"
#include "task.h"
#include "string.h" 
#include "timer.h"

void get_font(uint32_t addr,uint32_t offset,uint16_t size,uint8_t * hzk);
uint8_t   get_gbk_font(uint16_t code,font_type font,uint8_t * hzk);
uint8_t   get_unicode_font(uint16_t code,font_type font,uint8_t * hzk);
uint8_t get_gb_font(uint16_t code,font_type font,uint8_t * hzk);
uint8_t get_char_font(uint8_t code,font_type font,uint8_t * hzk);








void get_font(uint32_t addr,uint32_t offset,uint16_t size,uint8_t * hzk)
{
	uint32_t block,page,address;

	if(addr == 0xffffffff)
	{
		mem_set((void *)hzk,0,size);
		return;
	}

	block = src_cfg.block_font_addr;
	offset += addr;
	page = offset / nand_dev.page_mainsize;
	block = page / nand_dev.block_pagenum;
	page = page % nand_dev.block_pagenum;
	address = offset % nand_dev.page_mainsize;
	nand_dev.read(block,page,address,hzk,size);

}




uint8_t   get_gbk_font(uint16_t code,font_type font,uint8_t * hzk)
{
	unsigned char qh,ql;
	uint16_t    csize;
	uint32_t offset;
	uint16_t i;
	uint8_t size;

	size = font.width;


	csize=(size/8+((size%8)?1:0))*(size);

	qh = (code >> 8) & 0XFF;
	ql = (code) & 0XFF;

	if(qh<0x81||ql<0x40||ql==0xff||qh==0xff)
	{
		for(i=0;i<csize;i++)
		{
			hzk[i]=0x00;
		}

		return 1;
	}

	if(ql < 0X7F)
	{
		ql -= 0X40;
	}
	else
	{
		ql -= 0X41;
	}

	qh -= 0X81;
	offset=((unsigned long)190*qh+ql)*csize;

	get_font(font.addr, offset,csize,hzk);

	return 0;
}



uint8_t   get_unicode_font(uint16_t code,font_type font,uint8_t * hzk)
{
	
	uint16_t    csize;
	uint32_t  offset;
	uint8_t   size;

    size = font.width;
	csize=(size/8+((size%8)?1:0))*(size);
	offset=((unsigned long)code)*csize;
	get_font(font.addr, offset,csize,hzk);
	return 0;
}


uint8_t get_gb_font(uint16_t code,font_type font,uint8_t * hzk)
{
	uint8_t qh,ql;
	uint16_t csize;
	uint32_t offset;
	uint8_t size = font.width; 

	csize=(size/8+((size%8)?1:0))*(size);

	qh = (code >> 8) & 0XFF;
	ql = (code) & 0XFF;

	if((qh < 0xa1) || (ql < 0xa1) || (ql > 0xff) || (qh > 0xf7) || (qh >= 0xaa && qh <= 0xaf))
	{
	    mem_set((void *)hzk,0,csize);
		return 1;
	}

	qh = qh - 0xa1;
	ql -= 0xa1;

	offset = (qh * 94 + ql)*csize;

	get_font(font.addr,offset,csize,hzk);

	return 0;
	
}


uint8_t get_char_font(uint8_t code,font_type font,uint8_t * hzk)
{
	uint16_t csize;
	uint32_t offset;
	uint8_t size;
	
	
	if(code < 0x1f || code == 0x7f )
	{
		return 1;
	}

    size = font.width;
	csize=(size/8+((size%8)?1:0))*(2*size);

	code = code;

	offset = code*csize;

	get_font(font.addr, offset, csize, hzk);

	return 0;
	
}



void put_hz_pixel(uint16_t x,uint16_t y,uint16_t color,uint16_t bk_color,uint8_t *hzk,uint16_t size)
{
	uint16_t temp,t,t1;
	uint16_t y0 = y;
	uint16_t x0 = x;
	uint16_t csize=(size/8+((size%8)?1:0))*(size);
	for(t=0;t<csize;t++)
	{
		temp=hzk[t];
		
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)
			{
				disp_dev.put_pixel(x0,y0,color);
			}
			else
			{
				disp_dev.put_pixel(x0,y0,bk_color);
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


void put_hzn_pixel(uint16_t x,uint16_t y,uint16_t color,uint8_t *hzk,uint16_t size)
{
	uint16_t temp,t,t1;
	uint16_t y0 = y;
	uint16_t x0 = x;
	uint16_t csize=(size/8+((size%8)?1:0))*(size);
	for(t=0;t<csize;t++)
	{
		temp=hzk[t];
		
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)
			{
				disp_dev.put_pixel(x0,y0,color);
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




void put_hzn_pixel_dc(uint16_t x,uint16_t y,uint16_t color,uint8_t *hzk,uint16_t size,dc tdc)
{
	uint16_t temp,t,t1;
	uint16_t y0 = y;
	uint16_t x0 = x;
	uint16_t *p = (uint16_t *)tdc.p;
	uint16_t csize=(size/8+((size%8)?1:0))*(size);
	for(t=0;t<csize;t++)
	{
		temp=hzk[t];
		
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)
			{
				if(x0<tdc.width && y0<tdc.height)
				{
					color |= (1 << 15);
				    p[y0*tdc.width+x0] = color;
				}
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


void put_hzn_pixel_rect(uint16_t x,uint16_t y,uint16_t color,uint8_t *hzk,uint16_t size,t_rect rect)
{
	uint16_t temp,t,t1;
	uint16_t y0 = y;
	uint16_t x0 = x;
	uint16_t csize=(size/8+((size%8)?1:0))*(size);
	for(t=0;t<csize;t++)
	{
		temp=hzk[t];
		
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)
			{
			    if(pt_in_rect(&rect, x0, y0))
			    {
				    disp_dev.put_pixel(x0,y0,color);
				}
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








void put_char_pixel(uint16_t x,uint16_t y,uint16_t color,uint16_t bk_color,uint8_t *hzk,uint16_t size)
{
	uint16_t temp,t,t1;
	uint16_t y0 = y;
	uint16_t x0 = x;
	uint16_t csize=(size/8+((size%8)?1:0))*(2*size);
	
	for(t=0;t<csize;t++)
	{
		temp=hzk[t];
		
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)
			{
				disp_dev.put_pixel(x0,y0,color);
			}
			else
			{
				disp_dev.put_pixel(x0,y0,bk_color);
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

void put_charn_pixel(uint16_t x,uint16_t y,uint16_t color,uint8_t *hzk,uint16_t size)
{
	uint16_t temp,t,t1;
	uint16_t y0 = y;
	uint16_t x0 = x;
	uint16_t csize=(size/8+((size%8)?1:0))*(2*size);
	
	for(t=0;t<csize;t++)
	{
		temp=hzk[t];
		
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)
			{
				disp_dev.put_pixel(x0,y0,color);
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



void put_charn_pixel_rect(uint16_t x,uint16_t y,uint16_t color,uint8_t *hzk,uint16_t size,t_rect  rect)
{
	uint16_t temp,t,t1;
	uint16_t y0 = y;
	uint16_t x0 = x;
	uint16_t csize=(size/8+((size%8)?1:0))*(2*size);
	
	for(t=0;t<csize;t++)
	{
		temp=hzk[t];
		
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)
			{
				if(pt_in_rect(&rect, x0, y0))
			    {
				    disp_dev.put_pixel(x0,y0,color);
				}
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





//void put_charn_pixel(uint16_t x,uint16_t y,uint16_t color,uint8_t *hzk,uint16_t size)
//{
//	uint16_t temp,t,t1;
//	uint16_t y0 = y;
//	uint16_t x0 = x;
//	uint16_t csize=(size/8+((size%8)?1:0))*(2*size);
//	
//	for(t=0;t<csize;t++)
//	{
//		temp=hzk[t];
//		
//		for(t1=0;t1<8;t1++)
//		{
//			if(temp&0x80)
//			{
//				disp_dev.put_pixel(x0,y0,color);
//			}
//			temp<<=1;
//			x0++;
//			
//			if(x0 - x >= size)
//			{
//				x0 = x;
//				y0++;
//				break;
//			}
//		}
//	}
//		
//}




uint8_t get_font_width(font_type font)
{
    return font.width;
}


uint8_t get_font_height(font_type font)
{
    if(font.code == 0)
    {
        return (2 * font.width);
    }
    {
        return font.width;
    }
}





void show_font(uint16_t x,uint16_t y,uint16_t color,uint16_t back_color,uint16_t code,font_type font)
{
	uint8_t * hzk = NULL;
	uint16_t size,csize;
	
    size = font.width;
   
	csize=(size/8+((size%8)?1:0))*(size);
	hzk = (uint8_t *)my_malloc(csize);
	
	if(hzk == NULL)
	{
		return ;
	}
	switch(font.code)
	{
	    case 1:
	        get_gb_font(code,font,hzk);
	        break;
	    case 2:
	        get_gbk_font(code,font,hzk);
	        break;
	    case 3:
	        get_unicode_font(code,font,hzk);
	        break;
	    default:
	        my_free(hzk);
	        return;    
	}
	
	put_hz_pixel(x,y,color,back_color,hzk,size);
	my_free(hzk);
	hzk = NULL;

	
}




void show_nfont(uint16_t x,uint16_t y,uint16_t color,uint16_t code,font_type font)
{
	uint8_t * hzk = NULL;
	uint16_t size,csize;
	
    size = font.width;
   
	csize=(size/8+((size%8)?1:0))*(size);
	hzk = (uint8_t *)my_malloc(csize);
	
	if(hzk == NULL)
	{
		return ;
	}
	switch(font.code)
	{
	    case 1:
	        get_gb_font(code,font,hzk);
	        break;
	    case 2:
	        get_gbk_font(code,font,hzk);
	        break;
	    case 3:
	        get_unicode_font(code,font,hzk);
	        break;
	    default:
	        my_free(hzk);
	        return;    
	}
	/* Change By 王先银 at 2020/09/5 */
    put_hzn_pixel(x,y,color,hzk,size);

	my_free(hzk);
	hzk = NULL;

	
}





void show_charn_font(uint16_t x,uint16_t y,uint16_t color,uint8_t code,font_type font)
{
	uint8_t * hzk = NULL;

	uint16_t size,csize;

	size = font.width;

	csize=(size/8+((size%8)?1:0))*(2*size);
	hzk = (uint8_t *)my_malloc(csize);
	
	if(hzk == NULL)
	{
		return ;
	}
	get_char_font(code,font,hzk);
	put_charn_pixel(x,y,color,hzk,size);
	my_free(hzk);
	hzk = NULL;

	
}




void show_char_font(uint16_t x,uint16_t y,uint16_t color,uint16_t back_color,uint8_t code,font_type font)
{
	uint8_t * hzk = NULL;

	uint16_t size,csize;

	size = font.width;

	csize=(size/8+((size%8)?1:0))*(2*size);
	hzk = (uint8_t *)my_malloc(csize);
	
	if(hzk == NULL)
	{
		return ;
	}
	get_char_font(code,font,hzk);
	put_char_pixel(x,y,color,back_color,hzk,size);
	my_free(hzk);
	hzk = NULL;

	
}


font_type * ch_to_hz(font_type  * pfont)
{
    
    uint8_t i;
    
    if(pfont->code != 0)
    {
        return pfont;
    }
    for(i=0;i<src_cfg.font_set->font_num;i++)
    {
        if(src_cfg.font_set->pfont[i].code != 0)
        {
            if((src_cfg.font_set->pfont[i].width) == (pfont->width * 2))
            {
                return &(src_cfg.font_set->pfont[i]);
            }
        }
    }

     return NULL;
}


font_type * hz_to_ch(font_type * pfont)
{
     if(pfont->code == 0)
     {
        return pfont;
     }
	 
	 uint8_t i;

     for(i=0;i<src_cfg.font_set->font_num;i++)
     {
        if(src_cfg.font_set->pfont[i].code == 0)
        {
            if(src_cfg.font_set->pfont[i].width * 2 == (pfont->width))
            {
                return &(src_cfg.font_set->pfont[i]);
            }
        }
     }

     return NULL;
}




void put_charn_pixel_buffer(uint16_t x,uint16_t y,uint16_t color,uint8_t *hzk,uint16_t size,uint8_t * buf)
{
	uint16_t temp,t,t1;
	uint16_t y0 = y;
	uint16_t x0 = x;
	uint16_t csize=(size/8+((size%8)?1:0))*(2*size);
	
	for(t=0;t<csize;t++)
	{
		temp=hzk[t];
		
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)
			{
				disp_dev.put_pixel_buffer(x0,y0,color,buf);
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







void show_charn_buffer(uint16_t x,uint16_t y,uint16_t color,uint8_t code,font_type font,uint8_t * buf)
{
	uint8_t * hzk = NULL;

	uint16_t size,csize;

    size = font.width;
	
	csize=(size/8+((size%8)?1:0))*(2*size);
	hzk = (uint8_t *)my_malloc(csize);
	
	if(hzk == NULL)
	{
		return ;
	}
	get_char_font(code,font,hzk);
	put_charn_pixel_buffer(x,y,color,hzk,size,buf);
	my_free(hzk);
	hzk = NULL;

	
}


void put_hzn_pixel_buffer(uint16_t x,uint16_t y,uint16_t color,uint8_t *hzk,uint16_t size,uint8_t * buf)
{
	uint16_t temp,t,t1;
	uint16_t y0 = y;
	uint16_t x0 = x;
	uint16_t csize=(size/8+((size%8)?1:0))*(size);
	for(t=0;t<csize;t++)
	{
		temp=hzk[t];
		
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)
			{
				disp_dev.put_pixel_buffer(x0,y0,color,buf);
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






void put_char_pixel_buffer(uint16_t x,uint16_t y,uint16_t color,uint16_t back_color,uint8_t *hzk,uint16_t size,uint8_t * buf)
{
	uint16_t temp,t,t1;
	uint16_t y0 = y;
	uint16_t x0 = x;
	uint16_t csize=(size/8+((size%8)?1:0))*(2*size);
	
	for(t=0;t<csize;t++)
	{
		temp=hzk[t];
		
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)
			{
				disp_dev.put_pixel_buffer(x0,y0,color,buf);
			}
			else
			{
			    disp_dev.put_pixel_buffer(x0,y0,back_color,buf);
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



void show_nfont_buffer(uint16_t x,uint16_t y,uint16_t color,uint16_t code,font_type font,uint8_t * buf)
{
	uint8_t * hzk = NULL;
	uint16_t size,csize;

	size = font.width;

	csize=(size/8+((size%8)?1:0))*(size);
	hzk = (uint8_t *)my_malloc(csize);
	if(hzk == NULL)
	{
		return ;
	}
	switch(font.code)
	{
	    case 1:
	        get_gb_font(code,font,hzk);
	        break;
	    case 2:
	        get_gbk_font(code,font,hzk);
	        break;
	    case 3:
	        get_unicode_font(code,font,hzk);
	        break;
	    default:
	        my_free(hzk);
	        return;    
	}
	
	put_hzn_pixel_buffer(x,y,color,hzk,size,buf);
	my_free(hzk);
	hzk = NULL;

	
}










void show_char_buffer(uint16_t x,uint16_t y,uint16_t color,uint16_t back_color,uint8_t code,font_type font,uint8_t * buf)
{
	uint8_t * hzk = NULL;

	uint16_t size,csize;

	size = font.width;

	csize=(size/8+((size%8)?1:0))*(2*size);
	hzk = (uint8_t *)my_malloc(csize);
	
	if(hzk == NULL)
	{
		return ;
	}
	get_char_font(code,font,hzk);
	put_char_pixel_buffer(x,y,color,back_color,hzk,size,buf);
	my_free(hzk);
	hzk = NULL;

	
}






void put_hz_pixel_buffer(uint16_t x,uint16_t y,uint16_t color,uint16_t back_color,uint8_t *hzk,uint16_t size,uint8_t * buf)
{
	uint16_t temp,t,t1;
	uint16_t y0 = y;
	uint16_t x0 = x;
	uint16_t csize=(size/8+((size%8)?1:0))*(size);
	for(t=0;t<csize;t++)
	{
		temp=hzk[t];
		
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)
			{
				disp_dev.put_pixel_buffer(x0,y0,color,buf);
			}
			else
			{
			    disp_dev.put_pixel_buffer(x0,y0,back_color,buf);
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





void show_font_buffer(uint16_t x,uint16_t y,uint16_t color,uint16_t back_color,uint16_t code,font_type font,uint8_t * buf)
{
	uint8_t * hzk = NULL;
	uint16_t size,csize;
	size = font.width;

	csize=(size/8+((size%8)?1:0))*(size);
	hzk = (uint8_t *)my_malloc(csize);
	if(hzk == NULL)
	{
		return ;
	}
	switch(font.code)
	{
	    case 1:
	        get_gb_font(code,font,hzk);
	        break;
	    case 2:
	        get_gbk_font(code,font,hzk);
	        break;
	    case 3:
	        get_unicode_font(code,font,hzk);
	        break;
	    default:
	        my_free(hzk);
	        return;    
	}
	
	put_hzn_pixel_buffer(x,y,color,hzk,size,buf);
	my_free(hzk);
	hzk = NULL;

	
}


//void font_zoom(uint8_t * hzk, uint16_t size, uint8_t * dzk, uint16_t dsize)
//{
//    if(hzk == NULL || dzk == NULL)
//    {
//        return;
//    }

//    uint16_t i,j,k,b,t,i1,j1,k1,b1,t1;

//    b = dsize / 8 + ((dsize%8)?1:0);
//    b1 = size / 8 + ((size%8)?1:0);

//    memset(dzk,0,dsize*b);

//   k = 0;
//   

//    for(i=0;i<dsize;i++)
//    {
//        i1 = i * size / dsize;
//        k1 = i1 * b1;
//        
//        for(j=0;j<dsize;j++)
//        {
//            j1 = j * size / dsize;
//            t = k + j / 8;
//            t1 = k1 + j1 / 8;

//            if(hzk[t1] & (j1%8))
//            {
//                dzk[t] |= (1 << (j % 8));
//            }

//          
//        }

//        k += b;
//       
//    }
//}




//void char_zoom(uint8_t * hzk, uint16_t size, uint8_t * dzk, uint16_t dsize)
//{
//    if(hzk == NULL || dzk == NULL)
//    {
//        return;
//    }

//    uint16_t i,j,k,b,t,i1,j1,k1,b1,t1,line;

//    b = dsize / 8 + ((dsize%8)?1:0);
//    b1 = size / 8 + ((size%8)?1:0);
//    line = 2*dsize;

//    memset(dzk,0,line*b);
//	

//    
//   

//    for(i=0;i<line;i++)
//    {
//        i1 = i * size / dsize;
//        k1 = i1 * b1;
//		k = i * b;
//		
//        
//        for(j=0;j<dsize;j++)
//        {
//            j1 = j * size / dsize;
//            t = k + j / 8;
//            t1 = k1 + j1 / 8;
//			
//			
//			//PRINT("dsize=%d, i=%d, i1=%d, j=%d, j1=%d, t=%d, t1=%d\r\n",dsize,i,i1,j,j1,t,t1);
//            if(hzk[t1] & (1<<(j1%8)))
//            {
//                dzk[t] |= (1 << (j % 8));
//            }
//			
//			
//			
//			

//          
//        }

//        
//       
//    }
//}
















void show_nfont_rect(uint16_t x,uint16_t y,uint16_t color,uint16_t code,font_type font,t_rect rect)
{
	uint8_t * hzk = NULL;
	uint16_t size,csize;
	
    size = font.width;
   
	csize=(size/8+((size%8)?1:0))*(size);
	hzk = (uint8_t *)my_malloc(csize);
	
	if(hzk == NULL)
	{
		return ;
	}
	switch(font.code)
	{
	    case 1:
	        get_gb_font(code,font,hzk);
	        break;
	    case 2:
	        get_gbk_font(code,font,hzk);
	        break;
	    case 3:
	        get_unicode_font(code,font,hzk);
	        break;
	    default:
	        my_free(hzk);
	        return;    
	}
	
	put_hzn_pixel_rect(x,y,color,hzk,size,rect);
	my_free(hzk);
	hzk = NULL;

	
}






void show_charn_font_rect(uint16_t x,uint16_t y,uint16_t color,uint8_t code,font_type font,t_rect rect)
{
	uint8_t * hzk = NULL;

	uint16_t size,csize;

	size = font.width;

	csize=(size/8+((size%8)?1:0))*(2*size);
	hzk = (uint8_t *)my_malloc(csize);
	
	if(hzk == NULL)
	{
		return ;
	}
	get_char_font(code,font,hzk);
	
	put_charn_pixel_rect(x,y,color,hzk,size,rect);
	my_free(hzk);
	hzk = NULL;
	
}
















void put_charn_pixel_dc(uint16_t x,uint16_t y,uint16_t color,uint8_t *hzk,uint16_t size,dc tdc)
{
	uint16_t temp,t,t1;
	uint16_t y0 = y;
	uint16_t x0 = x;
	uint16_t csize=(size/8+((size%8)?1:0))*(2*size);
	uint16_t *p = (uint16_t *) tdc.p;
	
	for(t=0;t<csize;t++)
	{
		temp=hzk[t];
		
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)
			{
				if(x0<tdc.width  && y0<tdc.height)
				{
					color |= (1 << 15);
				    p[y0*tdc.width + x0] = color;
				}
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





void show_charn_dc(uint16_t x,uint16_t y,uint16_t color,uint8_t code,font_type font,dc tdc)
{
	uint8_t * hzk = NULL;

	uint16_t size,csize;

	size = font.width;

	csize=(size/8+((size%8)?1:0))*(2*size);
	hzk = (uint8_t *)my_malloc(csize);
	
	if(hzk == NULL)
	{
		return ;
	}
	get_char_font(code,font,hzk);
	put_charn_pixel_dc(x,y,color,hzk,size,tdc);
	my_free(hzk);
	hzk = NULL;

	
}





void show_nfont_dc(uint16_t x,uint16_t y,uint16_t color,uint16_t code,font_type font,dc tdc)
{
	uint8_t * hzk = NULL;
	uint16_t size,csize;
	
    size = font.width;
   
	csize=(size/8+((size%8)?1:0))*(size);
	hzk = (uint8_t *)my_malloc(csize);
	
	if(hzk == NULL)
	{
		return ;
	}
	switch(font.code)
	{
	    case 1:
	        get_gb_font(code,font,hzk);
	        break;
	    case 2:
	        get_gbk_font(code,font,hzk);
	        break;
	    case 3:
	        get_unicode_font(code,font,hzk);
	        break;
	    default:
	        my_free(hzk);
	        return;    
	}
	put_hzn_pixel_dc(x,y,color,hzk,size,tdc);
	my_free(hzk);
	hzk = NULL;

	
}

font_type * get_font_type(uint8_t font_num)
{
    uint8_t i;

    for(i=0;i<src_cfg.font_set->font_num;i++)
    {
        if(src_cfg.font_set->pfont[i].num == font_num)
        {
            return &(src_cfg.font_set->pfont[i]);
        }
        
    }
	
	return NULL;
}










