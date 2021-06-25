#ifndef _LED_H_
#define _LED_H_

#include "hmi_config.h"
#include "stdint.h"

#if LED_SUPPORT




void led_init(void);
uint8_t set_led_color(uint8_t color);
uint8_t set_led_mode(uint8_t mode);
uint8_t set_led_time(uint16_t time);
void led_work_state(uint8_t state);







#endif



#endif 


