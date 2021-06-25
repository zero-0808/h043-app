#ifndef __CFG_H__
#define __CFG_H__



#include "SDRAM_W98N_32M_16BIT.h"
#include "font.h"
#include "language.h"
#include "timers.h"
#include "hmi_config.h"
#include"fmc.h"



#define   MS             (1)


#define MAX_SCREEN_NUM                      200


#if    (TFT_TYPE_SIZE == TFT_TYPE_4_3)

#define   DISP_WIDTH_PIXEL         (524)
#define   DISP_HEIGHT_PIXEL        (285)

#elif     (TFT_TYPE_SIZE == TFT_TYPE_5_0)

#define   DISP_WIDTH_PIXEL         (800)
#define   DISP_HEIGHT_PIXEL        (480)


#elif     (TFT_TYPE_SIZE == TFT_TYPE_7_0)

#define   DISP_WIDTH_PIXEL         (800)
#define   DISP_HEIGHT_PIXEL        (480)



#endif



#define   DISP_BUFF_NUM            (3)
#define   DISP_PIXEL_SIZE          (2)






#define OS_RAM_SIZE                (1 * 1024 * 1024)    //分配给RTOS的内存
#define DISP_BUFF_SIZE             (DISP_WIDTH_PIXEL*DISP_HEIGHT_PIXEL*DISP_PIXEL_SIZE*DISP_BUFF_NUM)
#define MALLOC_MEM_SIZE            (SDRAM_SIZE - DISP_BUFF_SIZE - OS_RAM_SIZE)
#define MEM_BASE_ADDR              (Bank6_SDRAM_ADDR + DISP_BUFF_SIZE)



//enum ShapeType
//{
//	draw_nothing = 0,			//指针	

//	draw_shape_line,			//线
//	draw_shape_rect,			//空矩形
//	draw_shape_solidrect,		//填充矩形
//	draw_shape_ellipse,			//空椭圆
//	draw_shape_solidEllipse,	//填充椭圆	
//	draw_shape_circle,			//空圆
//	draw_shape_solidcircle,		//填充圆
//	draw_shape_text,			//文字
//	draw_shape_picture,			//图片

//	draw_control_button,		//按钮控件
//	draw_control_edit,			//文本控件
//	draw_control_cuticon,		//剪切小图表控件
//	draw_control_progress,		//进度条控件
//	draw_control_dial,			//表盘控件
//	draw_control_slider,		//滑块控件
//	draw_control_time,			//时间控件
//	draw_control_flash,			//动画控件
//	draw_control_icon,			//图标控件
//	draw_control_bight,			//曲线控件
//	draw_control_menu,			//菜单控件
//	draw_control_choose,		//选择控件
//	draw_control_QRcode,		//二维码控件
//	draw_control_data,			//数据记录控件
//	draw_control_history		//历史曲线
//};


//#define CFG_STE                     0X08
#define CFG_BTN                     0X0A
#define CFG_TEX                     0X0B
#define CFG_BAR                     0X0D
#define CFG_SLI                     0X0F
#define CFG_RTC                     0X10
#define CFG_ANI                     0X11
#define CFG_ICO                     0X12
#define CFG_GRA                     0X13
#define CFG_MEN                     0X14
#define CFG_SEL                     0X15
#define CFG_STR                     0X19
#define CFG_WIN                     0X1A
#define CFG_REC                     0X17




/*

typedef struct font_configure
{
	uint32_t ascii6_addr;
	uint32_t ascii8_addr;
	uint32_t ascii12_addr;
	uint32_t ascii16_addr;
	uint32_t ascii32_addr;
	uint32_t gb32_addr;
	uint32_t gb64_addr;
	uint32_t gbk12_addr;
	uint32_t gbk16_addr;
	uint32_t gbk24_addr;
	uint32_t unicode24_addr;
}font_configure;
*/

typedef struct 
{
	uint16_t dev_class;
	uint16_t start_screen;
	uint8_t screen_rotate;
	uint8_t gyroscope;
	uint8_t user_layer;
	uint8_t lang_num;
	uint8_t code;
	uint8_t lock_cfg;
	uint8_t baudrate;
	uint8_t touch;
	uint8_t crc16;
	uint8_t slider_switch;
	uint8_t buzzer;
	uint8_t start_music;
	uint8_t volume;
	uint8_t auto_bl;
	uint16_t standby_time;
	uint8_t  active_light;
	uint8_t standby_light;
	uint8_t light;
	uint8_t old_light;
	uint8_t bl_nortify;
	uint16_t standby_screen;
	uint8_t touch_nortify;
	uint8_t switch_nortify;
	uint8_t event_nortify;
	TimerHandle_t timer;
  
  
}prj_cfg;


typedef struct HVscreen_desp
{

    uint16_t Hscreen;
    uint16_t Vscreen;

}HVscreen_desp;

//自定义键盘
typedef struct kb_desp
{
    uint16_t shwkb;
    uint16_t shhkb;
    uint16_t mhwkb;
    uint16_t mhhkb;
    uint16_t svwkb;
    uint16_t svhkb;
    uint16_t mvwkb;
    uint16_t mvhkb;

    uint16_t sh_num;
    uint16_t mh_num;
    uint16_t sv_num;
    uint16_t mv_num;
    uint16_t res[4];
}kb_desp;


typedef struct screen_desp
{
    uint16_t screen_num;    //界面数目
    uint16_t H_screen_num;  //横屏界面数目
    uint8_t  HV_switch;     //是否支持横竖屏切换 1 是，0 否
    uint8_t  screen_mode;   //当前画面的横竖屏模式，00 横屏，01 竖屏，02 根据中立感应确定屏幕方向 
    kb_desp  * pkb_size;
    uint16_t * screen_pic;
    HVscreen_desp * HVscreen;
    
    
}screen_desp;









typedef struct source_configure
{
	short 	new_screen;
	short 	current_screen;
	uint16_t block_font_addr;   //字体格式文件font.bin的存储地址
    //uint16_t block_font_size; 
	uint16_t block_touch_addr;  //触控配置文件13.bin的存储地址
    //uint16_t block_touch_size;
	uint8_t * p_touch;
	uint8_t * p_text;
	uint32_t text_index;
	uint16_t block_cmd_addr;    //按钮控件的对内对外指令存储地址
    //uint16_t block_cmd_size;
	uint8_t *p_cmd;
	uint16_t block_icon_addr;   //图标文件
    //uint16_t block_icon_size;
	uint16_t block_lang_addr;   //多国语言字符串编辑
    //uint16_t block_lang_size; 
	uint16_t block_screen_addr; //页面描述信息
    //uint16_t block_screen_size;
	uint16_t block_bmp_addr;    //bmp图片
	screen_desp * ptscreen;
	font_info * font_set;
	lang_set * lang_info;
	
	
}source_configure;



extern prj_cfg * g_project;


void cfg_init(void);
void get_src(void);
uint8_t * get_touch_addrss(void);
void parse_cfg(void);
void get_pic(uint16_t num);
short get_pic_num(uint16_t screen_id);
uint16_t get_screen_id(uint16_t pic_now);
void update_project_configure(void);
uint8_t get_screen_HV(uint16_t screen_id);
uint16_t get_Vscreen(uint16_t Hscreen);
uint16_t get_Hscreen(uint16_t Hscreen);
void  adjust_screen_dir (uint16_t screen_id);



extern source_configure src_cfg;

#endif











