#ifndef _RECORD_H_
#define _RECORD_H_

#include "gui.h"
#include "rtc.h"



typedef struct warn_event
{
    uint16_t warn_data;
    uint16_t color;
    uint16_t str_id;  
}warn_event;


typedef struct event_time
{
    uint16_t date;
    rtc_time time0;
    rtc_time time1;
	uint8_t flag;
}event_time;


typedef struct str_info
{
    uint8_t len;
    uint8_t * p;
    
}str_info;



typedef struct slider_bar
{
    t_rect rect;
    uint16_t color;
    short pos;
    short old_pos;
    uint8_t flag;
    TimerHandle_t timer;
    
}slider_bar;


typedef struct record_info
{
    uint16_t record_index;
    uint16_t first_show;
    uint16_t select_record;
    uint16_t record_num;
    str_info * event_str;
	event_time * event_wran;
    
}record_info;




typedef struct data_record
{
    
    t_ctrl ctrl;
	uint8_t type;
	t_rect tp_rect;
	short pic_id;
	short pic_on;
	void (*act)(struct base_touch *,uint8_t);
	void (*draw_screen)(struct base_touch *);
	uint8_t screen_update;

    /* 内部指令的处理函数 */
	void (*cmd_handler_add)(struct data_record *,uint8_t *, uint8_t);
	void (*cmd_handler_relieve)(struct data_record *,uint8_t *,uint8_t);
	void (*cmd_handler_clear)(struct data_record *,uint8_t *,uint8_t);
	void (*cmd_handler_offset)(struct data_record *, uint8_t *,uint8_t);
	void (*cmd_handler_get)(struct data_record *, uint8_t *,uint8_t);
	
	font_type *pfont;
	uint16_t font_color;
	uint8_t grid;
	uint16_t grid_color;
	uint8_t background;
	uint16_t background_color;
	uint8_t  select;
	uint8_t  show_slider;
	

	uint8_t record_type;
	uint8_t show_sort;
	uint8_t align;
	uint8_t subitem_num;
	uint16_t * subitem;
	uint16_t show_record;
	uint16_t max_record;
	
	uint8_t record_byte;
	uint8_t save;
	uint32_t addr;
	uint32_t size;
	
	uint8_t * mem_addr;
	uint16_t event_num;
	slider_bar slider;
	t_rect select_rect;
	warn_event * event_descrip;
	record_info * pinfo_record;
	uint8_t state;
	
}data_record;


uint8_t record_create(uint8_t * buffer);
t_ctrl * srecord_create(uint8_t * buffer);



#endif /*_RECORD_H_*/



