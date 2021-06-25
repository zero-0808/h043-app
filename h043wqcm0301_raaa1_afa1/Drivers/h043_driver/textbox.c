
/***************************************************************************************
****************************************************************************************
* Project      : 20180525_HMI
* FILE         : textbox.c
* Description  : 
*                      
* Copyright (c) 2018 by ShenZhen JingHua Displays Co.,Ltd... All Rights Reserved.
* 
* History:
* Version		name			Date			Description
* 0.01			Qu Bingbing		2018/05/28		Initial Version
* 
****************************************************************************************
****************************************************************************************/

#include "textbox.h"
#include "mem_my.h"
#include "string.h"
#include "cfg.h"
#include "cmd.h"
#include "display.h"
#include "keyboard.h"


textbox * active_box = NULL;
textbox * old_active_box = NULL;
uint8_t shift_value;
uint8_t engch_value;


static uint16_t press_x,press_y;




static uint8_t textbox_msg( void * p, t_msg * p_msg )
{
    uint16_t x,y,flag,f=1;
    
	t_msg msg;
    
    if(p_msg->msg != MSG_TP)
    {
        return f;
    }
    textbox * pbox = (textbox *) p;
	
	if(pbox->input_way == 0 || pbox->input_way == 3 || pbox->ctrl.active == 0)
	{
		return f;
	}

	/*
	if(pbox->pic_id == small_keyboard.screen || pbox->pic_id == full_keyboard.screen)
	{
		return f;
	}
	*/
    x = p_msg->a;
    y = p_msg->b;
    flag = p_msg->c;
    if((flag != 1) && !GETBIT( pbox->state, BTN_STATE_ACTIVE))
	{
		return f;
	}

    if(pt_in_rect(&pbox->tp_rect, x, y))
	{
	    f = 0;
	}

	switch(flag)
	{
	    case 1:
	    {
	        if(!f)
	        {
	            SETBIT( pbox->state, BTN_STATE_ACTIVE);
	            //press_x = x;
	            //press_y = y;
			    msg.msg = MSG_TOUCH;
			    msg.a   = (pbox->ctrl.id >> 16) & (0xffff);
			    msg.b   = (pbox->ctrl.id) & (0xffff);
			    msg.c   = 1;
			    msg_dev.write_message((void *)&msg,0);
			    
	        }
	        break;
	    }
	    case 2:
	    {
	     //msg.msg = MSG_TOUCH;
		   //msg.a   = (pbox->ctrl.id >> 16) & (0xffff);
		   //msg.b   = (pbox->ctrl.id) & (0xffff);
		   //msg.c   = 2;
		   //msg_dev.write_message((void *)&msg,0); 
		   break;
	    }
	    case 0:
	    {
           msg.msg = MSG_TOUCH;
		   msg.a   = (pbox->ctrl.id >> 16) & (0xffff);
		   msg.b   = (pbox->ctrl.id) & (0xffff);
		   msg.c   = 0;
		   press_x = x;
		   press_y = y;
		   msg_dev.write_message((void *)&msg,0); 
	       CLRBIT( pbox->state, BTN_STATE_ACTIVE);
	      break;
	    }
	    
	}
    
    return f;
}



static void textbox_release( void* pCtrl )
{
    textbox * pbox = (textbox *) pCtrl;

	
	if(active_box == pbox)
	{
		//cursor_dev.close();
		if(pbox->timer)
		{
		    xTimerStop(pbox->timer,3);
		 }
		active_box = (textbox *)NULL;
	}
		
	if(pbox->text_show == 0)
	{
		if(pbox->text.font.back_image.p)
		{
			my_free((void *)pbox->text.font.back_image.p);
		}
	}
	else if(pbox->text_show == 1)
	{
	    if(pbox->text.art.art_icon.p)
	    {
	        my_free(pbox->text.art.art_icon.p);
	    }
	}
	
	if(pbox->pdraw)
	{
		my_free(pbox->pdraw);
	}

	if(pbox->timer)
	{
        xTimerDelete(pbox->timer,3);
	}

    my_free((void *) pbox);
	
	
}


static void textbox_cursor_timer(TimerHandle_t timer)
{
	
	textbox * pbox = pvTimerGetTimerID(timer);
	if(pbox->cursor_flag)
	{
	    pbox->cursor_flag = 0;
	}
	else
	{
	    pbox->cursor_flag = 1;
	}

	pbox->screen_update = 1;


}





static void textbox_act(base_touch *p,uint8_t flag)
{
    if(p->type != CFG_TEX)
    {
        return ;
    }

    //uint16_t x,y,h;


    textbox * pbox = (textbox *) p;
    

    if(flag == 0)
    {
        if(pbox->input_way == 1)
        {
            old_active_box = pbox;
            old_active_box->status = 1;
			if(old_active_box->text_show == 0)
			{
				if(old_active_box->text.font.back_image_type == 3)
				{
					old_active_box->code_update = 1;
					old_active_box->screen_update = 1;
				}
			}

		

			if(pbox->input.keyboard.key_type == 0)
			{
			    
//			    if(press_x + small_keyboard.width > disp_dev.width)
//			    {
//			        press_x = disp_dev.width - small_keyboard.width;
//			    }
//			    if(press_y + small_keyboard.height > disp_dev.height)
//			    {
//			        press_y = disp_dev.height - small_keyboard.height;
//			    }
			    kb_info.keyboard_num = 1;
			}
			else
			{
//			    press_x = (disp_dev.width - full_keyboard.image.width) / 2;
//			    press_y = (disp_dev.height - full_keyboard.image.height) / 2;
			    kb_info.keyboard_num = 2;
			    
			}

			kb_info.status = 1;
			kb_info.x = press_x;
		    kb_info.y = press_y;
		    xSemaphoreGive(kb_info.sem);

            
        }
        if(pbox->input_way == 2)
        {
			if(active_box)
			{	
				active_box->status = 0;

				if(active_box->text_show == 0)
				{
				    if(active_box->text.font.back_image_type == 3)
				    {
					    active_box->code_update = 1;
					    active_box->screen_update = 1;
				    }
				}

				if(active_box->timer)
				{
				    xTimerStop(active_box->timer,3);
				}
			}
            active_box = pbox;
            active_box->status = 1;
			
			if(active_box->text_show == 0)
			{
				if(active_box->text.font.back_image_type == 3)
				{
					active_box->code_update = 1;
					active_box->screen_update = 1;
				}

				//x = pbox->box_info.x;
                //y = pbox->box_info.y;
                //h = pbox->box_info.font_height;
                active_box->cursor_flag = 1;
			    if(pbox->timer)
			    {
			        pbox->cursor_flag = 1;
			        xTimerReset(pbox->timer,3);
			        xTimerStart(pbox->timer,3);
			    }
			    /*
			    cursor_dev.set_color(active_box->text.font.fore_color);
			    if(pbox->pic_id == small_keyboard.screen || pbox->pic_id == full_keyboard.screen)
			    {
				    cursor_dev.switch_box(x,y,h,0);
			    }
			    else
			    {
				    cursor_dev.switch_box(x,y,h,1);
			    }
			    */
			}
			//update_box_info(pbox);
            	
			
        }
    }

}




static void cmd_update_box_status(textbox * pbox,uint8_t * p,uint16_t len)
{
    uint8_t len1;
    
    if(pbox->tp_type == CFG_TEX)
    {
        

//        if(pbox->input_way != 0)
//        {
//            return;
//        }
//        
        if(len < pbox->maxmum_code)
        {
            len1 = len;
        }
        else
        {
            len1 = pbox->maxmum_code;
        }

        memcpy((void *)pbox->code,(const void *)p,len1);

        pbox->code_num = len1;
        *(pbox->pcode_addr) &= 0X00FFFFFF;
        *(pbox->pcode_addr) |= (pbox->code_num << 24);

        pbox->code_update = 1;
        pbox->screen_update = 1;

        
    }
}




static void cmd_get_box_status(textbox *pbox)
{
    if(pbox->tp_type == CFG_TEX)
    {
        uint8_t * buf = NULL;
        uint8_t len = pbox->code_num;

        buf = my_malloc(len + 13);

        buf[0] = 0xEE;
        buf[1] = 0xB1;
        buf[2] = 0x11;
        buf[3] = pbox->ctrl.id >> 24;
        buf[4] = pbox->ctrl.id >> 16;
        buf[5] = pbox->ctrl.id >> 8;
        buf[6] = (pbox->ctrl.id) & 0xFF;
        buf[7] = pbox->tp_type;
        memcpy(&buf[8],pbox->code,len);
        buf[len+8] = 0xFF;
        buf[len+9] = 0xFC;
        buf[len+10] = 0xFF;
        buf[len+11] = 0xFF;
        buf[len+12] = 0x3B;
        cmd_dev.notify_cmd(buf,len+13);
        my_free(buf);
    }
}





static uint16_t text_font_create(uint8_t * buffer,t_rect tp,text_type * p)
{
  
    p->font.ptfont = get_font_type(buffer[0]);
    p->font.fore_color = ((buffer[1] << 8) | buffer[2]);
    p->font.back_image_type = buffer[3];
    p->font.back_color = ((buffer[4] << 8) | buffer[5]);
    p->font.password = buffer[6];
    p->font.frame = buffer[7];
    p->font.h_align = buffer[8];
    p->font.v_align = buffer[9];
    p->font.line_space = buffer[10];
    p->font.column_space = buffer[11];
    
    //p->font.image.x = tp.left;
    //p->font.image.y = tp.top;
    //p->font.image.width = tp.right - tp.left + 1;
    //p->font.image.height = tp.bottom - tp.top + 1;
    //p->font.image.vdc = disp_dev.disp[2];
	
	p->font.back_image.p = NULL;
	
	if(p->font.back_image_type == 3)
	{
//		p->font.back_image.x = tp.left;
//		p->font.back_image.y = tp.top;
		p->font.back_image.width = tp.right - tp.left + 1;
		p->font.back_image.height = tp.bottom - tp.top + 1;
		p->font.back_image.p = (uint8_t *)my_malloc(p->font.back_image.width * p->font.back_image.height * disp_dev.bpp);
	}
    

    return 12;
    
}



static uint16_t text_art_creat(uint8_t * buffer,t_rect tp,text_type * p)
{
    uint32_t addr;
    addr = (buffer[0]<<24) | (buffer[1]<<16) | (buffer[2]<<8) | (buffer[3]);
    init_icon(&(p->art.art_icon), addr);
    p->art.wdec  = buffer[4];
    p->art.frame = buffer[5];
    p->art.h_align = buffer[6];
    p->art.v_align = buffer[7];
    p->art.line_space = buffer[8];
    p->art.column_space = buffer[9];
    return 8;
}





static void update_draw_text(textbox * pbox)
{
	if(pbox->pdraw == NULL)
	{
		return;
	}
    
    pbox->pdraw->line = 0;

    if(pbox->code_num == 0)
    {
        return;
    }

    if(pbox->pdraw->line_num == 0 || pbox->pdraw->col_num == 0)
    {
        return;
    }

    if(pbox->text_show == 0)
   { 
        uint16_t w,width;
        uint8_t line,i,j,k;
        uint8_t * code = pbox->code;
    
        font_type * ptfont = pbox->text.font.ptfont;

        if(ptfont->code == 0)
        {
            w = ptfont->width;
        }
        else
        {
            w = ptfont->width / 2;
        }

        line = pbox->pdraw->line_num;
        width = pbox->tp_rect.right - pbox->tp_rect.left + 1;
        pbox->pdraw->pline[0].width = 0;
       

        pbox->pdraw->line = 1;

        if(ptfont->code < 3)
        {

            for(j=0,i=0,k=0;(j<pbox->code_num) && (i<line);j++)
            {
                if(pbox->text.font.password)
                {
                    pbox->pdraw->pline[i].pcode[k] = '*';
                    pbox->pdraw->pline[i].width +=  w + pbox->text.font.column_space;
                    k++;
                    if(pbox->pdraw->pline[i].width + w > width || (j + 1) == pbox->code_num)
                    {
                        pbox->pdraw->pline[i].width -= pbox->text.font.column_space;
    					pbox->pdraw->pline[i].len = k;
    					k = 0;
    					i++;
    					if(i < line)
    					{
    						pbox->pdraw->pline[i].width = 0;
    					}
                    }
                }
                else
                {
                    if(code[j] < 0x80)
                    {
                        if(code[j] != '\r')
                        {

                            pbox->pdraw->pline[i].pcode[k] = code[j];
                            pbox->pdraw->pline[i].width +=  w + pbox->text.font.column_space;
                            k++;
                            if(pbox->pdraw->pline[i].width + w > width || (j + 1) == pbox->code_num)
                            {
                                pbox->pdraw->pline[i].width -= pbox->text.font.column_space;
        						pbox->pdraw->pline[i].len = k;
        						k = 0;
        						i++;
        						if(i < line)
        						{
        							pbox->pdraw->pline[i].width = 0;
        						}
                            }
                        
                        }
                        else if((j + 1) < pbox->code_num)
                        {
                            if(code[j+1] == '\n')
                            {
                                j++;
                                pbox->pdraw->pline[i].len = k;
                                pbox->pdraw->pline[i].width -= pbox->text.font.column_space;
                                k = 0;
                                i++;
                                if(i < line)
        						{
        							pbox->pdraw->pline[i].width = 0;
        						}
                            }
                            else
                            {
                                pbox->pdraw->pline[i].pcode[k] = code[j];
                                pbox->pdraw->pline[i].width +=  w + pbox->text.font.column_space;
                                k++;
                                if(pbox->pdraw->pline[i].width + w > width)
                                {
                                    pbox->pdraw->pline[i].width -= pbox->text.font.column_space;
            						pbox->pdraw->pline[i].len = k;
            						k = 0;
            						i++;
            						if(i < line)
            						{
            							pbox->pdraw->pline[i].width = 0;
            						}
                                }
                            }
                        }
                        else
                        {
                            pbox->pdraw->pline[i].pcode[k] = code[j];
                            pbox->pdraw->pline[i].width +=  w + pbox->text.font.column_space;
                            k++;
                            if(pbox->pdraw->pline[i].width + w > width)
                            {
                                pbox->pdraw->pline[i].width -= pbox->text.font.column_space;
        						pbox->pdraw->pline[i].len = k;
        						k = 0;
        						i++;
        						if(i < line)
        						{
        							pbox->pdraw->pline[i].width = 0;
        						}
                            }
                        }
                    }
                    else
                    {
                        if((j + 1) < pbox->code_num)
                        {
                            if(pbox->pdraw->pline[i].width + 2*w <= width)
                            {
                                pbox->pdraw->pline[i].pcode[k] = code[j];
						        k++;
						        j++;
						        pbox->pdraw->pline[i].pcode[k] = code[j];
						        pbox->pdraw->pline[i].width += 2*w + pbox->text.font.column_space;
						        k++;
                                if(pbox->pdraw->pline[i].width + w > width || (j + 1) >= pbox->code_num)
        						{
        							pbox->pdraw->pline[i].len = k;
        							pbox->pdraw->pline[i].width -= pbox->text.font.column_space;
        							k = 0;
        							i++;
        							if(i < line)
        						    {
        					            pbox->pdraw->pline[i].width = 0;
        						    }
        						}
                            }
                            else
                            {
                                j--;
        						pbox->pdraw->pline[i].len = k;
        						pbox->pdraw->pline[i].width -= pbox->text.font.column_space;
        						k = 0;
        						i++;
        						if(i < line)
        						{
        					        pbox->pdraw->pline[i].width = 0;
        						}
                            }
                        }
                        else
                        {
                            pbox->pdraw->pline[i].pcode[k] = '?'; //无效编码
        					k++;
        					pbox->pdraw->pline[i].len = k;
        					pbox->pdraw->pline[i].width += w;
        					k = 0;
        					i++;
                        }
                    }
                }
            }
		    pbox->pdraw->line = i;
		}
		else //unicode
		{
		    uint16_t width;

		    width = pbox->tp_rect.right - pbox->tp_rect.left - 1;
		    pbox->pdraw->pline[0].width = 0;
		    for(j=0,i=0,k=0;(j<pbox->code_num) && (i<line);j++)
		    {
		        if(pbox->text.font.password)
                {
                    pbox->pdraw->pline[i].pcode[k] = 0;
					k++;
					j++;
					pbox->pdraw->pline[i].pcode[k] = '*';
					k++;
					pbox->pdraw->pline[i].width += w  + pbox->text.font.column_space;
					
                    if(pbox->pdraw->pline[i].width + w > width || (j+2) >= pbox->code_num)
					{
					    pbox->pdraw->pline[i].width -= pbox->text.font.column_space;
						pbox->pdraw->pline[i].len = k;
						k = 0;
						i++;
						if(i < line)
						{
							pbox->pdraw->pline[i].width = 0;
						}
					}
                }
                else
                {
//                    uint16_t unicode;
                    
                    if((code[j]==0) && (code[j+1]<0x80))
                    {
                        if(code[j+1] != '\r')
                        {
                            pbox->pdraw->pline[i].pcode[k] = code[j];
        					k++;
        					j++;
        					pbox->pdraw->pline[i].pcode[k] = code[j];
        					k++;
        					pbox->pdraw->pline[i].width += w  + pbox->text.font.column_space;
        					
                            if(pbox->pdraw->pline[i].width + w > width || (j+2) >= pbox->code_num)
        					{
        					    pbox->pdraw->pline[i].width -= pbox->text.font.column_space;
        						pbox->pdraw->pline[i].len = k;
        						k = 0;
        						i++;
        						if(i < line)
        						{
        							pbox->pdraw->pline[i].width = 0;
        						}
        					}
                        }
                        else if((j + 3) < pbox->code_num)
                        {
                            if(code[j+2]==0 && code[j+3]=='\n' )
                            {
                                j+=3;
        						pbox->pdraw->pline[i].width -= pbox->text.font.column_space;
        						pbox->pdraw->pline[i].len = k;
        						k = 0;
        						i++;
        						if(i < line)
        						{
        							pbox->pdraw->pline[i].width = 0;
        						}
                            }
                            else
                            {
                                pbox->pdraw->pline[i].pcode[k] = code[j];
        						k++;
        						j++;
        						pbox->pdraw->pline[i].pcode[k] = code[j];
        						k++;
        						pbox->pdraw->pline[i].width += w + pbox->text.font.column_space;

        						if(pbox->pdraw->pline[i].width + w > width || (j+2) >= pbox->code_num)
        						{
        						    pbox->pdraw->pline[i].width -= pbox->text.font.column_space;
        							pbox->pdraw->pline[i].len = k;
        							k = 0;
        							i++;
        							if(i < line)
        							{
        								pbox->pdraw->pline[i].width = 0;
        							}
        						}
                                
                            }
                        }
                        else
                        {
                            pbox->pdraw->pline[i].pcode[k] = code[j];
        					k++;
        					j++;
        					pbox->pdraw->pline[i].pcode[k] = code[j];
        					k++;
        					pbox->pdraw->pline[i].width += w + pbox->text.font.column_space;
                            
        					if(pbox->pdraw->pline[i].width + w > width || (j+2) >= pbox->code_num)
        					{
        					    pbox->pdraw->pline[i].width -= pbox->text.font.column_space;
        						pbox->pdraw->pline[i].len = k;
        						k = 0;
        						i++;
        						if(i < line)
        						{
        							pbox->pdraw->pline[i].width = 0;
        						}
        					}
                        }
                    }
                    else
                    {
                        if(pbox->pdraw->pline[i].width + 2*w <=  width)
                        {
                            pbox->pdraw->pline[i].pcode[k] = code[j];
        					k++;
        					j++;
        					pbox->pdraw->pline[i].pcode[k] = code[j];
        					k++;
        					pbox->pdraw->pline[i].width += 2*w + pbox->text.font.column_space;

        					if(pbox->pdraw->pline[i].width + w > width || (j+2) >= pbox->code_num)
        					{
        					    pbox->pdraw->pline[i].width -= pbox->text.font.column_space;
        						pbox->pdraw->pline[i].len = k;
        						k = 0;
        						i++;
        						if(i < line)
        						{
        							pbox->pdraw->pline[i].width = 0;
        						}
        					}
                        }
                        else
                        {
                            j--;
        					if(pbox->pdraw->pline[i].width)
        					{
        					    pbox->pdraw->pline[i].width -= pbox->text.font.column_space;
        					}
        					pbox->pdraw->pline[i].len = k;
        					k = 0;
        					i++;
        					if(i < line)
        					{
        						pbox->pdraw->pline[i].width = 0;
        					}
                        }
                    }
                }
		    }

		    pbox->pdraw->line = i;
		}
	
   }
   else
   {
        uint8_t line,i,j,k;
        uint16_t w,width;
        uint8_t * code = pbox->code;

        
        width = pbox->tp_rect.right - pbox->tp_rect.left + 1;
        
        if(pbox->text.art.wdec == 0)
        {
            w = pbox->text.art.art_icon.width;
        }
        else if(pbox->text.art.wdec == 1)
        {
            w = pbox->text.art.art_icon.width / 2;
        }
        else if(pbox->text.art.wdec == 2)
        {
            w = pbox->text.art.art_icon.width / 4;
        }
        
        line = pbox->pdraw->line_num;
       
		pbox->pdraw->pline[0].width = 0;
		
        for(i=0,j=0,k=0;(j<pbox->code_num) && (i<line);j++)
        {
            if(code[j] != '.')
            {
                if(code[j] == '\r' || j+1 < pbox->code_num)
                {
                    if(code[j] == '\n')
                    {
                        j++;
                        pbox->pdraw->pline[i].len = k;
                        k = 0;
                        i++;
						if(i+1 < line)
						{	
							pbox->pdraw->pline[i].width = 0;
						}
                        continue;
                    }
                }
                
				if(pbox->pdraw->pline[i].width + pbox->text.art.art_icon.width <=  width)
                {
					pbox->pdraw->pline[i].pcode[k] = code[j];
                    k++;
                    pbox->pdraw->pline[i].width += pbox->text.art.art_icon.width;
                    if(pbox->pdraw->pline[i].width + w > width || (j + 1) == pbox->code_num)
                    {
						pbox->pdraw->pline[i].len = k;
                        k = 0;
                        i++;
						if(i+1 < line)
						{
							pbox->pdraw->pline[i].width = 0;
						}
					}
                        
                }
				else
				{
					j--;
					pbox->pdraw->pline[i].len = k;
					k = 0;
					i++;
					if(i+1 < line)
					{
						pbox->pdraw->pline[i].width = 0;
					}
				}
                      
            }
            else if(code[j])
            {
                pbox->pdraw->pline[i].pcode[k] = code[j];
                k++;
                pbox->pdraw->pline[i].width += w;
                if(pbox->pdraw->pline[i].width + w > width || (j + 1) == pbox->code_num)
                {
                    pbox->pdraw->pline[i].len = k;
                    k = 0;
                    i++;
					if(i+1 < line)
					{
						pbox->pdraw->pline[i].width = 0;
					}
                }
                
            }
        }
		pbox->pdraw->line = i;
		
   }

    



    
}



//返回0XFFFF时说明是无效编码
static uint16_t get_art_num(uint8_t code)
{
    if(((code >= '0') && (code <= '9')) || ((code >= 'A') && (code <= 'Z')))
    {
        return (code - '0');
    }
    else if((code >= 'a') && (code <= 'z'))
    {
        return (code - 53);
    }
    else
    {
        switch(code)
        {
            case ':':
                return 10;
            case '.':
                return 11;
            case '*':
                return 12;
            case '-':
                return 13;
            case '+':
                return 14;
            case '?':
                return 15;
            case '/':
                return 16;
            default :
                return 0xffff;
        }
        
        
    }
}






static void init_draw_text(textbox * pbox)
{

    pbox->pdraw = NULL;
    uint32_t size;
    uint8_t i;

    if(pbox->text_show == 0)
    {
        font_type *ptfont;
        uint16_t w,h;
        uint8_t line,col;
        

        if(pbox->text.font.ptfont == NULL)
        {
            return;
        }
		
		ptfont = pbox->text.font.ptfont;

        if(ptfont->code == 0)
        {
            w = ptfont->width;
        }
        else
        {
            w = ptfont->width / 2;
        }

        h = get_font_height(*ptfont);

        //line = (pbox->tp_rect.bottom - pbox->tp_rect.top + 1) / h;
        //col = (pbox->tp_rect.right - pbox->tp_rect.left +  1) / w;

        line = (pbox->tp_rect.bottom - pbox->tp_rect.top + 1) / (h + pbox->text.font.line_space);
	    col = (pbox->tp_rect.right - pbox->tp_rect.left +  1) / (w);

	    if((pbox->tp_rect.bottom - pbox->tp_rect.top + 1) % (h + pbox->text.font.line_space) > h)
	    {
	        line++;
	    }
        
        if(col == 0 || line == 0)
        {
            return;
        }

        if(ptfont->code == 3)
        {
            col = 2 * col;
        }

        size = sizeof(draw_text) + line * (sizeof(line_text) + col);

        pbox->pdraw = (draw_text *) my_malloc(size);
        
        pbox->pdraw->line_num = line;
        pbox->pdraw->col_num = col;

        pbox->pdraw->pline = (line_text *) ((uint8_t *) pbox->pdraw + sizeof(draw_text)); 

        for(i = 0; i < line ; i++)
        {
            pbox->pdraw->pline[i].pcode = (uint8_t *) ((uint8_t *) pbox->pdraw + sizeof(draw_text) + line * sizeof(line_text) + i * col);
        }
        
    }
    else
    {
        uint16_t w,h;
        uint8_t line,col;
        
        w = pbox->text.art.art_icon.width;
        h = pbox->text.art.art_icon.height;

        line = (pbox->tp_rect.bottom - pbox->tp_rect.top + 1) / h;

        if(pbox->text.art.wdec == 1)
        {
            col = (pbox->tp_rect.right -pbox->tp_rect.left +  1) / (w/2);   
        }
        else if(pbox->text.art.wdec == 2)
        {
            col = (pbox->tp_rect.right -pbox->tp_rect.left +  1) / (w/4);
        }
        else
        {
            col = (pbox->tp_rect.right -pbox->tp_rect.left +  1) / w;
        }

        if(line==0 || col==0)
        {
            return;
        }

        size = sizeof(draw_text) + line * (sizeof(line_text) + col);

        pbox->pdraw = (draw_text *) my_malloc(size);
        
        pbox->pdraw->line_num = line;
        pbox->pdraw->col_num = col;

        pbox->pdraw->pline = (line_text *) ((uint8_t *) pbox->pdraw + sizeof(draw_text)); 

        for(i = 0; i < line ; i++)
        {
            pbox->pdraw->pline[i].pcode = (uint8_t *) ((uint8_t *) pbox->pdraw + sizeof(draw_text) + line * sizeof(line_text) + i * col);
        }
        
    }
     
}


static void cursor_draw_fore(uint16_t x,uint16_t y,uint16_t h,uint16_t color)
{
    uint16_t * buff;
    uint16_t i;

    if((h + y) > disp_dev.height)
    {
        h = disp_dev.height - y;
    }


    if(disp_dev.dir == 0)
    {
    
        if(disp_dev.flag)
        {
            buff = (uint16_t *) disp_dev.disp[0].p;
        }
        else
        {
            buff = (uint16_t *) disp_dev.disp[1].p;
        }
     }
     else if(disp_dev.dir == 1)
     {
        buff = (uint16_t *) disp_dev.disp[3].p;
     }

    for(i=0;i<h;i++)
    {
        buff[(y+i)*disp_dev.width + x] = color;
    }

}


static void textbox_draw_screen(base_touch * p)
{
    if(p->type == CFG_TEX)
    {
        uint8_t *code,*buf;
        uint8_t  line,code_index,i;
        textbox * pbox = (textbox *) p;
        
     

        if(pbox->text_show == 0)
        {
           font_type * ch_font, *hz_font,* ptfont;
           uint16_t width,height,x,y,color;
           uint16_t hz;
           uint16_t htemp;
			
			if(pbox->text.font.ptfont == NULL)
			{
				return;
			}
			
			ptfont = pbox->text.font.ptfont;
           
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

           
           update_draw_text(pbox);
           color = pbox->text.font.fore_color;
           if(pbox->text.font.back_image_type == 3)
           {
                if((active_box == pbox || old_active_box == pbox) && (pbox->status) )
                {
                    uint16_t x,y;
                    t_rect rect;
                    dc ddc;

                    x = pbox->tp_rect.left;
                    y = pbox->tp_rect.top;
                    rect.left = 0;
                    rect.top = 0;
                    rect.right = pbox->text.font.back_image.width;
                    rect.bottom = pbox->text.font.back_image.height;

                    if(disp_dev.dir == 0)
                    {

                        if(disp_dev.flag)
                        {
                            ddc = disp_dev.disp[0];
                        }
                        else
                        {
                            ddc = disp_dev.disp[1];

                        }
                    }

                     vblt(ddc,x,y,rect,pbox->text.font.back_image);
                }


           }
           line = pbox->pdraw->line;
		   if(pbox->pdraw == NULL)
		   {
			   line = 0;
		   }
           htemp = line * (height + pbox->text.font.line_space);
           if(htemp > 0)
           {
                htemp -= pbox->text.font.line_space;
           }

           if(pbox->text.font.v_align == 0)
           {
                y = pbox->tp_rect.top;
           }
           else if(pbox->text.font.v_align == 1)
           {
                y = pbox->tp_rect.top + (pbox->tp_rect.bottom - pbox->tp_rect.top + 1 - htemp) / 2;
           }
           else
           {
                y = pbox->tp_rect.bottom - htemp + 1;
           }
           
           for(i = 0;i<line;i++)
           {    
                if(pbox->text.font.h_align == 0)
                {
                    x = pbox->tp_rect.left;
                }
                else if(pbox->text.font.h_align == 1)
                {
                    x = pbox->tp_rect.left + (pbox->tp_rect.right - pbox->tp_rect.left + 1 - 
                    pbox->pdraw->pline[i].width) / 2;
                }
                else
                {
                    x = pbox->tp_rect.right - pbox->pdraw->pline[i].width + 1;
                }

                code = pbox->pdraw->pline[i].pcode;

                if(ptfont->code < 3)
                {
                    for(code_index=0;code_index<pbox->pdraw->pline[i].len;code_index++)
                    {
                        if(code[code_index] < 0x80)
                        {
                            show_charn_font(x,y,color, code[code_index],*ch_font);
                            x += width + pbox->text.font.column_space;
                        }
                        else
                        {
                            hz = code[code_index] << 8 | code[code_index + 1];
						    show_nfont(x,y,color, hz,*hz_font);
						    code_index++;
                            x += 2*width + pbox->text.font.column_space;
                        }

                        
                    }

                }
                else
                {
                    for(code_index=0;code_index<pbox->pdraw->pline[i].len;code_index++)
                    {
                        if(code[code_index] == 0 && code[code_index+1] < 0x80)
                        {
                            show_charn_font(x,y,color, code[code_index + 1],*ch_font);
                            x += width + pbox->text.font.column_space;
                        }
                        else
                        {
                            hz = code[code_index] << 8 | code[code_index + 1];
						    show_nfont(x,y,color, hz,*hz_font);
                            x += 2*width + pbox->text.font.column_space;
                        }
                        code_index++;
                    }
                }

                if(i+1 < line)
                {
                    y += height + pbox->text.font.line_space;
                }
        
           }
			if(line == 0)
			{
				if(pbox->text.font.v_align == 0)
				{
					y = pbox->tp_rect.top;
				}
				else if(pbox->text.font.v_align == 1)
				{
					y = pbox->tp_rect.top + (pbox->tp_rect.bottom - pbox->tp_rect.top + 1 - height) / 2;
				}
				else
				{
					y = pbox->tp_rect.bottom - height + 1;
				}
				
				if(pbox->text.font.h_align == 0)
                {
                    x = pbox->tp_rect.left;
                }
                else if(pbox->text.font.h_align == 1)
                {
                    x = pbox->tp_rect.left + (pbox->tp_rect.right - pbox->tp_rect.left + 1) / 2;
                }
                else
                {
                    x = pbox->tp_rect.right;
                }

			}
           if(pbox->code_update)
           {
                pbox->code_update = 0;
                if(pbox == active_box && pbox->status)
                {
					if(x > pbox->tp_rect.right)
					{
						x = pbox->tp_rect.right;
					}
                    cursor_draw_fore(x,y,height,color);
                    xTimerReset(pbox->timer,3);
                    pbox->cursor_flag = 1;
                }

                
           }
           else
           {
                if(pbox == active_box && pbox->status)
                {
                    if(pbox->cursor_flag)
                    {
						if(x > pbox->tp_rect.right)
						{
							x = pbox->tp_rect.right;
						}
                        cursor_draw_fore(x,y,height,color);
                    }
                }
           }
           
        }
        if(pbox->text_show == 1)  //艺术字体
        {
            uint8_t i,line,*code,index;
            uint16_t cnt,h,w,htemp,x,y,wtemp;
            t_rect rect;
            
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
            else if(disp_dev.dir == 1)
            {
                buf = disp_dev.disp[3].p;
            }

            h = pbox->text.art.art_icon.height;
            if(pbox->text.art.wdec == 2)
            {
                w = pbox->text.art.art_icon.width / 4;
               
            }
            else if(pbox->text.art.wdec == 1)
            {
                w = pbox->text.art.art_icon.width / 2;
            }
            else
            {
                w = pbox->text.art.art_icon.width;
            }
			
			

            update_draw_text(pbox);

            line = pbox->pdraw->line;
			if(pbox->pdraw == NULL)
			{
				line = 0;
			}
            htemp = line * h;
			
			

            if(pbox->text.art.v_align == 0)
            {
                y = pbox->tp_rect.top;
            }
            else if(pbox->text.art.v_align == 1)
            {
                y = pbox->tp_rect.top + (pbox->tp_rect.bottom - pbox->tp_rect.top + 1 - htemp) / 2;
            }
            else 
            {
                y = pbox->tp_rect.bottom + 1 - htemp;
            }

            for(i=0;i<line;i++)
            {
                
                if(pbox->text.art.h_align == 0)
                {
                    x = pbox->tp_rect.left;
                }
                else if(pbox->text.art.h_align == 1)
                {
                    x = pbox->tp_rect.left + (pbox->tp_rect.right - pbox->tp_rect.left + 1 - pbox->pdraw->pline[i].width) / 2;
                }
                else
                {
                    x = pbox->tp_rect.bottom + 1 - pbox->pdraw->pline[i].width;
                }

                code = pbox->pdraw->pline[i].pcode;

                for(index = 0;index < pbox->pdraw->pline[i].len;index++)
                {
                    if(code[index] == '.')
                    {
                        wtemp = w;
                    }
                    else
                    {
                        wtemp = pbox->text.art.art_icon.width;
                    }

                    cnt = get_art_num(code[index]);

                    if(cnt != 0xffff)  //艺术字体的有效编码
                    {
                        if(get_icon_image(&(pbox->text.art.art_icon),cnt) == 0)
                        {
                            rect.left = x;
                            rect.right = rect.left + wtemp - 1;
                            rect.top = y;
                            rect.bottom = rect.top + h - 1; 
                            show_icon_image_mix(pbox->text.art.art_icon,rect,buf);
                         }
                    }

                    x += wtemp;
                    
                }
				
				y += h;
               
            }
        }
    }
}









uint8_t textbox_create(uint8_t * buffer)
{
    uint16_t x,temp;
    uint32_t addr;
    
    
    textbox * pbox = (textbox *) my_malloc(sizeof(textbox));
    
    if(pbox == NULL)
    {
        return 1;
    }
    memset((void *)pbox,0,sizeof(pbox));

    pbox->ctrl.id = ((buffer[0]<<24)|(buffer[1]<<16)|(buffer[4]<<8)|(buffer[5])<<0);
    pbox->ctrl.func_ctrl_procmsg = textbox_msg;
    pbox->ctrl.func_ctrl_release = textbox_release;
   

    pbox->tp_type = buffer[6];
    pbox->tp_rect.left = (buffer[7]<<8) | (buffer[8]);
    pbox->tp_rect.top = (buffer[9]<<8) | (buffer[10]);
    pbox->tp_rect.right = (buffer[11]<<8) | (buffer[12]);
    pbox->tp_rect.bottom = (buffer[13]<<8) | (buffer[14]);
    pbox->pic_id = (buffer[2]<<8) | (buffer[3]);
    pbox->pic_on = (buffer[15]<<8) | (buffer[16]);
	

    pbox->act = textbox_act;
    pbox->draw_screen = textbox_draw_screen;
	pbox->cmd_update_status = cmd_update_box_status;
    //pbox->update_screen = textbox_update_screen;
    pbox->screen_update = 0;
    pbox->cmd_get_status = cmd_get_box_status;

    pbox->text_show = buffer[17];
    
    if(pbox->text_show == 0)
    {
        x = text_font_create(&buffer[18],pbox->tp_rect, &(pbox->text));
		if(pbox->text.font.back_image.p)
		{
			//taskENTER_CRITICAL(); 
            get_touch_image(pbox->text.font.back_image, (base_touch *) pbox, 0);
            //taskEXIT_CRITICAL();
		}
    }
    else
    {
        x = text_art_creat(&buffer[18],pbox->tp_rect ,&(pbox->text));
    }

    x += 18;

    pbox->input_way = buffer[x];
    if(pbox->input_way == 1)
    {
		int temp;
		
        pbox->input.keyboard.key_type = buffer[x+1];
        pbox->input.keyboard.init_value = buffer[x+2];
        pbox->input.keyboard.limit = buffer[x+3];
        pbox->input.keyboard.decimal_num = buffer[x+4];
		
        temp = (buffer[x+5]<<24)|(buffer[x+6]<<16)|(buffer[x+7]<<8)|(buffer[x+8]);
        pbox->input.keyboard.max_value.integer = temp;
        memset(pbox->input.keyboard.max_value.decimal,'0',10);
        pbox->input.keyboard.max_value.symboy = '+';
        if(temp < 0)
        {
            pbox->input.keyboard.max_value.symboy = '-';
            pbox->input.keyboard.max_value.integer = -temp;
        }
        
		temp = (buffer[x+9]<<24)|(buffer[x+10]<<16)|(buffer[x+11]<<8)|(buffer[x+12]);
        pbox->input.keyboard.min_value.integer = temp;
        memset(pbox->input.keyboard.min_value.decimal,'0',10);
        pbox->input.keyboard.min_value.symboy = '+';
        if(temp < 0)
        {
            pbox->input.keyboard.min_value.symboy = '-';
            pbox->input.keyboard.min_value.integer = -temp;
        }
        
        x += 12;
    }
    else if(pbox->input_way == 3)
    {
        pbox->input.pram = buffer[x+1];
        x += 1;
    }
	
    pbox->maxmum_code = buffer[x+1];
    pbox->code_num = buffer[x+2];
    temp = x+3+pbox->code_num;
    pbox->pcode_addr = (uint32_t *)&buffer[temp];
    addr = (buffer[temp] << 24)|(buffer[temp+1] << 16) |(buffer[temp+2] << 8) | (buffer[temp+3]);
    if(*(pbox->pcode_addr) == 0XFFFFFFFF)
    {
       pbox->code = &src_cfg.p_text[src_cfg.text_index];
       memcpy((void *)pbox->code,&buffer[x+3],pbox->code_num);
       *pbox->pcode_addr = src_cfg.text_index;
       *pbox->pcode_addr &= ~(0XFF000000);
       *pbox->pcode_addr |= (pbox->code_num << 24);
		src_cfg.text_index += pbox->maxmum_code;
    }
    else
    {
       addr = *(pbox->pcode_addr)&0x00ffffff;
       pbox->code = &src_cfg.p_text[addr];
       pbox->code_num = (*(pbox->pcode_addr) >> 24);
       
    }
	
	pbox->timer = NULL;

   if(pbox->input_way == 2 && pbox->text_show == 0)
   {
        pbox->timer =  xTimerCreate((const char *) "textbox",
					    (TickType_t)500,
						(UBaseType_t)pdTRUE,
						(void*	)pbox,
						(TimerCallbackFunction_t)textbox_cursor_timer);
    }
    pbox->code_update = 0;
    pbox->state = 0;
	pbox->status = 0;
    init_draw_text(pbox);
	ctrl_init((t_ctrl*) pbox);

    return 0;
    
}






uint8_t stextbox_create(uint8_t * buffer)
{
    uint16_t x,temp;
    uint32_t addr;
    
    
    textbox * pbox = (textbox *) my_malloc(sizeof(textbox));
    if(pbox == NULL)
    {
        return 1;
    }
    memset((void *)pbox,0,sizeof(pbox));

    pbox->ctrl.id = ((buffer[0]<<24)|(buffer[1]<<16)|(buffer[4]<<8)|(buffer[5])<<0);
    //pbox->ctrl.func_ctrl_procmsg = textbox_msg;
    pbox->ctrl.func_ctrl_release = textbox_release;
   

    pbox->tp_type = buffer[6];
    //pbox->tp_rect.left = (buffer[7]<<8) | (buffer[8]);
    //pbox->tp_rect.top = (buffer[9]<<8) | (buffer[10]);
    //pbox->tp_rect.right = (buffer[11]<<8) | (buffer[12]);
    //pbox->tp_rect.bottom = (buffer[13]<<8) | (buffer[14]);
    //pbox->pic_id = (buffer[2]<<8) | (buffer[3]);
    //pbox->pic_on = (buffer[15]<<8) | (buffer[16]);
	

    //pbox->act = textbox_act;
    //pbox->draw_screen = textbox_draw_screen;
	pbox->cmd_update_status = cmd_update_box_status;
    //pbox->update_screen = textbox_update_screen;
    pbox->screen_update = 0;
    pbox->cmd_get_status = cmd_get_box_status;

    pbox->text_show = buffer[17];
    
    if(pbox->text_show == 0)
    {
        x = text_font_create(&buffer[18],pbox->tp_rect, &(pbox->text));
		if(pbox->text.font.back_image.p)
		{
			//taskENTER_CRITICAL(); 
            get_touch_image(pbox->text.font.back_image, (base_touch *) pbox, 0);
            //taskEXIT_CRITICAL();
		}
    }
    else
    {
        x = text_art_creat(&buffer[18],pbox->tp_rect ,&(pbox->text));
    }

    x += 18;

    pbox->input_way = buffer[x];
    if(pbox->input_way == 1)
    {
		int temp;
		
        pbox->input.keyboard.key_type = buffer[x+1];
        pbox->input.keyboard.init_value = buffer[x+2];
        pbox->input.keyboard.limit = buffer[x+3];
        pbox->input.keyboard.decimal_num = buffer[x+4];
		
        temp = (buffer[x+5]<<24)|(buffer[x+6]<<16)|(buffer[x+7]<<8)|(buffer[x+8]);
        pbox->input.keyboard.max_value.integer = temp;
        memset(pbox->input.keyboard.max_value.decimal,'0',10);
        pbox->input.keyboard.max_value.symboy = '+';
        if(temp < 0)
        {
            pbox->input.keyboard.max_value.symboy = '-';
            pbox->input.keyboard.max_value.integer = -temp;
        }
        
		temp = (buffer[x+9]<<24)|(buffer[x+10]<<16)|(buffer[x+11]<<8)|(buffer[x+12]);
        pbox->input.keyboard.min_value.integer = temp;
        memset(pbox->input.keyboard.min_value.decimal,'0',10);
        pbox->input.keyboard.min_value.symboy = '+';
        if(temp < 0)
        {
            pbox->input.keyboard.min_value.symboy = '-';
            pbox->input.keyboard.min_value.integer = -temp;
        }
        
        x += 12;
    }
    else if(pbox->input_way == 3)
    {
        pbox->input.pram = buffer[x+1];
        x += 1;
    }
	
    pbox->maxmum_code = buffer[x+1];
    pbox->code_num = buffer[x+2];
    temp = x+3+pbox->code_num;
    pbox->pcode_addr = (uint32_t *)&buffer[temp];
    addr = (buffer[temp] << 24)|(buffer[temp+1] << 16) |(buffer[temp+2] << 8) | (buffer[temp+3]);
    if(*(pbox->pcode_addr) == 0XFFFFFFFF)
    {
       pbox->code = &src_cfg.p_text[src_cfg.text_index];
       memcpy((void *)pbox->code,&buffer[x+3],pbox->code_num);
       *pbox->pcode_addr = src_cfg.text_index;
       *pbox->pcode_addr &= ~(0XFF000000);
       *pbox->pcode_addr |= (pbox->code_num << 24);
		src_cfg.text_index += pbox->maxmum_code;
    }
    else
    {
       addr = *(pbox->pcode_addr)&0x00ffffff;
       pbox->code = &src_cfg.p_text[addr];
       pbox->code_num = (*(pbox->pcode_addr) >> 24);
       
    }
	
	pbox->timer = NULL;
	pbox->pdraw = NULL;

	/*

   if(pbox->input_way == 2 && pbox->text_show == 0)
   {
        pbox->timer =  xTimerCreate((const char *) "textbox",
					    (TickType_t)500,
						(UBaseType_t)pdTRUE,
						(void*	)pbox,
						(TimerCallbackFunction_t)textbox_cursor_timer);
    }
    */
    pbox->code_update = 0;
    pbox->state = 0;
	pbox->status = 0;
    //init_box_info(pbox);
	ctrl_init((t_ctrl*) pbox);

    return 0;
    
}










uint8_t keyboard_textbox_create(uint16_t a, uint16_t b,uint8_t * buffer)
{
    uint16_t x,temp;
    uint32_t addr;
    //uint16_t screen;
    
    textbox * pbox = (textbox *) my_malloc(sizeof(textbox));
    if(pbox == NULL)
    {
        return 1;
    }
    memset((void *)pbox,0,sizeof(pbox));

    pbox->ctrl.id = ((buffer[0]<<24)|(buffer[1]<<16)|(buffer[4]<<8)|(buffer[5])<<0);
    pbox->ctrl.func_ctrl_procmsg = textbox_msg;
    pbox->ctrl.func_ctrl_release = textbox_release;
	
    

    pbox->tp_type = buffer[6];
    pbox->tp_rect.left = (buffer[7]<<8) | (buffer[8]);
    pbox->tp_rect.top = (buffer[9]<<8) | (buffer[10]);
    pbox->tp_rect.right = (buffer[11]<<8) | (buffer[12]);
    pbox->tp_rect.bottom = (buffer[13]<<8) | (buffer[14]);
    pbox->pic_id = (buffer[2]<<8) | (buffer[3]);
    pbox->pic_on = (buffer[15]<<8) | (buffer[16]);
	

    pbox->act = textbox_act;
    pbox->draw_screen = textbox_draw_screen;
    //pbox->update_screen = textbox_update_screen;
    //pbox->cmd_update_status = cmd_update_box_status;

    pbox->text_show = buffer[17];
    
    if(pbox->text_show == 0)
    {
        //screen = pbox->pic_id;
        x = text_font_create(&buffer[18],pbox->tp_rect, &(pbox->text));
		if(pbox->text.font.back_image.p)
		{
			//taskENTER_CRITICAL(); 
            get_touch_image(pbox->text.font.back_image, (base_touch *) pbox, 0);
            //taskEXIT_CRITICAL();
		}
    }
    else
    {
        x = text_art_creat(&buffer[18], pbox->tp_rect,&(pbox->text));
    }

    pbox->tp_rect.left += a;
    pbox->tp_rect.right += a;
    pbox->tp_rect.top += b;
    pbox->tp_rect.bottom += b;

    x += 18;

    pbox->input_way = buffer[x];
    if(pbox->input_way == 1)
    {
		int temp;
		
        pbox->input.keyboard.key_type = buffer[x+1];
        pbox->input.keyboard.init_value = buffer[x+2];
        pbox->input.keyboard.limit = buffer[x+3];
        pbox->input.keyboard.decimal_num = buffer[x+4];
		if(pbox->input.keyboard.decimal_num > 10)
		{
			pbox->input.keyboard.decimal_num = 10;  //小数位数不得超过10
		}
        temp = (buffer[x+5]<<24)|(buffer[x+6]<<16)|(buffer[x+7]<<8)|(buffer[x+8]);
        pbox->input.keyboard.max_value.integer = temp; 
        memset(pbox->input.keyboard.max_value.decimal,'0',10);
        pbox->input.keyboard.max_value.symboy = '+';
        if(temp < 0)
        {
            pbox->input.keyboard.max_value.symboy = '-';
            pbox->input.keyboard.max_value.integer = -temp;
        }
        
		temp = (buffer[x+9]<<24)|(buffer[x+10]<<16)|(buffer[x+11]<<8)|(buffer[x+12]);
        pbox->input.keyboard.min_value.integer = temp;
        memset(pbox->input.keyboard.min_value.decimal,'0',10);
        pbox->input.keyboard.min_value.symboy = '+';
        if(temp < 0)
        {
            pbox->input.keyboard.min_value.symboy = '-';
            pbox->input.keyboard.min_value.integer = -temp;
        }
        
        x += 12;
    }
    else if(pbox->input_way == 3)
    {
        pbox->input.pram = buffer[x+1];
        x += 1;
    }

    pbox->maxmum_code = buffer[x+1];
	if(pbox->input_way == 1)
	{
		if(pbox->input.keyboard.limit)
		{
			pbox->maxmum_code = 0xff;  //如果有数值限定，那么文本将不受上位机所配置的编码数的限制
		}
	}
    pbox->code_num = buffer[x+2];
	
    temp = x+3+pbox->code_num;
    pbox->pcode_addr = (uint32_t *)&buffer[temp];
    addr = (buffer[temp] << 24)|(buffer[temp+1] << 16) |(buffer[temp+2] << 8) | (buffer[temp+3]);
    if(*(pbox->pcode_addr) == 0XFFFFFFFF)
    {
       pbox->code = &src_cfg.p_text[src_cfg.text_index];
       memcpy((void *)pbox->code,&buffer[x+3],pbox->code_num);
       *pbox->pcode_addr = src_cfg.text_index;
       *pbox->pcode_addr &= ~(0XFF000000);
       *pbox->pcode_addr |= (pbox->code_num << 24);
		src_cfg.text_index += pbox->maxmum_code;
    }
    else
    {
       addr = *(pbox->pcode_addr)&0x00ffffff;
       pbox->code = &src_cfg.p_text[addr];
       pbox->code_num = (*(pbox->pcode_addr) >> 24);
       
    }

    pbox->timer = NULL;

   if(pbox->input_way == 2 && pbox->text_show == 0)
   {
        pbox->timer =  xTimerCreate((const char *) "textbox",
					    (TickType_t)500,
						(UBaseType_t)pdTRUE,
						(void*	)pbox,
						(TimerCallbackFunction_t)textbox_cursor_timer);
    }
   
	
    pbox->code_num = 0;
    pbox->code_update = 0;
    pbox->state = 0;
	pbox->status = 0;
    init_draw_text(pbox);
	ctrl_init((t_ctrl*) pbox);
	

    return 0;
    
}




uint8_t find_decimal(uint8_t * str, uint8_t len)    //如果返回值大于len,表在字符串中没有找到‘.’
{
    uint8_t i;
	
    for(i=0;i<len;i++)
    {
        if(str[i] == '.')
        {
            return i;
        }
    }

    return i;
}


//返回值为0时，表示字符串可以转化成数字

uint8_t check_string_convert_date(uint8_t * str, uint8_t len) 
{
    
    uint8_t i;
    uint8_t decimal = 0;   //用来记录小数点的个数
    
    if(len > 0)
    {
        if((str[0]<'0' || str[0]>'9') && (str[0] != '+') && (str[0] != '-') && (str[0] != '.'))
        {
            return 1;
        }

        if(str[0] == '.')
        {
            decimal++;
        }

        for(i=1;i<len;i++)
        {
            if((str[i]<'0' || str[i]>'9') && (str[i]!='.'))
            {
                return 1;
            }
            if(str[i] == '.')
            {
                decimal++;
                if(decimal > 1)
                {
                    return 1;
                }
            }
        }
    }

    return 0;
}




float_type  str_to_date(uint8_t * str,uint8_t len)
{
    float_type date; 
    uint8_t res,num,i;
    uint32_t mul = 1;
	uint32_t temp[10];
    
    date.symboy = '+';
    date.integer = 0;
    memset(date.decimal,'0',10);
	memset(temp,'0',10);
	
	

   
    if(len > 0)
    {
         res = check_string_convert_date(str,len);
         
        if(res == 0) //字符串能转化为数字
        {
            num = find_decimal(str, len);

            if(str[0]=='+' || str[0]=='-' )
            {
                if(str[0] == '-')
                {
                    date.symboy = '-';
                }
                i=1;
            }
            else
            {
                i=0;
            }

            for(;i<num;i++)    //计算整数部分的值
            {
                if(str[0]== '+' || str[0] == '-')
                {
                    date.integer = date.integer + (str[num-i] - '0') * mul;
                }
                else
                {
                    date.integer = date.integer + (str[num-i-1] - '0') * mul;
                }
                mul *= 10;
            }

            /*

            mul = 1;

            for(i=num+1;i<len;i++)   //计算小数部分的值
            {
                
                date += (str[i] - '0')*mul;
                mul *= 10;
            }

            */

            //memset(date.decimal,'0',10);
			num = (len-num-1) < 10?(len-num-1):10;
            if(i<len)
            {
                memcpy(date.decimal,&str[i+1],num);
            }

            if((date.integer==0) && (strcmp((const char *)date.decimal,(const char *)temp)==0))
            {
                date.symboy = '+';
            }
            
        
        }

        
     }
	
	 
	
	 return date;
      
   
}




//date：是要转化成字符串的数据
//str: 用来保存字符串的指针
//len: 保留小数的位数
// 返回值 字符串长度
//uint8_t   date_to_string(double date, uint8_t *str,uint8_t len)
//{
//    uint32_t i,mul;
//    double f;  
//    uint8_t k,temp,num;
//    
//	k = 0;
//	mul = 1;
//	
//	//PRINT("f=%d\r\n",(int)(date*10));
//	
//	if(date < 0)
//	{
//		str[k] = '-';
//		k++;
//	}
//	
//	date = -date;

//    i = date;
//	
//	while(1)
//	{
//		temp = i / mul;
//		
//		if(temp >= 10)
//		{
//			mul *= 10;
//		}
//		else
//		{
//			break;
//		}
//	}
//	
//    

//    while (1)
//    {
//        temp = (i / mul) % 10;
//        mul /= 10;
//        str[k] = temp + '0';
//        k++;
//        if(mul == 0)
//        {
//            break;
//        }
//    }

//    if(len)
//    {
//        str[k] = '.';
//        k++;
//    }

//    f = date - i;
//    num = k;

//    for(;k<num+len;k++)
//    {
//        f *= 10;
//		
//        temp = (int)f % 10;
//        str[k] = temp + '0';
//    }


//    return k;

//    
//       
//}



//date：是要转化成字符串的数据
//str: 用来保存字符串的指针
//len: 保留小数的位数,小于10；
// 返回值 字符串长度
uint8_t   date_to_string(float_type date, uint8_t *str,uint8_t len)
{
    uint32_t i,mul,temp;
    
    uint8_t k;
    
	k = 0;
	mul = 1;
	
	
	if(date.symboy == '-')
	{
		str[k] = '-';
		k++;
	}
	
	

    i = date.integer;
	
	while(1)
	{
		temp = i / mul;
		
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
        temp = (i / mul) % 10;
        mul /= 10;
        str[k] = temp + '0';
        k++;
        if(mul == 0)
        {
            break;
        }
    }

    if(len)
    {
        str[k] = '.';
        k++;
        memcpy(&str[k],date.decimal,len);
        k += len;
    }

    


    return k;

    
       
}






//返回值
//0:date1==date2
//1:date1>date2
//2:date1<date2

uint8_t  compare_float_type(float_type date1,float_type date2)
{
    uint8_t res = 0;
    int result;
    
    if(date1.symboy != date2.symboy)  //符号不相同
    {
        if(date1.symboy == '-')
        {
            res = 2;
        }
        else
        {
            res = 1;
        }
    }
    else //符号相同
    {
        if(date1.symboy == '+')
        {
            if(date1.integer > date2.integer)
            {
                res = 1;
            }
            else if(date1.integer < date2.integer)
            {
                res = 2;
            }
            else
            {
                result = strcmp((const char *)date1.decimal,(const char *)date2.decimal);
                if(result < 0)
                {
                    res = 2;
                }
                else if(result > 0)
                {
                    res = 1;
                }
            }
            
        }
        else
        {
            if(date1.integer > date2.integer)
            {
                res = 2;
            }
            else if(date1.integer < date2.integer)
            {
                res = 1;
            }
            else
            {
                result = strcmp((const char *)date1.decimal,(const char *)date2.decimal);
                if(result < 0)
                {
                    res = 1;
                }
                else if(result > 0)
                {
                    res = 2;
                }
            }
            
        }
    }

    return res;
    
}





void copy_to_box(textbox * pbox, uint8_t *str,uint8_t len)
{
      uint8_t num;
    
     if(pbox->input_way == 1)
     {
        if(pbox->input.keyboard.limit)
        {
            float_type date = str_to_date(str,len);

            if(compare_float_type(date,pbox->input.keyboard.max_value) == 1)
            {
                pbox->input.keyboard.value.symboy = pbox->input.keyboard.max_value.symboy;
                pbox->input.keyboard.value.integer = pbox->input.keyboard.max_value.integer;
                memcpy(pbox->input.keyboard.value.decimal,pbox->input.keyboard.max_value.decimal,10);
            }
            else if(compare_float_type(date,pbox->input.keyboard.min_value) == 2)
            {
                pbox->input.keyboard.value.symboy = pbox->input.keyboard.min_value.symboy;
                pbox->input.keyboard.value.integer = pbox->input.keyboard.min_value.integer;
                memcpy(pbox->input.keyboard.value.decimal,pbox->input.keyboard.min_value.decimal,10);
            }
            else
            {
                pbox->input.keyboard.value.symboy = date.symboy;
                pbox->input.keyboard.value.integer = date.integer;
                memcpy(pbox->input.keyboard.value.decimal,date.decimal,10);
            }

            pbox->code_num = date_to_string(pbox->input.keyboard.value, pbox->code,pbox->input.keyboard.decimal_num);
            
        }
        else
        {
           num = (len > pbox->maxmum_code)?(pbox->maxmum_code):(len);
           memcpy(pbox->code,str, num);
           pbox->code_num = num;
        }
        *(pbox->pcode_addr) &= 0X00FFFFFF;
        *(pbox->pcode_addr) |= (pbox->code_num << 24);
        pbox->cmd_get_status(pbox);
        
     }
}






/***************************************************************************************
* End of File: textbox.c

****************************************************************************************/
