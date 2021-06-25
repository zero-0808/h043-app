#ifndef _ORIENTATION_H_
#define _ORIENTATION_H_

#include "hmi_config.h"

#if  (DIR_SWITCH_SUPPORT)

void  lis3dh_init(void);
unsigned char get_lis3dh_dir(void);
unsigned char lis3dh_get_dir(void);
void set_screen_mode(unsigned char mode);
int i2c_write(int addr, unsigned char *data, int len);
int i2c_read(int addr, uint8_t  reg, unsigned char *rbuf, uint16_t len);

#endif








#endif

