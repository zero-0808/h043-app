#ifndef _TP_TEST_H_
#define _TP_TEST_H_

#include "ctrl.h"
#include "gui.h"

typedef struct 
{
    t_ctrl ctrl;
    uint8_t  tp_type;
    t_rect tp_rect;
    uint16_t pic_id;
    uint16_t pic_on;
    void (*act)(base_touch *, uint8_t);
    void (*draw_screen)(base_touch *);
	uint8_t screen_update;
	uint16_t x;
	uint16_t y;
	uint8_t state;

}tp_touch;


#endif
