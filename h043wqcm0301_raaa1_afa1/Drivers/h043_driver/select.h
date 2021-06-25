
/***************************************************************************************
****************************************************************************************
* Project      : 20180802_HMI_43_TEST
* FILE         : select.h
* Description  : 
*                      
* Copyright (c) 2018 by ShenZhen JingHua Displays Co.,Ltd... All Rights Reserved.
* 
* History:
* Version		name			Date			Description
* 0.01			Qu Bingbing		2018/08/08		Initial Version
* 
****************************************************************************************
****************************************************************************************/
#include "gui.h"
#include "font.h"
#include "str.h"
#include "timers.h"

   
typedef struct select_slide
{
   uint16_t endpoint_color;
   uint8_t endpoint_zoom;
   uint8_t option_num;
   uint8_t * option;
   
   
}select_slide;




typedef struct select_show
{
    uint8_t * option;
    uint8_t h_align;
    uint8_t v_align;
}select_show;




typedef union select_use
{
    select_show  sel_show;
    select_slide sel_slide;

}select_use;

typedef struct option_info//用来记录中心位置
{
    uint8_t option;
    uint16_t color;
    uint8_t percent;
    short pos;
    
}option_info;

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
    font_type *ptfont;
    uint16_t font_color;
    uint8_t use;
    select_use sel_use;
    //uint8_t language_num;
    //uint8_t *plang;
    //language_info * planguage;
    uint8_t select_num;
    uint16_t * pstr_id;
    short pos;
    short old_pos;
    option_info * opt_info;
    draw_text  * pdraw;
    TimerHandle_t timer;
    uint8_t state; 
}select_touch;

uint8_t select_create(uint8_t * buffer);
t_ctrl * sselect_create(uint8_t * buffer);


/***************************************************************************************
* End of File: select.h

****************************************************************************************/
