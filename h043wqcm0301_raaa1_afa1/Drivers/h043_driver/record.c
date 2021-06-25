
/***************************************************************************************
****************************************************************************************
* Project      : 20180802_HMI_43_TEST
* FILE         : record.c
* Description  : 
*                      
* Copyright (c) 2018 by ShenZhen JingHua Displays Co.,Ltd... All Rights Reserved.
* 
* History:
* Version		name			Date			Description
* 0.01			Qu Bingbing		2018/10/23		Initial Version
* 
****************************************************************************************
****************************************************************************************/

#include "record.h"
#include "cfg.h"
#include "mem_my.h"
#include "str.h"
#include "string.h"
#include "cmd.h"


#define SLIDER_WIDTH              (23)
#define COLOR_HSLIDER             (0xFFFF)
#define SLI_LIN_WIDTH             (SLIDER_WIDTH / 2 + 3)
#define LINE_SPACE                (5)
#define MAX_STR_LEN               (255)
#define MIN_SLI_HEIGHT            (25)           //滑动条最小高度的像素点
 










static void  update_first_show(data_record * ptrecord)
{
	
    uint16_t h0,h1,dh0,dh1;
    
	
	
	dh1 = ptrecord->slider.rect.top - ptrecord->tp_rect.top;
	h0 = ptrecord->tp_rect.bottom - ptrecord->tp_rect.top;
	h1 = ptrecord->slider.rect.bottom - ptrecord->slider.rect.top;
	dh0 = h0 - h1;
	
	ptrecord->pinfo_record->first_show = dh1 * (ptrecord->pinfo_record->record_num - ptrecord->show_record) / dh0;

   
}




static void update_select_record(data_record * ptrecord, uint16_t pos)
{

    uint16_t offset,select_num,record_num;
    
    


    offset = (pos - ptrecord->tp_rect.top) * ptrecord->show_record / (ptrecord->tp_rect.bottom - ptrecord->tp_rect.top); 

    if(ptrecord->show_sort == 0)
    {
		record_num = (ptrecord->pinfo_record->first_show + ptrecord->pinfo_record->record_index) % ptrecord->max_record;
		
        select_num = record_num + offset;
		
		if(select_num >= ptrecord->max_record)
		{
			select_num -= ptrecord->max_record; 
		}
    }
    else
    {
		if(ptrecord->pinfo_record->record_num  < ptrecord->max_record)
		{
			record_num = ptrecord->pinfo_record->record_num - ptrecord->pinfo_record->first_show  - 1;
		}
		else
		{
			if(ptrecord->pinfo_record->record_index > ptrecord->pinfo_record->first_show)
			{
				record_num = ptrecord->pinfo_record->record_index - ptrecord->pinfo_record->first_show - 1;
			}
			else
			{
				record_num = ptrecord->pinfo_record->record_index + ptrecord->max_record - ptrecord->pinfo_record->first_show - 1;
			}	
		}
		if(record_num < offset)
		{
			record_num += ptrecord->max_record;
		}
        select_num = record_num - offset;
		
		
    }


    ptrecord->pinfo_record->select_record = select_num;

    ptrecord->screen_update = 1;

    
    
}

static void slider_touch_update(data_record * ptrecord)
{
	short dh,h;
	
	h = ptrecord->slider.rect.bottom - ptrecord->slider.rect.top;
	dh = ptrecord->slider.pos - ptrecord->slider.old_pos;
	ptrecord->slider.rect.bottom += dh;
	ptrecord->slider.rect.top += dh;
	if(ptrecord->slider.rect.bottom > ptrecord->tp_rect.bottom)
	{
		ptrecord->slider.rect.bottom = ptrecord->tp_rect.bottom;
		ptrecord->slider.rect.top = ptrecord->slider.rect.bottom - h;
		ptrecord->slider.pos = (ptrecord->slider.rect.bottom + ptrecord->slider.rect.top) / 2;
	}
	else if(ptrecord->slider.rect.top < ptrecord->tp_rect.top || ptrecord->slider.rect.top > ptrecord->slider.rect.bottom)
	{
		ptrecord->slider.rect.top = ptrecord->tp_rect.top;
		ptrecord->slider.rect.bottom = ptrecord->slider.rect.top + h;
		ptrecord->slider.pos = (ptrecord->slider.rect.bottom + ptrecord->slider.rect.top) / 2;
	}
	ptrecord->slider.old_pos = ptrecord->slider.pos;
	update_first_show(ptrecord);
	
}







static uint8_t record_msg(void *p, t_msg * p_msg)
{
	uint16_t x,y;
	uint8_t flag,f,select_flag;
	t_msg msg;

	f = 1;
	select_flag = 0;

	if(p_msg->msg != MSG_TP)
    {
        return f;
    }

     data_record * ptrecord  = (data_record *) p;
    
    
    x = p_msg->a;
    y = p_msg->b;
    flag = p_msg->c;
    select_flag = 0;

    if((flag != 1) && !GETBIT( ptrecord->state, BTN_STATE_ACTIVE))
	{
		return f;
	}

	if(ptrecord->select)
	{

	    if(pt_in_rect(&ptrecord->select_rect, x, y))
    	{
    	    f = 0;
    	    select_flag = 1;
    	}

    	switch(flag)
    	{
    	    case 1:
            {
    	        if(!f)
    	        {
    	            SETBIT( ptrecord->state, BTN_STATE_ACTIVE);
    				msg.msg = MSG_TOUCH;
    			    msg.a   = (ptrecord->ctrl.id >> 16) & (0xffff);
    			    msg.b   = (ptrecord->ctrl.id) & (0xffff);
    			    msg.c   = 1;
    			    msg_dev.write_message((void *)&msg,0);
    			    update_select_record(ptrecord,y);
    			    
    	        }
    	        break;
    	    }

    	    case 0:
    	    {
    	        CLRBIT( ptrecord->state, BTN_STATE_ACTIVE);
    	        break;
    	    }

    	    default:
    	    {
    	        break;
    	    }

    	   
    	        
    	}
	    
	}

	if(ptrecord->slider.flag && select_flag == 0)
	{
	    
        if(pt_in_rect(&ptrecord->slider.rect, x, y))
        {
            f = 0;
        }

	    switch(flag)
	    {
	        case 1:
	        {
	            if(!f)
	            {
					SETBIT( ptrecord->state, BTN_STATE_ACTIVE);
					xTimerReset(ptrecord->slider.timer,3);
					xTimerStart(ptrecord->slider.timer,3);
					ptrecord->slider.pos = y;
					slider_touch_update(ptrecord);
	            }
	            break;
	        }
	       case 2:
	       {
				ptrecord->slider.pos = y;
			   slider_touch_update(ptrecord);
	       }
		   break;
	       case 0:
	       {
                CLRBIT( ptrecord->state, BTN_STATE_ACTIVE);
                xTimerStop(ptrecord->slider.timer,3);
                break;
	       
	       }
	    }
	    
	}

	return f;
	
}


static void record_release( void* pCtrl )
{

    data_record  * precord = (data_record *) pCtrl;

    if(precord->type == CFG_REC)
    {

        if(precord->subitem)
        {
            my_free(precord->subitem);
        }
        if(precord->event_descrip)
        {
            my_free(precord->event_descrip);
        }
        if(precord->slider.timer)
        {
             xTimerDelete(precord->slider.timer,3);
        }

        my_free(precord);
    }

    
    
}



static void upload_record_slect(data_record * ptrcord)
{
    uint8_t *p;
    p = my_malloc(15);

    p[0] = 0xEE;
    p[1] = 0xB1;
    p[2] = 0x5A;

    p[3] = ptrcord->ctrl.id >> 24;
    p[4] = ptrcord->ctrl.id >> 16;
    p[5] = ptrcord->ctrl.id >> 8;
    p[6] = (ptrcord->ctrl.id & 0xFF);
    p[7] = ptrcord->type;
    p[8] = ptrcord->pinfo_record->select_record >> 8;
	p[9] = ptrcord->pinfo_record->select_record;
    p[10] = 0xFF;
    p[11] = 0xFC;
    p[12] = 0xFF;
    p[13] = 0xFF;
	p[14] = ';';
    cmd_dev.notify_cmd(p,15);
    my_free(p);
}



static void record_act(base_touch *p,uint8_t flag)
{
	if(p->type == CFG_REC)
	{
		data_record * ptrecord = (data_record *) p;
		upload_record_slect(ptrecord);
	}

   
}












static draw_text * init_draw_text( t_rect rect, font_type * ptfont)
{

    uint16_t w,h;
    uint8_t line,col,i;
    draw_text * pdraw = NULL;
    uint32_t size;

    if(ptfont == NULL)
    {
        return pdraw;
    }

    if(ptfont->code == 0)
    {
       w = ptfont->width;
    }
    else
    {
       w = ptfont->width / 2;
    }

    h = get_font_height(*ptfont);

    line = (rect.bottom - rect.top + 1) / h;
    col = (rect.right - rect.left +  1) / w;
    	
    if(col == 0 || line == 0)
    {
    	return pdraw ;
    }

    if(ptfont->code == 3)
	{
		col = 2 * col;
	}

	size = sizeof(draw_text) + line * (sizeof(line_text) + col);
	pdraw = (draw_text *) my_malloc(size);
    pdraw->line_num = line;
    pdraw->col_num = col;

    pdraw->pline = (line_text *) ((uint8_t *) pdraw + sizeof(draw_text)); 

    
	for(i = 0; i < line ; i++)
	{
		pdraw->pline[i].pcode = (uint8_t *) ((uint8_t *) pdraw + sizeof(draw_text) + line * sizeof(line_text) + i * col);
	}
	
	return pdraw;
    
}


static void update_draw_text(draw_text  * pdraw, str_info tcode,font_type * ptfont,t_rect rect)
{
    uint16_t w;
    uint8_t line,col,i,j,k;
    
    if(pdraw == NULL || ptfont == NULL)
	{
		return;
	}
	
    pdraw->line = 0;
    
    if(pdraw->line_num == 0 || pdraw->col_num == 0)
    {
        return;
    }


    if(tcode.p == NULL || tcode.len == 0)
	{
	    return;
	}

	if(ptfont->code == 0)
	{
		w = ptfont->width;
	}
	else
	{
		w = ptfont->width / 2;
	}


	line = pdraw->line_num;
	col = pdraw->col_num;
	pdraw->line = 1;

	if(ptfont->code < 3)
	{
	    for(j=0,i=0,k=0;(j<tcode.len) && (i<line);j++)
	    {
	        if(tcode.p[j] < 0x80)
	        {

	             if(tcode.p[j] != '\r')
    			{
    				pdraw->pline[i].pcode[k] = tcode.p[j];
    				k++;
    				if(k >= col || (j + 1) == tcode.len)
    				{
    					pdraw->pline[i].len = k;
    				    pdraw->pline[i].width = k * w;
    					k = 0;
    					i++;
    				}
                            
    			}
    			else if((j + 1) < tcode.len)
    			{

    			    if(tcode.p[j] == '\n')
    				{
    					j++;
    					pdraw->pline[i].len = k;
    					pdraw->pline[i].width = k * w;
    					k = 0;
    					i++;
    				}
    				else
    				{
    					pdraw->pline[i].pcode[k] = tcode.p[j];
    					k++;
    					if(k >= col)
    					{
    						pdraw->pline[i].len = k;
    						pdraw->pline[i].width = k * w;
    						k = 0;
    						i++;
    					}
    				}
    			    
    			}
    			else
    			{
    			    pdraw->pline[i].pcode[k] = tcode.p[j];
    				k++;
    				if(k >= col || (j + 1) == tcode.len)
    				{
    					pdraw->pline[i].len = k;
    				    pdraw->pline[i].width = k * w;
    					k = 0;
    					i++;
    				}
    			    
    			}
	            
	        }
	        else
	        {

	            if((j + 1) < tcode.len)
			    {
			        if(k+1 < col)
					{
						pdraw->pline[i].pcode[k] = tcode.p[j];
						k++;
						j++;
						pdraw->pline[i].pcode[k] = tcode.p[j];
						k++;
						if(k >= col || (j + 1) >= tcode.len)
						{
							pdraw->pline[i].len = k;
							pdraw->pline[i].width = k * w;
							k = 0;
							i++;
						}
					}
					else
					{
						j--;
						pdraw->pline[i].len = k;
						pdraw->pline[i].width = k * w;
						k = 0;
						i++;
					}
			        
			    }
			    else
				{
					pdraw->pline[i].pcode[k] = '*';
					k++;
					pdraw->pline[i].len = k;
					pdraw->pline[i].width = k * w;
					k = 0;
					i++;
				}
                
	            
	            
	        }
	        
	    }

	    pdraw->line = i;
	    
	}
	else
	{
	    uint16_t width;
		width = rect.right - rect.left + 1;
		pdraw->pline[0].width = 0;
		for(j=0,i=0,k=0;(j<tcode.len) && (i<line);j++)
		{
		    if((tcode.p[j]==0) && (tcode.p[j+1]<0x80))
		    {
		        if(tcode.p[j+1] != '\r')
				{
					pdraw->pline[i].pcode[k] = tcode.p[j];
					k++;
					j++;
					pdraw->pline[i].pcode[k] = tcode.p[j];
					k++;
					pdraw->pline[i].width += w;

					if(pdraw->pline[i].width + w > width || (j+2) >= tcode.len)
					{
						pdraw->pline[i].len = k;
						k = 0;
						i++;
						if(i < line)
						{
						    pdraw->pline[i].width = 0;
						}
					}
				}
				else if((j + 3) < tcode.len)
				{
					if(tcode.p[j+2]==0 && tcode.p[j+3]=='\n' )
					{
						j+=3;
						pdraw->pline[i].len = k;
						k = 0;
						i++;
						if(i < line)
						{
							pdraw->pline[i].width = 0;
						}
					}
					else
					{
						pdraw->pline[i].pcode[k] = tcode.p[j];
						k++;
						j++;
						pdraw->pline[i].pcode[k] = tcode.p[j];
						k++;
						pdraw->pline[i].width += w;

						if(pdraw->pline[i].width + w > width || (j+2) >= tcode.len)
						{
							pdraw->pline[i].len = k;
							k = 0;
							i++;
							if(i < line)
							{
								pdraw->pline[i].width = 0;
							}
						}
								
					}
				}
		        
		    }
            else
		    {
		        if(pdraw->pline[i].width + 2*w <=  width)
				{
					pdraw->pline[i].pcode[k] = tcode.p[j];
					k++;
					j++;
					pdraw->pline[i].pcode[k] = tcode.p[j];
					k++;
					pdraw->pline[i].width += 2*w;

					if(pdraw->pline[i].width + w > width || (j+2) >= tcode.len)
					{
						pdraw->pline[i].len = k;
						k = 0;
						i++;
						if(i < line)
						{
						    pdraw->pline[i].width = 0;
						}
					}
				}
				else
				{
					j--;
					pdraw->pline[i].len = k;
					k = 0;
					i++;
					if(i < line)
					{
						pdraw->pline[i].width = 0;
					}
				}
		        
		    }
		    
		    
		}

		pdraw->line = i;
	}

	
	
    
}


static void show_font_rect(t_rect rect, str_info tcode, font_type * ptfont, uint16_t color,uint8_t h_align,uint8_t v_align )
{
    if(ptfont == NULL)
    {
        return;
    }
    draw_text  * pdraw = init_draw_text(rect,ptfont);

    if(pdraw == NULL)
    {
        return;
    }

    uint8_t *code;
    font_type * ch_font, *hz_font;
    uint16_t width,height,x,y,htemp;
    uint8_t  line,code_index,i;
    uint16_t hz;

    update_draw_text(pdraw, tcode,ptfont, rect);

    ch_font = hz_to_ch(ptfont);
    hz_font = ch_to_hz(ptfont);

    if(ptfont->code == 0)
	{
		width = ptfont->width;
	}
	else
	{
		width = ptfont->width / 2;
	}
	height = get_font_height(*ptfont);

	line = pdraw->line;
    htemp = line * height;


    if(v_align == 0)
	{
		y = rect.top;
	}
	else if(v_align == 1)
	{
		y = rect.top + (rect.bottom - rect.top + 1 - htemp) / 2;
	}
	else
	{
		y = rect.bottom - htemp + 1;
	}

	for(i = 0;i<line;i++)
	{
	    if(h_align == 0)
		{
			x = rect.left;
		}
		else if(h_align == 1)
		{
			x = rect.left + (rect.right - rect.left + 1 - pdraw->pline[i].width) / 2;
		}
		else
		{
			x = rect.right - pdraw->pline[i].width + 1;
		}

		code = pdraw->pline[i].pcode;

		if(ptfont->code < 3)
		{
			for(code_index=0;code_index<pdraw->pline[i].len;code_index++)
			{
				if(code[code_index] < 0x80)
				{
					show_charn_font(x,y,color, code[code_index],*ch_font);
					x += width;
				}
				else
				{
					hz = code[code_index] << 8 | code[code_index + 1];
					show_nfont(x,y,color, hz,*hz_font);
					code_index++;
					x += 2*width;
				}

					
			}

		}
		else
		{
		    for(code_index=0;code_index<pdraw->pline[i].len;code_index++)
			{
				if(code[code_index] == 0 && code[code_index+1] < 0x80)
				{
					show_charn_font(x,y,color, code[code_index + 1],*ch_font);
					x += width;
				}
				else
				{
					hz = code[code_index] << 8 | code[code_index + 1];
					show_nfont(x,y,color, hz,*hz_font);
					x += 2*width;
				}
				code_index++;
			}
	    
		}

		if(i+1 < line)
		{
			y += height;
		}
	    
	}
    

    my_free(pdraw);

    

    
    
}



static void get_record_str(str_info * dstr,str_info * sstr,uint8_t subitem)
{
    uint8_t i,len,item;

    dstr->len = 0;
    len = 0;
    item = 0;
	


    for(i=0;i<sstr->len;i++)
    {
        if(item == subitem && sstr->p[i] != ';')
        {
            dstr->p[len] = sstr->p[i];
            len++;
        }
        if(sstr->p[i] == ';')
        {
            item++;

            if(item > subitem)
            {
                dstr->len = len;
                break;
            }
        }
    }
    
}



warn_event * get_event_descrip(data_record * ptrecord, uint16_t data)
{

    uint16_t i;

    for(i=0;i<ptrecord->event_num;i++)
    {
        if(data == ptrecord->event_descrip[i].warn_data)
        {
            return &(ptrecord->event_descrip[i]);
        }
    }

    return NULL;
    
}





static uint8_t number_to_string(uint16_t number, uint8_t *p)
{
   


   uint16_t temp, mul;
   uint8_t i;
	
	

	mul = 1;
	i = 0;

    while(1)
	{
		temp = number / mul;
		
		if(temp >= 10)
		{
			mul *= 10;
		}
		else
		{
			break;
		}
	}
	
    

    while (1)
    {
        temp = (number / mul) % 10;
        mul /= 10;
        p[i] = temp + '0';
        i++;
        if(mul == 0)
        {
            break;
        }
    }


    return i;
    
    
}

static uint8_t  time_to_string(rtc_time time, uint8_t * p)
{
	

    p[0] = time.year / 1000 % 10 + '0';
    p[1] = time.year / 100 % 10 + '0';
    p[2] = time.year / 10 % 10 + '0';
    p[3] = time.year % 10 + '0';

    p[4] = '-';

    p[5] = time.month / 10 % 10 + '0';
    p[6] = time.month % 10 + '0';

    p[7] = '-';

    p[8] = time.day / 10 % 10 + '0';
    p[9] = time.day % 10 + '0';

    p[10] = ' ';
    
    p[11] = time.hour / 10 % 10 + '0';
    p[12] = time.hour % 10 + '0';

    p[13] = ':';

    p[14] = time.min / 10 % 10 + '0';
    p[15] = time.min % 10 + '0';

    p[16] = ':';

    p[17] = time.sec / 10 % 10 + '0';
    p[18] = time.sec % 10 + '0';
	
	return 19;
    
    
}



static uint16_t  get_record_string(data_record * ptrecord,str_info * pstr, uint16_t record,uint8_t subitem)
{

    uint16_t color,record_num;
    
    pstr->len = 0;
	
	record_num = ptrecord->pinfo_record->record_num;
	color = ptrecord->font_color;

    if(record >= record_num)
    {
        return color;
    }


    if(ptrecord->record_type  < 3)
    {
        warn_event * pevent;

        /*
        
        if(subitem > 1)
        {
            return color;
        }

        */


         pevent = get_event_descrip(ptrecord,ptrecord->pinfo_record->event_wran[record].date);
		 

        if(subitem == 0)
        {
           
           if(pevent)
           {
				uint8_t * p = NULL;
                color = pevent->color;
                p = get_str_code(pevent->str_id,&(pstr->len));
				memcpy(pstr->p,p,pstr->len);
           }
           else
           {
                
                pstr->len = number_to_string(ptrecord->pinfo_record->event_wran[record].date, pstr->p);  
           }
            
        }
        else if(subitem == 1)
        {

            if(pevent)
            {
                color = pevent->color;
            }

            pstr->len = time_to_string(ptrecord->pinfo_record->event_wran[record].time0, pstr->p);
            
        }
        else if(subitem == 2 && ptrecord->record_type == 2)
        {
            if(pevent)
            {
                color = pevent->color;
            }
			
			if(ptrecord->pinfo_record->event_wran[record].flag == 2)
			{
				pstr->len = time_to_string(ptrecord->pinfo_record->event_wran[record].time1, pstr->p);
			}
            
        }
    }
    else
    {
        get_record_str(pstr,&(ptrecord->pinfo_record->event_str[record]),subitem);

    }
	
	return color;
    
}




static void record_draw_screen(base_touch * p)
{


    if(p->type == CFG_REC)
    {
        uint16_t x,y,w,h,i,j;
        uint16_t sx,sy;
        uint32_t sum,sum0;
        uint16_t num,show_record,first_show,select_num;
        t_rect rect;
        str_info tstr;
        uint16_t color;
        
        
        
        data_record * ptrecord = (data_record *)p;

        if(ptrecord->background)
        {
            x = ptrecord->tp_rect.left;
            y = ptrecord->tp_rect.top;
            w = ptrecord->tp_rect.right - ptrecord->tp_rect.left;
            h = ptrecord->tp_rect.bottom - ptrecord->tp_rect.top;
            fill_rect(x,y,w,h,ptrecord->background_color);
            
        }

        if(w<SLIDER_WIDTH)
        {
            return;
        }
            

		show_record = (ptrecord->pinfo_record->record_num);

        show_record = (show_record < ptrecord->show_record) ? show_record : ptrecord->show_record;
		select_num = 0xFFFF;
        //绘制文字
		if(ptrecord->show_sort == 0) //时间顺序
		{
			first_show = (ptrecord->pinfo_record->first_show + ptrecord->pinfo_record->record_index) % ptrecord->max_record;
			if(ptrecord->pinfo_record->select_record != 0xFFFF)
			{
				if(ptrecord->pinfo_record->select_record >=  first_show)
				{
					select_num = ptrecord->pinfo_record->select_record - first_show;
				}
				else
				{
					select_num = ptrecord->pinfo_record->select_record + ptrecord->max_record - first_show;
				}
				
					
			}
		}
		else //时间逆序
		{
			if(ptrecord->pinfo_record->record_num < ptrecord->max_record)
			{
				first_show = ptrecord->pinfo_record->record_num - 1 - ptrecord->pinfo_record->first_show;
			}
			else
			{
				uint16_t index = ptrecord->pinfo_record->record_index;
				//first_show = ptrecord->pinfo_record->record_index - 1;
				if(ptrecord->pinfo_record->record_index < (ptrecord->pinfo_record->first_show + 1))
				{
					index += ptrecord->max_record;
				}
				first_show = index - 1 - ptrecord->pinfo_record->first_show;
			}
			
			if(ptrecord->pinfo_record->select_record != 0xFFFF)
			{
				if(ptrecord->pinfo_record->select_record <= first_show)
				{
					select_num = first_show - ptrecord->pinfo_record->select_record;
				}
				else
				{
					select_num = first_show + ptrecord->max_record - ptrecord->pinfo_record->select_record;
				}
			}
		}
		
		if(select_num < ptrecord->show_record)
		{
			uint8_t r0,g0,b0,r1,g1,b1;
			x = ptrecord->select_rect.left;
			w = ptrecord->select_rect.right - ptrecord->select_rect.left;
			y = ptrecord->tp_rect.top + select_num * (ptrecord->tp_rect.bottom - ptrecord->tp_rect.top) / ptrecord->show_record;
			h = (select_num + 1)* (ptrecord->tp_rect.bottom - ptrecord->tp_rect.top) / ptrecord->show_record - select_num * (ptrecord->tp_rect.bottom - ptrecord->tp_rect.top) / ptrecord->show_record;
			
			r0 = (ptrecord->font_color >> 10) & (0x1F);
			g0 = (ptrecord->font_color >> 5) & (0x1F);
			b0 = (ptrecord->font_color) & (0x1F);
			
			r1 = (ptrecord->background_color >> 10) & (0x1F);
			g1 = (ptrecord->background_color >> 5) & (0x1F);
			b1 = (ptrecord->background_color) & (0x1F);
			
			r0 = (r0 + r1) / 2;
			g0 = (g0 + g1) / 2;
			b0 = (b0 + b1) / 2;
			
			color = (r0 << 10) | (g0 << 5) | (b0);
			fill_rect(x,y,w,h,color);
			
			
		}

		if(ptrecord->grid) //绘制网格
        {

            x = ptrecord->tp_rect.left;
            y = ptrecord->tp_rect.top;
            w = ptrecord->tp_rect.right - ptrecord->tp_rect.left;
            h = ptrecord->tp_rect.bottom - ptrecord->tp_rect.top;
            
            for(i=0;i<(ptrecord->show_record+1);i++)
            {
                sy = y + (h*i)/ptrecord->show_record;
                
                if(i == 0 || i == ptrecord->show_record)
                {
                    draw_Hline(x,sy,w,ptrecord->grid_color);
                }
                else
                {
                    if(ptrecord->slider.flag)
                    {
                        draw_Hline(x,sy,w-SLIDER_WIDTH,ptrecord->grid_color);
                    }
                    else
                    {
                        draw_Hline(x,sy,w,ptrecord->grid_color);
                    }
                }
            }

            sum = 0;
            sum0 = 0;

            for(i=0;i<ptrecord->subitem_num;i++)
            {
                sum += ptrecord->subitem[i];
            }

            sx = x;

            if(ptrecord->slider.flag)
            {
                w -= SLIDER_WIDTH;
                draw_Vline(ptrecord->tp_rect.right,y,h,ptrecord->grid_color);
            }

            for(i=0;i<(ptrecord->subitem_num+1);i++)
            {
                if(i > 0 )
                {
                    sum0 += ptrecord->subitem[i-1];
                    sx = x + (sum0 * w)/sum;
                }

                draw_Vline(sx,y,h,ptrecord->grid_color);
            }

            
            
        }

        
		
        rect.top = ptrecord->tp_rect.top;
		h = ptrecord->tp_rect.bottom - ptrecord->tp_rect.top;
        tstr.p = (uint8_t *)my_malloc(MAX_STR_LEN);

        for(i=0;i<show_record;i++)
        {
            if(ptrecord->show_sort == 0)
            {
				num = (first_show + i) % ptrecord->max_record;
            }
            else
            {
				if(first_show < i)
				{
					num = first_show + ptrecord->max_record - i;
				}
				else
				{
					num = first_show - i;
				}
            }
			


            rect.bottom = ptrecord->tp_rect.top + (h*(i + 1))/ptrecord->show_record;
            rect.left = ptrecord->tp_rect.left;
            sum0 = 0;

            for(j=0;j<ptrecord->subitem_num;j++)
            {
                sum0 += ptrecord->subitem[j];
                rect.right = ptrecord->tp_rect.left + (sum0 * w)/sum;
                color = get_record_string(ptrecord, &tstr, num, j);
                show_font_rect(rect, tstr, ptrecord->pfont, color, ptrecord->align, 1);
                rect.left = rect.right;
                    
            }
			

            rect.top = rect.bottom;
        }


        my_free(tstr.p);


         if(ptrecord->slider.flag) //绘制滑动条
        {
            x = ptrecord->slider.rect.left;
            y = ptrecord->slider.rect.top;
            w = ptrecord->slider.rect.right - ptrecord->slider.rect.left + 1;
            h = ptrecord->slider.rect.bottom - ptrecord->slider.rect.top + 1;
            fill_rect(x,y,w,h,ptrecord->grid_color);
            x += (w - SLI_LIN_WIDTH) / 2;
            y = ptrecord->slider.pos - LINE_SPACE;
            draw_Hline(x,y, SLI_LIN_WIDTH,COLOR_HSLIDER);
            y = ptrecord->slider.pos;
            draw_Hline(x,y, SLI_LIN_WIDTH,COLOR_HSLIDER);
            y = ptrecord->slider.pos + LINE_SPACE;
            draw_Hline(x,y, SLI_LIN_WIDTH,COLOR_HSLIDER);
            
        }
        

    
    }

    
}






static void record_touch_back(TimerHandle_t timer)
{
	data_record * ptrecord = pvTimerGetTimerID(timer);
	ptrecord->screen_update = 1; //更新界面

}

static uint16_t find_record_data(data_record * ptrecord,uint16_t data)
{
    int i;
    uint16_t record = 0xFFFF;
    uint16_t index;

//    if(ptrecord->record_type != 0)
//    {
//        return record;
//    }


    if(ptrecord->pinfo_record->record_num <  ptrecord->max_record)
    {
		if(ptrecord->pinfo_record->record_num == 0)
		{
			return record;
		}
        index = ptrecord->pinfo_record->record_num - 1;
        
        for(i=index;i>=0;i--)
        {
            if(ptrecord->pinfo_record->event_wran[i].date == data)
            {
                record = i;
                break;
            }
        }
        
    }
    else
    {
        index = ptrecord->pinfo_record->record_index - 1;

        do
        {

            if(ptrecord->pinfo_record->event_wran[i].date == data)
            {
                record = i;
                break;
            }

            if(i == 0)
            {
                i += ptrecord->max_record;
            }

            i--;

            
            
        }
        while (i != index);
        
    }

    

    

    return record;
}


static void update_select_area(data_record * ptrecord)
{
    if(ptrecord->slider.flag)
    {
        ptrecord->select_rect.right = ptrecord->tp_rect.right - SLIDER_WIDTH;
    }
    else
    {
        ptrecord->select_rect.right = ptrecord->tp_rect.right;
    }

    if(ptrecord->pinfo_record->record_num <= ptrecord->show_record)
    {
        ptrecord->select_rect.bottom = ptrecord->select_rect.top + (ptrecord->tp_rect.bottom - 
        ptrecord->tp_rect.top) * ptrecord->pinfo_record->record_num / ptrecord->show_record;   
    }
   

    
}


static void update_slider_status(data_record * ptrecord)
{
    uint16_t h,pos;
    if(ptrecord->pinfo_record->record_num >  ptrecord->show_record)
    {
        ptrecord->slider.flag = 1; //标志要绘制滑动条了
        h = ptrecord->show_record * (ptrecord->tp_rect.bottom - ptrecord->tp_rect.top) / ptrecord->pinfo_record->record_num;
        if(h < MIN_SLI_HEIGHT)
        {
            h = MIN_SLI_HEIGHT;
        }

        pos = ptrecord->pinfo_record->first_show * (ptrecord->tp_rect.bottom - ptrecord->tp_rect.top) / ptrecord->pinfo_record->record_num;
        ptrecord->slider.rect.top = ptrecord->tp_rect.top + pos;
        ptrecord->slider.rect.bottom = ptrecord->slider.rect.top + h;
		ptrecord->slider.pos = (ptrecord->slider.rect.top + ptrecord->slider.rect.bottom) / 2;
		ptrecord->slider.old_pos = ptrecord->slider.pos;
    }
    else
    {
        ptrecord->slider.flag = 0;
    }
}



static void handler_record_add(data_record * ptrecord,uint8_t * p,uint8_t len)
{
    uint16_t record_num,next_record;
    uint16_t data;
	
	
	record_num = ptrecord->pinfo_record->record_num;
	
	
	
    

    if(ptrecord->record_type < 3)
    {
        if(len != 2 && len != 10)
        {
            return;
        }
        
        data = (p[0] << 8) | (p[1]);
        if(ptrecord->record_type == 0)
        {
            if(find_record_data(ptrecord, data) != 0xFFFF)
            {
				
                return;
            }
        }
		
		if(record_num == ptrecord->max_record)
		{
			next_record = ptrecord->pinfo_record->record_index;
			ptrecord->pinfo_record->record_index = (next_record + 1) % ptrecord->max_record;
		}
		else
		{
			next_record = ptrecord->pinfo_record->record_num;
			ptrecord->pinfo_record->record_num = next_record + 1;
			
		}

		
        ptrecord->pinfo_record->event_wran[next_record].date = data;
		ptrecord->pinfo_record->event_wran[next_record].flag = 1;

        if(len == 10)
        {
            ptrecord->pinfo_record->event_wran[next_record].time0.sec = p[2];
            ptrecord->pinfo_record->event_wran[next_record].time0.min = p[3];
            ptrecord->pinfo_record->event_wran[next_record].time0.hour = p[4];
            ptrecord->pinfo_record->event_wran[next_record].time0.day = p[5];
            ptrecord->pinfo_record->event_wran[next_record].time0.week = p[6];
            ptrecord->pinfo_record->event_wran[next_record].time0.month = p[7];
            ptrecord->pinfo_record->event_wran[next_record].time0.year = ((p[8] << 8) | p[9]);
        }
        else
        {
            rtc_dev.get_time(&(ptrecord->pinfo_record->event_wran[next_record].time0)); //获得RTC时间
        }  
    }
    else
    {
		if(record_num == ptrecord->max_record)
		{
			next_record = ptrecord->pinfo_record->record_index;
			ptrecord->pinfo_record->record_index = (next_record + 1) % ptrecord->max_record;
		}
		else
		{
			next_record = ptrecord->pinfo_record->record_num;
			ptrecord->pinfo_record->record_num = next_record + 1;
			
		}
        ptrecord->pinfo_record->event_str[next_record].len = len;
        memcpy(ptrecord->pinfo_record->event_str[next_record].p,p,len);
    }

    if(ptrecord->show_slider)
    {
        update_slider_status(ptrecord);
    }

    if(ptrecord->select)
    {
        update_select_area(ptrecord);
    }

    ptrecord->screen_update = 1;

}


static void handler_record_relieve(data_record * ptrecord,uint8_t * p,uint8_t len)
{

    uint16_t index,data;
     
    if(ptrecord->record_type != 2)
    {
        return;
    }

    if(len != 2 && len != 10)
    {
        return;
    }

    data = (p[0] << 8) | (p[1]);
    index = find_record_data(ptrecord,data);

    if(index != 0xFFFF)
    {
		if(ptrecord->pinfo_record->event_wran[index].flag == 1)
		{
			if(len == 10)
			{

				ptrecord->pinfo_record->event_wran[index].time1.sec = p[2];
				ptrecord->pinfo_record->event_wran[index].time1.min = p[3];
				ptrecord->pinfo_record->event_wran[index].time1.hour = p[4];
				ptrecord->pinfo_record->event_wran[index].time1.day = p[5];
				ptrecord->pinfo_record->event_wran[index].time1.week = p[6];
				ptrecord->pinfo_record->event_wran[index].time1.month = p[7];
				ptrecord->pinfo_record->event_wran[index].time1.year = ((p[8] << 8) | p[9]);
				
			}
			else
			{

				rtc_dev.get_time(&(ptrecord->pinfo_record->event_wran[index].time1)); //获得RTC时间
				
			}
			
			ptrecord->pinfo_record->event_wran[index].flag = 2;
		}
    }

    ptrecord->screen_update = 1;
    

    

    
}



static void handler_record_clear(data_record * ptrecord,uint8_t * p,uint8_t len)
{
    if(len != 0)
    {
        return;
    }
	
	if(ptrecord->record_type < 3)
	{	
		uint32_t size;
		
		size = ptrecord->max_record * sizeof(event_time) + sizeof(record_info);
		memset((void *)ptrecord->pinfo_record,0,size);
		ptrecord->pinfo_record->event_wran = (event_time *)((uint8_t *) ptrecord->pinfo_record + sizeof(record_info));
		
	}

    ptrecord->pinfo_record->first_show = 0;
    ptrecord->pinfo_record->record_num = 0;
    ptrecord->pinfo_record->select_record = 0xFFFF;
    ptrecord->pinfo_record->record_index = 0;

    if(ptrecord->show_slider)
    {
        update_slider_status(ptrecord);
    }

    if(ptrecord->select)
    {
        update_select_area(ptrecord);
    }
    ptrecord->screen_update = 1;

    
}




static void handler_record_get(data_record * ptrecord,uint8_t * p, uint8_t len)
{

    if(len != 0)
    {
        return;
    }
    
    uint16_t record_num = ptrecord->pinfo_record->record_num;
    uint8_t * buf = (uint8_t *) my_malloc(16);

    buf[0] = 0xEE;
    buf[1] = 0xB1;
    buf[2] = 0x55;
    buf[3] = ptrecord->ctrl.id >> 24;
    buf[5] = ptrecord->ctrl.id >> 16;
    buf[6] = ptrecord->ctrl.id >> 8;
    buf[7] = ptrecord->ctrl.id;
    buf[8] = ptrecord->type;
    buf[9] = record_num >> 8;
    buf[10] = record_num;
    buf[11] = 0xFF;
    buf[12] = 0xFC;
    buf[13] = 0xFF;
    buf[14] = 0xFF;
    buf[15] = 0x3B;

    cmd_dev.notify_cmd(buf,16);
    my_free((void *) buf);
 
    

    
    
}


static void handler_record_offset(data_record * ptrecord,uint8_t * p,uint8_t len)
{
    if(len != 2)
    {
        return;
    }

    uint16_t offset = (p[0] << 8) | p[1];
	
	if(ptrecord->pinfo_record->record_num < ptrecord->max_record)
	{
		if(ptrecord->pinfo_record->record_num < ptrecord->show_record)
		{
			return;
		}
		if((offset + ptrecord->show_record) > ptrecord->pinfo_record->record_num)
		{
			offset = ptrecord->pinfo_record->record_num - ptrecord->show_record;
		}
	}
    ptrecord->pinfo_record->first_show = offset % ptrecord->max_record;
    if(ptrecord->show_slider)
    {
        update_slider_status(ptrecord);
    }
    ptrecord->screen_update = 1;
}







static void first_count_pos(data_record * ptrecord)
{
	uint16_t pos,h0,h1;
	
	h1 = ptrecord->slider.rect.bottom - ptrecord->slider.rect.top;
	h0 = ptrecord->tp_rect.bottom - ptrecord->tp_rect.top;
	pos = ptrecord->pinfo_record->first_show * (h0 - h1) / (ptrecord->pinfo_record->record_num - ptrecord->show_record);
	ptrecord->slider.rect.top = ptrecord->tp_rect.top + pos;
	ptrecord->slider.rect.bottom = ptrecord->slider.rect.top + h1;
	ptrecord->slider.pos = (ptrecord->slider.rect.top + ptrecord->slider.rect.bottom) / 2;
	ptrecord->slider.old_pos = ptrecord->slider.pos;
	
	
	
}











uint8_t record_create(uint8_t * buffer)
{
    data_record * ptrecord = (data_record *) my_malloc(sizeof(data_record));
    uint16_t index = 0;
    uint32_t mem_addr,size;
    uint16_t i;

   
    

    
    if(ptrecord == NULL)
    {
        return 1;
    }
    memset((void *)ptrecord,0,sizeof(data_record));

    

    ptrecord->ctrl.id = ((buffer[0]<<24)|(buffer[1]<<16)|(buffer[4]<<8)|(buffer[5])<<0);
    ptrecord->ctrl.func_ctrl_procmsg = record_msg;
    ptrecord->ctrl.func_ctrl_release = record_release;
   

    ptrecord->type = buffer[6];
    ptrecord->tp_rect.left = (buffer[7]<<8) | (buffer[8]);
    ptrecord->tp_rect.top = (buffer[9]<<8) | (buffer[10]);
    ptrecord->tp_rect.right = (buffer[11]<<8) | (buffer[12]);
    ptrecord->tp_rect.bottom = (buffer[13]<<8) | (buffer[14]);
    ptrecord->pic_id = (buffer[2]<<8) | (buffer[3]);
    ptrecord->pic_on = (buffer[15]<<8) | (buffer[16]);
    ptrecord->act = record_act;
    ptrecord->draw_screen = record_draw_screen;
	ptrecord->pfont = get_font_type(buffer[17]);
	ptrecord->font_color = (buffer[18] << 8) | (buffer[19]);
	ptrecord->grid = buffer[20];
	ptrecord->grid_color = (buffer[21] << 8) | (buffer[22]);
	ptrecord->background = buffer[23];
	ptrecord->background_color = (buffer[24] << 8) | (buffer[25]);
	ptrecord->select = buffer[26];
	ptrecord->show_slider = buffer[27];
	


	ptrecord->cmd_handler_add = handler_record_add;
	ptrecord->cmd_handler_relieve = handler_record_relieve;
	ptrecord->cmd_handler_clear =  handler_record_clear;
	ptrecord->cmd_handler_offset = handler_record_offset;
	ptrecord->cmd_handler_get = handler_record_get;
 	

	ptrecord->record_type = buffer[28];
	ptrecord->show_sort = buffer[29];
	ptrecord->align = buffer[30];
	ptrecord->subitem_num = buffer[31];
	
	ptrecord->subitem = (uint16_t *)my_malloc(sizeof(uint16_t)*ptrecord->subitem_num);

	index = 32;

	for(i=0;i<ptrecord->subitem_num;i++)
	{
	    ptrecord->subitem[i] = (buffer[index] << 8) | (buffer[index+1]);
	    index += 2;
	}

	ptrecord->show_record = (buffer[index] << 8) | (buffer[index+1]);
	index += 2;
	ptrecord->max_record = (buffer[index] << 8) | (buffer[index+1]);
	index += 2;
	ptrecord->record_byte = buffer[index];
	index += 1;
	
    ptrecord->addr = (buffer[index] << 24) | (buffer[index+1] << 16) | (buffer[index+2] << 8) | (buffer[index+3]);
    index += 4;
    ptrecord->size = (buffer[index] << 24) | (buffer[index+1] << 16) | (buffer[index+2] << 8) | (buffer[index+3]);
    index += 4;

    
    ptrecord->mem_addr = &buffer[index];
    index += 4;
	
	
    if(ptrecord->record_type < 3)
	{
		ptrecord->event_num = (buffer[index] << 8) | (buffer[index+1]);
		index += 2;
		
		if(ptrecord->event_num)
		{
			ptrecord->event_descrip = (warn_event *) my_malloc(ptrecord->event_num * sizeof(warn_event));
			
			for(i=0;i<ptrecord->event_num;i++)
			{
				ptrecord->event_descrip[i].warn_data = (buffer[index] << 8) | (buffer[index+1]);
				index += 2;
				ptrecord->event_descrip[i].color = (buffer[index] << 8) | (buffer[index+1]);
				index += 2;
				ptrecord->event_descrip[i].str_id = (buffer[index] << 8) | (buffer[index+1]);
				index += 2;
			}
		}
	}


    

    


    mem_addr = (ptrecord->mem_addr[0] << 24) | (ptrecord->mem_addr[1] << 16) | (ptrecord->mem_addr[2] << 8) | 
    (ptrecord->mem_addr[3]);
    
    if(mem_addr)
    {
        ptrecord->pinfo_record = (record_info *)mem_addr;
    
    }
    else
    {
        
        if(ptrecord->record_type < 3)
        {

            size = ptrecord->max_record * sizeof(event_time) + sizeof(record_info);
            
        }
        else
        {
            size = ptrecord->max_record * (ptrecord->record_byte + sizeof(str_info)) + sizeof(record_info);
        }

        ptrecord->pinfo_record = (record_info *)my_malloc(size);
        memset((void *)ptrecord->pinfo_record,0,size);
        mem_addr = (uint32_t)ptrecord->pinfo_record;

        if(ptrecord->record_type < 3)
        {
            ptrecord->pinfo_record->event_wran = (event_time *)((uint8_t *) ptrecord->pinfo_record + sizeof(record_info));
        }
        else
        {
            ptrecord->pinfo_record->event_str = (str_info *)((uint8_t *) ptrecord->pinfo_record + sizeof(record_info));

            for(i=0;i<ptrecord->max_record;i++)
            {
                ptrecord->pinfo_record->event_str[i].p = (uint8_t *)((uint8_t *)ptrecord->pinfo_record + ptrecord->max_record * sizeof(str_info) + 
                i*ptrecord->record_byte + sizeof(record_info));
            }
        }

        ptrecord->pinfo_record->first_show = 0;
        ptrecord->pinfo_record->record_index = 0;
        ptrecord->pinfo_record->select_record = 0xFFFF;
        ptrecord->pinfo_record->record_num = 0;

        ptrecord->mem_addr[0] = mem_addr >> 24;
        ptrecord->mem_addr[1] = mem_addr >> 16;
        ptrecord->mem_addr[2] = mem_addr >> 8;
        ptrecord->mem_addr[3] = mem_addr;

        
    }

    if(ptrecord->select)
    {
        uint16_t record_num;
         
        record_num = ptrecord->pinfo_record->record_num;
        ptrecord->select_rect = ptrecord->tp_rect;
        if(record_num <= ptrecord->show_record)
        {
            ptrecord->select_rect.bottom = ptrecord->select_rect.top + (ptrecord->tp_rect.bottom - 
            ptrecord->tp_rect.top) * record_num / ptrecord->show_record;   
        }
        else if(ptrecord->show_slider)
        {
            ptrecord->select_rect.right = ptrecord->select_rect.right - SLIDER_WIDTH;
        
        }
		
		
        
    }

    


    if(ptrecord->show_slider)
    {
        ptrecord->slider.timer = xTimerCreate((const char *) "record",
								(TickType_t)50*MS,
								(UBaseType_t)pdTRUE,
								(void*	)ptrecord,
								(TimerCallbackFunction_t)record_touch_back);
								
		ptrecord->slider.rect.right = ptrecord->tp_rect.right;
	    ptrecord->slider.rect.left = ptrecord->tp_rect.right - SLIDER_WIDTH;
		ptrecord->slider.color = ptrecord->grid_color;

	    if(ptrecord->show_record < ptrecord->pinfo_record->record_num)
	    {
	        uint16_t h;
	        ptrecord->slider.flag = 1; //标志要绘制滑动条了
	        h = ptrecord->show_record * (ptrecord->tp_rect.bottom - ptrecord->tp_rect.top) / ptrecord->pinfo_record->record_num;
	        if(h < MIN_SLI_HEIGHT)
	        {
	            h = MIN_SLI_HEIGHT;
	        }
			ptrecord->slider.rect.top = ptrecord->tp_rect.top;
			ptrecord->slider.rect.bottom = ptrecord->slider.rect.top + h;
			ptrecord->slider.pos = (ptrecord->slider.rect.top + ptrecord->slider.rect.bottom) / 2;
			ptrecord->slider.old_pos = ptrecord->slider.pos;
			
			if(ptrecord->pinfo_record->first_show)
			{
				first_count_pos(ptrecord);
			}

	        
	        
	        
	        
	    }
    }
	
	ctrl_init((t_ctrl *) ptrecord);
	return 0;
}





t_ctrl * srecord_create(uint8_t * buffer)
{
    data_record * ptrecord = (data_record *) my_malloc(sizeof(data_record));
    uint16_t index = 0;
    uint32_t mem_addr,size;
    uint16_t i;

   
    

    
    if(ptrecord == NULL)
    {
        return NULL;
    }
    memset((void *)ptrecord,0,sizeof(data_record));

    

    ptrecord->ctrl.id = ((buffer[0]<<24)|(buffer[1]<<16)|(buffer[4]<<8)|(buffer[5])<<0);
    ptrecord->ctrl.func_ctrl_procmsg = record_msg;
    ptrecord->ctrl.func_ctrl_release = record_release;
   

    ptrecord->type = buffer[6];
//    ptrecord->tp_rect.left = (buffer[7]<<8) | (buffer[8]);
//    ptrecord->tp_rect.top = (buffer[9]<<8) | (buffer[10]);
//    ptrecord->tp_rect.right = (buffer[11]<<8) | (buffer[12]);
//    ptrecord->tp_rect.bottom = (buffer[13]<<8) | (buffer[14]);
//    ptrecord->pic_id = (buffer[2]<<8) | (buffer[3]);
//    ptrecord->pic_on = (buffer[15]<<8) | (buffer[16]);
//    ptrecord->act = record_act;
//    ptrecord->draw_screen = record_draw_screen;
//	ptrecord->pfont = get_font_type(buffer[17]);
//	ptrecord->font_color = (buffer[18] << 8) | (buffer[19]);
//	ptrecord->grid = buffer[20];
//	ptrecord->grid_color = (buffer[21] << 8) | (buffer[22]);
//	ptrecord->background = buffer[23];
//	ptrecord->background_color = (buffer[24] << 8) | (buffer[25]);
//	ptrecord->select = buffer[26];
//	ptrecord->show_slider = buffer[27];
	


	ptrecord->cmd_handler_add = handler_record_add;
	ptrecord->cmd_handler_relieve = handler_record_relieve;
	ptrecord->cmd_handler_clear =  handler_record_clear;
	ptrecord->cmd_handler_offset = handler_record_offset;
	ptrecord->cmd_handler_get = handler_record_get;
 	

	ptrecord->record_type = buffer[28];
//	ptrecord->show_sort = buffer[29];
//	ptrecord->align = buffer[30];
	ptrecord->subitem_num = buffer[31];	
//	ptrecord->subitem = (uint16_t *)my_malloc(sizeof(uint16_t)*ptrecord->subitem_num);

	index = 32 + ptrecord->subitem_num * 2;

//	for(i=0;i<ptrecord->subitem_num;i++)
//	{
//	    ptrecord->subitem[i] = (buffer[index] << 8) | (buffer[index+1]);
//	    index += 2;
//	}

	ptrecord->show_record = (buffer[index] << 8) | (buffer[index+1]);
	index += 2;
	ptrecord->max_record = (buffer[index] << 8) | (buffer[index+1]);
	index += 2;
	ptrecord->record_byte = buffer[index];
	index += 1;
	
    ptrecord->addr = (buffer[index] << 24) | (buffer[index+1] << 16) | (buffer[index+2] << 8) | (buffer[index+3]);
    index += 4;
    ptrecord->size = (buffer[index] << 24) | (buffer[index+1] << 16) | (buffer[index+2] << 8) | (buffer[index+3]);
    index += 4;

    
    ptrecord->mem_addr = &buffer[index];
    index += 4;
	
	
    if(ptrecord->record_type < 3)
	{
		ptrecord->event_num = (buffer[index] << 8) | (buffer[index+1]);
		index += 2;
		
//		if(ptrecord->event_num)
//		{
//			ptrecord->event_descrip = (warn_event *) my_malloc(ptrecord->event_num * sizeof(warn_event));
//			
//			for(i=0;i<ptrecord->event_num;i++)
//			{
//				ptrecord->event_descrip[i].warn_data = (buffer[index] << 8) | (buffer[index+1]);
//				index += 2;
//				ptrecord->event_descrip[i].color = (buffer[index] << 8) | (buffer[index+1]);
//				index += 2;
//				ptrecord->event_descrip[i].str_id = (buffer[index] << 8) | (buffer[index+1]);
//				index += 2;
//			}
//		}
		
		index += ptrecord->event_num * 6;
	}


    

    


    mem_addr = (ptrecord->mem_addr[0] << 24) | (ptrecord->mem_addr[1] << 16) | (ptrecord->mem_addr[2] << 8) | 
    (ptrecord->mem_addr[3]);
    
    if(mem_addr)
    {
        ptrecord->pinfo_record = (record_info *)mem_addr;
    
    }
    else
    {
        
        if(ptrecord->record_type < 3)
        {

            size = ptrecord->max_record * sizeof(event_time) + sizeof(record_info);
            
        }
        else
        {
            size = ptrecord->max_record * (ptrecord->record_byte + sizeof(str_info)) + sizeof(record_info);
        }

        ptrecord->pinfo_record = (record_info *)my_malloc(size);
        memset((void *)ptrecord->pinfo_record,0,size);
        mem_addr = (uint32_t)ptrecord->pinfo_record;

        if(ptrecord->record_type < 3)
        {
            ptrecord->pinfo_record->event_wran = (event_time *)((uint8_t *) ptrecord->pinfo_record + sizeof(record_info));
        }
        else
        {
            ptrecord->pinfo_record->event_str = (str_info *)((uint8_t *) ptrecord->pinfo_record + sizeof(record_info));

            for(i=0;i<ptrecord->max_record;i++)
            {
                ptrecord->pinfo_record->event_str[i].p = (uint8_t *)((uint8_t *)ptrecord->pinfo_record + ptrecord->max_record * sizeof(str_info) + 
                i*ptrecord->record_byte + sizeof(record_info));
            }
        }

        ptrecord->pinfo_record->first_show = 0;
        ptrecord->pinfo_record->record_index = 0;
        ptrecord->pinfo_record->select_record = 0xFFFF;
        ptrecord->pinfo_record->record_num = 0;

        ptrecord->mem_addr[0] = mem_addr >> 24;
        ptrecord->mem_addr[1] = mem_addr >> 16;
        ptrecord->mem_addr[2] = mem_addr >> 8;
        ptrecord->mem_addr[3] = mem_addr;

        
    }

//    if(ptrecord->select)
//    {
//        uint16_t record_num;
//         
//        record_num = ptrecord->pinfo_record->record_num;
//        ptrecord->select_rect = ptrecord->tp_rect;
//        if(record_num <= ptrecord->show_record)
//        {
//            ptrecord->select_rect.bottom = ptrecord->select_rect.top + (ptrecord->tp_rect.bottom - 
//            ptrecord->tp_rect.top) * record_num / ptrecord->show_record;   
//        }
//        else if(ptrecord->show_slider)
//        {
//            ptrecord->select_rect.right -= SLIDER_WIDTH;
//        
//        }
//        
//    }

    


//    if(ptrecord->show_slider)
//    {
//        ptrecord->slider.timer = xTimerCreate((const char *) "record",
//								(TickType_t)50*MS,
//								(UBaseType_t)pdTRUE,
//								(void*	)ptrecord,
//								(TimerCallbackFunction_t)record_touch_back);
//								
//		ptrecord->slider.rect.right = ptrecord->tp_rect.right;
//	    ptrecord->slider.rect.left = ptrecord->tp_rect.right - SLIDER_WIDTH;
//		ptrecord->slider.color = ptrecord->grid_color;

//	    if(ptrecord->show_record < ptrecord->pinfo_record->record_num)
//	    {
//	        uint16_t h;
//	        ptrecord->slider.flag = 1; //标志要绘制滑动条了
//	        h = ptrecord->show_record * (ptrecord->tp_rect.bottom - ptrecord->tp_rect.top) / ptrecord->pinfo_record->record_num;
//	        if(h < MIN_SLI_HEIGHT)
//	        {
//	            h = MIN_SLI_HEIGHT;
//	        }
//			ptrecord->slider.rect.top = ptrecord->tp_rect.top;
//			ptrecord->slider.rect.bottom = ptrecord->slider.rect.top + h;
//			ptrecord->slider.pos = (ptrecord->slider.rect.top + ptrecord->slider.rect.bottom) / 2;
//			ptrecord->slider.old_pos = ptrecord->slider.pos;
//			
//			if(ptrecord->pinfo_record->first_show)
//			{
//				first_count_pos(ptrecord);
//			}

//	        
//	        
//	        
//	        
//	    }
//    }
	
	ctrl_init((t_ctrl *) ptrecord);
	
	return (t_ctrl *) ptrecord;
}





/***************************************************************************************
* End of File: record.c

****************************************************************************************/
