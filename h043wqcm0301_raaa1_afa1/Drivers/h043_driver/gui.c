#include "gui.h"
#include "display.h"
#include "nand.h"
#include "cfg.h"
#include "mem_my.h"
#include <string.h>
#include "stdlib.h"
#include <math.h>



//gui_layer  * bottom_layer = NULL;



//ճ����ʾ��
void blt( uint8_t * dp, uint16_t dx, uint16_t dy, uint16_t dw, uint16_t dh, uint8_t * sp )
{
	uint16_t i,size;
	uint8_t *sp1,*dp1;
	

	if((dp==NULL)||(sp==NULL))
	{
		return;
	}

	if(dx >= disp_dev.width)
	{
		return;
	}

	if(dy >= disp_dev.height)
	{
		return;
	}

	if(dx + dw >= disp_dev.width)
	{
		dw = disp_dev.width - dx;
	}

	if(dy + dh >= disp_dev.height)
	{
		dh = disp_dev.height - dy;
	}

	size = dw * disp_dev.bpp;

	for(i = dy; i < dy + dh; i++)
	{
		sp1 = &sp[i * disp_dev.width * disp_dev.bpp + dx * disp_dev.bpp];
		dp1 = &dp[i * disp_dev.width * disp_dev.bpp + dx * disp_dev.bpp];

		memcpy((void *)dp1,(const void *)sp1,size);
	}
	


}


//void vblt(uint8_t * dp, uint16_t dx, uint16_t dy, dc sdc )
//{
//	uint16_t i,size;
//	uint8_t *dp1,*sp1;
//	
//	if((dp == NULL)||(sdc.p == NULL))
//	{
//		return;
//	}

//	if(dx >= disp_dev.width)
//	{
//		return;
//	}

//	if(dy >= disp_dev.height)
//	{
//		return;
//	}

//	size = sdc.width*disp_dev.bpp;

//	for(i = 0; i < sdc.height; i++)
//	{
//		sp1 = &(sdc.p[i*size]);
//		dp1 = &dp[(dy + i) * disp_dev.width * disp_dev.bpp + dx * disp_dev.bpp];
//		memcpy((void *)dp1,(void *)sp1,size);
//	}
//}

void tvblt(uint8_t * dp, uint16_t dx, uint16_t dy, dc sdc )
{
	uint16_t i,size;
	uint8_t *dp1,*sp1;
	
	if((dp == NULL)||(sdc.p == NULL))
	{
		return;
	}

	if(dx >= disp_dev.width)
	{
		return;
	}

	if(dy >= disp_dev.height)
	{
		return;
	}

	size = sdc.width*disp_dev.bpp;

	for(i = 0; i < sdc.height; i++)
	{
		sp1 = &(sdc.p[i*size]);
		dp1 = &dp[(dy + i) * disp_dev.width * disp_dev.bpp + dx * disp_dev.bpp];
		mem_copy4((void *)dp1,(void *)sp1,size);
	}
}


void vbltdc(dc ddc, uint16_t dx, uint16_t dy,uint16_t width,uint16_t height, dc sdc )
{
	uint16_t i,size;
	uint8_t *dp1,*sp1;
	
	if((ddc.p==NULL)||(sdc.p ==NULL))
	{
		return;
	}

	if(dx >= disp_dev.width)
	{
		return;
	}

	if(dy >= disp_dev.height)
	{
		return;
	}

	if(ddc.width != sdc.width || ddc.height != sdc.height)
	{
	    return ;
	}

	if(dx > ddc.width || dy > ddc.height)
	{
	    return;
	}

	if(dx + width > ddc.width)
	{
	   dx = ddc.width - dx;
	}
	if(dy + height > ddc.height)
	{
	    height = ddc.height - dy;
	}

	size = width*disp_dev.bpp;
	
	

	for(i = 0; i < height; i++)
	{
		sp1 = &(sdc.p[(i+dy)*sdc.width*disp_dev.bpp + dx*disp_dev.bpp]);
		dp1 = &(ddc.p[(dy+i)*ddc.width*disp_dev.bpp + dx*disp_dev.bpp]);
		memcpy((void *)dp1,(void *)sp1,size);
		
	}
	
	
}

void tvbltdc(dc ddc, uint16_t dx, uint16_t dy,uint16_t width,uint16_t height, dc sdc )
{
	uint16_t i,size;
	uint8_t *dp1,*sp1;
	
	if((ddc.p==NULL)||(sdc.p ==NULL))
	{
		return;
	}

	if(dx >= disp_dev.width)
	{
		return;
	}

	if(dy >= disp_dev.height)
	{
		return;
	}

	if(ddc.width != sdc.width || ddc.height != sdc.height)
	{
	    return ;
	}

	if(dx > ddc.width || dy > ddc.height)
	{
	    return;
	}

	if(dx + width > ddc.width)
	{
	   dx = ddc.width - dx;
	}
	if(dy + height > ddc.height)
	{
	    height = ddc.height - dy;
	}

	size = width*disp_dev.bpp;
	
	

	for(i = 0; i < height; i++)
	{
		sp1 = &(sdc.p[(i+dy)*sdc.width*disp_dev.bpp + dx*disp_dev.bpp]);
		dp1 = &(ddc.p[(dy+i)*ddc.width*disp_dev.bpp + dx*disp_dev.bpp]);
		//taskENTER_CRITICAL();
		mem_copy4((void *)dp1,(void *)sp1,size);
		//taskEXIT_CRITICAL();
		
	}
	
	
}


 





//void blt_dc_zoom(bdc ddc, dc sdc, t_rect rect)
//{
//    if(ddc.x+ddc.width > ddc.vdc.width || ddc.y+ddc.height > ddc.vdc.height)
//    {
//        return;
//    }

//    uint16_t x1,y1,x2,y2,x,y;
//	float x0,y0,fw,fh,fx1,fx2,fy1,fy2;
//	float s1,s2,s3,s4;
//	uint8_t r,g,b;
//	uint8_t a1,a2,a3,a4;

//    uint16_t *dp  = (uint16_t *)ddc.vdc.p;
//    uint16_t *sp  = (uint16_t *)sdc.p;
//	
//	fw = (float)sdc.width / ddc.width;
//	fh = (float)sdc.height / ddc.height;
//	
//	uint16_t color;

//    for(y=0;y<ddc.height;y++)
//    {
//		if((y+ddc.y)>=rect.top && (y+ddc.y)<=rect.bottom)
//		{
//			y0 = y*fh;
//			y1 = (uint16_t)y0;
//			
//			if(y1<sdc.height-1)
//			{
//				y2 = y1 + 1;
//			}
//			else
//			{
//				y2 = y1;
//			}
//			
//			fy1 = y0 - y1;
//			fy2 = 1.0 - fy1;
//			
//        
//			for(x=0;x<ddc.width;x++)
//			{
//				if((x+ddc.x)>=rect.left && (x+ddc.x)<=rect.right)
//				{
//					x0 = x*fw;
//					x1 = (uint16_t)x0;
//					if(x1<sdc.width)
//					{
//						x2 = x1 + 1; 
//					}
//					else
//					{
//						x2 = x1;
//					}
//					
//					fx1 = x0 - x1;
//					fx2 = 1.0 - fx1;
//					
//					s1 = fx1 * fy1;
//					s2 = fx2 * fy1;
//					s3 = fx2 * fy2;
//					s4 = fx1 * fx2;
//					
//					a1 = ((sp[x1+y1*sdc.width] >> 15) & (1));
//					a2 = ((sp[x2+y1*sdc.width] >> 15) & (1));
//					a3 = ((sp[x2+y2*sdc.width] >> 15) & (1));
//					a4 = ((sp[x1+y2*sdc.width] >> 15) & (1));
//					
//					r = (a1*((sp[x1*y1*sdc.width] >> 10)&0x1f)*s3) + (a2*((sp[x2*y1*sdc.width] >> 10)&0x1f)*s4) + (a3*((sp[x2*y2*sdc.width] >> 10)&0x1f)*s2) + (a4*((sp[x1*y2*sdc.width] >> 10)&0x1f)*s1);
//					g = (a1*((sp[x1*y1*sdc.width] >> 5)&0x1f)*s3) + (a2*((sp[x2*y1*sdc.width] >> 5)&0x1f)*s4) + (a3*((sp[x2*y2*sdc.width] >> 5)&0x1f)*s2) + (a4*((sp[x1*y2*sdc.width] >> 5)&0x1f)*s1); 
//					b = (a1*((sp[x1*y1*sdc.width])&0x1f)*s3) + (a2*((sp[x2*y1*sdc.width])&0x1f)*s4) + (a3*((sp[x2*y2*sdc.width])&0x1f)*s2) + (a4*((sp[x1*y2*sdc.width])&0x1f)*s1); 
//					
//					color = (r<<10) | (g<<5) | (b);
//					
//					if(color > 0)
//					{
//						dp[(y+ddc.y)*ddc.vdc.width + (x+ddc.x)] = color;
//					}
//					
//					
//				}
//			}
//		}
//    }
//}


//void blt_dc_zoom(bdc ddc, dc sdc, t_rect rect)
//{
//    if(ddc.x+ddc.width > ddc.vdc.width || ddc.y+ddc.height > ddc.vdc.height)
//    {
//        return;
//    }

//    uint16_t x,y;
//	float fw,fh,x0,y0;
//	uint16_t x1,y1,x2,y2;
//	float s1,s2,s3,s4;
//	float dx1,dx2,dy1,dy2;
//	
//	
//	fw = (float)sdc.width / ddc.width;
//	fh = (float)sdc.height / ddc.height;
//	uint16_t *dp  = (uint16_t *)ddc.vdc.p;
//	uint16_t *sp  = (uint16_t *)sdc.p;

//    for(y=0;y<ddc.height;y++)
//    {
//		if((y+ddc.y)>=rect.top && (y+ddc.y)<=rect.bottom)
//		{
//			
//			y0 = y*fh;
//			y1 = (uint16_t) y0;
//			dy1 = y0 - y1;
//			if(y1 < sdc.height - 1)
//			{
//				y2 = y1 + 1;
//				dy2 = ((float)y2 - y0);
//			}
//			else
//			{
//				y2 = y1;
//				dy2 = dy1;
//			}
//			
//			
//			
//        
//			for(x=0;x<ddc.width;x++)
//			{
//				if((x+ddc.x)>=rect.left && (x+ddc.x)<=rect.right)
//				{
//					x0 = x*fw;
//					x1 = (uint16_t)x0;
//					dx1 = x0 - x1;
//					if(x1 < sdc.width -1)
//					{
//						x2 = x1 + 1;
//						dx2 = ((float)x2 - x0);
//					}
//					else
//					{
//						x2 = x1;
//						dx2 = dx1;
//					}
//					
//					s1 = dx1*dy1;
//					s2 = dx2*dy1;
//					s3 = dx2*dy2;
//					s4 = dx1*dy2;
//					
//					
//					if(sp[y1*sdc.width+x1]&(1<<15) && s1<0.25)
//					{
//						dp[(y+ddc.y)*ddc.vdc.width + (x+ddc.x)] = sp[y1*sdc.width+x1] & (~(1<<5));
//					}
//					else if(sp[y1*sdc.width+x2]&(1<<15) && s2<0.25)
//					{
//						dp[(y+ddc.y)*ddc.vdc.width + (x+ddc.x)] = sp[y1*sdc.width+x2] & (~(1<<5));
//					}
//					else if(sp[y2*sdc.width+x2]&(1<<15) && s3<0.25)
//					{
//						dp[(y+ddc.y)*ddc.vdc.width + (x+ddc.x)] = sp[y2*sdc.width+x2] & (~(1<<5));
//					}
//					else if(sp[y2*sdc.width+x1]&(1<<15) && s4<0.25)
//					{
//						dp[(y+ddc.y)*ddc.vdc.width + (x+ddc.x)] = sp[y2*sdc.width+x1] & (~(1<<5));
//					}
//						
//					
//					
//				}
//			}
//		}
//    }
//}


void blt_dc_zoom(bdc ddc, dc sdc, t_rect rect)
{
    if(ddc.x+ddc.width > ddc.vdc.width || ddc.y+ddc.height > ddc.vdc.height)
    {
        return;
    }

    uint16_t x,y;
	float fw,fh,x0,y0;
	uint16_t x1,y1,x2,y2;
	
	
	
	fw = (float)sdc.width / ddc.width;
	fh = (float)sdc.height / ddc.height;
	uint16_t *dp  = (uint16_t *)ddc.vdc.p;
	uint16_t *sp  = (uint16_t *)sdc.p;

    for(y=0;y<ddc.height;y++)
    {
		if((y+ddc.y)>=rect.top && (y+ddc.y)<=rect.bottom)
		{
			
			y0 = y*fh;
			y1 = (uint16_t) y0;
			
			if(y1 < sdc.height - 1)
			{
				y2 = y1 + 1;
				
			}
			else
			{
				y2 = y1;
				
			}
			
			
			
        
			for(x=0;x<ddc.width;x++)
			{
				if((x+ddc.x)>=rect.left && (x+ddc.x)<=rect.right)
				{
					x0 = x*fw;
					x1 = (uint16_t)x0;
					
					if(x1 < sdc.width -1)
					{
						x2 = x1 + 1;
						
					}
					else
					{
						x2 = x1;
						
					}
					
					
					
					
					if(sp[y1*sdc.width+x1]&(1<<15))
					{
						dp[(y+ddc.y)*ddc.vdc.width + (x+ddc.x)] = sp[y1*sdc.width+x1] & (~(1<<5));
					}
					else if(sp[y1*sdc.width+x2]&(1<<15))
					{
						dp[(y+ddc.y)*ddc.vdc.width + (x+ddc.x)] = sp[y1*sdc.width+x2] & (~(1<<5));
					}
					else if(sp[y2*sdc.width+x2]&(1<<15))
					{
						dp[(y+ddc.y)*ddc.vdc.width + (x+ddc.x)] = sp[y2*sdc.width+x2] & (~(1<<5));
					}
					else if(sp[y2*sdc.width+x1]&(1<<15))
					{
						dp[(y+ddc.y)*ddc.vdc.width + (x+ddc.x)] = sp[y2*sdc.width+x1] & (~(1<<5));
					}
						
					
					
				}
			}
		}
    }
}






void vblt_bdc(dc ddc,bdc sdc)
{
    uint16_t x,y,w,h,i;
    uint32_t size;
    uint8_t *dp,*sp;
    
    x=sdc.x;
    y=sdc.y;
    w = sdc.width;
    h = sdc.height;
    
    if(sdc.vdc.width !=ddc.width || sdc.vdc.height != ddc.height)
    {
        return;
    }

    if(x > sdc.vdc.width || y > sdc.vdc.height)
    {
        return;
    }

    if(x+w > sdc.vdc.width)
    {
        w = sdc.vdc.width - x;
    }
    if(y+h > sdc.vdc.height)
    {
        h = sdc.vdc.height - y;
    }

    size = w*disp_dev.bpp;

    h += sdc.y;

    for(i=sdc.y;i<h;i++)
    {
        dp = &ddc.p[(x+i*ddc.width)*disp_dev.bpp];
        sp = &sdc.vdc.p[(x+i*sdc.vdc.width)*disp_dev.bpp];
        memcpy((void *)dp,(void*)sp,size);
    }
    
}






//void vblt(dc ddc, uint16_t dx, uint16_t dy,t_rect rect,dc sdc )
//{
//	uint16_t i,size,width,height;
//	uint8_t *dp1,*sp1;
//	
//	if((ddc.p==NULL)||(sdc.p ==NULL))
//	{
//		return;
//	}

//	if(dx >= ddc.width || dy >= ddc.height)
//	{
//		return;
//	}

//	
//	
//	if(rect.left >= sdc.width || rect.top >= sdc.height)
//	{
//		return ;
//	}
//	
//	if(rect.right >= sdc.width)
//	{
//		rect.right = sdc.width;
//	}
//	if(rect.bottom >= sdc.height)
//	{
//		rect.bottom = sdc.height;
//	}
//	
//	width = rect.right - rect.left;
//	height = rect.bottom - rect.top;
//	
//	width = sdc.width;
//	height = sdc.height;
//	
//	if(width + dx > ddc.width)
//	{
//		width = ddc.width - dx;
//	}
//	
//	if(height + dy > ddc.height)
//	{
//		height = ddc.height - dy;
//	}
//	
//	size = width*disp_dev.bpp;


//	for(i = 0; i < height; i++)
//	{
//		sp1 = &(sdc.p[(rect.left)*disp_dev.bpp + (rect.top+i)*sdc.width*disp_dev.bpp]);
////		sp1 = &(sdc.p[(dx)*disp_dev.bpp + (dy+i)*sdc.width*disp_dev.bpp]);			
//		dp1 = &(ddc.p[(dy + i) * disp_dev.width * disp_dev.bpp + dx * disp_dev.bpp]);
//		memcpy((void *)dp1,(void *)sp1,size);
//	}
//}


void vblt(dc ddc, uint16_t dx, uint16_t dy,t_rect rect, dc sdc )
{
	uint16_t i,size,width,height;
	uint8_t *dp1,*sp1;
	
	if((ddc.p==NULL)||(sdc.p ==NULL))
	{
		return;
	}

	if(dx >= ddc.width || dy >= ddc.height)
	{
		return;
	}

	
	
	if(rect.left >= sdc.width || rect.top >= sdc.height)
	{
		return ;
	}
	
	if(rect.right >= sdc.width)
	{
		rect.right = sdc.width;
	}
	if(rect.bottom >= sdc.height)
	{
		rect.bottom = sdc.height;
	}
	
	width = rect.right - rect.left;
	height = rect.bottom - rect.top;
	
	if(width + dx > ddc.width)
	{
		width = ddc.width - dx;
	}
	
	if(height + dy > ddc.height)
	{
		height = ddc.height - dy;
	}
	
	size = width*disp_dev.bpp;


	for(i = 0; i < height; i++)
	{
		sp1 = &(sdc.p[(rect.left)*disp_dev.bpp + (rect.top+i)*sdc.width*disp_dev.bpp]);
		dp1 = &(ddc.p[(dy + i) * disp_dev.width * disp_dev.bpp + dx * disp_dev.bpp]);
		memcpy((void *)dp1,(void *)sp1,size);
	}
}





void draw_virtual_hline(uint16_t xs,uint16_t ys,uint16_t xe)
{
	uint16_t i;
	
	if(xs > disp_dev.width || xe > disp_dev.width || ys > disp_dev.height)
	{
		return ;
	}

	if(xe > xs)
	{
		for(i=xs;i<xe;i++)
		{
			if((i-xs)%2)
			{
				disp_dev.put_pixel(i,ys,0x0);
			}
			else
			{
				disp_dev.put_pixel(i,ys,0xffff);
			}
		}
	}
	else
	{
		for(i=xs;i>xe;i--)
		{
			if((xs - i)%2)
			{
				disp_dev.put_pixel(i,ys,0x0);
			}
			else
			{
				disp_dev.put_pixel(i,ys,0xffff);
			}
		}
	}	
	
}



/*

void save_virtual_hline(uint16_t xs,uint16_t ys,uint16_t xe,uint16_t * frame)
{
	uint16_t i;
    uint16_t * p;

    
    if(disp_dev.flag == 0)
    {
	    p = (uint16_t *)disp_dev.get_addr(1);
	}
	else
	{
	    p = (uint16_t *)disp_dev.get_addr(0);
	}
	
	
	if(xs > disp_dev.width || xe > disp_dev.width || ys > disp_dev.height)
	{
		return ;
	}

	if(xe > xs)
	{
		for(i=xs;i<xe;i++)
		{
			frame[i-xs] = p[disp_dev.width*ys+i];
		}
	}
	else
	{
		for(i=xs;i>xe;i--)
		{
			frame[xs-i] = p[disp_dev.width*ys+i];
		}
	}	
	
}

*/








void recover_hline(uint16_t xs,uint16_t ys,uint16_t xe,uint16_t * frame)
{
	uint16_t i;
    
	
	if(xs > disp_dev.width || xe > disp_dev.width || ys > disp_dev.height)
	{
		return ;
	}

	if(xe > xs)
	{
		for(i=xs;i<xe;i++)
		{
			disp_dev.put_pixel(i,ys,frame[i-xs]);
		}
	}
	else
	{
		for(i=xs;i>xe;i--)
		{
			disp_dev.put_pixel(i,ys,frame[xs-i]);
		}
	}	
	
}





void draw_virtual_vline(uint16_t xs, uint16_t ys, uint16_t ye)
{
	uint16_t i;
	
	if(xs > disp_dev.width || ys > disp_dev.height || ye > disp_dev.height)
	{
		return ;
	}
	if(ye > ys)
	{
		for(i=ys;i<ye;i++)
		{
			if((i-ys)%2)
			{
				disp_dev.put_pixel(xs,i,0x0);
			}
			else
			{
				disp_dev.put_pixel(xs,i,0xffff);
			}
		}
	}
	else
	{
		for(i=ys;i>ye;i--)
		{
			if((ys - i)%2)
			{
				disp_dev.put_pixel(xs,i,0x0);
			}
			else
			{
				disp_dev.put_pixel(xs,i,0xffff);
			}
		}
	}
}







void save_virtual_vline(uint16_t xs,uint16_t ys,uint16_t ye,uint16_t * frame)
{
	uint16_t i;
    uint16_t * p;

    if(disp_dev.flag == 0)
    {
	    p = (uint16_t *)disp_dev.get_addr(1);
	}
	else
	{
	    p = (uint16_t *)disp_dev.get_addr(2);
	}
	
	
	if(xs > disp_dev.width || ye > disp_dev.height|| ys > disp_dev.height)
	{
		return ;
	}

	if(ye > ys)
	{
		for(i=ys;i<ye;i++)
		{
			frame[i-ys] = p[disp_dev.width*i+xs];
		}
	}
	else
	{
		for(i=ys;i>ye;i--)
		{
			frame[ys-i] = p[disp_dev.width*i+xs];
		}
	}	
	
}









void recover_vline(uint16_t xs,uint16_t ys,uint16_t ye,uint16_t * frame)
{
	uint16_t i;
    
	
	
	if(xs > disp_dev.width || ye > disp_dev.height|| ys > disp_dev.height)
	{
		return ;
	}

	if(ye > ys)
	{
		for(i=ys;i<ye;i++)
		{
			disp_dev.put_pixel(xs,i,frame[i-ys]);
		}
	}
	else
	{
		for(i=ys;i>ye;i--)
		{
			disp_dev.put_pixel(xs,i,frame[ys-i]);
		}
	}	
	
}



uint8_t pt_in_rect( t_rect * p_rc, uint16_t x, uint16_t y )
{
	return ( x >= p_rc->left
		&& x <= p_rc->right
		&& y >= p_rc->top
		&& y <= p_rc->bottom
		) ? 1 : 0;
}

static void get_line_image(uint8_t *p,uint16_t num, uint16_t x,uint16_t y,uint16_t width)
{
	uint32_t block,page,addr;
	uint32_t size1;

	
	if(p == NULL)
	{
		return;
	}

	if( x > disp_dev.width)
	{
		x = disp_dev.width;
	}

	if(y >= disp_dev.height)
	{
		y = disp_dev.height;
	}

	if(x + width > disp_dev.width)
	{
		width = disp_dev.width - x;
	}
	

	//page = (((y * disp_dev.width + x) + num * disp_dev.width * disp_dev.height) * disp_dev.bpp) / nand_dev.page_mainsize;
	page = (y * disp_dev.width + x)*disp_dev.bpp / nand_dev.page_mainsize;
	page += (num * disp_dev.width * disp_dev.height*disp_dev.bpp/ nand_dev.page_mainsize);

	if((disp_dev.width * disp_dev.height * disp_dev.bpp) % nand_dev.page_mainsize)
	{
		page += num;
	}

	block = page / nand_dev.block_pagenum;
	block += src_cfg.block_bmp_addr;
	page = page % nand_dev.block_pagenum;
	addr = ((y * disp_dev.width + x)* disp_dev.bpp) % nand_dev.page_mainsize;

	size1 = width * disp_dev.bpp;
	
	
	nand_dev.read(block,page,addr,p,size1);
	
	
}

void get_touch_image(dc vdc, base_touch * touch, uint8_t flag)
{
	uint16_t num,i;
	uint16_t x,y,width,height;

	if(flag != 0 && flag != 1)
	{
		return;	
	}

	if(touch->pic_on < 0)
	{
		return;
	}

	
	if(flag == 0)
	{
		num = touch->pic_on;
	}
	else if(flag == 1)
	{
		num = touch->pic_id;
	}

	//x = touch->tp_rect.left;
	//y = touch->tp_rect.top;

	x = touch->tp_rect.left;
	y = touch->tp_rect.top;
	
	width = vdc.width;
	height = vdc.height;
	
//	width = touch->tp_rect.right - touch->tp_rect.left + 1;
//	height = touch->tp_rect.bottom - touch->tp_rect.top + 1;

	for(i = 0;i < height; i++)
	{
		
		get_line_image(&vdc.p[i*width*disp_dev.bpp],num, x, y + i, width);
	}
	
	
}

void draw_vline(uint16_t * dp,uint16_t x,uint16_t y,uint16_t len,uint16_t color)
{
    uint16_t i;
    
    if(x>=disp_dev.width)
    {
        return;
    }
    if(y>=disp_dev.height)
    {
        return;
    }
    if(y+len>=disp_dev.height)
    {
        len = disp_dev.height-y;
    }
    
    for(i=0;i<len;i++)
    {
        dp[x+(y+i)*disp_dev.width] = color;
    }
}


void draw_hline(uint16_t *dp,uint16_t x,uint16_t y,uint16_t len, uint16_t color)
{
    uint16_t i;

    if(x>=disp_dev.width)
    {
        return;
    }
    if(y>=disp_dev.height)
    {
        return;
    }
    if(x+len>disp_dev.width)
    {
        len = disp_dev.width - x;
    }

    for(i=0;i<len;i++)
    {
        dp[x+i+y*disp_dev.width]=color;
    }
}



//void draw_image_vline(uint16_t * dp,uint16_t x,uint16_t y,uint16_t len,dc sdc)
//{
//    uint16_t i;
//    
//    if(sdc.p==NULL)
//    {
//        return ;
//    }
//    if(x>=sdc.x+sdc.width||x<sdc.x)
//    {
//        return;
//    }
//    if(y>sdc.y+sdc.height)
//    {
//        return;
//    }
//    if(y<sdc.y)
//    {
//        y=sdc.y;
//    }
//    if(y+len>=sdc.y+sdc.height)
//    {
//        len=sdc.height+sdc.y-y;
//    }
//    for(i=0;i<len;i++)
//    {
//        dp[x+(y+i)*disp_dev.width] = sdc.p[(x-sdc.x)*(y-sdc.y+i)*sdc.width];
//    }
//}


//void draw_image_hline(uint16_t * dp,uint16_t x,uint16_t y,uint16_t len,dc sdc)
//{
//    uint16_t i;
//    
//    if(sdc.p==NULL)
//    {
//        return;
//    }
//    if(y<sdc.y||y>sdc.y+sdc.width)
//    {
//        return;
//    }
//    if(x>sdc.x+sdc.width)
//    {
//        return;
//    }
//    if(x<sdc.x)
//    {
//        x=sdc.x;
//    }
//    if(x+len>=sdc.x+sdc.width)
//    {
//        len = sdc.height + sdc.y - y;
//    }
//    for(i=0;i<len;i++)
//    {
//        dp[x+i+y*disp_dev.width]=sdc.p[x-sdc.x+i+(y-sdc.y)*sdc.width];
//    }
//}


//static gui_layer * create_layer(void)
//{
//    gui_layer * player = (gui_layer *)my_malloc(sizeof(gui_layer));

//    memset((void *)player,0,sizeof(gui_layer));

//    player->layer.height = disp_dev.height;
//    player->layer.width = disp_dev.width;
//    player->layer.x = 0;
//    player->layer.y = 0;
//    player->layer.p  = (uint8_t *)my_malloc(disp_dev.bpp*disp_dev.height*disp_dev.width);

//    return player;
//   
//}


uint8_t  rect_overlap(t_rect rect0,t_rect rect1,t_rect * prect)
{
	
    if(rect0.left > rect1.right || rect0.right < rect1.left)
    {
        return 0 ;
    }
    if(rect0.top > rect1.bottom || rect0.bottom < rect1.top)
    {
        return 0;
    }
	
	//�õ��ص�����
	if(prect)
	{
		if(rect0.left < rect1.left)
		{
			prect->left = rect1.left;
		}
		else
		{
			prect->left = rect0.left;
		}
	
		if(rect0.right > rect1.right)
		{
			prect->right = rect1.right;
		}
		else
		{
			prect->right = rect0.right;
		}
	
		if(rect0.top < rect1.top)
		{
			prect->top = rect1.top;
		}
		else
		{
			prect->top = rect0.top;
		}
		if(rect0.bottom > rect1.bottom)
		{
			prect->bottom = rect1.bottom;
		}
		else
		{
			prect->bottom = rect0.bottom;
		}
	}

    return 1;
}



//static uint8_t rect_insert_layer(gui_layer * player, t_rect rect)
//{
//    uint8_t res;
//    layer_rect * p;

//    res = 0;
//    
//    if(player->head_rect)
//    {
//        res = rect_overlap(player->head_rect->rect, rect);
//        if(res)
//        {
//            return res;
//        }

//        p = player->head_rect;

//        while(p->next)
//        {
//            p = p->next;
//            res = rect_overlap(p->rect,rect);
//            if(res)
//            {
//                break;
//            }
//            
//        }
//        if(res == 0)
//        {
//            p->next = (layer_rect *) my_malloc(sizeof(layer_rect));
//            p->next->next = NULL;
//            p->next->rect = rect;
//        }

//        return res;
//        
//    }
//    else
//    {
//        
//        
//    }
//	
//	return 0;
//}

//static uint8_t report_gui_layer(t_rect rect)
//{

//    if(bottom_layer == NULL)
//    {
//        bottom_layer = create_layer();
//    }

//    return 0;
//    
//}


void get_image(dc image,uint16_t x,uint16_t y,uint16_t num)
{

    uint16_t width,height,i;

    if(image.p == NULL)
    {
        return;
    }

    

    width = image.width;
    height = image.height;

    for(i=0;i<height;i++)
    {
        get_line_image(&image.p[i*width*disp_dev.bpp], num, x, y+i, width);
    }
    
    
}


static void swap_uint16_t(uint16_t * a, uint16_t * b)
{
    uint16_t t = *a;
    *a = *b;
    *b = t;
}



void write_line(uint16_t x1,uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    uint16_t steep = abs(y2 - y1) > abs(x2 - x1);
    int16_t dx,dy,err,ystep;

    if (steep)
    {
        swap_uint16_t(&x1,&y1);
        swap_uint16_t(&x2,&y2);
    }
    if(x1 > x2)
    {
        swap_uint16_t(&x1,&x2);
        swap_uint16_t(&y1,&y2);
    }

    dx = x2 - x1;
    dy = abs(y2 - y1);
    err = dx / 2;

    if(y1 < y2)
    {
        ystep = 1;
    }
    else
    {
        ystep = -1;
    }

    for (; x1<=x2; x1++)
    {
        if(steep)
        {
            disp_dev.put_pixel(y1,x1,color);
        }
        else
        {
            disp_dev.put_pixel(x1,y1,color);
        }

        err -= dy;

        if(err < 0)
        {
            y1 += ystep;
            err += dx;
        }
        
    }
}


void draw_Vline(uint16_t x,uint16_t y, uint16_t h, uint16_t color)
{
   write_line(x, y, x, y+h-1, color); 
}

void draw_Hline(uint16_t x,uint16_t y, uint16_t w, uint16_t color)
{
    write_line(x,y,x+w-1,y,color);
}



void draw_line(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color)
{
    if(x1 == x2)
    {
        if(y1 > y2)
        {
            swap_uint16_t(&y1,&y2);
        }
        draw_Vline(x1,y1,y2-y1+1,color);
    }
    else if(y1 == y2)
    {
        if(x1 > x2)
        {
            swap_uint16_t(&x1,&x2);
        }
        draw_Hline(x1,y1,x2-x1+1,color);
    }
    else
    {
        write_line(x1,y1,x2,y2,color);
    }
}



void draw_circle(uint16_t x, uint16_t y, uint16_t r,uint16_t color)
{
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t a = 0;
    int16_t b = r;

    disp_dev.put_pixel(x,y+r,color);
    disp_dev.put_pixel(x,y-r,color);
    disp_dev.put_pixel(x+r,y,color);
    disp_dev.put_pixel(x-r,y,color);

    while(a<b)
    {
        if(f>=0)
        {
            b--;
            ddF_y += 2;
            f += ddF_y;
        }
        a++;
        ddF_x += 2;
        f += ddF_x;
        
        disp_dev.put_pixel(x+a,y+b,color);
        disp_dev.put_pixel(x-a,y+b,color);
        disp_dev.put_pixel(x+a,y-b,color);
        disp_dev.put_pixel(x-a,y-b,color);
        disp_dev.put_pixel(x+b,y+a,color);
        disp_dev.put_pixel(x-b,y+a,color);
        disp_dev.put_pixel(x+b,y-a,color);
        disp_dev.put_pixel(x-b,y-a,color);
        
    }


}



void draw_circle_helper(uint16_t x,uint16_t y, uint16_t r,uint8_t corner,uint16_t color)
{
    
    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t a     = 0;
    int16_t b     = r;

    while(a<b)
    {
        if (f >= 0)
        {
            b--;
            ddF_y += 2;
            f     += ddF_y;
        }

        a++;
        ddF_x += 2;
        f += ddF_x;

        if (corner & 0x4)
        {
            disp_dev.put_pixel(x+a,y+b,color);
            disp_dev.put_pixel(x+b,y+a,color);
        }
        if(corner & 0x2)
        {
           disp_dev.put_pixel(x+a,y-b,color);
           disp_dev.put_pixel(x+b,y-a,color);
        }
        if(corner & 0x8)
        {
            disp_dev.put_pixel(x-b,y+a,color);
            disp_dev.put_pixel(x-a,y+b,color);
        }
        if(corner & 0x1)
        {
            disp_dev.put_pixel(x-b,y-a,color);
            disp_dev.put_pixel(x-a,y-b,color);
        }
    }
}




void fill_circle_helper(uint16_t x,uint16_t y, uint16_t r,uint8_t corner,int16_t delta,uint16_t color)
{
    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t a     = 0;
    int16_t b     = r;

    while(a<b)
    {
        if (f >= 0)
        {
            b--;
            ddF_y += 2;
            f     += ddF_y;
        }

        a++;
        ddF_x += 2;
        f += ddF_x;

        if (corner & 0x1)
        {
            draw_Vline(x+a,y-b,2*b+1+delta, color);
            draw_Vline(x+b,y-a,2*a+1+delta, color); 
        }
        if(corner & 0x2)
        {
            draw_Vline(x-a,y-b,2*b+1+delta, color);
            draw_Vline(x-b,y-a,2*a+1+delta, color); 
        }
     }

     
    
}


void fill_circle(uint16_t x,uint16_t y, uint16_t r,uint16_t color)
{
    draw_Vline(x, y-r, 2*r+1,color);
    fill_circle_helper(x,y, r,3,0,color);
}



void draw_rect(uint16_t x,uint16_t y,uint16_t w,uint16_t h,uint16_t color)
{
    draw_Hline(x, y, w, color);
    draw_Hline(x, y+h-1, w,color);
    draw_Vline(x, y, h, color);
    draw_Vline(x+w-1,y,h, color);
}




void fill_rect(uint16_t x,uint16_t y,uint16_t w,uint16_t h,uint16_t color)
{
    uint16_t i;

    for(i=x;i<x+w;i++)
    {
        draw_Vline(i,y,h,color);
    }
}




void draw_triangle(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color)
{
    draw_line(x0,y0, x1,y1,color);
    draw_line(x1,y1, x2,y2,color);
    draw_line(x2,y2, x0,y0,color);
    
}




void fill_triangle(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color)
{
    int16_t a, b, y, last;

    if (y0 > y1)
    {
        swap_uint16_t(&y0, &y1);
        swap_uint16_t(&x0, &x1);
    }
    if(y1 > y2)
    {
        swap_uint16_t(&y1, &y2);
        swap_uint16_t(&x1, &x2);
    }
    if(y0 > y1)
    {
        swap_uint16_t(&y0, &y1);
        swap_uint16_t(&x0, &x1);
    }

    if(y0 == y2)
    {
        a = b = x0;
        if(x1 < a)
        {
            a = x1;
        }
        else if(x1 > b)
        {
            b = x1;
        }
        if(x2 < a)
        {
            a = x2;
        }
        else if(x2 > b)
        {
            b = x2;
        }

        draw_Hline(a, y0, b-a+1, color);

        return;
    }

    int16_t dx01,dy01,dx02,dy02,dx12,dy12;

    dx01 = x1 - x0,
    dy01 = y1 - y0,
    dx02 = x2 - x0,
    dy02 = y2 - y0,
    dx12 = x2 - x1,
    dy12 = y2 - y1;

    int32_t sa,sb;

    sa   = 0,
    sb   = 0;

    if(y1 == y2)
    {
        last = y1;
    }
    else
    {
        last = y1 - 1;
    }

    for(y=y0; y<=last; y++)
    {
        a   = x0 + sa / dy01;
        b   = x0 + sb / dy02;
        sa += dx01;
        sb += dx02;

        if(a > b)
        {
            swap_uint16_t((uint16_t *)&a, (uint16_t *)&b);
        }

        draw_Hline(a, y, b-a+1, color);
    }

    sa = dx12 * (y - y1);
    sb = dx02 * (y - y0);

    for(; y<=y2; y++)
    {
        a   = x1 + sa / dy12;
        b   = x0 + sb / dy02;
        sa += dx12;
        sb += dx02;

        if(a > b)
        {
            swap_uint16_t((uint16_t *)&a, (uint16_t *)&b);
        }

        draw_Hline(a, y, b-a+1, color);
    }
    
    
}


uint16_t get_complementary_color(uint16_t color)
{
	uint8_t r,g,b;
	uint16_t col;
	
	r = 0x1f - ((color >> 10)&0x1f);
	g = 0x1f - ((color >> 5)&0x1f);
	b = 0x1f - ((color)&0x1f);
	
	col = (r<<10)|(g<<5)|(b);
	
	return col;
	
	
}


uint16_t mix_color(uint16_t color1,uint16_t color2,uint8_t percent)
{

    if(percent > 100)
    {
        return color1;
    }
    if(percent == 0)
    {
        return color2;
    }

    uint8_t r,g,b;

    r = (((color1 >> 10) & 0x1F) * percent + ((color2 >> 10) & 0x1F) * (100 - percent)) / 100;
    g = (((color1 >> 5) & 0x1F) * percent + ((color2 >> 5) & 0x1F) * (100 - percent)) / 100;
    b = ((color1  & 0x1F) * percent + (color2 & 0x1F) * (100 - percent)) / 100;

    return ((r << 10) | (g << 5) | (b));
    

}


void set_pexil_width(uint16_t x,uint16_t y,uint16_t color,uint8_t width,uint8_t flag)
{
	if(flag == 0)
	{
		switch(width)
		{
			case 4:
				disp_dev.put_pixel(x,y-2,color);
			case 3:
				disp_dev.put_pixel(x,y+1,color);
			case 2:
				disp_dev.put_pixel(x,y-1,color);
			case 1:
				disp_dev.put_pixel(x,y,color);		
		}
			
	}
	else
	{
		switch(width)
		{
			case 4:
				disp_dev.put_pixel(x-2,y,color);
			case 3:
				disp_dev.put_pixel(x+1,y,color);
			case 2:
				disp_dev.put_pixel(x-1,y,color);
			case 1:
				disp_dev.put_pixel(x,y,color);		
		}
	}
}


void write_width_line(uint16_t x1,uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color,uint8_t width)
{
    uint16_t steep = abs(y2 - y1) > abs(x2 - x1);
    int16_t dx,dy,err,ystep;
	uint8_t flag = 0;

    if (steep)
    {
        swap_uint16_t(&x1,&y1);
        swap_uint16_t(&x2,&y2);
		flag = 1;
    }
    if(x1 > x2)
    {
        swap_uint16_t(&x1,&x2);
        swap_uint16_t(&y1,&y2);
    }

    dx = x2 - x1;
    dy = abs(y2 - y1);
    err = dx / 2;

    if(y1 < y2)
    {
        ystep = 1;
    }
    else
    {
        ystep = -1;
    }

    for (; x1<=x2; x1++)
    {
        if(steep)
        {
            set_pexil_width(y1,x1,color,width,flag);
        }
        else
        {
            set_pexil_width(x1,y1,color,width,flag);
        }

        err -= dy;

        if(err < 0)
        {
            y1 += ystep;
            err += dx;
        }
        
    }
}


void draw_width_Vline(uint16_t x,uint16_t y, uint16_t h, uint16_t color,uint8_t width)
{
   write_width_line(x, y, x, y+h-1, color,width); 
}

void draw_width_Hline(uint16_t x,uint16_t y, uint16_t w, uint16_t color,uint8_t width)
{
    write_width_line(x,y,x+w-1,y,color,width);
}


void draw_width_line(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color,uint8_t width)
{
    if(x1 == x2)
    {
        if(y1 > y2)
        {
            swap_uint16_t(&y1,&y2);
        }
        draw_width_Vline(x1,y1,y2-y1+1,color,width);
    }
    else if(y1 == y2)
    {
        if(x1 > x2)
        {
            swap_uint16_t(&x1,&x2);
        }
        draw_width_Hline(x1,y1,x2-x1+1,color,width);
    }
    else
    {
        write_width_line(x1,y1,x2,y2,color,width);
    }
}










