#ifndef __TIMER_H
#define __TIMER_H

#include "stdint.h"

void delay_init(void);
void delay_us(uint32_t nus);
void delay_ms(uint32_t nus);
void delay_xms(uint32_t nus);

#endif
