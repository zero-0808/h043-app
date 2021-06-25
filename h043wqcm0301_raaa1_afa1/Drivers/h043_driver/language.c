
/***************************************************************************************
****************************************************************************************
* Project      : 20180615_HMI
* FILE         : language.c
* Description  : 
*                      
* Copyright (c) 2018 by ShenZhen JingHua Displays Co.,Ltd... All Rights Reserved.
* 
* History:
* Version		name			Date			Description
* 0.01			Qu Bingbing		2018/06/20		Initial Version
* 
****************************************************************************************
****************************************************************************************/

#include "language.h"
#include "nand.h"
#include "cfg.h"
#include "mem_my.h"

void lang_init(void)
{
    uint32_t addr,size;
    uint32_t block,page;
    size = 16;
    src_cfg.lang_info = (lang_set *) my_malloc(size);
    nand_dev.read(src_cfg.block_lang_addr,0,0,(uint8_t *)src_cfg.lang_info,size);
    if(src_cfg.lang_info->lang_num == 0xff)
    {
        my_free(src_cfg.lang_info);
        src_cfg.lang_info = NULL;
        return;
    }

    size += src_cfg.lang_info->lang_num * sizeof(lang_descrip);
    my_free(src_cfg.lang_info);
    src_cfg.lang_info = (lang_set *)my_malloc(size);
    nand_dev.read(src_cfg.block_lang_addr,0,0,(uint8_t *)src_cfg.lang_info,size);
    src_cfg.lang_info->lan_des = (lang_descrip *)((uint8_t *)src_cfg.lang_info + 16);
    size = 0;

    size = src_cfg.lang_info->lan_des[src_cfg.lang_info->lang].size;
    addr = src_cfg.lang_info->lan_des[src_cfg.lang_info->lang].addr;

    page = addr / nand_dev.page_mainsize;
    block = page / nand_dev.block_pagenum;
    block += src_cfg.block_lang_addr;
    //block += (src_cfg.block_lang_addr+src_cfg.block_font_addr);
    page = page % nand_dev.block_pagenum;
    addr = addr % nand_dev.page_mainsize;

    src_cfg.lang_info->plang = my_malloc(size);
    nand_dev.read(block,page,addr,(uint8_t *)src_cfg.lang_info->plang,size);
    src_cfg.lang_info->plang->str_des =  (str_descrip *)((uint8_t *)src_cfg.lang_info->plang + 16);
    //src_cfg.lang_info->plang->str = ((uint8_t *)src_cfg.lang_info->plang + 16 + src_cfg.lang_info->plang->str_num *sizeof(str_descrip));
	src_cfg.lang_info->plang->str = (uint8_t *)src_cfg.lang_info->plang;
    
}


void set_lang(uint8_t lang)
{
    uint32_t addr,size;
    uint32_t block,page;

    if(src_cfg.lang_info == NULL)
    {
        return;
    }

    if(src_cfg.lang_info->lang == lang)
    {
        return;
    }

    src_cfg.lang_info->lang = lang;
    size = src_cfg.lang_info->lan_des[src_cfg.lang_info->lang].size;
    addr = src_cfg.lang_info->lan_des[src_cfg.lang_info->lang].addr;
    page = addr / nand_dev.page_mainsize;
    block = page / nand_dev.block_pagenum;
    block += src_cfg.block_lang_addr;
    page = page % nand_dev.block_pagenum;
    addr = addr % nand_dev.page_mainsize;

    my_free(src_cfg.lang_info->plang);
    src_cfg.lang_info->plang = my_malloc(size);
    nand_dev.read(block,page,addr,(uint8_t *)src_cfg.lang_info->plang,size);
    src_cfg.lang_info->plang->str_des =  (str_descrip *)((uint8_t *)src_cfg.lang_info->plang + 16);
    //src_cfg.lang_info->plang->str = ((uint8_t *)src_cfg.lang_info->plang + 16 + src_cfg.lang_info->plang->str_num *sizeof(str_descrip));
	src_cfg.lang_info->plang->str = (uint8_t *)src_cfg.lang_info->plang;
}




/***************************************************************************************
* End of File: language.c

****************************************************************************************/
