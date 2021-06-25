#ifndef _TEXTBOX_H_
#define _TEXTBOX_H_

//#include "lpc_types.h"
#include "font.h"
#include "gui.h"
#include "icon.h"
#include "timers.h"
#include "str.h"




typedef struct text_font
{
    font_type * ptfont;
    uint16_t fore_color;
    uint8_t back_image_type;
    uint16_t back_color;
    dc back_image;
    uint8_t password;
    uint8_t frame;
    uint8_t h_align;
    uint8_t v_align;
    uint8_t line_space;
	uint8_t column_space;
   

}text_font;





typedef struct text_art
{
    icon  art_icon;
    uint8_t wdec;     //0:100%  1:50%  2:25%
    uint8_t frame;
    uint8_t h_align;
    uint8_t v_align;
    uint8_t line_space;
	uint8_t column_space;
   
}text_art;




typedef union text_type
{
    text_font font;
    text_art  art;
    
}text_type;



typedef struct
{
    uint8_t symboy;
    uint32_t integer; 
    uint8_t decimal[10];
}float_type;





typedef struct keyboard_input
{
    uint8_t key_type;
    uint8_t init_value;
    uint8_t limit;
    uint8_t decimal_num;
    float_type max_value;
    float_type min_value;
    float_type value;
}keyboard_input;



typedef union input_set
{
    keyboard_input keyboard;
    uint8_t pram;   
}input_set;



/*
typedef struct textbox_info
{
    uint8_t max_num;
    uint8_t num;
    //uint8_t max_dec_num;  //显示艺术字体小数点的最大数目
    //uint8_t dec_num; //显示艺术字体小数点的数目
    uint8_t line_num;
    //uint8_t line_dec_num;//一行能够显示的小数点的数目
    uint8_t num_line;
    uint16_t font_width;
    uint16_t dec_width;  //小数点的宽度
    uint16_t font_height;
    uint16_t x;
    uint16_t y;
    
}textbox_info;
*/

//typedef struct
//{
//    uint16_t width;
//    uint8_t len;
//    uint8_t *pcode; 
//}line_text;



//typedef struct
//{
//    uint8_t line_num;
//    uint8_t col_num;
//    uint8_t line;
//    line_text * pline;

//}draw_text;





typedef struct textbox
{
    t_ctrl ctrl;
	uint8_t  tp_type;
	t_rect tp_rect;
	uint16_t pic_id;
	uint16_t pic_on;
	void (*act)(base_touch *, uint8_t);
	void (*draw_screen)(base_touch *);
	uint8_t screen_update;
	//void (*update_screen)(base_touch *,uint8_t);
	void (*cmd_update_status)(struct textbox *,uint8_t *,uint16_t);
	void (*cmd_get_status)(struct   textbox *);

	uint8_t text_show;
	text_type text;

	uint8_t input_way;
	input_set input;
	uint8_t maxmum_code;
	uint8_t code_num;
	uint32_t *pcode_addr;
	uint8_t * code;
	draw_text * pdraw;
	uint8_t code_update;
	uint8_t cursor_flag;
	TimerHandle_t timer; 
	uint8_t state;
	uint8_t status;
	

}textbox;


uint8_t textbox_create(uint8_t * buffer);
uint8_t stextbox_create(uint8_t * buffer);
uint8_t keyboard_textbox_create(uint16_t a, uint16_t b,uint8_t * buffer);
void copy_to_box(textbox * pbox, uint8_t *str,uint8_t len);

extern textbox * active_box;
extern textbox * old_active_box;





#endif /*_TEXTBOX_H_*/

