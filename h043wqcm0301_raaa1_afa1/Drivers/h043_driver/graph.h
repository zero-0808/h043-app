#ifndef _GRAPH_H_
#define _GRAPH_H_

#include "gui.h"

typedef struct
{
    uint16_t len;
    uint16_t color;
    uint8_t * sample;
}channel_info;

typedef struct
{
    uint8_t channel_sta;
    uint8_t sample_depth;
    uint16_t sample_num;
    channel_info * channel;
    
    
}graph_info;


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
	
	
	
	uint8_t anti;
	uint8_t dir;
	uint8_t line_num;
	uint8_t zoom;
	uint16_t start_value;
	uint16_t end_value;
	uint8_t channel_num;
	graph_info * pgraph;

}tgraph;

uint8_t graph_create(uint8_t * buffer);


#endif /*_GRAPH_H_*/

