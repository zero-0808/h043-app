
/***************************************************************************************
****************************************************************************************
* Project      : 20180615_HMI
* FILE         : str.c
* Description  : 
*                      
* Copyright (c) 2018 by ShenZhen JingHua Displays Co.,Ltd... All Rights Reserved.
* 
* History:
* Version		name			Date			Description
* 0.01			Qu Bingbing		2018/06/20		Initial Version
* 
****************************************************************************************
****************************************************************************************/
#include "lang_Thai.h"
#include "lang_arabic.h"
#include "str.h"
#include "mem_my.h"
#include "cfg.h"
#include "string.h"



	
static uint8_t str_touch_msg(void *p, t_msg * p_msg)
{

	return 1;
	
   
}



    
    
    
static void str_touch_release( void* pCtrl )
{

	str_touch * pstr = (str_touch *) pCtrl;

	if(pstr->pdraw)
	{
		my_free(pstr->pdraw);
	}


	my_free(pstr);
   
}



    
    
static void str_touch_act(base_touch *p,uint8_t flag)
{

   
	
}


    
    
    
    
    
    
static void init_draw_text(str_touch * pstr)
{

	pstr->pdraw = NULL;
	uint32_t size;
	uint8_t i;
	font_type *ptfont;
	uint16_t w,h;
	uint8_t line,col;

	if(pstr->ptfont == NULL)
	{
		return;
	}
	
	ptfont = pstr->ptfont;
	

	if(ptfont->code == 0)
	{
		w = ptfont->width;
	}
	else
	{
		w = ptfont->width / 2;
	}

	h = get_font_height(*ptfont);

	line = (pstr->tp_rect.bottom - pstr->tp_rect.top + 1) / h;
	col = (pstr->tp_rect.right - pstr->tp_rect.left +  1) / w;
		
	if(col == 0 || line == 0)
	{
		return;
	}

	if(ptfont->code == 3)
	{
		col = 2 * col;
	}
	size = sizeof(draw_text) + line * (sizeof(line_text) + 2*col);
	pstr->pdraw = (draw_text *) my_malloc(size);
	pstr->pdraw->line_num = line;
	pstr->pdraw->col_num = col;

	pstr->pdraw->pline = (line_text *) ((uint8_t *) pstr->pdraw + sizeof(draw_text)); 

	for(i = 0; i < line ; i++)
	{
		pstr->pdraw->pline[i].pcode = (uint8_t *) ((uint8_t *) pstr->pdraw + sizeof(draw_text) + line * sizeof(line_text) + 2*i * col);
	}
		
 
	

}


uint8_t * get_str_code(uint16_t str_id,uint8_t * len)
{
	uint16_t index;
	
	if(src_cfg.lang_info==NULL||str_id >= src_cfg.lang_info->plang->str_num)
	{
		*len = 0;
		return NULL;
	}

	index = src_cfg.lang_info->plang->str_des[str_id].addr;
	*len = src_cfg.lang_info->plang->str_des[str_id].len;

	return &(src_cfg.lang_info->plang->str[index]);

	
}

    
    
    
    
    
    
    
static void update_draw_text(str_touch * pstr)
{

	if(pstr->pdraw == NULL || pstr->ptfont == NULL)
	{
		return;
	}
	
	pstr->pdraw->line = 0;

   
	if(pstr->pdraw->line_num == 0 || pstr->pdraw->col_num == 0)
	{
		return;
	}

	uint16_t w,str_id;
	uint8_t line,col,i,j,k,len;
	font_type *ptfont;
	str_id = (pstr->pstr_id[0] << 8) | (pstr->pstr_id[1]);
	uint8_t * code = get_str_code(str_id,&len);
	uint16_t unicode;
	uint8_t tmp_len;
	lang_Thai_t lang_tai;
	if(code == NULL || len == 0)
	{
		return;
	}
	
	ptfont = pstr->ptfont;

	if(ptfont->code == 0)
	{
		w = ptfont->width;
	}
	else
	{
		w = ptfont->width / 2;
	}
    
	line = pstr->pdraw->line_num;
	col = pstr->pdraw->col_num;

	pstr->pdraw->line = 1;
	if(ptfont->code < 3)
	{
		for(j=0,i=0,k=0;(j<len) && (i<line);j++)
		{
		   
			if(code[j] < 0x80)
			{
				if(code[j] != '\r')
				{
					pstr->pdraw->pline[i].pcode[k] = code[j];
					k++;
					if(k >= col || (j + 1) == len)
					{
						pstr->pdraw->pline[i].len = k;
						pstr->pdraw->pline[i].width = k * w;
						k = 0;
						i++;
					}
                            
				}
				else if((j + 1) < len)
				{
					if(code[j+1] == '\n')
					{
						j++;
						pstr->pdraw->pline[i].len = k;
						pstr->pdraw->pline[i].width = k * w;
						k = 0;
						i++;
					}
					else
					{
						pstr->pdraw->pline[i].pcode[k] = code[j];
						k++;
						if(k >= col)
						{
							pstr->pdraw->pline[i].len = k;
							pstr->pdraw->pline[i].width = k * w;
							k = 0;
							i++;
						}
					}
				}
				else
				{
					pstr->pdraw->pline[i].pcode[k] = code[j];
					k++;
					if(k >= col || (j + 1) == len)
					{
						pstr->pdraw->pline[i].len = k;
						pstr->pdraw->pline[i].width = k * w;
						k = 0;
						i++;
					}
				}
			}
			else
			{
				if((j + 1) < len)
				{
					if(k+1 < col)
					{
						pstr->pdraw->pline[i].pcode[k] = code[j];
						k++;
						j++;
						pstr->pdraw->pline[i].pcode[k] = code[j];
						k++;
						if(k >= col || (j + 1) >= len)
						{
							pstr->pdraw->pline[i].len = k;
							pstr->pdraw->pline[i].width = k * w;
							k = 0;
							i++;
						}
					}
					else
					{
						j--;
						pstr->pdraw->pline[i].len = k;
						pstr->pdraw->pline[i].width = k * w;
						k = 0;
						i++;
					}
				}
				else
				{
					pstr->pdraw->pline[i].pcode[k] = 0;
					k++;
					pstr->pdraw->pline[i].len = k;
					pstr->pdraw->pline[i].width = k * w;
					k = 0;
					i++;
				}
			}
		   
		}
		pstr->pdraw->line = i;
	}
	else //unicode
	{
		uint16_t width;
		width = pstr->tp_rect.right - pstr->tp_rect.left - 1;
		pstr->pdraw->pline[0].width = 0;
		for(j=0,i=0,k=0;(j<len) && (i<line);j++)
		{
			if((j+1<len)&&(j%2==0))
			{
				unicode = code[j] << 8 | code[j + 1];  
			}      
			else
			{
				unicode = 0;
			}
			
			if((code[j]==0) && (code[j+1]<0x80))
			{
				if(code[j+1] != '\r')
				{
					pstr->pdraw->pline[i].pcode[k] = code[j];
					k++;
					j++;
					pstr->pdraw->pline[i].pcode[k] = code[j];
					k++;
					pstr->pdraw->pline[i].width += w;

					if(pstr->pdraw->pline[i].width + w > width || (j+2) >= len)//
					{
						pstr->pdraw->pline[i].len = k;
						k = 0;
						i++;
						if(i < line)
						{
							pstr->pdraw->pline[i].width = 0;
						}
					}
				}
				else if((j + 3) < len)
				{
					if(code[j+2]==0 && code[j+3]=='\n' )
					{
						j+=3;
						pstr->pdraw->pline[i].len = k;
						k = 0;
						i++;
						if(i < line)
						{
							pstr->pdraw->pline[i].width = 0;
						}
					}
					else
					{
						pstr->pdraw->pline[i].pcode[k] = code[j];
						k++;
						j++;
						pstr->pdraw->pline[i].pcode[k] = code[j];
						k++;
						pstr->pdraw->pline[i].width += w;

						if(pstr->pdraw->pline[i].width + w > width || (j+2) >= len)
						{
							pstr->pdraw->pline[i].len = k;
							k = 0;
							i++;
							if(i < line)
							{
								pstr->pdraw->pline[i].width = 0;
							}
						}
								
					}
				}
				else
				{
					pstr->pdraw->pline[i].pcode[k] = code[j];
					k++;
					j++;
					pstr->pdraw->pline[i].pcode[k] = code[j];
					k++;
					pstr->pdraw->pline[i].width += w;

					if(pstr->pdraw->pline[i].width + w > width || (j+2) >= len)
					{
						pstr->pdraw->pline[i].len = k;
						k = 0;
						i++;
						if(i < line)
						{
							pstr->pdraw->pline[i].width = 0;
						}
					}
				}
			}
			else if(Thai_Check(unicode)==1)	//泰语
			{
				uint8_t tai_len;
				tai_len = Thai_Splice(&code[j],len-j);
				if(pstr->pdraw->pline[i].width + w <=  width)
				{
					for(tmp_len=0;tmp_len<tai_len;tmp_len++)
					{
						pstr->pdraw->pline[i].pcode[k+2*tmp_len] = code[j+2*tmp_len];
						//k++;
						//j++;
						pstr->pdraw->pline[i].pcode[k+2*tmp_len+1] = code[j+2*tmp_len+1];
						//k++;
					}
					k += (2*(tai_len));
					j += (2*(tai_len)-1);
					//pstr->pdraw->pline[i].width += 2*w;
					pstr->pdraw->pline[i].width += w;

					if(pstr->pdraw->pline[i].width + w > width || (j+2) >= len)
					{
						pstr->pdraw->pline[i].len = k;
						k = 0;
						i++;
						if(i < line)
						{
							pstr->pdraw->pline[i].width = 0;
						}
					}
				}
				else
				{
					j--;
					pstr->pdraw->pline[i].len = k;
					k = 0;
					i++;
					if(i < line)
					{
						pstr->pdraw->pline[i].width = 0;
					}
				}
			}			
			else if(Arabic_Check(unicode)==1)//阿拉伯语
			{
				uint8_t arabic_len;
				arabic_len = Arabic_Len(&code[j],len-j);
				if(pstr->pdraw->pline[i].width+arabic_len*w<=  width)
				{
					for(tmp_len=0;tmp_len<arabic_len;tmp_len++)
					{
						pstr->pdraw->pline[i].pcode[k+2*tmp_len] = code[j+2*tmp_len];
						//k++;
						//j++;
						pstr->pdraw->pline[i].pcode[k+2*tmp_len+1] = code[j+2*tmp_len+1];
						//k++;
					}
					k += (2*(arabic_len));
					j += (2*(arabic_len)-1);
					// pstr->pdraw->pline[i].pcode[k] = code[j];
					// k++;
					// j++;
					// pstr->pdraw->pline[i].pcode[k] = code[j];
					// k++;
					//pstr->pdraw->pline[i].width += 2*w;
					pstr->pdraw->pline[i].width += arabic_len*w;

					if(pstr->pdraw->pline[i].width + arabic_len*w > width || (j+2) >= len)
					{
						pstr->pdraw->pline[i].len = k;
						k = 0;
						i++;
						if(i < line)
						{
							pstr->pdraw->pline[i].width = 0;
						}
					}
				}
				else
				{
					j--;
					pstr->pdraw->pline[i].len = k;
					k = 0;
					i++;
					if(i < line)
					{
						pstr->pdraw->pline[i].width = 0;
					}
				}
			}
			else if(unicode>=0x4e00&&unicode<=0x9fcb)//汉字
			{
				//Get_Thai_Info(&lang_tai,unicode,*hz_font);
				if(pstr->pdraw->pline[i].width + 2*w <=  width)
				//if(pstr->pdraw->pline[i].width + w <=  width)
				{
					pstr->pdraw->pline[i].pcode[k] = code[j];
					k++;
					j++;
					pstr->pdraw->pline[i].pcode[k] = code[j];
					k++;
					pstr->pdraw->pline[i].width += 2*w;
					//pstr->pdraw->pline[i].width += w;

					if(pstr->pdraw->pline[i].width + w > width || (j+2) >= len)
					{
						pstr->pdraw->pline[i].len = k;
						k = 0;
						i++;
						if(i < line)
						{
							pstr->pdraw->pline[i].width = 0;
						}
					}
				}
				else
				{
					j--;
					pstr->pdraw->pline[i].len = k;
					k = 0;
					i++;
					if(i < line)
					{
						pstr->pdraw->pline[i].width = 0;
					}
				}
			}
			else
			{
				if(pstr->pdraw->pline[i].width + w <=  width)
				//if(pstr->pdraw->pline[i].width + w <=  width)
				{
					pstr->pdraw->pline[i].pcode[k] = code[j];
					k++;
					j++;
					pstr->pdraw->pline[i].pcode[k] = code[j];
					k++;
					pstr->pdraw->pline[i].width += w;
					//pstr->pdraw->pline[i].width += w;

					if(pstr->pdraw->pline[i].width + w > width || (j+2) >= len)
					{
						pstr->pdraw->pline[i].len = k;
						k = 0;
						i++;
						if(i < line)
						{
							pstr->pdraw->pline[i].width = 0;
						}
					}
				}
				else
				{
					j--;
					pstr->pdraw->pline[i].len = k;
					k = 0;
					i++;
					if(i < line)
					{
						pstr->pdraw->pline[i].width = 0;
					}
				}
			}

		 
		}
		
		pstr->pdraw->line = i;
	}

}



    
    
    
static uint8_t splice_len = 0,splice_num,main_pos;   
static uint8_t arabic_flag=0,fix_len,fix_i=0;//标记当前显示语言 1,阿拉伯语
static void str_draw_screen(base_touch * p)
{
	if(p->type == CFG_STR)
	{
		uint8_t *code;
		uint8_t  line,code_index,i,tmp_index;
		str_touch * pstr = (str_touch *) p;
		font_type * ch_font, *hz_font,*ptfont;
		uint16_t width,height,x,y,color;
		uint16_t hz;
        uint16_t lang_check;
		uint16_t htemp;
        uint16_t* arbic_table=NULL;
		uint16_t* arbic_tmp=NULL;
		uint16_t tmp_len;
		uint16_t convert_len,tai_code;
		uint8_t offset=0,remain=0;
		//uint8_t splice_len = 0,splice_num,main_pos;
		lang_Thai_t lang_tai[4];
		if(pstr->pdraw == NULL || pstr->ptfont==NULL)
		{
			return;
		}

		ptfont = pstr->ptfont;
		   
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
		update_draw_text(pstr);
		color = pstr->font_color;
		  
		line = pstr->pdraw->line;
		
		htemp = line * height;

		if(pstr->v_align == 0)
		{
			y = pstr->tp_rect.top;
		}
		else if(pstr->v_align == 1)
		{
			y = pstr->tp_rect.top + (pstr->tp_rect.bottom - pstr->tp_rect.top + 1 - htemp) / 2;
		}
		else
		{
			y = pstr->tp_rect.bottom - htemp + 1;
		}
		   
		for(i = 0;i<line;i++)
		{
            code = pstr->pdraw->pline[i].pcode;
            lang_check = Arabic_Check_Len(code,pstr->pdraw->pline[i].len);
            if(lang_check>0)//阿拉伯语
            {
                arabic_flag = 1;
            }
            else
            {
                arabic_flag = 0;
            }
			if(pstr->h_align == 0)
			{
                //lang_check = Arabic_Len(code,pstr->pdraw->pline[i].len);
                //lang_check = code[0] << 8 | code[1];
                
                if(arabic_flag==1)//阿拉伯语
                //if(src_cfg.lang_info->lang==9)
                {
                    
                    //x = pstr->tp_rect.left + (pstr->tp_rect.right - pstr->tp_rect.left + 1) ;
					x = pstr->tp_rect.right ;
                }
                else
                {
                    arabic_flag = 0;
                    x = pstr->tp_rect.left;
                }
			}
			else if(pstr->h_align == 1)
			{
				x = pstr->tp_rect.left + (pstr->tp_rect.right - pstr->tp_rect.left + 1 - 
				pstr->pdraw->pline[i].width) / 2;
			}
			else
			{
				x = pstr->tp_rect.right - pstr->pdraw->pline[i].width + 1;
			}

			
    
			if(ptfont->code < 3)
			{
				for(code_index=0;code_index<pstr->pdraw->pline[i].len;code_index++)
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
				/* Change By 王先银 at 2020/09/4 */
                if(arabic_flag==1)//阿拉伯语
                {
                    arbic_table = my_malloc(pstr->pdraw->pline[i].len);
					arbic_tmp = my_malloc(pstr->pdraw->pline[i].len);
					if(arbic_table==NULL||arbic_tmp==NULL)
					{
						return ;
					}
					tmp_len = 0;
					for(code_index=0;code_index<pstr->pdraw->pline[i].len;code_index+=2)
					{
						arbic_tmp[tmp_len++] = code[code_index] << 8 | code[code_index + 1];
					}
					convert_len = arabic_unicode_convert(arbic_table,arbic_tmp,tmp_len);
					for(code_index=0;code_index<convert_len;code_index++)
					{
						/* Change By 王先银 at 2020/09/4 */
						hz = arbic_table[code_index];
						
						if(hz>=0x0080)
						{
                            Get_Thai_Info(&lang_tai[0],hz,*hz_font);
							//get_arabic_offset(&offset,&remain,hz,*hz_font);
                            if(lang_tai[0].width<3)
                            {
                                x =x-(lang_tai[0].width+2);
                            }
                            else
                            {
                                
                                x =x-(lang_tai[0].width);
                            }
//							if(code_index==0)
//							{
//								//remain = 0;
//                                //offset = 0;
//                                //x =x-(2*width-offset);
//                                x =x-(lang_tai[0].width-1);
//							}
//                            else
//                            {
//                                if(offset+remain>=(2*width-4))
//                                {
//                                    x =x-4;
//                                }
//                                else
//                                {
//                                    x =x-(2*width-offset-remain);
//                                }
//                                
//                            }
                            
                            //x -= (2*width);
                            //show_nfont(x,y,color, hz,*hz_font);
                            Show_Thai(lang_tai[0],hz,x,y,color,*hz_font,1);
                            if(lang_tai[0].width<3)
                            {
                                x =x-2;
                            }
							fix_i  = 1;
                            //x -= offset;
						}
                        else if(hz==0x0020&&code_index==0)
                        {
                        }   
						else
						{
							
							//处理阿拉伯语和英语混合
							fix_len = Arabic_Fix_English(&arbic_table[code_index],convert_len-code_index);
							x -= (fix_len*width);
							if(fix_i==1)
							{
								fix_i = 0;
								x -= width;
							}
							for(fix_i=0;fix_i<fix_len;fix_i++)
							{
                                hz = arbic_table[code_index+fix_i];
								show_nfont(x,y,color, hz,*hz_font);
								x += width;
							}
                            x -= (fix_len*width); //回到写的地方
                            code_index += (fix_len-1);
						}
						

					}
                    my_free(arbic_table);
                    my_free(arbic_tmp);
					
                }
                else
                 {
					for(code_index=0;code_index<pstr->pdraw->pline[i].len;code_index++)
					{
						if(code[code_index] == 0 && code[code_index+1] < 0x80)
						{
							show_charn_font(x,y,color, code[code_index + 1],*ch_font);
                            
                            if(code[code_index+1]==0x2e||code[code_index+1]==0x2c)//标点
                            {
                                x += 3;
                            }
                            else
                            {
                                x += width;
                            }
						}
						else
						{
							hz = code[code_index] << 8 | code[code_index + 1];
							
							
							if (Thai_Check(hz)==1)//泰语
							{
								tmp_len = pstr->pdraw->pline[i].len-code_index;
								splice_len = Thai_Splice(&code[code_index],tmp_len);
								if(splice_len>1)//需要拼接
								{
                                    
									for(splice_num=0;splice_num<splice_len;splice_num++)
									{
										tai_code = code[code_index+2*splice_num] << 8 | code[code_index+2*splice_num + 1];
										Get_Thai_Info(&lang_tai[splice_num],tai_code,*hz_font);
										if(lang_tai[splice_num].type == THAI_SUBJECT)
										{
											main_pos = splice_num;
										}
									}
									for(splice_num=0;splice_num<splice_len;splice_num++)
									{
										tai_code = code[code_index+2*splice_num] << 8 | code[code_index+2*splice_num + 1];
										if(splice_num!=main_pos)
										{
											Show_Thai(lang_tai[main_pos],tai_code,x,y,color,*hz_font,0);
										}
										else
										{
											Show_Thai(lang_tai[main_pos],tai_code,x,y,color,*hz_font,1);
										}
										
									}
                                    if(lang_tai[main_pos].width<2)
                                    {
                                        lang_tai[main_pos].width = 2;
                                    }
                                    x += (lang_tai[main_pos].width+2);
									//x += (width);
									code_index += (splice_len-1)*2;
								}
								else if (splice_len==1)//不需要拼接
								{
									Get_Thai_Info(&lang_tai[0],hz,*hz_font);
                                    Show_Thai(lang_tai[0],hz,x,y,color,*hz_font,1);
									//show_nfont(x,y,color, hz,*hz_font);
									//x += width;
                                    if(lang_tai[0].width<2)
                                    {
                                        lang_tai[0].width = 2;
                                    }
                                    x += (lang_tai[0].width+2);
									//x += (width);
								}
								else
								{
									/* code */
								}
								
								
							}
							else
							{
								Get_Thai_Info(&lang_tai[0],hz,*hz_font);
                                Show_Thai(lang_tai[0],hz,x,y,color,*hz_font,1);
								x += (lang_tai[0].width+2);
								//show_nfont(x,y,color, hz,*hz_font);
								//x += 1*width;
							}
							
						}
						code_index++;
					}                
                }
				
			}

			if(i+1 < line)
			{
				y += height;
			}
		
		}
			 
	
	}
}
    
    
    
uint8_t str_touch_create(uint8_t * buffer)
{
	str_touch * pstr = (str_touch *) my_malloc(sizeof(str_touch));
   
	
	if(pstr == NULL)
	{
		return 1;
	}
	memset((void *)pstr,0,sizeof(str_touch));

	pstr->ctrl.id = ((buffer[0]<<24)|(buffer[1]<<16)|(buffer[4]<<8)|(buffer[5])<<0);
	pstr->ctrl.func_ctrl_procmsg = str_touch_msg;
	pstr->ctrl.func_ctrl_release = str_touch_release;
   

	pstr->tp_type = buffer[6];
	pstr->tp_rect.left = (buffer[7]<<8) | (buffer[8]);
	pstr->tp_rect.top = (buffer[9]<<8) | (buffer[10]);
	pstr->tp_rect.right = (buffer[11]<<8) | (buffer[12]);
	pstr->tp_rect.bottom = (buffer[13]<<8) | (buffer[14]);
	pstr->pic_id = (buffer[2]<<8) | (buffer[3]);
	pstr->pic_on = (buffer[15]<<8) | (buffer[16]);
	pstr->act = str_touch_act;
	pstr->draw_screen = str_draw_screen;

	pstr->pstr_id = &buffer[17];
	pstr->ptfont = get_font_type(buffer[19]);
//	pstr->font_index = 10;
	pstr->font_color = (buffer[20] << 8) | (buffer[21]);
	pstr->h_align = buffer[22];
	pstr->v_align = buffer[23];        
	init_draw_text(pstr);
	ctrl_init((t_ctrl *) pstr);
	
	return 0;
}



    
    
    
t_ctrl * sstr_touch_create(uint8_t * buffer)
{
	str_touch * pstr = (str_touch *) my_malloc(sizeof(str_touch));
   
	
	if(pstr == NULL)
	{
		return NULL;
	}
	memset((void *)pstr,0,sizeof(str_touch));

	pstr->ctrl.id = ((buffer[0]<<24)|(buffer[1]<<16)|(buffer[4]<<8)|(buffer[5])<<0);
	//pstr->ctrl.func_ctrl_procmsg = str_touch_msg;
	pstr->ctrl.func_ctrl_release = str_touch_release;
   

	pstr->tp_type = buffer[6];
	//pstr->tp_rect.left = (buffer[7]<<8) | (buffer[8]);
	//pstr->tp_rect.top = (buffer[9]<<8) | (buffer[10]);
	//pstr->tp_rect.right = (buffer[11]<<8) | (buffer[12]);
	//pstr->tp_rect.bottom = (buffer[13]<<8) | (buffer[14]);
	//pstr->pic_id = (buffer[2]<<8) | (buffer[3]);
	//pstr->pic_on = (buffer[15]<<8) | (buffer[16]);
	//pstr->act = str_touch_act;
	//pstr->draw_screen = str_draw_screen;

	pstr->pstr_id = &buffer[17];
	//pstr->font_index = (buffer[19]);
	//pstr->font_color = (buffer[20] << 8) | (buffer[21]);
	//pstr->h_align = buffer[22];
	//pstr->v_align = buffer[23];        
	//init_draw_text(pstr);
	ctrl_init((t_ctrl *) pstr);
	
	return (t_ctrl *) pstr;
}



/***************************************************************************************
* End of File: str.c

****************************************************************************************/
