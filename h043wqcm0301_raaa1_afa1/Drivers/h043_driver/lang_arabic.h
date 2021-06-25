/*
 * @Author: 王先银
 * @Date: 2020-09-10 09:25:54
 * @LastEditTime: 2020-09-15 16:03:09
 * @LastEditors: Please set LastEditors
 * @Description: 阿拉伯语显示
 * @FilePath: \HMI_APP\Src\lang_arabic.h
 */
#ifndef __LANG_ARABIC_H
#define __LANG_ARABIC_H
#ifdef __cplusplus
extern "C"
{
#endif
#include "str.h"
#include "mem_my.h"
#include "cfg.h"
#include "string.h"

uint8_t Arabic_Fix_English(uint16_t* code,uint16_t len);
/**
 * @description: 判断当前unicode码值
 * @param {unicode,unicode编码值} 
 * @return {1,是阿拉伯;0,非阿拉伯语} 
 */
uint8_t Arabic_Check(uint16_t unicode);
uint8_t Arabic_Check_Len(uint8_t* code,uint16_t len);
uint8_t Arabic_Len(uint8_t* code,uint16_t len);
uint16_t arabic_unicode_convert(uint16_t* convert_code,uint16_t* code,uint16_t len );
void get_arabic_offset(uint8_t* offset,uint8_t* remain,uint16_t code,font_type font);
#ifdef __cplusplus
}
#endif
#endif

