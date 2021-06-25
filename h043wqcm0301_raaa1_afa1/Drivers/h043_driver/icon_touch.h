#ifndef _ICON_TOUCH_H_
#define _ICON_TOUCH_H_

#include "gui.h"
#include "icon.h"




typedef struct icon_touch
{
    t_ctrl ctrl;
    uint8_t type;
    t_rect tp_rect;
    short pic_id;
    short pic_on;
    void (*act)(struct base_touch *,uint8_t);
    void (*draw_screen)(struct base_touch *);
    //void (*update_screen)(struct base_touch *,uint8_t );
    //void (*cmd_update_status)(struct base_touch *,uint8_t *,uint16_t);
    //void (*cmd_get_status)(struct base_touch *);
    uint8_t screen_update;
    
    //bdc         back_image;
	uint8_t  *  init_show;
	uint8_t     touch_act;
	uint16_t    step_length;
	uint8_t *   frame_num;
	uint8_t     state;
	icon        touch_icon;
    
}icon_touch;


uint8_t icon_create(uint8_t * buffer);
t_ctrl * sicon_create(uint8_t * buffer);





#endif /*_ICON_TOUCH_H_*/

