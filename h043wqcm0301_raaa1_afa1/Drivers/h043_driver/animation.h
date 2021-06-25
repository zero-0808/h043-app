#ifndef _ANIMATION_H_
#define _ANIMATION_H_

#include "gui.h"
#include "icon.h"
#include "timers.h"




typedef  struct animation
{
    t_ctrl ctrl;
	uint8_t type;
	t_rect tp_rect;
	short pic_id;
	short pic_on;
	void (*act)(struct base_touch *,uint8_t);
	void (*draw_screen)(struct   animation *);
	//void (*update_screen)(struct base_touch *,uint8_t flag);
	//void (*cmd_update_status)(struct base_touch *,uint8_t *,uint16_t);
	//void (*cmd_get_status)(struct base_touch *);
	uint8_t screen_update;

    //bdc         back_image;
	uint8_t  *  init_show;
	uint8_t  *   auto_play;
	uint16_t    frame_time;
	uint16_t    play_times;
	uint8_t  *  times;
	uint8_t     play_end;
	uint8_t     touch_act;
	uint16_t    step_length;
	uint8_t *   frame_num;
	TimerHandle_t timer;
	uint8_t     state;
	icon        animation_icon;
	
}animation;

uint8_t animation_create(uint8_t * buffer);
t_ctrl * sanimation_create(uint8_t * buffer);


#endif /*_ANIMATION_H_*/





