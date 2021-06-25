#include "pop_win.h"
#include "cfg.h"
#include "mem_my.h"
#include "display.h"
#include "string.h"
#include "textbox.h"
#include "animation.h"
#include "icon_touch.h"
#include "menu.h"
#include "btn.h"
#include "progress.h"
#include "rtc.h"
#include "graph.h"
#include "slider.h"
#include "select.h"


win_operate  g_win;



uint8_t rtc_create(uint8_t * buffer);


static uint8_t pop_win_msg(void *p, t_msg * p_msg)
{
    return 1;
}

static void pop_win_release( void *p_ctrl )
{
    pop_win * ptwin = (pop_win *) p_ctrl;

    if(ptwin->image.p)
    {
        my_free(ptwin->image.p);
    }
    my_free(p_ctrl);
}


static void pop_win_act(base_touch * p_btn, uint8_t flag)
{
    return;
}



static uint8_t * find_touch_screen(uint16_t screen)
{
	uint8_t * p;
	uint16_t pic_id;
	uint16_t len;



	p = src_cfg.p_touch;

	while(1)
	{
		len = (p[0] << 8) | p[1];
		if(len == 0)
		{
			break;
		}
		pic_id = (p[4] << 8) | p[5];


		if(pic_id == screen)
		{
			return p;
		}

		if(pic_id > screen )
		{
			return NULL;
		}

		p += len;


	}

	return NULL;
}



static uint8_t * find_touch_cfg(uint32_t id)
{
    uint8_t * p;
    uint16_t screen,len,pic_id;
    uint32_t temp_id;

    screen = id >> 16;

    p = find_touch_screen(src_cfg.ptscreen->screen_pic[screen]);

    if(p==NULL)
    {
        return NULL;
    }


    while(1)
    {

        len = (p[0] << 8) | p[1];

    	if(len == 0)
    	{
    		return NULL;
    	}
    	pic_id = (p[4] << 8) | p[5];

    	if(pic_id != src_cfg.ptscreen->screen_pic[screen])
    	{
    		return NULL;
    	}

    	temp_id = (p[2] << 24) | (p[3] << 16) | (p[6] << 8) | (p[7]);

    	if(id == temp_id)
    	{
    	    return &p[2];
    	}

		p += len;


	}









}





static void win_draw_screen(base_touch * tp)
{
    if(tp->type == CFG_WIN)
    {
        dc ddc;
        pop_win * ptwin = (pop_win *) tp;
        uint16_t x,y;
        t_rect rect;
        x = ptwin->tp_rect.left;
	    y = ptwin->tp_rect.top;
	    rect.left = 0;
	    rect.right = ptwin->image.width;
	    rect.top = 0;
		rect.bottom = ptwin->image.height;



        if(ptwin->image.p == NULL)
        {
            ptwin->image.p = my_malloc(ptwin->image.height * ptwin->image.width * disp_dev.bpp);
            get_image(ptwin->image,ptwin->origin_x,ptwin->origin_y, ptwin->pic_id);

        }

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
		else if(disp_dev.dir == 1)
		{
		    ddc = disp_dev.disp[3];
		}
		vblt(ddc,x,y,rect,ptwin->image);
    }
}



static uint8_t  rect_over(t_rect rect0,t_rect rect1)
{
	if(rect0.left > rect1.left || rect0.top > rect1.top ||
	    rect0.right < rect1.right || rect0.bottom < rect1.bottom)
	 {
	    return 0;
	 }


	 return 1;

}







void hide_win(uint32_t id)
{
    pop_win * ptwin;

    ptwin =  (pop_win *)get_ctrl(id);

    if(ptwin == NULL || ptwin->type != CFG_WIN)
    {
        return ;
    }

    ctrl_release_screen(id >> 16);
    active_ctrl();
	extern uint8_t update_screen_key;
	update_screen_key = 1;




}





void pop_up_win(uint16_t x,uint16_t y,uint32_t id)
{


    uint8_t * p;


    uint8_t  type;
    uint16_t touch_id,temp_id;
    uint16_t len,screen;
    short sx,sy,ex,ey,dx,dy;
    t_rect rect0,rect1;
    uint32_t id0;
    base_touch * tp;

	tp = (base_touch *)get_ctrl(id);

	if(tp)
	{
		return;
	}


    temp_id = id >> 16;

    p = find_touch_cfg(id);

    if((x > disp_dev.width - 1) ||(y > disp_dev.height - 1))
    {
        return ;
    }

    if(p == NULL || p[6] != CFG_WIN)
    {
        return;
    }

    deactive_ctrl();

    pop_win * ptwin = NULL;

    ptwin = my_malloc(sizeof(pop_win));

    if(ptwin == NULL)
    {
        return ;
    }
    memset(ptwin,0,sizeof(pop_win));

    deactive_ctrl();


	sx = ((p[7] << 8) | (p[8]));
	sy = ((p[9] << 8) | p[10]);
	ex = ((p[11] << 8) | (p[12]));
	ey = ((p[13] << 8) | (p[14]));

	dx = x - sx;
	dy = y - sy;

	if(dx + ex > disp_dev.width - 1)
	{
	    dx = disp_dev.width - 1 - ex;
		//x = sx + dx;
	}

	if(dy + ey > disp_dev.height - 1)
	{
	    dy = disp_dev.height - 1 - ey;
		//y = sy +dy;
	}

    ptwin->ctrl.id = ((p[0] << 24) | (p[1] << 16) | (p[4] << 8) | (p[5] << 0));
    ptwin->ctrl.func_ctrl_procmsg  = pop_win_msg;
    ptwin->ctrl.func_ctrl_release  = pop_win_release;
    ptwin->screen_update = 1;

    ptwin->type = p[6];
    ptwin->tp_rect.left = sx + dx;
    ptwin->tp_rect.top= sy + dy;
    ptwin->tp_rect.right = ex + dx;
    ptwin->tp_rect.bottom = ey + dy;
    ptwin->pic_id   = (p[2] << 8) | (p[3]);;
    ptwin->pic_on   = -1;
    ptwin->act = pop_win_act;
    ptwin->draw_screen = win_draw_screen;
    ptwin->origin_x = ((p[7] << 8) | (p[8]));
    ptwin->origin_y = ((p[9] << 8) | p[10]);
    ptwin->image.p = NULL ;
    ptwin->image.width = ptwin->tp_rect.right - ptwin->tp_rect.left + 1;
    ptwin->image.height = ptwin->tp_rect.bottom - ptwin->tp_rect.top + 1;
    ctrl_init((t_ctrl *)ptwin);

	screen = get_pic_num(temp_id);


    p = find_touch_screen(screen);

    rect0.left = sx;
    rect0.right = ex;
    rect0.top = sy;
    rect0.bottom = ey;


    while(1)
    {
        len = (p[0] << 8) | p[1];
		if(len == 0)
		{
			return;
		}
		touch_id  = (p[2] << 8) | p[3];

		if(touch_id != temp_id)
		{
			return;
		}

		rect1.left = ((p[9] << 8) | (p[10]));
	    rect1.top = ((p[11] << 8) | p[12]);
	    rect1.right = ((p[13] << 8) | (p[14]));
	    rect1.bottom = ((p[15] << 8) | (p[16]));


		if(rect_over(rect0,rect1))
		{
		    id0 = ((p[2] << 24) | (p[3] << 16) | (p[6] << 8) | (p[7] << 0));
		    type = p[8];

    		switch(type)
    		{
    			case CFG_BTN:
    				btn_create(&p[2]);
    				break;
    		  case CFG_BAR:
    				prog_bar_create(&p[2]);
    				break;
    			case CFG_SLI:
    				slider_create(&p[2]);
    				break;
    			case CFG_TEX:
    				textbox_create(&p[2]);
    				break;
    			case CFG_RTC:
    				rtc_create(&p[2]);
    				break;
    			case CFG_ANI:
    				animation_create(&p[2]);
    			    break;
    			case CFG_GRA:
    				graph_create(&p[2]);
    				break;
    			case CFG_ICO:
    				icon_create(&p[2]);
    				break;
//    			case CFG_STE:
//    				text_create(&p[2]);
//    				break;
    			case CFG_STR:
    				str_touch_create(&p[2]);
    				break;
    			case CFG_SEL:
    				select_create(&p[2]);
    				break;
    			default:
    			    id0 = 0xFFFFFFFF;
    				break;
    		}

            tp = (base_touch *)get_ctrl(id0);

            if(tp)
            {
                tp->tp_rect.left += dx;
                tp->tp_rect.top += dy;
                tp->tp_rect.right += dx;
                tp->tp_rect.bottom += dy;
                tp->screen_update = 1;
            }

		}

		p += len;

    }


    //keyboard_touch_create(x,y,small_keyboard.screen);


}


void win_task(void *pvParameters)
{
	g_win.sem = xSemaphoreCreateBinary();

    while(1)
    {

        xSemaphoreTake(g_win.sem,portMAX_DELAY);

        if(g_win.opt)
        {
           pop_up_win(g_win.x,g_win.y,g_win.id);
        }
        else
        {
            hide_win(g_win.id);
        }
    }
}


