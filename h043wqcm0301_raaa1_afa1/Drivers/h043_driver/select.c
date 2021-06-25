
/***************************************************************************************
****************************************************************************************
* Project      : 20180802_HMI_43_TEST
* FILE         : select.c
* Description  :
*
* Copyright (c) 2018 by ShenZhen JingHua Displays Co.,Ltd... All Rights Reserved.
*
* History:
* Version       name            Date            Description
* 0.01          Qu Bingbing     2018/08/08      Initial Version
*
****************************************************************************************
****************************************************************************************/

#include "select.h"
#include "mem_my.h"
#include "stdlib.h"
#include "math.h"
#include "string.h"
#include "display.h"
#include "cmd.h"
#include "cfg.h"



typedef struct 
{
    uint8_t len;
    uint8_t *p;
	
}code_info;


static uint8_t select_msg(void *p, t_msg * p_msg)
{
    uint16_t x, y, flag, f = 1;
    t_msg msg;


    if(p_msg->msg != MSG_TP)
    {
        return f;
    }

    select_touch * pselect = (select_touch * )p;



    if(pselect->ctrl.active == 0)
    {
        return f;
    }


    x = p_msg->a;
    y = p_msg->b;
    flag = p_msg->c;




    if((flag != 1) && !GETBIT( pselect->state, BTN_STATE_ACTIVE))
    {
        return f;
    }



    if(pt_in_rect(&pselect->tp_rect, x, y))
    {
        f = 0;
    }

    switch(flag)
    {
        case 1:
        {
            if(!f)
            {
                pselect->old_pos = y;
                pselect->pos = y;
                SETBIT( pselect->state, BTN_STATE_ACTIVE);
                SETBIT( pselect->state, BTN_STATE_LIGHT);
               // msg.msg = MSG_TOUCH;
               // msg.a   = (pselect->ctrl.id >> 16) & (0xffff);
               // msg.b   = (pselect->ctrl.id) & (0xffff);
               // msg.c   = 1;
               //msg_dev.write_message((void *)&msg, 0);
               xTimerReset(pselect->timer,3);
			   xTimerStart(pselect->timer,3);
                
                

            }

            break;
        }
        case 2:
        {
            pselect->pos = y;
            //msg.msg = MSG_TOUCH;
            //msg.a   = (pselect->ctrl.id >> 16) & (0xffff);
            //msg.b   = (pselect->ctrl.id) & (0xffff);
            //msg.c   = 2;
         
            msg_dev.write_message((void *)&msg, 0);
            break;
        }
        case 0:
        {

            pselect->pos = pselect->old_pos;
            //msg.msg = MSG_TOUCH;
            //msg.a   = (pselect->ctrl.id >> 16) & (0xffff);
            //msg.b   = (pselect->ctrl.id) & (0xffff);
            //msg.c   = 0;
            //msg_dev.write_message((void *)&msg, 0);
			pselect->screen_update += 1;
            xTimerStop(pselect->timer,3);
            CLRBIT( pselect->state, BTN_STATE_ACTIVE);

            break;
        }
    }

    return f;

}




static void select_release( void* pCtrl )
{
    select_touch * pselect = (select_touch *) pCtrl;

    if(pselect->tp_type != CFG_SEL)
    {
        return;
    }


    if(pselect->opt_info)
    {
        my_free(pselect->opt_info);
    }

    if(pselect->pdraw)
    {
        my_free(pselect->pdraw);
    }

    if(pselect->pstr_id)
    {
        my_free(pselect->pstr_id);
    }
    if(pselect->timer)
    {
         xTimerDelete(pselect->timer,3);
    }
    
    my_free(pselect);
}











/*
static uint8_t calculate_select_options(select_touch * pselect)
{
    uint16_t len, i;
    uint8_t * code;
    uint8_t num = 0;
    uint8_t language_num = *(pselect->plang);
    code = pselect->planguage[language_num].code;
    len = pselect->planguage[language_num].len;

    for(i = 0; i < len; i++)
    {
        if(code[i] == ';')
        {
            num++;
        }
    }


    return num;


}
*/


/*
static void get_code_info(select_touch * pselect,uint8_t option,code_info * pcode)
{

    uint16_t len, i;
    uint8_t * code;
    uint8_t index = 0;
    uint8_t option_num  = calculate_select_options(pselect);
    uint8_t language_num = *(pselect->plang);
    

    pcode->len = 0;
    pcode->p = NULL;

    if(option >= option_num)
    {
        return;
    }

    code = pselect->planguage[language_num].code;
    len = pselect->planguage[language_num].len;

    if(option == 0)
    {
        pcode->p = code;
    }

    for(i=0;i<len;i++)
    {
        if(code[i] != ';')
        {
            if(option == index)
            {
                pcode->len += 1;
            }
        }
        else
        {
            index++;

            if(index > option)
            {
                break;
            }
            if(index == option)
            {
                pcode->p = &code[i + 1];
            }
        }
    }

    
}
*/


static uint8_t option_add(uint8_t option, uint8_t count, uint8_t base)
{
    return (option + count) % base;
}

static uint8_t option_sub(uint8_t option, uint8_t count, uint8_t base)
{
    while(option < count)
    {
        option += base;
    }

    return (option - count);
}



static void upload_select_num(select_touch * pselect)
{
    
    uint8_t *p;
    p = my_malloc(14);

    

    p[0] = 0xEE;
    p[1] = 0xB1;
    p[2] = 0x11;
    p[3] = pselect->ctrl.id >> 24;
    p[4] = pselect->ctrl.id >> 16;
    p[5] = pselect->ctrl.id >> 8;
    p[6] = (pselect->ctrl.id & 0xFF);
    p[7] = pselect->tp_type;
    p[8] = *(pselect->sel_use.sel_slide.option);
    p[9] = 0xFF;
    p[10] = 0xFC;
    p[11] = 0xFF;
    p[12] = 0xFF;
	p[13] = ';';
    cmd_dev.notify_cmd(p,14);
    my_free(p);
}




static void update_option_infmation(select_touch * pselect)
{
    short dpos[3], center, end_dst;
    uint8_t num, count, percent, i;
    num = pselect->select_num;
    if(pselect->old_pos == pselect->pos)
    {
        if((!GETBIT( pselect->state, BTN_STATE_ACTIVE)) && GETBIT( pselect->state, BTN_STATE_LIGHT))
        {
            CLRBIT( pselect->state, BTN_STATE_LIGHT);
            uint16_t temp;
			center = (pselect->tp_rect.top + (pselect->tp_rect.bottom - pselect->tp_rect.top   + 1) / 2);
            if(pselect->sel_use.sel_slide.option_num == 0)
            {
                dpos[0] = abs(center - pselect->opt_info[1].pos);
                dpos[1] = abs(center - pselect->opt_info[2].pos);
                dpos[2] = abs(center - pselect->opt_info[3].pos);
                if(dpos[0]<dpos[1] && dpos[0]<dpos[2])
                {
                     *(pselect->sel_use.sel_slide.option) = pselect->opt_info[1].option;
                }
                else if(dpos[2]<dpos[1] && dpos[2]<dpos[0])
                {
                    *(pselect->sel_use.sel_slide.option) = pselect->opt_info[3].option;
                }
                else
                {
                    *(pselect->sel_use.sel_slide.option) = pselect->opt_info[2].option;
                }

                upload_select_num(pselect);

                temp = (pselect->tp_rect.bottom - pselect->tp_rect.top + 1) / 3;

                pselect->opt_info[2].pos = pselect->tp_rect.top + (pselect->tp_rect.bottom - pselect->tp_rect.top + 1) / 2;
                pselect->opt_info[2].color = pselect->font_color;
                pselect->opt_info[2].percent = 100;
                pselect->opt_info[1].pos = pselect->opt_info[2].pos - temp;
                pselect->opt_info[1].color = pselect->sel_use.sel_slide.endpoint_color;
                pselect->opt_info[1].percent = pselect->sel_use.sel_slide.endpoint_zoom;
                pselect->opt_info[0].pos = pselect->opt_info[2].pos   - 2 * (pselect->tp_rect.bottom - pselect->tp_rect.top + 1) / 3;;
                pselect->opt_info[0].color = pselect->sel_use.sel_slide.endpoint_color;
                pselect->opt_info[0].percent = pselect->sel_use.sel_slide.endpoint_zoom;
                pselect->opt_info[3].pos = pselect->opt_info[2].pos   +  temp;
                pselect->opt_info[3].color = pselect->sel_use.sel_slide.endpoint_color;
                pselect->opt_info[3].percent = pselect->sel_use.sel_slide.endpoint_zoom;
                pselect->opt_info[4].pos = pselect->opt_info[2].pos   +  2 * (pselect->tp_rect.bottom - pselect->tp_rect.top + 1) / 3;
                pselect->opt_info[4].color = pselect->sel_use.sel_slide.endpoint_color;
                pselect->opt_info[4].percent = pselect->sel_use.sel_slide.endpoint_zoom;

                
                pselect->opt_info[2].option =  *(pselect->sel_use.sel_slide.option);
                pselect->opt_info[1].option = option_sub(pselect->opt_info[2].option, 1, num);
                pselect->opt_info[0].option = option_sub(pselect->opt_info[1].option, 1, num);
                pselect->opt_info[3].option = option_add(pselect->opt_info[2].option, 1, num);
                pselect->opt_info[4].option = option_add(pselect->opt_info[3].option, 1, num);
        
                
            }
            else
            {
                dpos[0] = abs(center - pselect->opt_info[2].pos);
                dpos[1] = abs(center - pselect->opt_info[3].pos);
                dpos[2] = abs(center - pselect->opt_info[4].pos);

                
                if(dpos[0]<dpos[1] && dpos[0]<dpos[2])
                {
                     *(pselect->sel_use.sel_slide.option) = pselect->opt_info[2].option;
                }
                else if(dpos[2]<dpos[1] && dpos[2]<dpos[0])
                {
                    *(pselect->sel_use.sel_slide.option) = pselect->opt_info[4].option;
                }
                else
                {
                    *(pselect->sel_use.sel_slide.option) = pselect->opt_info[3].option;
                }

                upload_select_num(pselect);

                temp = (pselect->tp_rect.bottom - pselect->tp_rect.top + 1) / 5;

                pselect->opt_info[3].pos = pselect->tp_rect.top + (pselect->tp_rect.bottom - pselect->tp_rect.top + 1) / 2;
                pselect->opt_info[3].color = pselect->font_color;
                pselect->opt_info[3].percent = 100;
                pselect->opt_info[2].pos = pselect->opt_info[3].pos - (pselect->tp_rect.bottom - pselect->tp_rect.top + 1) / 5;
                pselect->opt_info[2].color = mix_color(pselect->font_color, pselect->sel_use.sel_slide.endpoint_color, 50);
                pselect->opt_info[2].percent = (pselect->sel_use.sel_slide.endpoint_zoom + 100) / 2;
                pselect->opt_info[1].pos = pselect->opt_info[3].pos - 2 * (pselect->tp_rect.bottom - pselect->tp_rect.top + 1) / 5;
                pselect->opt_info[1].color = pselect->sel_use.sel_slide.endpoint_color;
                pselect->opt_info[1].percent = pselect->sel_use.sel_slide.endpoint_zoom;
                pselect->opt_info[0].pos = pselect->opt_info[3].pos - 3 * (pselect->tp_rect.bottom - pselect->tp_rect.top + 1) / 5;
                pselect->opt_info[0].color = pselect->sel_use.sel_slide.endpoint_color;
                pselect->opt_info[0].percent = pselect->sel_use.sel_slide.endpoint_zoom;
                pselect->opt_info[4].pos = pselect->opt_info[3].pos   +  (pselect->tp_rect.bottom - pselect->tp_rect.top + 1) / 5;
                pselect->opt_info[4].color = mix_color(pselect->font_color, pselect->sel_use.sel_slide.endpoint_color, 50);
                pselect->opt_info[4].percent = (pselect->sel_use.sel_slide.endpoint_zoom + 100) / 2;
                pselect->opt_info[5].pos = pselect->opt_info[3].pos   +  2 * (pselect->tp_rect.bottom - pselect->tp_rect.top + 1) / 5;
                pselect->opt_info[5].color = pselect->sel_use.sel_slide.endpoint_color;
                pselect->opt_info[5].percent = pselect->sel_use.sel_slide.endpoint_zoom;
                pselect->opt_info[6].pos = pselect->opt_info[3].pos   +  3 * (pselect->tp_rect.bottom - pselect->tp_rect.top + 1) / 5;
                pselect->opt_info[6].color = pselect->sel_use.sel_slide.endpoint_color;
                pselect->opt_info[6].percent = pselect->sel_use.sel_slide.endpoint_zoom;

                pselect->opt_info[3].option = *(pselect->sel_use.sel_slide.option);
                pselect->opt_info[2].option = option_sub(pselect->opt_info[3].option, 1, num);
                pselect->opt_info[1].option = option_sub(pselect->opt_info[2].option, 1, num);
                pselect->opt_info[0].option = option_sub(pselect->opt_info[1].option, 1, num);
                pselect->opt_info[4].option = option_add(pselect->opt_info[3].option, 1, num);
                pselect->opt_info[5].option = option_add(pselect->opt_info[4].option, 1, num);
                pselect->opt_info[6].option = option_add(pselect->opt_info[5].option, 1, num);
            }
            
            

        }
        else
        {
            return;
        }
    
    }
    else
    {
        dpos[0] = pselect->pos - pselect->old_pos;
        pselect->old_pos = pselect->pos;
        center = (pselect->tp_rect.top + (pselect->tp_rect.bottom - pselect->tp_rect.top   + 1) / 2);
        if(pselect->sel_use.sel_slide.option_num == 0)
        {
            end_dst = (pselect->tp_rect.bottom - pselect->tp_rect.top   + 1) / 3;
            dpos[1] = pselect->opt_info[2].pos - center;
            dpos[2] = dpos[0] + dpos[1];
            if(dpos[2] >= 0)
            {
                count = dpos[2] / ((pselect->tp_rect.bottom - pselect->tp_rect.top   + 1) / 3);
                if(dpos[0] < 0)
                {
					//dpos[0] = -(-dpos[0]%((pselect->tp_rect.bottom - pselect->tp_rect.top   + 1) / 3));
					dpos[0] += ((pselect->tp_rect.bottom - pselect->tp_rect.top   + 1) / 3) * count;
                }
                else
                {
                    //dpos[0] %= (pselect->tp_rect.bottom - pselect->tp_rect.top   + 1) / 3;
					dpos[0] -= ((pselect->tp_rect.bottom - pselect->tp_rect.top   + 1) / 3) * count;
                }
               for(i = 0; i < 5; i++)
                {
                    pselect->opt_info[i].pos += dpos[0];
                    percent = abs(center - pselect->opt_info[i].pos) * 100 / end_dst;
                    if(percent > 100)
                    {
                        percent = 100;
                    }
                    percent = 100 - percent;
                    pselect->opt_info[i].color = mix_color(pselect->font_color, pselect->sel_use.sel_slide.endpoint_color, percent);
                    pselect->opt_info[i].percent = (100 * percent + pselect->sel_use.sel_slide.endpoint_zoom *  (100 - percent)) / 100;
                    pselect->opt_info[i].option = option_sub(pselect->opt_info[i].option, count, num);
                }
            }
            else
            {
                dpos[2] = -dpos[2];
                count = dpos[2] / ((pselect->tp_rect.bottom - pselect->tp_rect.top   + 1) / 3);
                if(dpos[0] < 0)
                {
                    //dpos[0] = -(-dpos[0]%((pselect->tp_rect.bottom - pselect->tp_rect.top   + 1) / 3));
					dpos[0] += ((pselect->tp_rect.bottom - pselect->tp_rect.top   + 1) / 3) * count;
                }
                else
                {
                    //dpos[0] %= (pselect->tp_rect.bottom - pselect->tp_rect.top   + 1) / 3;
					dpos[0] -= ((pselect->tp_rect.bottom - pselect->tp_rect.top   + 1) / 3) * count;
                }
                for(i = 0; i < 5; i++)
                {
                    pselect->opt_info[i].pos += dpos[0];
                    percent = abs(center - pselect->opt_info[i].pos) * 100 / end_dst;
                    if(percent > 100)
                    {
                        percent = 100;
                    }
                    percent = 100 - percent;
                    pselect->opt_info[i].color = mix_color(pselect->font_color, pselect->sel_use.sel_slide.endpoint_color, percent);
                    pselect->opt_info[i].percent = (100 * percent + pselect->sel_use.sel_slide.endpoint_zoom *  (100 - percent)) / 100;
                    pselect->opt_info[i].option = option_add(pselect->opt_info[i].option, count, num);
                }
                
            }
            
        }
        else
        {
            end_dst = 2*(pselect->tp_rect.bottom - pselect->tp_rect.top   + 1) / 5;
            dpos[1] = pselect->opt_info[3].pos - center;
            dpos[2] = dpos[0] + dpos[1];
            if(dpos[2] >= 0)
            {
                count = dpos[2] / ((pselect->tp_rect.bottom - pselect->tp_rect.top   + 1) / 5);
                if(dpos[0] < 0)
                {
                    //dpos[0] = -(-dpos[0]%((pselect->tp_rect.bottom - pselect->tp_rect.top   + 1) / 5));
					dpos[0] += ((pselect->tp_rect.bottom - pselect->tp_rect.top   + 1) / 5) * count;
                }
                else
                {
                    //dpos[0] %= (pselect->tp_rect.bottom - pselect->tp_rect.top   + 1) / 5;
					dpos[0] -= ((pselect->tp_rect.bottom - pselect->tp_rect.top   + 1) / 5) * count;
                }
				
                for(i = 0; i < 7; i++)
                {
                    pselect->opt_info[i].pos += dpos[0];
                    percent = abs(center - pselect->opt_info[i].pos) * 100 / end_dst;
                    if(percent > 100)
                    {
                        percent = 100;
                    }
                    percent = 100 - percent;
                    pselect->opt_info[i].color = mix_color(pselect->font_color, pselect->sel_use.sel_slide.endpoint_color, percent);
                    pselect->opt_info[i].percent = (100 * percent + pselect->sel_use.sel_slide.endpoint_zoom *  (100 - percent)) / 100;
                    pselect->opt_info[i].option = option_sub(pselect->opt_info[i].option, count, num);
                }
            }
            else
            {
				dpos[2] = -dpos[2];
                count = dpos[2] / ((pselect->tp_rect.bottom - pselect->tp_rect.top   + 1) / 5);
                if(dpos[0] < 0)
                {
                    //dpos[0] = -(-dpos[0]%((pselect->tp_rect.bottom - pselect->tp_rect.top   + 1) / 5));
					dpos[0] += ((pselect->tp_rect.bottom - pselect->tp_rect.top   + 1) / 5) * count;
                }
                else
                {
                    //dpos[0] %= (pselect->tp_rect.bottom - pselect->tp_rect.top   + 1) / 5;
					dpos[0] -= ((pselect->tp_rect.bottom - pselect->tp_rect.top   + 1) / 5) * count;
                }

                for(i = 0; i < 7; i++)
                {
                    pselect->opt_info[i].pos += dpos[0];
                    percent = abs(center - pselect->opt_info[i].pos) * 100 / end_dst;
                    if(percent > 100)
                    {
                        percent = 100;
                    }
                    percent = 100 - percent;
                    pselect->opt_info[i].color = mix_color(pselect->font_color, pselect->sel_use.sel_slide.endpoint_color, percent);
                    pselect->opt_info[i].percent = (100 * percent + pselect->sel_use.sel_slide.endpoint_zoom *  (100 - percent)) / 100;
                    pselect->opt_info[i].option = option_add(pselect->opt_info[i].option, count, num);
                }
                
            }
            
                        
        }
        
    }
   

}

static void select_act(base_touch *p, uint8_t flag)
{

//    if(p->type == CFG_SEL)
//    {
//        select_touch * pselect = (select_touch *) p;

//        if(flag == 0 || flag == 2)
//        {
//            pselect->screen_update = 1;
//        }
//    }

}


static void init_draw_text(select_touch * pselect)
{
    uint16_t w,h;
    uint8_t line,col,i;
    uint32_t size;
	font_type *ptfont;
	
	if(pselect->ptfont == NULL)
	{
		return;
	}
	
	ptfont = pselect->ptfont;
    
    pselect->pdraw = NULL;

    if(ptfont->code == 0)
	{
		w = ptfont->width;
	}
	else
	{
		w = ptfont->width / 2;
	}

	h = get_font_height(*ptfont);

	line = (pselect->tp_rect.bottom - pselect->tp_rect.top + 1) / h;
	col = (pselect->tp_rect.right - pselect->tp_rect.left +  1) / w;
		
	if(col == 0 || line == 0)
	{
		return;
	}

	if(ptfont->code == 3)
	{
		col = 2 * col;
	}

	size = sizeof(draw_text) + line * (sizeof(line_text) + col);
	pselect->pdraw = (draw_text *) my_malloc(size);
	pselect->pdraw->line_num = line;
	pselect->pdraw->col_num = col;

	pselect->pdraw->pline = (line_text *) ((uint8_t *) pselect->pdraw + sizeof(draw_text)); 

	for(i = 0; i < line ; i++)
	{
		pselect->pdraw->pline[i].pcode = (uint8_t *) ((uint8_t *) pselect->pdraw + sizeof(draw_text) + line * sizeof(line_text) + i * col);
	}
}



static void update_draw_text(select_touch * pselect)
{
    uint16_t w;
    uint8_t line,col,i,j,k;
    
    if(pselect->pdraw == NULL || pselect->ptfont == NULL)
	{
		return;
	}

	code_info tcode;
	
	pselect->pdraw->line = 0;

   
	if(pselect->pdraw->line_num == 0 || pselect->pdraw->col_num == 0)
	{
		return;
	}

	//get_code_info(pselect,*(pselect->sel_use.sel_show.option),&tcode);
	//get_code_info(pselect,pselect->opt_info[i].option,&tcode);
	tcode.p = get_str_code(pselect->pstr_id[*(pselect->sel_use.sel_show.option)],&(tcode.len));

	if(tcode.p == NULL || tcode.len == 0)
	{
	    return;
	}
	
	font_type * ptfont = pselect->ptfont;

	if(ptfont->code == 0)
	{
		w = ptfont->width;
	}
	else
	{
		w = ptfont->width / 2;
	}

	line = pselect->pdraw->line_num;
	col = pselect->pdraw->col_num;
	pselect->pdraw->line = 1;

	if(ptfont->code < 3)
	{
	    for(j=0,i=0,k=0;(j<tcode.len) && (i<line);j++)
	    {   
	        if(tcode.p[j] < 0x80)
	        {
	        
    	        if(tcode.p[j] != '\r')
    			{
    				pselect->pdraw->pline[i].pcode[k] = tcode.p[j];
    				k++;
    				if(k >= col || (j + 1) == tcode.len)
    				{
    					pselect->pdraw->pline[i].len = k;
    					pselect->pdraw->pline[i].width = k * w;
    					k = 0;
    					i++;
    				}
                            
    			}
    			else if((j + 1) < tcode.len)
    			{
    			    if(tcode.p[j] == '\n')
    				{
    					j++;
    					pselect->pdraw->pline[i].len = k;
    					pselect->pdraw->pline[i].width = k * w;
    					k = 0;
    					i++;
    				}
    				else
    				{
    					pselect->pdraw->pline[i].pcode[k] = tcode.p[j];
    					k++;
    					if(k >= col)
    					{
    						pselect->pdraw->pline[i].len = k;
    						pselect->pdraw->pline[i].width = k * w;
    						k = 0;
    						i++;
    					}
    				}
    			    
    			}
    			else
    			{
    			    pselect->pdraw->pline[i].pcode[k] = tcode.p[j];
    				k++;
    				if(k >= col || (j + 1) == tcode.len)
    				{
    					pselect->pdraw->pline[i].len = k;
    					pselect->pdraw->pline[i].width = k * w;
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
						pselect->pdraw->pline[i].pcode[k] = tcode.p[j];
						k++;
						j++;
						pselect->pdraw->pline[i].pcode[k] = tcode.p[j];
						k++;
						if(k >= col || (j + 1) >= tcode.len)
						{
							pselect->pdraw->pline[i].len = k;
							pselect->pdraw->pline[i].width = k * w;
							k = 0;
							i++;
						}
					}
					else
					{
						j--;
						pselect->pdraw->pline[i].len = k;
						pselect->pdraw->pline[i].width = k * w;
						k = 0;
						i++;
					}
			        
			    }
			    else
				{
					pselect->pdraw->pline[i].pcode[k] = '*';
					k++;
					pselect->pdraw->pline[i].len = k;
					pselect->pdraw->pline[i].width = k * w;
					k = 0;
					i++;
				}
			    
			}
	    }
	    pselect->pdraw->line = i;
	}
    else
    {
        uint16_t width;
		width = pselect->tp_rect.right - pselect->tp_rect.left - 1;
		pselect->pdraw->pline[0].width = 0;
		for(j=0,i=0,k=0;(j<tcode.len) && (i<line);j++)
		{
		    if((tcode.p[j]==0) && (tcode.p[j+1]<0x80))
		    {
		        if(tcode.p[j+1] != '\r')
				{
					pselect->pdraw->pline[i].pcode[k] = tcode.p[j];
					k++;
					j++;
					pselect->pdraw->pline[i].pcode[k] = tcode.p[j];
					k++;
					pselect->pdraw->pline[i].width += w;

					if(pselect->pdraw->pline[i].width + w > width || (j+2) >= tcode.len)
					{
						pselect->pdraw->pline[i].len = k;
						k = 0;
						i++;
						if(i < line)
						{
							pselect->pdraw->pline[i].width = 0;
						}
					}
				}
				else if((j + 3) < tcode.len)
				{
					if(tcode.p[j+2]==0 && tcode.p[j+3]=='\n' )
					{
						j+=3;
						pselect->pdraw->pline[i].len = k;
						k = 0;
						i++;
						if(i < line)
						{
							pselect->pdraw->pline[i].width = 0;
						}
					}
					else
					{
						pselect->pdraw->pline[i].pcode[k] = tcode.p[j];
						k++;
						j++;
						pselect->pdraw->pline[i].pcode[k] = tcode.p[j];
						k++;
						pselect->pdraw->pline[i].width += w;

						if(pselect->pdraw->pline[i].width + w > width || (j+2) >= tcode.len)
						{
							pselect->pdraw->pline[i].len = k;
							k = 0;
							i++;
							if(i < line)
							{
								pselect->pdraw->pline[i].width = 0;
							}
						}
								
					}
				}
				else
				{
					pselect->pdraw->pline[i].pcode[k] = tcode.p[j];
					k++;
					j++;
					pselect->pdraw->pline[i].pcode[k] = tcode.p[j];
					k++;
					pselect->pdraw->pline[i].width += w;

					if(pselect->pdraw->pline[i].width + w > width || (j+2) >= tcode.len)
					{
						pselect->pdraw->pline[i].len = k;
						k = 0;
						i++;
						if(i < line)
						{
							pselect->pdraw->pline[i].width = 0;
						}
					}
				}
				
		    }
		    else
		    {
		        if(pselect->pdraw->pline[i].width + 2*w <=  width)
				{
					pselect->pdraw->pline[i].pcode[k] = tcode.p[j];
					k++;
					j++;
					pselect->pdraw->pline[i].pcode[k] = tcode.p[j];
					k++;
					pselect->pdraw->pline[i].width += 2*w;

					if(pselect->pdraw->pline[i].width + w > width || (j+2) >= tcode.len)
					{
						pselect->pdraw->pline[i].len = k;
						k = 0;
						i++;
						if(i < line)
						{
							pselect->pdraw->pline[i].width = 0;
						}
					}
				}
				else
				{
					j--;
					pselect->pdraw->pline[i].len = k;
					k = 0;
					i++;
					if(i < line)
					{
						pselect->pdraw->pline[i].width = 0;
					}
				}
		        
		    }
		}
		pselect->pdraw->line = i;
    }

	
}


static uint16_t get_code_width(code_info * pcode,uint8_t code,uint16_t w)
{
    if(code < 3)
    {
        return pcode->len * w;
    }
    else
    {
        uint8_t * code,j;
        uint16_t width = 0;
        code = pcode->p;

        for(j=0;j<pcode->len;j++)
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
		
		return width;
    }
	
	
}




//static void select_show_font(select_touch * pselect)
//{
//    uint16_t w,h,color,htemp,x,y,hz;
//    uint8_t  i,line,code_index;
//    uint8_t *code;
//    font_type * ch_font, *hz_font;
//    
//    if(pselect->ptfont == NULL)
//    {
//        return;
//    }
//	font_type * ptfont = pselect->ptfont;
//    if(ptfont->code == 0)
//    {
//        w = ptfont->width;
//    }
//    else
//    {
//        w = ptfont->width / 2;

//    }

//    ch_font = hz_to_ch(ptfont);
//	hz_font = ch_to_hz(ptfont);

//    h = get_font_height(*ptfont);
//    
//    if(pselect->use == 0)
//    {
//        code_info tcode;
//        uint8_t num,j;
//        uint16_t width;
//        short x,y;
//        dc text;
//        bdc btext;
//        
//        if(pselect->sel_use.sel_slide.option_num == 0)
//        {
//            num = 5;
//        }
//        else
//        {
//            num = 7;
//        }
//        if(h > (pselect->tp_rect.bottom - pselect->tp_rect.top + 1) / (num-2))
//        {
//            return;
//        }

//        for(i=0;i<num;i++)
//        {
//			
//            //get_code_info(pselect,pselect->opt_info[i].option,&tcode);
//            tcode.p = get_str_code(pselect->pstr_id[pselect->opt_info[i].option],&(tcode.len));
//            width = get_code_width(&tcode,g_project->code,w);
//            
//            if(width > (pselect->tp_rect.right - pselect->tp_rect.left + 1))
//            {
//                continue;
//            }

//            text.height = h;
//            text.width = width;
//            text.p = (uint8_t *) my_malloc(h*width*disp_dev.bpp);
//			memset(text.p,0x0,h*width*disp_dev.bpp);
//            btext.width = width * pselect->opt_info[i].percent / 100;
//            btext.height = h * pselect->opt_info[i].percent / 100;
//            btext.y = pselect->opt_info[i].pos - btext.height / 2;
//            btext.x = pselect->tp_rect.left + (pselect->tp_rect.right - pselect->tp_rect.left + 1 - btext.width) / 2;
//            if(disp_dev.flag == 0)
//            {
//                btext.vdc = disp_dev.disp[1];
//            }
//            else
//            {
//                btext.vdc = disp_dev.disp[0];
//            }
//            x = 0;
//            y = 0;

//			

//            for(j=0;j<tcode.len;j++)
//            {
//                if(ptfont->code < 3)
//                {
//                    if(tcode.p[j] < 0x80)
//                    {
//            			if(ch_font)
//            			{
//            				show_charn_dc(x, y, pselect->opt_info[i].color,tcode.p[j], *ch_font,text);
//            			}
//                        x += w;
//                    }
//                    else
//                    {
//                        hz = (tcode.p[j] << 8) | (tcode.p[j+1]);
//            			if(hz_font)
//            			{
//            				show_nfont_dc(x,y,pselect->opt_info[i].color,hz,*hz_font,text);
//            			}
//                        j++;
//                        x += 2*w;
//                    }
//                }
//                else
//                {
//                    hz = (tcode.p[j] << 8) | (tcode.p[j+1]);

//                    if(hz < 0x80)
//                    {
//                        if(ch_font)
//            			{
//            				show_charn_dc(x, y, pselect->opt_info[i].color,tcode.p[j+1], *ch_font,text);
//            			}
//                        x += w;
//                    }
//                    else
//                    {
//            			if(hz_font)
//            			{
//            				show_nfont_dc(x,y,pselect->opt_info[i].color,hz,*hz_font,text);
//            			}
//                        x += 2*w; 
//                    }

//                    j++;
//                    
//                }
//            }

//            blt_dc_zoom(btext,text,pselect->tp_rect);

//			
//			my_free(text.p);
//            
//        }
//    }
//    else
//    {
//         color = pselect->font_color;
//		 line = pselect->pdraw->line;
//		 htemp = line * h;
//		if(pselect->sel_use.sel_show.v_align == 0)
//		{
//			y = pselect->tp_rect.top;
//		}
//		else if(pselect->sel_use.sel_show.v_align == 1)
//		{
//			y = pselect->tp_rect.top + (pselect->tp_rect.bottom - pselect->tp_rect.top + 1 - htemp) / 2;
//		}
//		else
//		{
//			y = pselect->tp_rect.bottom - htemp + 1;
//		}

//		for(i = 0;i<line;i++)
//		{
//		    if(pselect->sel_use.sel_show.h_align == 0)
//			{
//				x = pselect->tp_rect.left;
//			}
//			else if(pselect->sel_use.sel_show.h_align == 1)
//			{
//				x = pselect->tp_rect.left + (pselect->tp_rect.right - pselect->tp_rect.left + 1 - pselect->pdraw->pline[i].width) / 2;
//			}
//			else
//			{
//				x = pselect->tp_rect.right - pselect->pdraw->pline[i].width + 1;
//			}

//			code = pselect->pdraw->pline[i].pcode;

//			if(ptfont->code < 3)
//			{
//				for(code_index=0;code_index<pselect->pdraw->pline[i].len;code_index++)
//				{
//					if(code[code_index] < 0x80)
//					{
//						show_charn_font(x,y,color, code[code_index],*ch_font);
//						x += w;
//					}
//					else
//					{
//						hz = code[code_index] << 8 | code[code_index + 1];
//						show_nfont(x,y,color, hz,*hz_font);
//						code_index++;
//						x += 2*w;
//					}

//						
//				}
//    
//			}
//			else
//			{
//			    for(code_index=0;code_index<pselect->pdraw->pline[i].len;code_index++)
//				{
//					if(code[code_index] == 0 && code[code_index+1] < 0x80)
//					{
//						show_charn_font(x,y,color, code[code_index + 1],*ch_font);
//						x += w;
//					}
//					else
//					{
//						hz = code[code_index] << 8 | code[code_index + 1];
//						show_nfont(x,y,color, hz,*hz_font);
//						x += 2*w;
//					}
//					code_index++;
//				}
//			    
//			}

//			if(i+1 < line)
//			{
//				y += h;
//			}
//		
//		}
//        
//    }
//}


static void select_show_font(select_touch * pselect)
{
    
    if(pselect->ptfont == NULL)
    {
        return;
    }
    
	
    uint16_t w,h,hz,color,htemp,x,y;
    uint8_t i,line,code_index;
    font_type * ch_font, *hz_font;
	font_type * ptfont = pselect->ptfont;
	uint8_t *code;
	


	if(ptfont->code == 0)
    {
        w = ptfont->width;
    }
    else
    {
        w = ptfont->width / 2;

    }


    ch_font = hz_to_ch(ptfont);
	hz_font = ch_to_hz(ptfont);

    h = get_font_height(*ptfont);

    if(pselect->use == 0)
    {
        uint8_t num,j;
        code_info tcode;
        uint16_t width,x,y;
       
        
        if(pselect->sel_use.sel_slide.option_num == 0)
        {
            num = 5;
        }
        else
        {
            num = 7;
        }

        
        if(h > (pselect->tp_rect.bottom - pselect->tp_rect.top + 1) / (num-2))
        {
            return;
        }

        for(i=0;i<num;i++)
        {

             tcode.p = get_str_code(pselect->pstr_id[pselect->opt_info[i].option],&(tcode.len));
             width = get_code_width(&tcode,g_project->code,w);

            if(width > (pselect->tp_rect.right - pselect->tp_rect.left + 1))
            {
                continue;
            }

            

            y = pselect->opt_info[i].pos - h  / 2;
            x = pselect->tp_rect.left + (pselect->tp_rect.right - pselect->tp_rect.left + 1 - width) / 2;

            for(j=0;j<tcode.len;j++)
            {
                if(g_project->code < 3)
                {
                    if(tcode.p[j] < 0x80)
                    {
            			if(ch_font)
            			{
            				
            				show_charn_font_rect(x,y,pselect->opt_info[i].color,tcode.p[j],*ch_font,pselect->tp_rect);
            			}
                        x += w;
                    }
                    else
                    {
                        hz = (tcode.p[j] << 8) | (tcode.p[j+1]);
            			if(hz_font)
            			{
            				show_nfont_rect(x,y,pselect->opt_info[i].color,hz,*hz_font,pselect->tp_rect);
            			}
                        j++;
                        x += 2*w;
                    }
                }
                else
                {

                    hz = (tcode.p[j] << 8) | (tcode.p[j+1]);

                    if(hz < 0x80)
                    {
                        if(ch_font)
            			{
            				show_charn_font_rect(x,y,pselect->opt_info[i].color,tcode.p[j+1],*ch_font,pselect->tp_rect);
            			}
                        x += w;
                    }
                    else
                    {
            			if(hz_font)
            			{
            				show_nfont_rect(x,y,pselect->opt_info[i].color,hz,*hz_font,pselect->tp_rect);
            			}
                        x += 2*w; 
                    }

                    j++;
                    
                    
                }
                
            }
                    
        }

        
    }
    else
    {
        color = pselect->font_color;
        line = pselect->pdraw->line;
        htemp = line * h;
        
        if(pselect->sel_use.sel_show.v_align == 0)
        {
            y = pselect->tp_rect.top;
        }
        else if(pselect->sel_use.sel_show.v_align == 1)
        {
            y = pselect->tp_rect.top + (pselect->tp_rect.bottom - pselect->tp_rect.top + 1 - htemp) / 2;
        }
        else
        {
            y = pselect->tp_rect.bottom - htemp + 1;
        }


        for(i = 0;i<line;i++)
        {
            if(pselect->sel_use.sel_show.h_align == 0)
			{
				x = pselect->tp_rect.left;
			}
			else if(pselect->sel_use.sel_show.h_align == 1)
			{
				x = pselect->tp_rect.left + (pselect->tp_rect.right - pselect->tp_rect.left + 1 - pselect->pdraw->pline[i].width) / 2;
			}
			else
			{
				x = pselect->tp_rect.right - pselect->pdraw->pline[i].width + 1;
			}

			code = pselect->pdraw->pline[i].pcode;

			if(g_project->code < 3)
			{
				for(code_index=0;code_index<pselect->pdraw->pline[i].len;code_index++)
				{
					if(code[code_index] < 0x80)
					{
						show_charn_font(x,y,color, code[code_index],*ch_font);
						x += w;
					}
					else
					{
						hz = code[code_index] << 8 | code[code_index + 1];
						show_nfont(x,y,color, hz,*hz_font);
						code_index++;
						x += 2*w;
					}

						
				}
    
			}
			else
			{
			    for(code_index=0;code_index<pselect->pdraw->pline[i].len;code_index++)
				{
					if(code[code_index] == 0 && code[code_index+1] < 0x80)
					{
						show_charn_font(x,y,color, code[code_index + 1],*ch_font);
						x += w;
					}
					else
					{
						hz = code[code_index] << 8 | code[code_index + 1];
						show_nfont(x,y,color, hz,*hz_font);
						x += 2*w;
					}
					code_index++;
				}
			    
			}

			if(i+1 < line)
			{
				y += h;
			}
            
        }
        
    }

	
}







 void init_options_infmation(select_touch * pselect)
{
    uint8_t num, num1;
    uint16_t temp;

    //num = calculate_select_options(pselect);
	num = pselect->select_num;
    num1 = *(pselect->sel_use.sel_slide.option);

    if(num == 0)
    {
        return;
    }

    if(pselect->sel_use.sel_slide.option_num == 0)
    {
        pselect->opt_info = my_malloc(5 * sizeof(option_info));
                            
        temp = (pselect->tp_rect.bottom - pselect->tp_rect.top + 1) / 3;
        pselect->opt_info[2].pos = pselect->tp_rect.top + (pselect->tp_rect.bottom - pselect->tp_rect.top + 1) / 2;
        pselect->opt_info[2].color = pselect->font_color;
        pselect->opt_info[2].percent = 100;
        pselect->opt_info[1].pos = pselect->opt_info[2].pos - temp;
        pselect->opt_info[1].color = pselect->sel_use.sel_slide.endpoint_color;
        pselect->opt_info[1].percent = pselect->sel_use.sel_slide.endpoint_zoom;
        pselect->opt_info[0].pos = pselect->opt_info[2].pos   - 2 * (pselect->tp_rect.bottom - pselect->tp_rect.top + 1) / 3;;
        pselect->opt_info[0].color = pselect->sel_use.sel_slide.endpoint_color;
        pselect->opt_info[0].percent = pselect->sel_use.sel_slide.endpoint_zoom;
        pselect->opt_info[3].pos = pselect->opt_info[2].pos   +  temp;
        pselect->opt_info[3].color = pselect->sel_use.sel_slide.endpoint_color;
        pselect->opt_info[3].percent = pselect->sel_use.sel_slide.endpoint_zoom;
        pselect->opt_info[4].pos = pselect->opt_info[2].pos   +  2 * (pselect->tp_rect.bottom - pselect->tp_rect.top + 1) / 3;
        pselect->opt_info[4].color = pselect->sel_use.sel_slide.endpoint_color;
        pselect->opt_info[4].percent = pselect->sel_use.sel_slide.endpoint_zoom;

        if(num1 > num - 1)
        {
            num1 = num - 1;
        }
        pselect->opt_info[2].option = num1;
        pselect->opt_info[1].option = option_sub(pselect->opt_info[2].option, 1, num);
        pselect->opt_info[0].option = option_sub(pselect->opt_info[1].option, 1, num);
        pselect->opt_info[3].option = option_add(pselect->opt_info[2].option, 1, num);
        pselect->opt_info[4].option = option_add(pselect->opt_info[3].option, 1, num);


    }
    else
    {

        pselect->opt_info = my_malloc(7 * sizeof(option_info));
  
        pselect->opt_info[3].pos = pselect->tp_rect.top + (pselect->tp_rect.bottom - pselect->tp_rect.top + 1) / 2;
        pselect->opt_info[3].color = pselect->font_color;
        pselect->opt_info[3].percent = 100;
        pselect->opt_info[2].pos = pselect->opt_info[3].pos - (pselect->tp_rect.bottom - pselect->tp_rect.top + 1) / 5;
        pselect->opt_info[2].color = mix_color(pselect->font_color, pselect->sel_use.sel_slide.endpoint_color, 50);
        pselect->opt_info[2].percent = (pselect->sel_use.sel_slide.endpoint_zoom + 100) / 2;
        pselect->opt_info[1].pos = pselect->opt_info[3].pos - 2 * (pselect->tp_rect.bottom - pselect->tp_rect.top + 1) / 5;
        pselect->opt_info[1].color = pselect->sel_use.sel_slide.endpoint_color;
        pselect->opt_info[1].percent = pselect->sel_use.sel_slide.endpoint_zoom;
        pselect->opt_info[0].pos = pselect->opt_info[3].pos - 3 * (pselect->tp_rect.bottom - pselect->tp_rect.top + 1) / 5;
        pselect->opt_info[0].color = pselect->sel_use.sel_slide.endpoint_color;
        pselect->opt_info[0].percent = pselect->sel_use.sel_slide.endpoint_zoom;
        pselect->opt_info[4].pos = pselect->opt_info[3].pos   +  (pselect->tp_rect.bottom - pselect->tp_rect.top + 1) / 5;
        pselect->opt_info[4].color = mix_color(pselect->font_color, pselect->sel_use.sel_slide.endpoint_color, 50);
        pselect->opt_info[4].percent = (pselect->sel_use.sel_slide.endpoint_zoom + 100) / 2;
        pselect->opt_info[5].pos = pselect->opt_info[3].pos   +  2 * (pselect->tp_rect.bottom - pselect->tp_rect.top + 1) / 5;
        pselect->opt_info[5].color = pselect->sel_use.sel_slide.endpoint_color;
        pselect->opt_info[5].percent = pselect->sel_use.sel_slide.endpoint_zoom;
        pselect->opt_info[6].pos = pselect->opt_info[3].pos   +  3 * (pselect->tp_rect.bottom - pselect->tp_rect.top + 1) / 5;
        pselect->opt_info[6].color = pselect->sel_use.sel_slide.endpoint_color;
        pselect->opt_info[6].percent = pselect->sel_use.sel_slide.endpoint_zoom;

        if(num1 > num - 1)
        {
            num1 = num - 1;
        }
        pselect->opt_info[3].option = num1;
        pselect->opt_info[2].option = option_sub(pselect->opt_info[3].option, 1, num);
        pselect->opt_info[1].option = option_sub(pselect->opt_info[2].option, 1, num);
        pselect->opt_info[0].option = option_sub(pselect->opt_info[1].option, 1, num);
        pselect->opt_info[4].option = option_add(pselect->opt_info[3].option, 1, num);
        pselect->opt_info[5].option = option_add(pselect->opt_info[4].option, 1, num);
        pselect->opt_info[6].option = option_add(pselect->opt_info[5].option, 1, num);
    }
}


static void select_draw_screen(base_touch * tp)
{


    if(tp->type == CFG_SEL)
    {
        select_touch * pselect = (select_touch *) tp;

        if(pselect->use == 0)
        {
			if(pselect->opt_info)
			{
				update_option_infmation(pselect);
			}
			else
			{
			
				init_options_infmation(pselect);
			}
        }
        else
        {
            update_draw_text(pselect);
        }

        select_show_font(pselect);
    }


}




 static void select_touch_back(TimerHandle_t timer)
 {
     select_touch * pselect = pvTimerGetTimerID(timer);
     pselect->screen_update = 1; //更新界面  
 
 }










uint8_t select_create(uint8_t * buffer)
{
    select_touch * pselect = (select_touch *) my_malloc(sizeof(select_touch));
    uint16_t index = 0;
    uint8_t i;


    if(pselect == NULL)
    {
        return 1;
    }
    memset((void *)pselect, 0, sizeof(select_touch));
	
	
    pselect->ctrl.id = ((buffer[0] << 24) | (buffer[1] << 16) | (buffer[4] << 8) | (buffer[5]) << 0);
    pselect->ctrl.func_ctrl_procmsg = select_msg;
    pselect->ctrl.func_ctrl_release = select_release;


    pselect->tp_type = buffer[6];
    pselect->tp_rect.left = (buffer[7] << 8) | (buffer[8]);
    pselect->tp_rect.top = (buffer[9] << 8) | (buffer[10]);
    pselect->tp_rect.right = (buffer[11] << 8) | (buffer[12]);
    pselect->tp_rect.bottom = (buffer[13] << 8) | (buffer[14]);
    pselect->pic_id = (buffer[2] << 8) | (buffer[3]);
    pselect->pic_on = (buffer[15] << 8) | (buffer[16]);
	pselect->screen_update = 0;
    pselect->act = select_act;
    pselect->draw_screen = select_draw_screen;
    pselect->use = buffer[17];
//    if(buffer[18] < src_cfg.font_set->font_num)
//    {
//	    pselect->ptfont = get_font_type(buffer[18]);
//	}

	pselect->ptfont = get_font_type(buffer[18]);
	
    pselect->font_color = (buffer[19] << 8) | buffer[20];


    if(pselect->use == 0)
    {
        pselect->sel_use.sel_slide.endpoint_color = (buffer[21] << 8) | (buffer[22]);
        pselect->sel_use.sel_slide.endpoint_zoom = 100 - buffer[23];
        pselect->sel_use.sel_slide.option_num = buffer[24];
        pselect->sel_use.sel_slide.option = &buffer[25];
        index = 26;
    }
    else
    {
        pselect->sel_use.sel_show.option = &buffer[21];
        pselect->sel_use.sel_show.h_align = buffer[22];
        pselect->sel_use.sel_show.v_align = buffer[23];
        index = 24;
    }


    pselect->select_num = buffer[index];
   
    index += 1;


    if(pselect->select_num)
    {
        pselect->pstr_id = (uint16_t *) my_malloc(pselect->select_num * sizeof(uint16_t));
        memset(pselect->pstr_id, 0, pselect->select_num * sizeof(uint16_t));

        for(i = 0; i < pselect->select_num; i++)
        {
            pselect->pstr_id[i] = (buffer[index] << 8) | (buffer[index+1]);
			index += 2;
        }
    }

    pselect->pos = 0;
    pselect->old_pos = 0;
    pselect->state = 0;
    
    
    if(pselect->use)
    {
		pselect->ctrl.active = 0;
        init_draw_text(pselect);
    }
    
    pselect->timer = xTimerCreate((const char *) "select",
								(TickType_t)50*MS,
								(UBaseType_t)pdTRUE,
								(void*	)pselect,
								(TimerCallbackFunction_t)select_touch_back);
								
    ctrl_init((t_ctrl *) pselect);

    return 0;
}











t_ctrl * sselect_create(uint8_t * buffer)
{
    select_touch * pselect = (select_touch *) my_malloc(sizeof(select_touch));
    uint16_t index = 0;
    uint8_t i;


    if(pselect == NULL)
    {
        return NULL;
    }
    memset((void *)pselect, 0, sizeof(select_touch));
	
	
    pselect->ctrl.id = ((buffer[0] << 24) | (buffer[1] << 16) | (buffer[4] << 8) | (buffer[5]) << 0);
    pselect->ctrl.func_ctrl_procmsg = select_msg;
    pselect->ctrl.func_ctrl_release = select_release;


    pselect->tp_type = buffer[6];
    //pselect->tp_rect.left = (buffer[7] << 8) | (buffer[8]);
    //pselect->tp_rect.top = (buffer[9] << 8) | (buffer[10]);
    //pselect->tp_rect.right = (buffer[11] << 8) | (buffer[12]);
    //pselect->tp_rect.bottom = (buffer[13] << 8) | (buffer[14]);
    //pselect->pic_id = (buffer[2] << 8) | (buffer[3]);
    //pselect->pic_on = (buffer[15] << 8) | (buffer[16]);
	pselect->screen_update = 0;
    //pselect->act = select_act;
    //pselect->draw_screen = select_draw_screen;
    pselect->use = buffer[17];
    /*
    if(buffer[18] < src_cfg.font_set->font_num)
    {
	    pselect->font = src_cfg.font_set->pfont[buffer[18]];
	}
	else
	{
	    pselect->font.addr = 0;
	    pselect->font.code = 0xFF;
	    pselect->font.width = 0;
	}
	*/
	
    //pselect->font_color = (buffer[19] << 8) | buffer[20];


    if(pselect->use == 0)
    {
        //pselect->sel_use.sel_slide.endpoint_color = (buffer[21] << 8) | (buffer[22]);
        //pselect->sel_use.sel_slide.endpoint_zoom = buffer[23];
        //pselect->sel_use.sel_slide.option_num = buffer[24];
        pselect->sel_use.sel_slide.option = &buffer[25];
        index = 26;
    }
    else
    {
        pselect->sel_use.sel_show.option = &buffer[21];
        //pselect->sel_use.sel_show.h_align = buffer[22];
        //pselect->sel_use.sel_show.v_align = buffer[23];
        index = 24;
    }

/*

    pselect->language_num = buffer[index];
    pselect->plang = &(buffer[index + 1]);
    if(*(pselect->plang) == 0xff)
    {
        *(pselect->plang) = 0;
    }
*/

    index += 1;

    if(pselect->select_num)
   {
       pselect->pstr_id = (uint16_t *) my_malloc(pselect->select_num * sizeof(uint16_t));
       memset(pselect->pstr_id, 0, pselect->select_num * sizeof(uint16_t));

       for(i = 0; i < pselect->select_num; i++)
       {
           pselect->pstr_id[i] = (buffer[index] << 8) | (buffer[index+1]);
		   index += 2;
       }
   }

    /*

    pselect->pos = 0;
    pselect->old_pos = 0;
    pselect->state = 0;
    if(pselect->use == 0)
    {
			
        init_options_infmation(pselect);
    }
    else
    {
		pselect->ctrl.active = 0;
        init_draw_text(pselect);
    }
	*/
    ctrl_init((t_ctrl *) pselect);
    
    return (t_ctrl *) pselect;
}



/***************************************************************************************
* End of File: select.c

****************************************************************************************/
