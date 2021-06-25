#include "icon.h"
#include "gui.h"
#include "mem_my.h"
#include "display.h"
#include "ff.h"
#include "nand.h"
#include "cfg.h"
#include "string.h"

//static uint8_t is_icon_file(uint8_t * filename)
//{
//	uint8_t len;
//	len = strlen((const char*)filename);
//	if(len < 3)
//	{	
//		return 0;
//	}
//	
//	if((filename[len-4]=='I')&&(filename[len-3]=='C')&&(filename[len-2]=='O')&&(filename[len-1]=='N'))
//	{
//		return 1;
//	}
//	else
//	{
//		return 0;
//	}
//	
//}


 uint16_t color_mix(uint16_t col1,uint16_t col2,uint8_t a)
 {
	 uint8_t c1[3];
	 uint8_t c2[3];
	 uint8_t c3[3];
	 
	 c1[0]=(col1)&(0x1f);
	 c1[1]=(col1>>5)&(0x1f);
	 c1[2]=(col1>>10)&(0x1f);
	 
	 c2[0]=(col2)&(0x1f);
	 c2[1]=(col2>>5)&(0x1f);
	 c2[2]=(col2>>10)&(0x1f);
	 
	 c3[0]=c1[0]*a/0xff + c2[0]*(0xff-a)/0xff;
	 c3[1]=c1[1]*a/0xff + c2[1]*(0xff-a)/0xff;
	 c3[2]=c1[2]*a/0xff + c2[2]*(0xff-a)/0xff;
	 
	 return (c3[2]<<10)|(c3[1]<<5)|(c3[0]);
 }
 

static void zoom_icon_image(uint8_t * simage,uint16_t sw,uint16_t sh,uint8_t * dimage,uint16_t dw,uint16_t dh)
{
    uint32_t * src = (uint32_t *)simage;
    uint32_t * dst = (uint32_t *)dimage;
    uint16_t i,j,i1,j1;
   

    for(i=0;i<dh;i++)
    {
        for(j=0;j<dw;j++)
        {
           i1 = i*sh/dh;
           j1 = j*sw/dw;
           dst[i*dw+j] = src[i1*sw+j1];
        }
    }
}

//static void zoom_image(uint8_t * simage,uint16_t sw,uint16_t sh,uint8_t * dimage,uint16_t dw,uint16_t dh)
//{
//    uint16_t * src = (uint16_t *)simage;
//    uint16_t * dst = (uint16_t *)dimage;
//    uint16_t i,j,i1,j1;
//   

//    for(i=0;i<dh;i++)
//    {
//        for(j=0;j<dw;j++)
//        {
//           i1 = i*sh/dh;
//           j1 = j*sw/dw;
//           dst[i*dw+j] = src[i1*sw+j1];
//        }
//    }
//}

uint16_t  RGB888_to_RGB555(uint8_t * rgb)
{
    uint8_t r,g,b;

    r = rgb[2];
    g = rgb[1];
    b = rgb[0];

    return (b >> 3) | ((g >> 3)<<5) | ((r >> 3) << 10);
}





void show_vicon_image(icon_image image,uint16_t x,uint16_t y,t_rect rect,uint8_t *framebuf)
{
    if(image.p== NULL || framebuf == NULL)
    {
        return;
    }

   
    
    uint16_t w,h,i,j;
    uint8_t a;
    uint16_t color;
    uint16_t * p = (uint16_t *) framebuf;
    uint8_t * buff;
    w = rect.right - rect.left + 1;
    h = rect.bottom - rect.top + 1;
	
	uint32_t k;


    if(w > image.width || h > image.height)
    {
        return;
    }

    buff = &(image.p[(rect.top * image.width + rect.left) * 4]); 

   
	k = y * disp_dev.width;
    

    for(i=y+rect.top;i<=y+rect.bottom;i++)
    {
        for(j=x+rect.left;j<=x+rect.right;j++)
        {
           a = buff[3];
           
           if(a)
           {
               
               color = RGB888_to_RGB555(buff);
               
               if(a==0xff)
			   {
				   p[k+j] = color;
			   }
			   else
			   {
				   p[k+j] = color_mix(color,p[k+j],a);
			   }
           }

           buff += 4;
        }

		//buff = &(image.p[((j-y) * image.width + rect.left) * 4]);
	    k += disp_dev.width;
		
    }

    
 }



/*
void show_icon_image(icon_image image,uint16_t x,uint16_t y,uint8_t *framebuf)
{
    if(image.p== NULL || framebuf == NULL)
    {
        return;
    }

    uint16_t w,h,i,j;
	uint32_t k;
    uint8_t a;
    uint16_t color;
    uint16_t * p = (uint16_t *) framebuf;
    uint8_t * buff = image.p;
    w = image.width;
    h = image.height;

    

   
    
	k = y * disp_dev.width;
    

    for(i=y;i<y+h;i++)
    {
        for(j=x;j<x+w;j++)
        {
           a = buff[3];
           
           if(a)
           {
               
               color = RGB888_to_RGB555(buff);
               
               if(a==0xff)
			   {
				   p[k+j] = color;
			   }
			   else
			   {
				   p[k+j] = color_mix(color,p[k+j],a);
			   }
           }

           buff += 4;
        }

		
	    k += disp_dev.width;
		
    }

    
 }
 */

 void show_icon_image(icon ticon,icon_image image)
 {
    
    uint16_t w,h,w1,h1;

    h = image.height;
    w = image.width;
    w1 = ticon.width;
    h1 = ticon.height;
    

    if(w==w1 && h==h1)
    {
        uint32_t size;
        size = h1 * w1 * ticon.byte;
        memcpy(image.p,ticon.p,size);   
    }
    else
    {
        zoom_icon_image(ticon.p,w1,h1,image.p,w,h);
    }
 }

// void show_image(uint8_t *image,uint16_t width,uint16_t height,t_rect rect,uint8_t *framebuf)
//{
//    if(image == NULL || framebuf == NULL)
//    {
//        return;
//    }

//    uint16_t h,w;
//    
//	dc sdc;

//    w = rect.right - rect.left + 1;
//    h = rect.bottom - rect.top + 1;
//	
//	sdc.p = image;
////	sdc.x = 0;
////	sdc.y = 0;
//	sdc.width = w;
//	sdc.height = h;

//   

//    if(h != height || w != width)
//    {
//        sdc.p = my_malloc(w*h*sizeof(unsigned short));

//        if(sdc.p == NULL)
//        {
//            return;
//        }

//        zoom_image(image, width,height,sdc.p,w,h);     
//    }

//   
//	vblt(framebuf,rect.left,rect.top,  sdc );
//	
//	if(sdc.p != image)
//	{
//		my_free(sdc.p);
//	}

//    

//    
// }

 uint8_t init_icon(icon * picon,uint32_t addr)
 {
    uint8_t buf[16];
    uint32_t block,page,adr;
    uint32_t size;

    picon->addr = addr;
    page = picon->addr / nand_dev.page_mainsize;
    block = src_cfg.block_icon_addr + page / nand_dev.block_pagenum;
    page = page % nand_dev.block_pagenum;
    adr = picon->addr % nand_dev.page_mainsize;

    nand_dev.read(block,page,adr,buf,16);

    picon->width = (buf[5]<<8) | (buf[4]);
    picon->height = (buf[7]<<8) | (buf[6]);
    picon->num = (buf[9]<<8) | (buf[8]);   
    picon->byte = 4;
    picon->count = 0;
    size = picon->width*picon->height*picon->byte;
    picon->p = my_malloc(size);

    if(picon->p == NULL)
    {
        return 1;
    }
	
//    adr += 16;
//    if(adr >= nand_dev.page_mainsize)
//    {
//        page++;
//        adr -= nand_dev.page_mainsize;
//        if(page == nand_dev.block_pagenum)
//        {
//            page = 0;
//            block++;
//        }
//    }

//     nand_dev.read(nand_dev.lut[block],page,adr,picon->p,size);
    
	return 0;
    
    
 }

 uint8_t get_icon_image(icon *picon,uint16_t count)
 {
      uint32_t block,page,addr;
      uint32_t size,offset;

      if(count >= picon->num)
      {
          return 1;
      }

      size = picon->height * picon->width * picon->byte;
      offset = picon->addr + count*size + 16;

      page = offset / nand_dev.page_mainsize;
      block = src_cfg.block_icon_addr + page / nand_dev.block_pagenum;
      page = page % nand_dev.block_pagenum;
      addr = offset % nand_dev.page_mainsize;

      nand_dev.read(block,page,addr,picon->p,size);
	  
	  return 0;
      
 }


 
 void show_icon_image_mix(icon ticon,t_rect rect,uint8_t *framebuf)
{
    if(ticon.p == NULL || framebuf == NULL)
    {
        return;
    }
	

    uint16_t h,w,i,j;
    uint8_t * buff = ticon.p;
    uint8_t a;
    uint16_t color;
	uint16_t *p = (uint16_t *) framebuf;
	uint32_t k;
	
	uint8_t * buf = buff;

    w = rect.right - rect.left;
    h = rect.bottom - rect.top;

   

    if(h != ticon.height || w != ticon.width)
    {
		
		
        buff = my_malloc(w*h*sizeof(unsigned int));

        if(buff == NULL)
        {
            return;
        }
		
		buf = buff;

        zoom_icon_image(ticon.p, ticon.width,ticon.height,buff,w,h);     
    }
	
	k = rect.top * disp_dev.width;

    for(i=rect.top;i<rect.bottom;i++)
    {
		
        for(j=rect.left;j<rect.right;j++)
        {
           a = buff[3];
           
           if(a)
           {
               
               color = RGB888_to_RGB555(buff);
			   if(a==0xff)
			   {
				   p[k+j] = color;
			   }
			   else
			   {
				   p[k+j] = color_mix(color,p[k+j],a);
			   }
			   
               
           }

           buff += 4;
        }
		
		k += disp_dev.width;
		
		
    }
	
	if(buf != ticon.p)
	{
	
		my_free(buf);

	}

    

    
 }

 

void show_icon_test(void)
{
    uint8_t res;
	FATFS fs;
	t_rect rect;
	icon ticon;
	FIL file;
	uint32_t bw,size,size1,temp_size;
	
    res = f_mount (&fs,"0:",1);

    if(res != FR_OK)
    {
        return;
    }

    res = f_open(&file,"0:/ICON/11",FA_READ);
    uint8_t * buf = my_malloc(16);
    res = f_read(&file,(void *)buf,16,&bw);
	ticon.width	= buf[4] | (buf[5] << 8);
    ticon.height = buf[6] | (buf[7] << 8);
    size = ticon.width*ticon.height*4;
    ticon.p = my_malloc(size);

    size1 = 0;

    while(size1 < size)
    {

        temp_size = ((size - size1) > 512) ? (512) : (size - size1);
        res = f_read(&file,&ticon.p[size1],temp_size,&bw);	
		size1 += temp_size;
    }

    rect.left = 0;
    rect.right = 0+ ticon.width - 1;
    rect.top = 100;
    rect.bottom = 100 + ticon.height - 1;


    show_icon_image_mix(ticon,rect,disp_dev.disp[0].p);

    my_free(ticon.p);
	f_close(&file);
	
	
	res = f_open(&file,"0:/ICON/22",FA_READ);
    res = f_read(&file,(void *)buf,16,&bw);

    ticon.width = buf[4] | (buf[5] << 8);
    ticon.height = buf[6] | (buf[7] << 8);

 
    size = ticon.width*ticon.height*4;
    ticon.p = my_malloc(size);

    size1 = 0;

    while(size1 < size)
    {

        temp_size = ((size - size1) > 512) ? (512) : (size - size1);
        res = f_read(&file,&ticon.p[size1],temp_size,&bw);	
		size1 += temp_size;
    }

    rect.left = 200;
    rect.right = 200+ ticon.width - 1;
    rect.top = 100;
    rect.bottom = 100 + ticon.height - 1;
    show_icon_image_mix(ticon,rect,disp_dev.disp[0].p);
    my_free(ticon.p);
	f_close(&file);
	
	
	res = f_open(&file,"0:/ICON/33",FA_READ);
    res = f_read(&file,(void *)buf,16,&bw);

    ticon.width = buf[4] | (buf[5] << 8);
    ticon.height = buf[6] | (buf[7] << 8);

    size = ticon.width*ticon.height*4;
    ticon.p = my_malloc(size);

    size1 = 0;

    while(size1 < size)
    {

        temp_size = ((size - size1) > 512) ? (512) : (size - size1);
        res = f_read(&file,&ticon.p[size1],temp_size,&bw);	
		size1 += temp_size;
    }

    rect.left = 400;
    rect.right = 400+ ticon.width - 1;
    rect.top = 100;
    rect.bottom = 100 + ticon.height - 1;
    show_icon_image_mix(ticon,rect,disp_dev.disp[0].p);
    my_free(ticon.p);
	f_close(&file);
	
	
	res = f_open(&file,"0:/ICON/44",FA_READ);
	buf = my_malloc(16);
    res = f_read(&file,(void *)buf,16,&bw);

    ticon.width = buf[4] | (buf[5] << 8);
    ticon.height = buf[6] | (buf[7] << 8);

    
    size = ticon.width*ticon.height*4;
    ticon.p = my_malloc(size);

    size1 = 0;

    while(size1 < size)
    {

        temp_size = ((size - size1) > 512) ? (512) : (size - size1);
        res = f_read(&file,&ticon.p[size1],temp_size,&bw);	
		size1 += temp_size;
    }

    rect.left = 600;
    rect.right = 600+ ticon.width - 1;
    rect.top = 100;
    rect.bottom = 100 + ticon.height - 1;
    show_icon_image_mix(ticon,rect,disp_dev.disp[0].p);
    my_free(buf);
	my_free(ticon.p);
	f_close(&file);

    while(1)
    {
        ;
    }
    
    
    
    
}
