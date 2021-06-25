#ifndef _RTC_H_
#define _RTC_H_


#include "FreeRTOS.h"
#include "timers.h"
#include "font.h"
#include "gui.h"




typedef struct rtc_time
{
	uint16_t year;
	char  month;
	char  day;
	char  week;
	char  hour;
	char  min;
	char  sec;
}rtc_time;



typedef struct rtc_device
{
	void (*init)(void);
	void (*set_time)(rtc_time);
	void (*get_time)(rtc_time *);
	SemaphoreHandle_t  sem;
	rtc_time time;
	uint8_t update_flag;

} rtc_device;





typedef struct rtc_counter
{
    uint8_t code_num;
    uint8_t * code;
    uint32_t counter;
    uint32_t counter_value;
    uint8_t count_type;
    font_type * ptfont;
    uint16_t color;
    uint8_t start;
      
}rtc_counter;





typedef struct rtc_clock
{
   uint8_t code_num;
   uint8_t * code;
   uint8_t language;
   uint8_t time_lag;
   font_type *ptfont;
   uint16_t color;
   uint8_t set_type;
   rtc_time time;
   
}rtc_clock;




typedef union rtc_format
{
    rtc_counter counter;
    rtc_clock  clock;
     
}rtc_format;


typedef struct rtc_tp
{
    t_ctrl ctrl;
	uint8_t  tp_type;
	t_rect tp_rect;
	short pic_id;
	short pic_on;
	void (*act)(base_touch *,uint8_t);
	void (*draw_screen)(base_touch *);

	//void (*cmd_update_status)(struct base_touch *,uint8_t *,uint16_t);
	//void (*cmd_get_status)(struct base_touch *);
	uint8_t screen_update;
	uint8_t active;
	//bdc back_image;
	uint8_t format;
	rtc_format rtc_type;
	uint8_t state;
	
}rtc_tp;

extern rtc_device rtc_dev;
void create_rtc_touch(void);




#endif /*_RTC_H_*/

