#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include "gui.h"



typedef struct keyboard
{
    uint16_t screen_id;
//	uint16_t width;
//	uint16_t height;
    uint8_t  active;
	//SemaphoreHandle_t mutex_sem;
       
}keyboard;

typedef struct
{
    t_ctrl ctrl;
    uint8_t type;
    t_rect tp_rect;
    short pic_id;
    short pic_on;
    void (*act)(struct base_touch *,uint8_t);
    void (*draw_screen)(struct base_touch *);
    uint8_t screen_update;
    dc image;
}pop_keyboard;




typedef struct keyboard_info
{
    uint8_t keyboard_num;
    uint8_t status;
    uint8_t flag;
    SemaphoreHandle_t sem;
    uint16_t x;
    uint16_t y;
    
}keyboard_info;



extern keyboard  small_keyboard,full_keyboard;
extern keyboard_info kb_info;



void system_keyboard_init(void);


#endif /*_KEYBOARD_H_*/

