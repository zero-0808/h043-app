#ifndef _MENU_H_
#define _MENU_H_

#include "gui.h"
#include "icon.h"
#include "font.h"
//#include "text.h"

typedef struct 
{
   uint8_t color;
   uint16_t back_color;
   uint16_t fore_color;
}facade_color;










typedef  struct
{
    t_ctrl ctrl;
    uint8_t  tp_type;
    t_rect tp_rect;
    uint16_t pic_id;
    uint16_t pic_on;
    void (*act)(base_touch *, uint8_t);
    void (*draw_screen)(base_touch *);
	uint8_t screen_update;

    uint8_t facade;
	font_type *pfont;
	uint16_t back_color;
	uint16_t fore_color;

	
	icon up_icon;
	icon down_icon;
	uint32_t text_id;

	uint8_t style;
	uint8_t dir;
	uint8_t menu_num;
	uint8_t num;
	//uint8_t language_num;
	//uint8_t * planguage_num;
	uint16_t * pstr_id;
	uint8_t state;

}menu;

uint8_t menu_create(uint8_t * buffer);



#endif /*_MENU_H_*/

