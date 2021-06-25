
#ifndef __NAND_H__
#define __NAND_H__

#include "r_typedefs.h"
#include "stm32h7xx.h"

#define _RUNNING_NANDFLASH_NONE			(0)
#define _RUNNING_NANDFLASH_K9F1G08U0C	(1)
#define _RUNNING_NANDFLASH_K9F1G08U0A	(2)

#define _CUR_USING_NANDFLASH			(_RUNNING_NANDFLASH_K9F1G08U0C)
#define NAND_MAX_SIZE 0x10000000
#define NAND_MAX_PAGE_SIZE			4096		//定义NAND FLASH的最大的PAGE大小（不包括SPARE区），默认4096字节
#define NAND_ECC_SECTOR_SIZE		512			//执行ECC计算的单元大小，默认512字节

#define APP_NAND_START_BLOCK 0//存储应用文件的起始块

typedef struct
{
	void (*init)(void);
	uint8_t (*create_lut)(uint8_t);
	u32 (*read_id)(void);
	uint8_t  (*block_erase)(uint32_t,uint32_t block_offset);
	uint8_t  (*page_program)(uint32_t PageNum,uint16_t ColNum,uint8_t *pBuffer,uint16_t NumByteToWrite);
	uint32_t (*read)(uint32_t,uint32_t,uint32_t,uint8_t*,uint32_t);
    uint32_t (*program)(uint32_t addr,uint8_t * buf,uint32_t size);
    uint16_t page_totalsize;     	
    uint16_t page_mainsize;      
    uint16_t page_sparesize;     	
    uint8_t  block_pagenum;      	
    uint16_t plane_blocknum;     
    uint16_t block_totalnum;     	
    uint16_t good_blocknum;      	  
    uint16_t use_blocknum;
    u16 valid_blocknum;     	//有效块数量(供文件系统使用的好块数量)
    uint32_t id;             		
    uint16_t *lut;
	u32 block_current_index;//当前使用的块
	u32 block_page_current_index;//当前块使用的第几页
	u32 ecc_hard;				//硬件计算出来的ECC值
	u32 ecc_hdbuf[NAND_MAX_PAGE_SIZE/NAND_ECC_SECTOR_SIZE];//ECC硬件计算值缓冲区
	u32 ecc_rdbuf[NAND_MAX_PAGE_SIZE/NAND_ECC_SECTOR_SIZE];//ECC读取的值缓冲区
}nand_attriute;



//#define K9F1G_CLE   ((volatile uint8_t *)0x98100000)
//#define K9F1G_ALE		((volatile uint8_t *)0x98080000)
//#define K9F1G_DATA  ((volatile uint8_t *)0x98000000)

//NAND FLASH操作相关延时函数
#define NAND_TADL_DELAY				42			//tADL等待延迟,最少70ns
#define NAND_TWHR_DELAY				36			//tWHR等待延迟,最少60ns
#define NAND_TRHW_DELAY				60			//tRHW等待延迟,最少100ns
#define NAND_TPROG_DELAY			700			//tPROG等待延迟,典型值200us,最大需要700us
#define NAND_TBERS_DELAY			8			//tBERS等待延迟,典型值3.5ms,最大需要10ms
#define NAND_TRST_FIRST_DELAY		6			//tRST复位后的第一次等待时间，最大为3ms
#define NAND_RB_READPAGE_TIME_OUT 12000//当读取nandflash 的rb脚时，读取状态未捕捉到则采用死延时方法 by liuzhiheng 2020-10-17 根据手册给的25us时间 12000*2.08333
#define NAND_RB_WRITEPAGE_TIME_OUT 336000//当写nandflash 的rb脚时，读取状态未捕捉到则采用死延时方法 by liuzhiheng 2020-10-17 根据手册给的最大700us时间
#define NAND_RB_TBERS_TIME_OUT 4800000//当写nandflash 的rb脚时，读取状态未捕捉到则采用死延时方法 by liuzhiheng 2020-10-17 根据手册给的最大10ms时间

#define NAND_RB  	            HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_6)//NAND Flash的闲/忙引脚

#define NAND_ADDRESS			0X80000000	//nand flash的访问地址,接NCE3,地址为:0X8000 0000
#define NAND_CMD				1<<16		//发送命令
#define NAND_ADDR				1<<17		//发送地址

//NAND FLASH命令
#define NAND_READID         	0X90    	//读ID指令
#define NAND_FEATURE			0XEF    	//设置特性指令
#define NAND_RESET          	0XFF    	//复位NAND
#define NAND_READSTA        	0X70   	 	//读状态
#define NAND_AREA_A         	0X00
#define NAND_AREA_TRUE1     	0X30
#define NAND_AREA_SEQUENTIAL   	0X31

#define NAND_WRITE0        	 	0X80
#define NAND_WRITE_TURE1    	0X10
#define NAND_ERASE0        	 	0X60
#define NAND_ERASE1         	0XD0
#define NAND_MOVEDATA_CMD0  	0X00
#define NAND_MOVEDATA_CMD1  	0X35
#define NAND_MOVEDATA_CMD2  	0X85
#define NAND_MOVEDATA_CMD3  	0X10

//NAND FLASH状态
#define NSTA_READY       	   	0X40		//nand已经准备好
#define NSTA_ERROR				0X01		//nand错误
#define NSTA_TIMEOUT        	0X02		//超时
#define NSTA_ECC1BITERR       	0X03		//ECC 1bit错误
#define NSTA_ECC2BITERR       	0X04		//ECC 2bit以上错误


#define K9F1G_CLE   ((volatile uint8_t *)0x82000000)
#define K9F1G_ALE	((volatile uint8_t *)0x81000000)
#define K9F1G_DATA  ((volatile uint8_t *)0x80000000)





#define K9FXX_ID                    0xECF10000  /* Byte 3 and 2 only */

#define K9FXX_READ_1                0x00
#define K9FXX_READ_2                0x30
#define K9FXX_READ_ID               0x90
#define K9FXX_RESET                 0xFF
#define K9FXX_BLOCK_PROGRAM_1       0x80
#define K9FXX_BLOCK_PROGRAM_2       0x10
#define K9FXX_BLOCK_ERASE_1         0x60
#define K9FXX_BLOCK_ERASE_2         0xD0
#define K9FXX_READ_STATUS           0x70

#define K9FXX_BUSY                  (1 << 6)
#define K9FXX_OK                    (1 << 0)


#define FIO_BASE_ADDR       0x20098000
#define FIO2DIR        (*(volatile unsigned long *)(FIO_BASE_ADDR + 0x40))
#define FIO2MASK       (*(volatile unsigned long *)(FIO_BASE_ADDR + 0x50))
#define FIO2PIN        (*(volatile unsigned long *)(FIO_BASE_ADDR + 0x54))
#define FIO2SET        (*(volatile unsigned long *)(FIO_BASE_ADDR + 0x58))
#define FIO2CLR        (*(volatile unsigned long *)(FIO_BASE_ADDR + 0x5C))


#define ERR_RETVAL_OK               (0)
#define ERR_RETVAL_ERROR            (-1)
#define ERR_RETVAL_WRONG_INPUT      (-2)


extern nand_attriute nand_dev;


u16 NAND_LBNToPBN(u32 LBNNum);
extern void nandwaitrbdown(void);
extern u8 NAND_WaitRB(vu8 rb , uint32_t timeout);


#endif 






