

/***************************************************************************************
****************************************************************************************
* Project      : 20180525_HMI
* FILE         : menu.c
* Description  : 
*                      
* Copyright (c) 2018 by ShenZhen JingHua Displays Co.,Ltd... All Rights Reserved.
* 
* History:
* Version		name			Date			Description
* 0.01			Qu Bingbing		2018/06/04		Initial Version
* 
****************************************************************************************
****************************************************************************************/
#include "menu.h"
#include "mem_my.h"
#include "cmd.h"
#include "display.h"
#include "string.h"
#include "cfg.h"
#include "str.h"


typedef struct 
{
    uint8_t len;
    uint8_t *p;
}menu_num_language;


//typedef struct 
//{
//    uint16_t x;
//    uint16_t y;
//}position;

typedef struct
{
	uint16_t x;
	uint16_t y;
}position;




static uint8_t menu_msg(void *p, t_msg * p_msg)
{
	uint16_t x,y,flag,f=1;
	t_msg msg;
	
	
	if(p_msg->msg != MSG_TP)
	{
		return f;
	}
	
	menu * pmenu = (menu * )p;

	
	
	if(pmenu->ctrl.active == 0)
	{
        return f;
	}


	x = p_msg->a;
	y = p_msg->b;
	flag = p_msg->c;
	
	
	

	if((flag != 1) && !GETBIT( pmenu->state, BTN_STATE_ACTIVE))
	{
		return f;
	}
	
	

	if(pt_in_rect(&pmenu->tp_rect, x, y))
	{
		f = 0;
	}

	switch(flag)
	{
		case 1:
		{
			if(!f || pmenu->style == 0)
			{
			    if(!f)
			    {
			        if(pmenu->dir == 0)
			        {
			            pmenu->num = ((float)(y-pmenu->tp_rect.top)) / ((float)(pmenu->tp_rect.bottom - pmenu->tp_rect.top) / pmenu->menu_num);
			        }
			        else
			        {
			            pmenu->num = ((float)(x-pmenu->tp_rect.left)) / ((pmenu->tp_rect.right - pmenu->tp_rect.left) / pmenu->menu_num);
			        }
					
					if(pmenu->num >= pmenu->menu_num)
					{
						pmenu->num = pmenu->menu_num - 1;
					}
			    }
				SETBIT( pmenu->state, BTN_STATE_ACTIVE);
				msg.msg = MSG_TOUCH;
				msg.a   = (pmenu->ctrl.id >> 16) & (0xffff);
				msg.b   = (pmenu->ctrl.id) & (0xffff);
				msg.c   = 1;//����
				msg_dev.write_message((void *)&msg,0);
				f = 0;
					
			}

			break;
		}
		case 2:
		{
			
			break;
		}
		case 0:
		{
			
			//if(f || (p_btn->pic_next == -1))
			msg.msg = MSG_TOUCH;
			msg.a   = (pmenu->ctrl.id >> 16) & (0xffff);
			msg.b   = (pmenu->ctrl.id) & (0xffff);
			msg.c   = 0;//̧��
			msg_dev.write_message((void *)&msg,0);
				
		
			//if(!f)//��̧��ʱ���ڴ��ط�Χ��
			//{
			/*
				switch(p_btn->use)
				{

					case switch_pic:
						msg.msg = MSG_PIC;
						msg.a =* ((uint16_t *)p_btn->tp_use);
						msg_dev.write_message((void *)&msg,0);
						break;
					case custom_key:
						break;
					default:
						break;
					
				}

				*/
				
			//}
			
			CLRBIT( pmenu->state, BTN_STATE_ACTIVE);
			
			break;
		}
	}

	return f;
	
}


static void menu_release( void* pCtrl )
{
    menu * pmenu = (menu *) pCtrl;

    if(pmenu->tp_type != CFG_MEN)
    {
        return;
    }


	
	if(pmenu->up_icon.p)
	{
		my_free(pmenu->up_icon.p);
	}
	if(pmenu->down_icon.p)
	{
		my_free(pmenu->down_icon.p);
	}
    if(pmenu->pstr_id)
    {
        my_free(pmenu->pstr_id);
    }

    my_free(pmenu);
}

/*

static void get_menu_num_language(menu * pmenu,uint8_t num,menu_num_language * pinfo)
{
    uint8_t * p;
    uint8_t i,len,index;
    uint8_t language_num = *(pmenu->planguage_num);
    p = pmenu->planguage[language_num].code;
    len = pmenu->planguage[language_num].len;
    index = 0;
    pinfo->p = NULL;
    pinfo->len = 0;
	font_type * pfont = pmenu->pfont;
    if(num == 0)
    {
        pinfo->p = p;
    }
    for(i=0;i<len;i++)
    {
        if(p[i] != ';')
        {
            if(num == index)
            {
                pinfo->len += 1;
            }
        }
        else
        {
            index++;
            if(index > num)
            {
				if(pfont->code == 3)
				{
					pinfo->len -= 1;
				}
                break;
            }
            if(index == num)
            {
                pinfo->p = &p[i+1];
            }
        }
    }
    
    
}
*/

static  void copy_to_textbox(menu * pmenu)
{
    uint8_t * p;
	uint32_t input_id;
    uint8_t num = pmenu->num;
	
	
	
	
	input_id = ((pmenu->ctrl.id) & (0xFFFF0000)) | ((pmenu->text_id) & (0x0000FFFF));
	
	
	menu * pm = (menu * )get_ctrl(input_id);

    if(pm->tp_type == CFG_TEX)
	{
		menu_num_language pinfo;
		
		pinfo.p = get_str_code(pmenu->pstr_id[num],&(pinfo.len));
		p = (uint8_t *) my_malloc(pinfo.len + 11);

		p[0] = 0xEE;
		p[1] = 0xB1;
		p[2] = 0x10;
		p[3] = (input_id >> 24);
		p[4] = (input_id >> 16);
		p[5] = (input_id >> 8);
		p[6] = (input_id & 0xFF);

		memcpy(&p[7],pinfo.p,pinfo.len);

		p[pinfo.len + 7] = 0xFF;
		p[pinfo.len + 8] = 0xFC;
		p[pinfo.len + 9] = 0xFF;
		p[pinfo.len + 10] = 0xFF;

		cmd_dev.usart_cmd(p,pinfo.len + 11);

		my_free(p);
	}
	else if(pm->tp_type == CFG_STR)
	{
		p = (uint8_t *) my_malloc(16);
		
		p[0] = 0xEE;
		
		p[1] = 0xB1;
		p[2] = 0x37;
		
		p[3] = (input_id >> 24);
		p[4] = (input_id >> 16);
		p[5] = (input_id >> 8);
		p[6] = (input_id & 0xFF);
		
		p[7] = pmenu->pstr_id[num] >> 8;
		p[8] = pmenu->pstr_id[num] & 0xFF;
		
		p[9] = 0xFF;
		p[10] = 0xFC;
		p[11] = 0xFF;
		p[12] = 0xFF;
		
		
		cmd_dev.usart_cmd(p,13);
		
		my_free(p);
		
		
		
		
		
	}
	

    
    

    
}

static void hide_menu(menu * pmenu)
{
    uint8_t * p;

    p = my_malloc(14);

    p[0] = 0xEE;
    p[1] = 0xB1;
    p[2] = 0x13;

    p[3] = pmenu->ctrl.id >> 24;
    p[4] = pmenu->ctrl.id >> 16;
    p[5] = pmenu->ctrl.id >> 8;
    p[6] = (pmenu->ctrl.id & 0xFF);
    p[7] = 0x00;
    p[8] = pmenu->text_id >> 8;
    p[9] = (pmenu->text_id & 0xFF);

    p[10] = 0xFF;
    p[11] = 0xFC;
    p[12] = 0xFF;
    p[13] = 0xFF;

    cmd_dev.usart_cmd(p,14);
    my_free(p);
}


static void upload_menu_num(menu * pmenu, uint8_t flag)
{
    uint8_t *p;
    p = my_malloc(14);

    p[0] = 0xEE;
    p[1] = 0xB1;
    p[2] = 0x14;

    p[3] = pmenu->ctrl.id >> 24;
    p[4] = pmenu->ctrl.id >> 16;
    p[5] = pmenu->ctrl.id >> 8;
    p[6] = (pmenu->ctrl.id & 0xFF);
    p[7] = pmenu->num;
    p[8] = flag;
    p[9] = 0xFF;
    p[10] = 0xFC;
    p[11] = 0xFF;
    p[12] = 0xFF;
	p[13] = ';';
    cmd_dev.notify_cmd(p,14);
    my_free(p);
}

static void menu_act(base_touch *p,uint8_t flag)
{

    if(p->type == CFG_MEN)
    {
        menu * pmenu = (menu *) p;
        
        if(flag == 1)
        {
            if(pmenu->num != 0xff)
            {
                upload_menu_num(pmenu,1);
               pmenu->screen_update = 1; 
            }
       }
       else if(flag == 0)
       {
            if(pmenu->style == 0)
            {
				if(pmenu->num != 0xFF)
				{
					upload_menu_num(pmenu,0);
					copy_to_textbox(pmenu);
				}
                hide_menu(pmenu);
            }
            else
            {
                pmenu->num = 0xff;
                pmenu->screen_update = 1;
            }
       }
    }
    
}

static void show_menu_string(position pos,font_type font, menu_num_language * pinfo,uint16_t color)
{
    if(pinfo->len == 0 || pinfo->p == NULL)
    {
        return;
    }
    uint8_t i,len = pinfo->len;
    uint8_t * code = pinfo->p;
    uint16_t hz,w,x,y;

    font_type * ch_font;
    font_type * hz_font;

    hz_font = ch_to_hz(&font);
    ch_font = hz_to_ch(&font);
    
	if(font.code == 0)
	{
		w = font.width;
	}
	else
	{
		w = font.width / 2;
	}
	
	x = pos.x;
	y = pos.y;

    for(i=0;i<len;i++)
    {
        if(font.code < 3)
        {
            if(code[i] < 0x80)
            {
    			if(ch_font)
    			{
    				show_charn_font(x, y, color,code[  i], *ch_font);
    			}
                x += w;
            }
            else
            {
                hz = (code[i] << 8) | (code[i+1]);
    			if(hz_font)
    			{
    				show_nfont(x,y,color,hz,*hz_font);
    			}
                i++;
                x += 2*w;
            }
        }
        else
        {
            hz = (code[i] << 8) | (code[i+1]);

            if(hz < 0x80)
            {
                if(ch_font)
    			{
    				show_charn_font(x, y, color,code[  i+1], *ch_font);
    			}
                x += w;
            }
            else
            {
    			if(hz_font)
    			{
    				show_nfont(x,y,color,hz,*hz_font);
    			}
                x += 2*w; 
            }

            i++;
            
        }
    }
}

static void menu_show_font(menu * pmenu)
{
    uint8_t i;
    menu_num_language pinfo;
    uint16_t color;
    position pos;
    uint16_t w,h,width,temp,x,y;

    font_type * pfont;
	
	if(pmenu->pfont == NULL)
	{
		return;
	}
	
	pfont = pmenu->pfont;

    if(pfont->code == 0)
	{
		w = pfont->width;
	}
	else
	{
		w = pfont->width / 2;
	}
    
    h = get_font_height(*pfont);

    if(pmenu->dir == 0)
    {
        temp = (((float)(pmenu->tp_rect.bottom - pmenu->tp_rect.top)) / pmenu->menu_num);
    }
    else
    {
        
        temp = (((float)(pmenu->tp_rect.right - pmenu->tp_rect.left)) / pmenu->menu_num);
    }

    x = pmenu->tp_rect.left;
    y = pmenu->tp_rect.top;

    for(i=0;i<pmenu->menu_num;i++)
    {
        pinfo.p = get_str_code(pmenu->pstr_id[i],&(pinfo.len));

        if(i != pmenu->num)
        {
            color = pmenu->fore_color;
        }
        else
        {
            color = pmenu->back_color;
        }

        

        if(pfont->code < 3)
        {
           width = pinfo.len * w;
        }
        else //为unicode编码时不能根据编码长度来得出显示区域的宽度
        {
            uint8_t * code,j;
            width = 0;
            code = pinfo.p;

            for(j=0;j<pinfo.len;j++)
            {
                if(code[j]==0 && code[j+1] < 0x80)
                {
                    width += w; 
                }
                else
                {
                    width += 2*w;
                }

                j++;
            }
        }
           
        if(pmenu->dir == 0)
        {
			y = (((float)((pmenu->tp_rect.bottom - pmenu->tp_rect.top) * i)) / pmenu->menu_num);
			temp  = (((float)((pmenu->tp_rect.bottom - pmenu->tp_rect.top) * (i + 1))) / pmenu->menu_num);
			temp -= y;
			y += pmenu->tp_rect.top;
            if(width<=(pmenu->tp_rect.right-pmenu->tp_rect.left + 1) && (h<=temp))
            {
				
                pos.x = pmenu->tp_rect.left + ((pmenu->tp_rect.right-pmenu->tp_rect.left + 1) - width) / 2;
                pos.y = y + (temp - h)/2;
            }
            else   //越界不显示
            {
                continue;
            }
        }
        else
        {
			x = (((float)((pmenu->tp_rect.right - pmenu->tp_rect.left + 1) * i)) / pmenu->menu_num);
			temp = (((float)((pmenu->tp_rect.right - pmenu->tp_rect.left + 1) * (i + 1))) / pmenu->menu_num);
			temp -= x;
			x += pmenu->tp_rect.left;
            if(h<=(pmenu->tp_rect.bottom-pmenu->tp_rect.top + 1) && (width<=temp))
            {
                pos.x = x + (temp - width) / 2;
                pos.y = pmenu->tp_rect.top +  ((pmenu->tp_rect.bottom-pmenu->tp_rect.top + 1) - h) / 2;
            }
            else //越界不显示
            {
                continue;
            }

        }

        show_menu_string(pos,*pfont,&pinfo,color);
        

        
        
    }
}

static void menu_draw_screen(base_touch * p)
{
    if(p->type != CFG_MEN)
    {
        return ;

    }
    
    uint16_t fore_color,back_color;
    uint16_t x,y,w,h;
	uint8_t i;
	icon_image image;
	uint32_t size;
	
    menu * pmenu = (menu *) p;
    uint8_t * buf;

    fore_color = pmenu->fore_color;
    back_color = pmenu->back_color;
    x = pmenu->tp_rect.left;
    y = pmenu->tp_rect.top;
    
    if(disp_dev.dir == 0)
    {
    
        if(disp_dev.flag)
        {
            buf = disp_dev.disp[0].p;
            
        }
        else
        {
            buf = disp_dev.disp[1].p;
            
        }
     }
     else
     {
        buf = disp_dev.disp[3].p;
     }
     
    
    if(pmenu->facade == 0)
    {
        h = pmenu->tp_rect.bottom - y + 1;
        w = pmenu->tp_rect.right - x + 1;
        fill_rect(x,y,w,h,back_color);
		if(pmenu->dir == 0)
		{
			uint16_t y1 = pmenu->tp_rect.top;
			uint16_t x1 = pmenu->tp_rect.left;
			for(i=1;i<pmenu->menu_num;i++)
			{
				y1 = pmenu->tp_rect.top + (((float)((pmenu->tp_rect.bottom - pmenu->tp_rect.top) * i)) / pmenu->menu_num);
				draw_Hline(x1,y1,w,fore_color);
				
			}
		}
		else
		{	
			uint16_t y1 = pmenu->tp_rect.top;
			uint16_t x1 = pmenu->tp_rect.left;
			for(i=1;i<pmenu->menu_num;i++)
			{
				x1 += pmenu->tp_rect.left + (((float)((pmenu->tp_rect.right - pmenu->tp_rect.left) * i)) / pmenu->menu_num);
				draw_Vline(x1,y1,h,fore_color);
			}
			
		}
        
    }
    else
    {
		
        get_icon_image(&(pmenu->up_icon),0);
        show_icon_image_mix(pmenu->up_icon, pmenu->tp_rect,buf);
		
    }

    if(pmenu->num != 0xFF)
    {
		uint16_t temp; 
        if(pmenu->dir== 0)
        {
            y = pmenu->tp_rect.top + (((float)((pmenu->tp_rect.bottom - pmenu->tp_rect.top) * pmenu->num)) / pmenu->menu_num);
			temp = pmenu->tp_rect.top + (((float)((pmenu->tp_rect.bottom - pmenu->tp_rect.top) * (pmenu->num + 1))) / pmenu->menu_num);
			h = temp - y;
			w = pmenu->tp_rect.right - pmenu->tp_rect.left + 1;
			x = pmenu->tp_rect.left;
			if(pmenu->num >= (pmenu->menu_num - 1))
			{
				h = pmenu->tp_rect.bottom - y + 1;
			}
        }
        else
        {
            x = pmenu->tp_rect.left + (((float)((pmenu->tp_rect.right - pmenu->tp_rect.left) * pmenu->num)) / pmenu->menu_num);
			temp = pmenu->tp_rect.left + (((float)((pmenu->tp_rect.right - pmenu->tp_rect.left) * (pmenu->num + 1))) / pmenu->menu_num);
            //w = (((float)(pmenu->tp_rect.right - pmenu->tp_rect.left)) / pmenu->menu_num);
			w = temp - x;
//			x = pmenu->tp_rect.left + pmenu->num * w;
			h = pmenu->tp_rect.bottom - pmenu->tp_rect.top;
			y = pmenu->tp_rect.top;
			if(pmenu->num >= (pmenu->menu_num - 1))
			{
				w = pmenu->tp_rect.right - x + 1;
			}
        }

        if(pmenu->facade == 0)
        {
            fill_rect(x, y, w, h, fore_color);
        }
        else
        {
            t_rect rect;
            rect.left = x - pmenu->tp_rect.left;
            rect.right = rect.left + w - 1;
            rect.top = y - pmenu->tp_rect.top;
            rect.bottom = rect.top + h - 1;
			get_icon_image(&(pmenu->down_icon),0);
			image.height = pmenu->tp_rect.bottom - pmenu->tp_rect.top + 1;
			image.width = pmenu->tp_rect.right - pmenu->tp_rect.left + 1;
			size = image.width * image.height * pmenu->down_icon.byte;
			image.p = (uint8_t *) my_malloc(size);
			show_icon_image(pmenu->down_icon, image);
            show_vicon_image(image,x,y,rect,buf);
			my_free(image.p);
        }
    }
	
	if(pmenu->facade == 2 || pmenu->facade == 0)
	{
		menu_show_font(pmenu);
	}

    

    
}




uint8_t menu_create(uint8_t * buffer)
{
    menu * pmenu = (menu *) my_malloc(sizeof(menu));
    uint16_t index = 0;
    uint32_t icon_addr;
    uint8_t i;
//    icon menu_icon;
    
    if(pmenu == NULL)
    {
        return 1;
    }
    memset((void *)pmenu,0,sizeof(menu));

    pmenu->ctrl.id = ((buffer[0]<<24)|(buffer[1]<<16)|(buffer[4]<<8)|(buffer[5])<<0);
    pmenu->ctrl.func_ctrl_procmsg = menu_msg;
    pmenu->ctrl.func_ctrl_release = menu_release;
   

    pmenu->tp_type = buffer[6];
    pmenu->tp_rect.left = (buffer[7]<<8) | (buffer[8]);
    pmenu->tp_rect.top = (buffer[9]<<8) | (buffer[10]);
    pmenu->tp_rect.right = (buffer[11]<<8) | (buffer[12]);
    pmenu->tp_rect.bottom = (buffer[13]<<8) | (buffer[14]);
    pmenu->pic_id = (buffer[2]<<8) | (buffer[3]);
    pmenu->pic_on = (buffer[15]<<8) | (buffer[16]);
    pmenu->act = menu_act;
    pmenu->draw_screen = menu_draw_screen;
	pmenu->facade = buffer[17];
	index = 18;
	if(pmenu->facade == 0 || pmenu->facade == 2)
	{
	    pmenu->pfont = get_font_type(buffer[index]);
	    pmenu->back_color = (buffer[index+1]<<8)|(buffer[index+2]);
	    pmenu->fore_color = (buffer[index+3]<<8)|(buffer[index+4]);
	    index += 5;
	}
	
//	pmenu->fore_image.p = NULL;
//	pmenu->back_image.p = NULL;
	//pmenu->menu_icon.p = NULL;
	if(pmenu->facade == 1 || pmenu->facade == 2)
	{
//	    uint32_t size;
		
	    icon_addr = (buffer[index]<<24)|(buffer[index+1]<<16)|(buffer[index+2]<<8)|(buffer[index+3]);
	    index += 4;
        init_icon(&(pmenu->up_icon), icon_addr);
        icon_addr = (buffer[index]<<24)|(buffer[index+1]<<16)|(buffer[index+2]<<8)|(buffer[index+3]);
        index += 4;
        init_icon(&(pmenu->down_icon), icon_addr);
//        size = menu_icon.width * menu_icon.height * menu_icon.byte; 
//        pmenu->fore_image.width = menu_icon.width;
//        pmenu->fore_image.height = menu_icon.height;
//        pmenu->fore_image.p = (uint8_t *) my_malloc(size);
//        get_icon_image(&menu_icon,0);
//		memcpy(pmenu->fore_image.p,menu_icon.p,size);

//        pmenu->back_image.width = menu_icon.width;
//        pmenu->back_image.height = menu_icon.height;
//        pmenu->back_image.p = (uint8_t *) my_malloc(size);
//        get_icon_image(&menu_icon,1);
//        memcpy(pmenu->back_image.p,menu_icon.p,size);
//        my_free(menu_icon.p);
	    
		
		
	}
	
	

	pmenu->style = buffer[index];
	pmenu->dir = buffer[index+1];
	pmenu->menu_num = buffer[index + 2];
	/*
	pmenu->language_num = buffer[index + 3];
	pmenu->planguage_num = &buffer[index + 4];
	if(*(pmenu->planguage_num) == 0xFF)
	{
		*(pmenu->planguage_num) = 0;
	}
	*/
	index += 3;

	if(pmenu->menu_num)
    {
	    pmenu->pstr_id = (uint16_t *) my_malloc(pmenu->menu_num * sizeof(uint16_t));
	    memset(pmenu->pstr_id,0,pmenu->menu_num * sizeof(uint16_t));
	    
	    for(i=0;i<pmenu->menu_num;i++)
	    {
	        pmenu->pstr_id[i] = (buffer[index] << 8) | (buffer[index+1]);
	        index += 2;
	    }
	}
	pmenu->num = 0xff;
	pmenu->text_id = 0xffff;
	pmenu->state = 0;
	ctrl_init((t_ctrl *) pmenu);
	
	return 0;
}


/***************************************************************************************
* End of File: menu.c

****************************************************************************************/
