#ifndef _SLIDER_H_
#define _SLIDER_H_



#include "gui.h"
#include "icon.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"



typedef enum touch_rect
{
    touch_null,
    
    touch_scale,
    touch_vernier,
    
}touch_rect;




typedef struct slider 
{
    t_ctrl ctrl;
	uint8_t  tp_type;
	t_rect tp_rect;
	short pic_id;
	short pic_on;
	void (*act)(base_touch *,uint8_t);	
	void (*draw_screen)(base_touch *);
	uint8_t screen_update;
	void (*cmd_update_status)(struct slider *,uint8_t *,uint16_t);
	void (*cmd_get_status)(struct   slider *);
	touch_rect rect;
    uint8_t notify;
    uint8_t slider_dir;
    uint8_t slider_class;
    uint16_t start_margin;
    uint16_t end_margin;
    uint16_t vernier_offset;
    uint16_t vernier_width;
    uint16_t vernier_height;
    icon slider_icon;
	t_rect touch;
	//bdc back_image;
	uint32_t start_value;
	uint32_t end_value;
	uint32_t value;
	uint32_t old_value;
	uint8_t state;
	uint8_t * pvalue;
	TimerHandle_t timer;
}slider;


t_ctrl * sslider_create(uint8_t * buffer);
uint8_t slider_create(uint8_t * buffer);



#endif /*_SLIDER_H_*/

