/*
 * @Author: 王先银
 * @Date: 2020-09-10 09:25:54
 * @LastEditTime: 2020-10-09 09:06:27
 * @LastEditors: Please set LastEditors
 * @Description: 泰语显示
 * @FilePath: \HMI_APP\Src\lang_Thai.h
 */
#ifndef __LANG_THAI_H
#define __LANG_THAI_H
#ifdef __cplusplus
extern "C"
{
#endif
#include "str.h"
#include "mem_my.h"
#include "cfg.h"
#include "string.h"

typedef enum{
    THAI_SHOE           =2, //鞋子字符
    THAI_SUBJECT        =1, //主体字符
    THAI_CAP            =3, //帽子字符
    THAI_TONE           =4, //声调字符
    THAI_INDEPEND       =5, //独立字符
    THAI_POINT          =6, //标点字符
    THAI_OTHER          =7, //其他字符
}THAI_ENUM;

typedef struct{
    THAI_ENUM type;    /*拼接位置*/
    uint8_t width;      /*字符实际占据宽度*/
    uint8_t height;     /*字符实际占据高度*/
    uint8_t left_offset;   /*字符在点阵左偏移*/
    uint8_t top_offset;    /*字符在点阵上偏移*/
}lang_Thai_t;

/**
 * @description: 判断当前unicode码值
 * @param {unicode,unicode编码值} 
 * @return {1,是泰文;0,非泰文} 
 */
uint8_t Thai_Check(uint16_t unicode);

/**
 * @description:泰文的拼接
 * @param {code,unicode编码指针;len,编码长度} 
 * @return {0,异常;2-4,拼接的unicode个数;1,独立或标点字符} 
 */
uint8_t Thai_Splice(uint8_t* code,uint16_t len);

/**
 * @description: 获取泰文的点阵信息
 * @param {lang,泰文结构体;
 * code,unicode编码;
 * font,字体结构体} 
 * @return {0,异常;1,成功} 
 */
uint8_t Get_Thai_Info(lang_Thai_t* lang,uint16_t code,font_type font);

/**
 * @description: 显示泰文
 * @param {lang_main,泰文结构体;
 * code,unicode编码
 * x,横坐标
 * y,纵坐标
 * color,颜色
 * font,字体结构体
 * flag,0,显示上下标;1,显示主体} 
 * @return {NULL} 
 */
void Show_Thai(lang_Thai_t lang_main,uint16_t code,uint16_t x,uint16_t y,uint16_t color, font_type font,uint8_t flag);
#ifdef __cplusplus
}
#endif
#endif

