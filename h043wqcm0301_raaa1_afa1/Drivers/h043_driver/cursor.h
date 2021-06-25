#ifndef _CURSOR_H_
#define _CURSOR_H_

#include "display.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"


typedef struct cursor_device
{
    void (*init)(void);
    void (*set_color)(uint16_t);
    void (*set_time)(uint32_t);
    //void (*move)(uint16_t,uint16_t);
    void (*switch_box)(uint16_t,uint16_t,uint16_t,uint8_t);
    void (*close)(void);
    uint16_t x;
    uint16_t y;
    uint16_t h;
    uint16_t color;
    uint32_t time;
    uint8_t flag;
    //uint8_t status;
    uint16_t * image;
    TimerHandle_t timer; 
    
}cursor_device;

extern cursor_device cursor_dev;



#endif /*_CURSOR_H_*/

