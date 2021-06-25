#ifndef  __FONT_H__
#define  __FONT_H__

//#include "lpc_types.h"
#include "stm32h7xx_hal.h"
#include "gui.h"

typedef struct
{
	uint8_t num;
    uint8_t code;
    uint8_t width;
	uint8_t height;
    uint32_t addr;
}font_type;

typedef struct
{
    uint8_t font_num;
    font_type * pfont;   
}font_info;

//void get_arabic_offset(uint8_t* offset,uint8_t* remain,uint16_t code,font_type font);

uint8_t get_gb_font(uint16_t code,font_type font,uint8_t * hzk);
uint8_t   get_gbk_font(uint16_t code,font_type font,uint8_t * hzk);
uint8_t   get_unicode_font(uint16_t code,font_type font,uint8_t * hzk);

void show_char_font(uint16_t x,uint16_t y,uint16_t color,uint16_t back_color,uint8_t code,font_type type);
void show_charn_font(uint16_t x,uint16_t y,uint16_t color,uint8_t code,font_type type);
void show_charn_buffer(uint16_t x,uint16_t y,uint16_t color,uint8_t code,font_type type,uint8_t * buf);
void show_char_buffer(uint16_t x,uint16_t y,uint16_t color,uint16_t back_color,uint8_t code,font_type type,uint8_t * 
buf);

void show_font(uint16_t x,uint16_t y,uint16_t color,uint16_t back_color,uint16_t code,font_type type);
void show_nfont(uint16_t x,uint16_t y,uint16_t color,uint16_t code,font_type type);
void show_nfont_buffer(uint16_t x,uint16_t y,uint16_t color,uint16_t code,font_type type,uint8_t * buf);
void show_font_buffer(uint16_t x,uint16_t y,uint16_t color,uint16_t back_color,uint16_t code,font_type type,uint8_t * buf);
void show_nfont_rect(uint16_t x,uint16_t y,uint16_t color,uint16_t code,font_type font,t_rect rect);
void show_charn_font_rect(uint16_t x,uint16_t y,uint16_t color,uint8_t code,font_type font,t_rect rect);

void put_charn_pixel_dc(uint16_t x,uint16_t y,uint16_t color,uint8_t *hzk,uint16_t size,dc tdc);
void show_charn_dc(uint16_t x,uint16_t y,uint16_t color,uint8_t code,font_type font,dc tdc);
void show_nfont_dc(uint16_t x,uint16_t y,uint16_t color,uint16_t code,font_type font,dc tdc);
font_type * get_font_type(uint8_t font_num);







uint8_t get_font_width(font_type type);
uint8_t get_font_height(font_type type);
font_type * ch_to_hz(font_type * font);
font_type * hz_to_ch(font_type * font);







#endif 





