#ifndef _POP_WIN_H_
#define _POP_WIN_H_

#include "gui.h"


typedef struct pop_win 
{

    t_ctrl ctrl;
	uint8_t  type;
	t_rect tp_rect;
	short pic_id;
	short pic_on;
	void (*act)(base_touch *,uint8_t);
	void (*draw_screen)(base_touch *);
	uint8_t screen_update;
	uint16_t origin_x;
	uint16_t origin_y;
	dc image;
	uint8_t  state;
}pop_win;


typedef struct win_operate
{
    uint32_t id;
	uint32_t opt;
    SemaphoreHandle_t sem;
    uint16_t x;
    uint16_t y;
    
}win_operate;


extern win_operate  g_win;


#endif /*_POP_WIN_H_*/

