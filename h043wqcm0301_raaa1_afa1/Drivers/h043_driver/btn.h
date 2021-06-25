#ifndef _BTN_H_
#define _BTN_H_

//#include "lpc_types.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "gui.h"
#include "mem_my.h"
#include "string.h"
#include "timers.h"
#include "str.h"




typedef struct btn_cmd_info
{
    uint32_t addr;
    uint16_t len;
}btn_cmd_info;



typedef struct btn_screen
{
    uint8_t status;
	uint8_t old_status;
	uint16_t screen;
	//uint8_t *pstatus;
}btn_screen;


    typedef enum key_type
    {
        key_char,
        key_enter,
        key_clear,
        key_backspace,
        key_esc,
        key_shift,
        key_left,
        key_right,
        key_space,
        key_pm,
        key_engchs,
    
    }key_type;



typedef struct btn_key
{
    uint8_t status;
    uint8_t old_status; 
    key_type type;
    uint8_t value;
    //uint8_t *pstatus;
}btn_key;








typedef enum btn_switch_style
{
	transient,
	on_off,
	turn_on,
	turn_off,
	sustain,
	
}btn_switch_style;




typedef enum btn_use
{
	switch_screen,
	switch_btn,
	custom_key,
	custom_cmd,
	popup_list	
	
}btn_use;




typedef struct btn_cmd
{
    uint8_t status;
	uint8_t old_status; 
	uint32_t up_addr;
	uint16_t up_len;

	uint32_t down_addr;
	uint16_t down_len;
	
	uint16_t  times;
	//uint8_t *pstatus;
	
}btn_cmd;



typedef struct btn_list
{
    uint8_t status;
	uint8_t old_status; 
	uint16_t menu;
	uint16_t text;
	//uint8_t *pstatus;
}btn_list;







typedef struct btn_switch
{
	
	uint8_t status;
	uint8_t old_status; 
	
	btn_switch_style style;
	
	uint32_t up_in_addr;
	uint16_t up_in_len;
	uint32_t down_in_addr;
	uint16_t  down_in_len;
	
	uint32_t up_out_addr;
	uint16_t up_out_len;
	uint32_t down_out_addr;
	uint16_t down_out_len;
	
	uint16_t times;
	TimerHandle_t timer1;          //���ڲ������Ϊ����ʱʹ��
	TimerHandle_t timer2; 
	
	uint8_t *pstatus;
	
	
}btn_switch;







typedef struct btn
{
	t_ctrl ctrl;
	uint8_t  tp_type;
	t_rect tp_rect;
	short pic_id;
	short pic_on;
	void (*act)(base_touch *,uint8_t);
	void (*draw_screen)(base_touch *);
	uint8_t screen_update;
	void (*cmd_update_status)(struct btn *,uint8_t *,uint16_t);
	void (*cmd_get_status)(struct btn *);
	dc depress;
	//bdc release;
	uint8_t virtual_fame;
	uint8_t notify;
	font_type * ptfont;
	uint16_t rcolor;
	uint16_t pcolor;
	uint16_t rstr_id;
	uint16_t pstr_id;
	draw_text  * pdraw;
	btn_use use;
	uint8_t state;
	void * tp_use;
	
}t_btn;


uint8_t btn_create(uint8_t * buffer);
t_ctrl * sbtn_create(uint8_t * buffer);
uint8_t keyboard_btn_create(uint16_t x, uint16_t y,uint8_t * buffer);
void btn_key_delete(void);




#endif /*_BTN_H_*/

