#ifndef _CMD_FUNC_H_
#define _CMD_FUNC_H_

#define MAX_CMD_NUM         53
#define MAX_CTRL_NUM        40

#include "cmd.h"


typedef uint8_t (*cmd_func)(uint8_t * p,uint16_t len);



typedef struct
{
    uint8_t cmd;
    cmd_func func;
    
}cmd_id_func;




#endif /*_CMD_FUNC_H_*/

