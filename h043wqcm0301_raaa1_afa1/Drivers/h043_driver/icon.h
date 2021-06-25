#ifndef _ICON_H_
#define _ICON_H_



#include "gui.h"


typedef struct
{
    uint16_t width;
    uint16_t height;
    uint8_t * p;
    
}icon_image;





typedef struct icon_head
{
    uint8_t icon[4];
    uint16_t width;
    uint16_t height;
    uint16_t reserve[5];
}icon_head;


typedef struct icon 
{
    uint32_t addr;
    uint16_t width;
    uint16_t height;
    uint16_t num;
    uint16_t count;
    uint8_t byte;
    uint8_t * p;
}icon;









void show_icon_image(icon ticon,icon_image image);
void show_image(uint8_t *image,uint16_t width,uint16_t height,t_rect rect,uint8_t *framebuf);
uint8_t get_icon_image(icon *picon,uint16_t count);
uint8_t init_icon(icon * picon,uint32_t addr);
void show_icon_image_mix(icon ticon,t_rect rect,uint8_t *framebuf);
void show_vicon_image(icon_image image,uint16_t x,uint16_t y,t_rect rect,uint8_t *framebuf);






#endif /*_ICON_H_*/

