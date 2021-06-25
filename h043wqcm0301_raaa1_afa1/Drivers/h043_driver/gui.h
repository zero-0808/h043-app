#ifndef  __GUI_H__
#define  __GUI_H__



#include "ctrl.h"


#define BTN_STATE_INIT        0 // �Ƿ��ʼ����
#define BTN_STATE_ACTIVE      1 // �Ƿ��д��������㡣
#define BTN_STATE_LIGHT       2 // �Ƿ�������������ʾ��
#define BTN_STATE_HALFLIGHT   3 // �Ƿ�������������ʾ��
 
typedef struct{
	uint16_t left;
	uint16_t top;
	uint16_t right;
	uint16_t bottom;
} t_rect;

typedef struct layer_rect
{
    t_rect rect;
    struct layer_rect * next;
}layer_rect;

typedef struct dc
{
	uint8_t * p;
	uint16_t width;
	uint16_t height;
}dc;

typedef struct bdc
{
	uint16_t x;
	uint16_t y;
	uint16_t width;
	uint16_t height;
	dc vdc;
}bdc;

typedef struct base_touch
{
	t_ctrl ctrl;
	uint8_t type;
	t_rect tp_rect;
	short pic_id;
	short pic_on;
	void (*act)(struct base_touch *,uint8_t);
	void (*draw_screen)(struct base_touch *);
	uint8_t screen_update;
	//void (*init_screen)(struct base_touch *);
	//void (*update_screen)(struct base_touch *,uint8_t);
	//void (*cmd_update_status)(struct base_touch *,uint8_t *,uint16_t);
	//void (*cmd_get_status)(struct base_touch *);
}base_touch;

//typedef struct gui_layer
//{
//   dc layer;
//   layer_rect * head_rect;
//   struct gui_layer * next;
//}gui_layer;


uint8_t pt_in_rect( t_rect * p_rc, uint16_t x, uint16_t y );
void get_touch_image(dc vdc, base_touch * touch, uint8_t flag);
void get_image(dc image,uint16_t x,uint16_t y, uint16_t num);
void blt( uint8_t * dp, uint16_t dx, uint16_t dy, uint16_t dw, uint16_t dh, uint8_t * sp );
void vblt(dc ddc, uint16_t dx, uint16_t dy,t_rect rect,dc sdc );
void vbltdc(dc ddc, uint16_t dx, uint16_t dy,uint16_t width,uint16_t height, dc sdc );
void tvbltdc(dc ddc, uint16_t dx, uint16_t dy,uint16_t width,uint16_t height, dc sdc );
void vblt_bdc(dc ddc,bdc sdc);
void blt_dc_zoom(bdc ddc, dc sdc, t_rect rect);
uint8_t  rect_overlap(t_rect rect0,t_rect rect1,t_rect * prect);



//void vblt(uint8_t * dp, uint16_t dx, uint16_t dy, dc sdc );
void tvblt(uint8_t * dp, uint16_t dx, uint16_t dy, dc sdc );
void draw_virtual_hline(uint16_t xs,uint16_t ys,uint16_t xe);
void draw_virtual_vline(uint16_t xs,uint16_t ys,uint16_t xe);
//void save_virtual_hline(uint16_t xs,uint16_t ys,uint16_t xe,uint16_t * frame);
void save_virtual_vline(uint16_t xs,uint16_t ys,uint16_t xe,uint16_t * frame);
void recover_vline(uint16_t xs,uint16_t ys,uint16_t ye,uint16_t * frame);
void recover_hline(uint16_t xs,uint16_t ys,uint16_t ye,uint16_t * frame);
void draw_hline(uint16_t *dp,uint16_t x,uint16_t y,uint16_t len, uint16_t color);
void draw_vline(uint16_t *dp,uint16_t x,uint16_t y,uint16_t len, uint16_t color);
void draw_image_hline(uint16_t *dp,uint16_t x,uint16_t y,uint16_t len,dc sdc);
void draw_image_vline(uint16_t *dp,uint16_t x,uint16_t y,uint16_t len,dc sdc);

void draw_line(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color);
void draw_circle(uint16_t x, uint16_t y, uint16_t r,uint16_t color);
void draw_circle_helper(uint16_t x,uint16_t y, uint16_t r,uint8_t corner,uint16_t color);
void fill_circle(uint16_t x,uint16_t y, uint16_t r,uint16_t color);
void draw_rect(uint16_t x,uint16_t y,uint16_t w,uint16_t h,uint16_t color);
void fill_rect(uint16_t x,uint16_t y,uint16_t w,uint16_t h,uint16_t color);
void draw_triangle(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color);
void fill_triangle(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color);
uint16_t get_complementary_color(uint16_t color);
void draw_Hline(uint16_t x,uint16_t y, uint16_t w, uint16_t color);
void draw_Vline(uint16_t x,uint16_t y, uint16_t h, uint16_t color);
uint16_t mix_color(uint16_t color1,uint16_t color2,uint8_t percent);
void draw_width_line(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color,uint8_t width);

















#endif
