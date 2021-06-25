/*
 * @Author: 王先银
 * @Date: 2020-09-10 09:24:04
 * @LastEditTime: 2020-10-15 18:57:11
 * @LastEditors: Please set LastEditors
 * @Description: 阿拉伯语显示
 * @FilePath: \HMI_APP\Src\lang_arabic.c
 */
#include "lang_arabic.h"
#include "display.h"
#include "font.h"
#include "str.h"
#include "mem_my.h"
#include "cfg.h"
#include "string.h"
//阿拉伯语转换
//[][0]:前连对应unicode码;[][1]:后连对应unicode码;[][2]:前后连对应unicode码;[][3]:单独对应unicode码;
const uint16_t Arbic_Position[][4]=  // first, last, middle, alone
{

	{ 0xfe80, 0xfe80, 0xfe80, 0xfe80},    // 0x621

	{ 0xfe82, 0xfe81, 0xfe82, 0xfe81},

	{ 0xfe84, 0xfe83, 0xfe84, 0xfe83},

	{ 0xfe86, 0xfe85, 0xfe86, 0xfe85},

	{ 0xfe88, 0xfe87, 0xfe88, 0xfe87},

	{ 0xfe8a, 0xfe8b, 0xfe8c, 0xfe89},

	{ 0xfe8e, 0xfe8d, 0xfe8e, 0xfe8d},

	{ 0xfe90, 0xfe91, 0xfe92, 0xfe8f},   // 0x628

	{ 0xfe94, 0xfe93, 0xfe94, 0xfe93},

	{ 0xfe96, 0xfe97, 0xfe98, 0xfe95},   // 0x62A

	{ 0xfe9a, 0xfe9b, 0xfe9c, 0xfe99},

	{ 0xfe9e, 0xfe9f, 0xfea0, 0xfe9d},

	{ 0xfea2, 0xfea3, 0xfea4, 0xfea1},

	{ 0xfea6, 0xfea7, 0xfea8, 0xfea5},

	{ 0xfeaa, 0xfea9, 0xfeaa, 0xfea9},

	{ 0xfeac, 0xfeab, 0xfeac, 0xfeab},   // 0x630  

	{ 0xfeae, 0xfead, 0xfeae, 0xfead},

	{ 0xfeb0, 0xfeaf, 0xfeb0, 0xfeaf},

	{ 0xfeb2, 0xfeb3, 0xfeb4, 0xfeb1},

	{ 0xfeb6, 0xfeb7, 0xfeb8, 0xfeb5},

	{ 0xfeba, 0xfebb, 0xfebc, 0xfeb9},

	{ 0xfebe, 0xfebf, 0xfec0, 0xfebd},

	{ 0xfec2, 0xfec3, 0xfec4, 0xfec1},

	{ 0xfec6, 0xfec7, 0xfec8, 0xfec5},  // 0x638

	{ 0xfeca, 0xfecb, 0xfecc, 0xfec9},

	{ 0xfece, 0xfecf, 0xfed0, 0xfecd},  //0x63A

	{ 0x63b, 0x63b, 0x63b, 0x63b},

	{ 0x63c, 0x63c, 0x63c, 0x63c},

	{ 0x63d, 0x63d, 0x63d, 0x63d},

	{ 0x63e, 0x63e, 0x63e, 0x63e},

	{ 0x63f, 0x63f, 0x63f, 0x63f},

	{ 0x640, 0x640, 0x640, 0x640},   // 0x640

	{ 0xfed2, 0xfed3, 0xfed4, 0xfed1},

	{ 0xfed6, 0xfed7, 0xfed8, 0xfed5},

	{ 0xfeda, 0xfedb, 0xfedc, 0xfed9},

	{ 0xfede, 0xfedf, 0xfee0, 0xfedd},

	{ 0xfee2, 0xfee3, 0xfee4, 0xfee1},

	{ 0xfee6, 0xfee7, 0xfee8, 0xfee5},

	{ 0xfeea, 0xfeeb, 0xfeec, 0xfee9},

	{ 0xfeee, 0xfeed, 0xfeee, 0xfeed},   // 0x648

	{ 0xfef0, 0xfef3, 0xfef4, 0xfeef},

	{0xfef2, 0xfef3, 0xfef4, 0xfef1},   // 0x64A

};

//判断是否是连接前面的，采用判断该字符前一个字符的判定方法，方法是，看前一个字符是否在集合set1中。
//如果在，则是有连接前面的。集合1如下:
static uint16_t theSet1[23]=
{
    0x62c, 0x62d, 0x62e, 0x647, 0x639, 0x63a, 0x641, 0x642,
    0x62b, 0x635, 0x636, 0x637, 0x643, 0x645, 0x646, 0x62a,
    0x644, 0x628, 0x64a, 0x633, 0x634, 0x638, 0x626
};
//判断是否是连接后面的，采用判断该字符后一个字符的判定方法，方法是，看后一个字符是否在集合set2中。
//如果在，则是有连接后面的。集合2如下:		
static uint16_t theSet2[35]=
{
    0x62c,0x62d,0x62e,0x647,0x639,0x63a,0x641,0x642,
	0x62b,0x635,0x636,0x637,0x643,0x645,0x646,0x62a,
	0x644,0x628,0x64a,0x633,0x634,0x638,0x626,0x627,
	0x623,0x625,0x622,0x62f,0x630,0x631,0x632,0x648,
	0x624,0x629,0x649
};
//阿拉伯文连字符规则:
//连字符是以0x644开头，后面跟的是0x622，0x623，0x625，0x627，并根据情况取下面的字符数组0或1，
//如果0x644前一个字符是在集合1（同上面的集合1）中间，那么取数组1，否则取数组0。
//数组如下:		
static uint16_t arabic_specs[][2]=
{
	{0xFEF5,0xFEF6},
	{0xFEF7,0xFEF8},
	{0xFEF9,0xFEFA},
	{0xFEFB,0xFEFC},
};
static uint8_t arabic_front(uint16_t unicode);
static uint8_t arabic_front(uint16_t unicode);
static uint8_t arabic_next(uint16_t unicode);
static uint8_t arabic_hyphen(uint16_t curr,uint16_t next);
static void cal_arabic_offset(uint8_t* offset,uint8_t* remain,uint8_t *hzk,uint16_t size);
static uint16_t arabic_convert(uint16_t unicode,uint8_t front_flag,uint8_t next_flag);


/*******************************************************************************
* Function     : arabic_unicode_convert
* Description  : 阿拉伯文的unicode转换
* Input Para   : 
-->convert_code:转换后的字符
-->code:当前字符
-->len:当前字符长度
* Output Para  : NULL
* Return Value : 转换后的unicode的长度
*******************************************************************************/
uint16_t arabic_unicode_convert(uint16_t* convert_code,uint16_t* code,uint16_t len )
{
	uint16_t i,j=0,k=0;
	uint16_t curr_convert;
	uint8_t front_flag,next_flag,hyphen_flag;
	//front_flag = arabic_front(front);
	//next_flag = arabic_next(next);

	for(i=0;i<len;i++)
	{
		if(i==0)
		{
			next_flag = arabic_next(code[i+1]);
			if(next_flag==1)
			{
				curr_convert = arabic_convert(code[i],0,next_flag);
				convert_code[j++] = curr_convert;
			}
		}
		else
		{
			if((i+1)<len)
			{
				hyphen_flag = arabic_hyphen(code[i],code[i+1]);
                
				if(hyphen_flag==1)
				{
					front_flag =arabic_front(code[i-1]);
					if(code[i]==0x0622)
					{
						k = 0;
					}
					else if(code[i]==0x0623)
					{
						k = 1;
					}
					else if(code[i]==0x0625)
					{
						k = 2;
					}
					else if(code[i]==0x0627)
					{
						k = 3;
					}
					curr_convert = arabic_specs[k][front_flag];
					convert_code[j++] = curr_convert;
					i++;
					//if(front_flag==1)
					//{
					//	curr_convert = arabic_specs[code[i]-0x0644][1];
					//}
					//else
					//{
					//	curr_convert = arabic_specs[code[i]-0x0644][1];
					//}
				}
				else
				{
					front_flag = arabic_front(code[i-1]);
					next_flag = arabic_next(code[i+1]);
					curr_convert = arabic_convert(code[i],front_flag,next_flag);
					convert_code[j++] = curr_convert;
				}
			}
			else
			{
				front_flag = arabic_front(code[i-1]);
				curr_convert = arabic_convert(code[i],front_flag,0);
				convert_code[j++] = curr_convert;
			}
			
			
		}
	}
	return j;
}

/*******************************************************************************
* Function     : get_arabic_offset
* Description  : 获取阿拉伯文的点阵偏移
* Input Para   : 
-->offset:当前字符的起始偏移
-->remain:当前字符的剩余
-->code:当前字符
-->font:当前字体结构体
* Output Para  : NULL
* Return Value : NULL
*******************************************************************************/
void get_arabic_offset(uint8_t* offset,uint8_t* remain,uint16_t code,font_type font)
{
	uint8_t * hzk = NULL;
	uint16_t size,csize;
	
    size = font.width;
   
	csize=(size/8+((size%8)?1:0))*(size);
	hzk = (uint8_t *)my_malloc(csize);
	
	if(hzk == NULL)
	{
		return ;
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
	        return;    
	}
	/* Change By 王先银 at 2020/09/5 */
	cal_arabic_offset(offset,remain,hzk,size);
	my_free(hzk);
	hzk = NULL;

	
}
/**
 * @description: 计算阿拉伯语中混合的英语
 * @param {code,unicode编码值
 * len,编码长度} 
 * @return {正数,混和的英语个数;0,无英语} 
 */
uint8_t Arabic_Fix_English(uint16_t* code,uint16_t len)
{
	uint8_t j = 0,i;
    if(len>0)
    {
        for(i=0;i<len;i++)
        {
            if(code[i]<0x0080)
            {
                j++;
                
            }
            else
            {
                return j;
            }
            
        }
    }
    return j;	
}

/**
 * @description: 计算当前阿拉伯语的字节数
 * @param {code,unicode编码值
 * len,编码长度} 
 * @return {正数,阿拉伯文个数;0,非阿拉伯语} 
 */
uint8_t Arabic_Check_Len(uint8_t* code,uint16_t len)
{
    uint8_t j = 0,i;
    uint16_t unicode;
    if(len>1)
    {
        for(i=0;i<len;i+=2)
        {
            unicode = code[i] << 8 | code[i + 1];
            if(unicode>=0x0600&&unicode<0x0700)
            {
                j++;
                
            }
            else
            {
                //return j;
            }
            
        }
    }
    return j;
}

/**
 * @description: 判断当前unicode码值
 * @param {unicode,unicode编码值} 
 * @return {1,是阿拉伯;0,非阿拉伯语} 
 */
uint8_t Arabic_Check(uint16_t unicode)
{
    if(((unicode>=0x0600)&&(unicode<0x0700)))//阿拉伯语
    {
        return 1;
    }
    return 0;
}

/**
 * @description: 计算当前阿拉伯语的字节数
 * @param {code,unicode编码值
 * len,编码长度} 
 * @return {正数,阿拉伯文个数;0,非阿拉伯语} 
 */
uint8_t Arabic_Len(uint8_t* code,uint16_t len)
{
    uint8_t j = 0,i;
    uint16_t unicode;
    if(len>1)
    {
        for(i=0;i<len;i+=2)
        {
            unicode = code[i] << 8 | code[i + 1];
            if(unicode>=0x0600&&unicode<0x0700)
            {
                j++;
                
            }
            else
            {
                return j;
            }
            
        }
    }
    return j;
}

/*******************************************************************************
* Function     : cal_arabic_offset
* Description  : 计算阿拉伯文的点阵偏移
* Input Para   : 
-->offset:当前字符的起始偏移
-->remain:当前字符的剩余
-->hzk:当前字符指针
-->font:当前字体结构体
* Output Para  : NULL
* Return Value : NULL
*******************************************************************************/
static void cal_arabic_offset(uint8_t* offset,uint8_t* remain,uint8_t *hzk,uint16_t size)
{
	uint16_t temp,t,t1;
	uint16_t y0 = 0;
	uint16_t x0 = 0;
	uint8_t offset_tmp=size,remain_tmp=0;
	uint16_t csize=(size/8+((size%8)?1:0))*(size);
	for(t=0;t<csize;t++)
	{
		temp=hzk[t];
		
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)
			{
				if(x0>remain_tmp)       //x最大的点阵坐标
				{
					remain_tmp = x0;
				}
				if(x0<offset_tmp)       //x开始有点阵信息
				{
					offset_tmp = x0;
				}
				
				//disp_dev.put_pixel(x0,y0,color);
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
	*offset = offset_tmp;
	*remain = size-remain_tmp+1;
		
}

/*******************************************************************************
* Function     : arabic_front
* Description  : 判断当前阿拉伯文是否是前连的
* Input Para   : 
-->unicode:当前字符
* Output Para  : NULL
* Return Value : 0,否;1，是
*******************************************************************************/
static uint8_t arabic_front(uint16_t unicode)
{
	uint8_t i;
	for(i=0;i<23;i++)
	{
		if(unicode == theSet1[i])
		{
			return 1;
		}
	}
	return 0;
}

/*******************************************************************************
* Function     : arabic_next
* Description  : 判断当前阿拉伯文是否是后连的
* Input Para   : 
-->unicode:当前字符
* Output Para  : NULL
* Return Value : 0,否;1，是
*******************************************************************************/
static uint8_t arabic_next(uint16_t unicode)
{
	uint8_t i;
	for(i=0;i<35;i++)
	{
		if(unicode == theSet2[i])
		{
			return 1;
		}
	}
	return 0;
}

/*******************************************************************************
* Function     : arabic_hyphen
* Description  : 阿拉伯文的连字符判断
* Input Para   : 
-->curr:当前字符
-->next:当前字符的后字符
* Output Para  : NULL
* Return Value : 0,否;1，是
*******************************************************************************/
static uint8_t arabic_hyphen(uint16_t curr,uint16_t next)
{
	if(curr==0x0644)
	{
		if(next==0x0622||next==0x0623||next==0x0625||next==0x0627)
		{
			return 1;
		}
	}
	return 0;
}
/**
 * @description: 阿拉伯文转换
 * @param {unicode,unicode编码
 * front_flag,前连标志
 * next_flag,后连标志} 
 * @return {type} 
 */
static uint16_t arabic_convert(uint16_t unicode,uint8_t front_flag,uint8_t next_flag)
{
	uint8_t pos=0;
	uint16_t unicode_convert;
	if(unicode<0x0080)
	{
		unicode_convert  = unicode;
		//return unicode_convert;
	} 
	else if(unicode>=0x0621&&unicode<=0x064a)
	{
		if(front_flag==1)
		{
			if(next_flag==1)
			{
				pos = 2;
			}
			else
			{
				pos = 0;
			}
		}
		else 
		{
			if(next_flag==1)
			{
				pos = 1;
			}
			else
			{
				pos = 3;
			}
		}
		unicode_convert = Arbic_Position[unicode-0X0621][pos];
	}
	
	return unicode_convert;
}
