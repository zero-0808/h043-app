#ifndef _STR_H_
#define _STR_H_

//#include "lpc_types.h"
#include "ctrl.h"
#include "gui.h"
#include "font.h"


typedef struct
{
    uint16_t width;
    uint8_t len;
    uint8_t *pcode;
    
}line_text;


typedef struct
{
    uint8_t line_num;
    uint8_t col_num;
    uint8_t line;
    line_text * pline;

}draw_text;


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

	uint8_t * pstr_id;
	uint16_t font_color;
	uint8_t h_align;
	uint8_t v_align;
	font_type * ptfont;
	draw_text  * pdraw;
	uint8_t state;

}str_touch;


uint8_t str_touch_create(uint8_t * buffer);
t_ctrl * sstr_touch_create(uint8_t * buffer);
uint8_t * get_str_code(uint16_t str_id,uint8_t * len);




#endif /*_STR_H_*/

