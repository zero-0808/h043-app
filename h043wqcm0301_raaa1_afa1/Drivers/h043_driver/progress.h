#ifndef _PROGRESS_H_
#define _PROGRESS_H_

//#include"lpc_types.h"
#include "stm32h7xx_hal.h"
#include "gui.h"
#include "font.h"






typedef  enum
{
    h_lelft,
    h_right,
    v_top,
    v_bottom,   
}bar_dir;



typedef enum color_type
{
    col_transprant,
    col_color,
    col_image,   
}color_type;




typedef enum notiy_type
{
    no_notiy,
    press_notiy,
    release_notiy,
}notiy_type;




typedef enum show_type
{
    show_blank,
    show_center,
    show_end,
    
}show_type;






typedef struct prog_bar
{
    t_ctrl ctrl;
	uint8_t  tp_type;
	t_rect tp_rect;
	uint16_t pic_id;
	uint16_t pic_on;
	void (*act)(base_touch *, uint8_t);
	void (*draw_screen)(base_touch *);
	uint8_t screen_update;
	void (*cmd_update_status)(struct prog_bar *,uint8_t *,uint16_t);
	void (*cmd_get_status)(struct prog_bar *);
	
	bar_dir dir;
	color_type fore_color_type;
	color_type back_color_type;
	uint16_t fore_color;
	uint16_t  back_color;
	dc fore_image;
	//bdc back_image;
	notiy_type notiy;
	font_type * ptfont;
	uint16_t font_color;
	uint8_t percent;
	show_type font_pos;
	
	

	uint32_t start;
	uint32_t end;
	uint32_t value;
	uint32_t old_value;

	uint8_t state;
	uint8_t * pvalue;
	

	//void (* prog_act)(struct prog_bar *,t_msg *);
    
}prog_bar;



t_ctrl * sprog_bar_create(uint8_t * buffer);
uint8_t prog_bar_create(uint8_t * buffer);






#endif /*_PROGRESS_H_*/

