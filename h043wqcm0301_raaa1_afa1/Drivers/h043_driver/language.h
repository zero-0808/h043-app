#ifndef _LANGUAGE_H_
#define _LANGUAGE_H_

#include "stdint.h"

typedef struct
{
    uint32_t addr;
    uint16_t size;
    uint8_t lang_id;
}lang_descrip;

typedef struct 
{
    uint16_t addr;
    uint16_t len;
}str_descrip;

typedef struct 
{
	uint16_t str_num;
    uint8_t code;
    str_descrip * str_des;
    uint8_t * str;
}lang_table;

typedef struct
{
    uint8_t lang_num;
    uint8_t lang;
    lang_descrip * lan_des;
    lang_table * plang;
}lang_set;


void lang_init(void);
void set_lang(uint8_t lang);


#endif /*_LANGUAGE_H_*/

