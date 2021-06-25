/*
 * @Author: your name
 * @Date: 2020-09-10 09:24:04
 * @LastEditTime: 2020-10-09 14:31:44
 * @LastEditors: Please set LastEditors
 * @Description: 泰语显示
 * @FilePath: \HMI_APP\Src\lang_Thai.c
 */
#include "lang_Thai.h"
#include "display.h"
#include "font.h"
#include "str.h"
#include "mem_my.h"
#include "cfg.h"
#include "string.h"
// 泰文显示：
//泰文编码范围0E00-0E3a，0E3f-0E5a
//1.分5种字符：鞋子、主体、帽子、声调、独立、标点
//2.鞋子、帽子、声调字符可和主体字符上下拼接
//3.如果用①②③④⑤⑥分别表示鞋子字符，主体字符，帽子字符，声调字符，独立字符和标点符号。
// 则可能出现的组合情况有：①②，①②③，①②④，①②③④，②③，②④，②③④，⑤和⑥
// ①和③之间没有②的时候，③需要往下移动一点。
// ②的编码是0E1B，0E1D，0E1F时，③或④或③④，最好向左移动一点。
// 1.鞋子   编码段：0E38-0E39
// 2.主体   编码段：0E01-0E2E,0E32
// 3.帽子   编码段：0E31,0E33-0E37,0E47,0E4C-0E4E
// 4.声调   编码段：0E48-0E4B
// 5.独立   编码段：0E2F,0E30,0E3A,0E40-0E46,0E5A-0E5B
// 6.标点   编码段：0E00,0E3F,0E4F-0E59
// 7.其他   编码段：0E3B-0E3E,0E5B-0E7F
//泰文 上下标字符 unicode
static uint16_t Thai_sup_sub[] = {0x0e31, 0x0e33, 0x0e34, 0x0e35, 0x0e36, 0x0e37, 0x0e38, 0x0e39, 
0x0e47, 0x0e48, 0x0e49, 0x0e4a, 0x0e4b, 0x0e4c, 0x0e4d, 0x0e4e};



/**
 * @description: 判断当前unicode码值
 * @param {unicode,unicode编码值} 
 * @return {1,是泰文;0,非泰文} 
 */
uint8_t Thai_Check(uint16_t unicode)
{
    if(((unicode>=0x0e00)&&(unicode<0x0e3b))||((unicode>0x0e3e)&&(unicode<0x0e5b)))
    {
        return 1;
    }
    return 0;
}

/**
 * @description: 
 * @param {type} 
 * @return {type} 
 */
THAI_ENUM Thai_Kind(uint16_t unicode)
{
    uint8_t sup_sub_len,i;
    THAI_ENUM type;
    //uint16_t unicode;
    type = 0;
    sup_sub_len = sizeof(Thai_sup_sub)/sizeof(Thai_sup_sub[0]);
    if((unicode>0x0e00&&unicode<0x0e2f)||(unicode==0x0e32))/*主体字符*/
    {
        type = THAI_SUBJECT;
    }
    else if(unicode>0x0e37&&unicode<0x0e3a)/*鞋子字符*/
    {
        type = THAI_SHOE;
    }
    else if((unicode>0x0e32&&unicode<0x0e38)||(unicode==0x0e31)||
    (unicode>0x0e4b&&unicode<0x0e4f)||(unicode==0x0e47))/*帽子字符*/
    {
        type = THAI_CAP;
    }
    else if(unicode>0x0e47&&unicode<0x0e4c)/*声调字符*/
    {
        type = THAI_TONE;
    }
    else if((unicode>0x0e4e&&unicode<0x0e5a)||(unicode==0x0e00)||
    (unicode==0x0e3f))/*标点字符*/
    {
        type = THAI_POINT;
    }
    else/*独立字符*/
    {
        type = THAI_INDEPEND;
    }
    
    return type;
}


/**
 * @description:泰文的拼接
 * @param {code,unicode编码指针;len,编码长度} 
 * @return {0,异常;2-4,拼接的unicode个数;1,独立或标点字符} 
 */
uint8_t Thai_Splice(uint8_t* code,uint16_t len)
{
    uint8_t j = 0,i;
    THAI_ENUM kind,tmp_kind=0;
    uint16_t unicode;
    if(len>1)
    {
        for(i=0;i<len;i+=2)
        {
            unicode = code[i] << 8 | code[i + 1];
            kind = Thai_Kind(unicode);
            if(tmp_kind<kind)
            {
                tmp_kind = kind;
                
            }
            else
            {
                return j;
            }
            if(kind>4)/*独立或标点字符*/
            {
                if(j==0)
                {
                    j = 1;
                }
                return j;
            }
            else
            {
                j++;
            }
            
        }
    }
    return j;
}

/**
 * @description: 获取泰文的点阵信息
 * @param {lang,泰文结构体;
 * code,unicode编码;
 * font,字体结构体} 
 * @return {0,异常;1,成功} 
 */
uint8_t Get_Thai_Info(lang_Thai_t* lang,uint16_t code,font_type font)
{
	uint8_t * hzk = NULL;
    uint8_t top_end,left_end,flag=0;
    uint8_t ret=0;
	uint16_t size,csize;
	uint16_t temp,t,t1;
	uint16_t y0 = 0;
	uint16_t x0 = 0;
    THAI_ENUM kind;
    size = font.width;
    lang->left_offset = size;
    lang->top_offset = font.height;
    top_end = 0;
    left_end = 0;
	csize=(size/8+((size%8)?1:0))*(size);
	hzk = (uint8_t *)my_malloc(csize);
	lang->type = Thai_Kind(code);
	if(hzk == NULL)
	{
		return ret;
	}
	switch(font.code)
	{
	    case 1:
	        get_gb_font(code,font,hzk);
	        break;
	    case 2:
	        get_gbk_font(code,font,hzk);
	        break;
	    case 3:
	        get_unicode_font(code,font,hzk);
	        break;
	    default:
	        my_free(hzk);
	        return ret;    
	}
	for(t=0;t<csize;t++)
	{
		temp=hzk[t];
		
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)
			{
				if(x0>left_end)       //x轴点阵结束坐标
				{
					left_end = x0;
				}
              
				if(x0<lang->left_offset)       //x轴点阵开始坐标
				{
                    
					lang->left_offset = x0;
				}
                if(y0<lang->top_offset)//y轴点阵开始坐标
                {
                    if(font.code==0x0e1b||font.code==0x0e1d||font.code==0x0e1f)
                    {
                        if(flag==0)
                        {
                            lang->top_offset = y0;
                            flag=1;//特殊字符，只记录开始位置的y坐标
                        }
                        
                    }
                    else
                    {
                        lang->top_offset = y0;
                    }
                    
                    
                }
                if(y0>top_end)  //y轴点阵结束坐标
                {
                    top_end = y0;
                }
			}
			temp<<=1;
			x0++;
			
			if(x0 >= size)
			{
				x0 = 0;
				y0++;
				break;
			}
		}
	}

    lang->width = left_end-lang->left_offset+1;
    lang->height = top_end - lang->top_offset+1;
	my_free(hzk);
	hzk = NULL;
    ret = 1;
    return ret;
}
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
void Show_Thai(lang_Thai_t lang_main,uint16_t code,uint16_t x,uint16_t y,uint16_t color, font_type font,uint8_t flag)
{
	uint8_t * hzk = NULL;
    uint16_t top_s,top_e;
	uint16_t size,csize;
	uint16_t temp,t,t1;
	uint16_t y0 = y;
	uint16_t x0 = x;
    top_s = (uint16_t)(lang_main.top_offset)+y;
    top_e = (uint16_t)(lang_main.top_offset)+(uint16_t)(lang_main.height)+y;
    size = font.width;
	csize=(size/8+((size%8)?1:0))*(size);
	hzk = (uint8_t *)my_malloc(csize);
	if(hzk == NULL)
	{
		return;
	}
    switch(font.code)
    {
        case 1:
            get_gb_font(code,font,hzk);
            break;
        case 2:
            get_gbk_font(code,font,hzk);
            break;
        case 3:
            get_unicode_font(code,font,hzk);
            break;
        default:
            my_free(hzk);
            return ;    
    }
    for(t=0;t<csize;t++)
    {
        temp=hzk[t];
        
        for(t1=0;t1<8;t1++)
        {
            if(temp&0x80)
            {
                if(flag==0)
                {
                    if(y0>top_e ||y0<top_s)
                    {
                        disp_dev.put_pixel(x0-lang_main.left_offset,y0,color);
                    }
                }
                else
                {
                    disp_dev.put_pixel(x0-lang_main.left_offset,y0,color);
                }
                
                
                
            }
            temp<<=1;
            x0++;
            
            if((x0-x) >= size)
            {
                x0 = x;
                y0++;
                break;
            }
        }
    }
	my_free(hzk);
	hzk = NULL;
}

