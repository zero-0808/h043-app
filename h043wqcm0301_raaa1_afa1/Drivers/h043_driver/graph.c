
/***************************************************************************************
****************************************************************************************
* Project      : 20180615_HMI
* FILE         : graph.c
* Description  : 
*                      
* Copyright (c) 2018 by ShenZhen JingHua Displays Co.,Ltd... All Rights Reserved.
* 
* History:
* Version		name			Date			Description
* 0.01			Qu Bingbing		2018/06/25		Initial Version
* 
****************************************************************************************
****************************************************************************************/
#include "graph.h"
#include "cfg.h"
#include "mem_my.h"
#include "display.h"
#include "string.h"

#define MAX_CHANNEL_NUM   (8)


static uint8_t graph_msg(void *p, t_msg * p_msg)
{

	return 1;
	
   
}


static void graph_release( void* pCtrl )
{

	tgraph * pgraph = (tgraph *) pCtrl;

	my_free(pgraph);
   
}





    
    
static void graph_act(base_touch *p,uint8_t flag)
{

   
	
}






    
    
    
    
static void graph_draw_screen(base_touch * p)
{
    if(p->type != CFG_GRA)
    {
        return;
    }

    tgraph * pgraph = (tgraph *) p;

    uint16_t x0,y0,h,color,i;
    int x1,y1,x2,y2,x3,y3;
    uint8_t j;
    channel_info channel;

    x0 = pgraph->tp_rect.left;
    y0 = pgraph->tp_rect.bottom;
	
//	uint8_t width;


	
    if(pgraph->dir == 0)
    {
        h = pgraph->tp_rect.bottom - pgraph->tp_rect.top;
        
        if(pgraph->anti == 0)
        {
            if(pgraph->zoom == 0)
            {
                if(pgraph->line_num == 0)
                {
                    for(j=0;j<MAX_CHANNEL_NUM;j++)
                    {
                        if((pgraph->pgraph->channel_sta) & (1<<j))
                        {
                            channel = pgraph->pgraph->channel[j];
                            color = channel.color;
                            if(pgraph->pgraph->sample_depth == 0)
                            {
                                if(channel.len == 1)
                                {
                                    if(channel.sample[0] <= h)
                                    {
                                        x1 = x0;
                                        y1 = y0 - channel.sample[0];
                                        disp_dev.put_pixel(x1,y1,color);
                                    }
                                }
                                else if(channel.len > 1)
                                {
                                        
                                    for(i=1;i<channel.len;i++)
                                    {
                                        if(channel.sample[i-1]<=h || channel.sample[i]<=h)
                                        {
                                            x1 = x0+((i-1)*(pgraph->tp_rect.right-pgraph->tp_rect.left+1)) / pgraph->pgraph->sample_num;
                                            x2 = x0+((i)*(pgraph->tp_rect.right-pgraph->tp_rect.left+1)) / pgraph->pgraph->sample_num; 
                                            y1 = y0 - channel.sample[i-1];
                                            y2 = y0 - channel.sample[i];
                                            y3 = y0 - h;
                                            if(channel.sample[i-1] > h)
                                            {
                                                x3 = x1 - ((y1-y3)*(x1-x2) / (y1-y2));
                                                x1 = x3;
                                                y1 = y3;
                                            }
                                            else if(channel.sample[i] > h)
                                            {
                                                x3 = x1 - ((y1-y3)*(x1-x2) / (y1-y2));
                                                x2 = x3;
                                                y2 = y3;
                                            }

                                            draw_line(x1,y1,x2,y2,color);
                                        }
                                    
                                    }
                              
                                }
                            
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        
    }

    
}





    
    
    
uint8_t graph_create(uint8_t * buffer)
{
	tgraph * pgraph = (tgraph *) my_malloc(sizeof(tgraph));
   
	
	if(pgraph == NULL)
	{
		return 1;
	}
	uint16_t sample_num;
	uint8_t sample_depth;
	memset((void *)pgraph,0,sizeof(tgraph));

	pgraph->ctrl.id = ((buffer[0]<<24)|(buffer[1]<<16)|(buffer[4]<<8)|(buffer[5])<<0);
	pgraph->ctrl.func_ctrl_procmsg = graph_msg;
	pgraph->ctrl.func_ctrl_release = graph_release;
   

	pgraph->tp_type = buffer[6];
	pgraph->tp_rect.left = (buffer[7]<<8) | (buffer[8]);
	pgraph->tp_rect.top = (buffer[9]<<8) | (buffer[10]);
	pgraph->tp_rect.right = (buffer[11]<<8) | (buffer[12]);
	pgraph->tp_rect.bottom = (buffer[13]<<8) | (buffer[14]);
	pgraph->pic_id = (buffer[2]<<8) | (buffer[3]);
	pgraph->pic_on = (buffer[15]<<8) | (buffer[16]);
	pgraph->act = graph_act;
	pgraph->draw_screen = graph_draw_screen;

	sample_num = (buffer[17] << 8) | (buffer[18]);

	sample_depth = buffer[19];

	pgraph->anti = buffer[20];
	pgraph->dir = buffer[21];
	pgraph->line_num = buffer[22];
	pgraph->zoom = buffer[23];
	pgraph->start_value = (buffer[24] << 8) | (buffer[25]);
	pgraph->end_value = (buffer[26] << 8) | (buffer[27]);
	pgraph->channel_num = buffer[28];

	if(pgraph->dir == 0)
	{
	    if(sample_num > (pgraph->tp_rect.right - pgraph->tp_rect.left + 1))
	    {
	        sample_num = (pgraph->tp_rect.right - pgraph->tp_rect.left + 1);
	    }
	}
	else
	{
	   if(sample_num > (pgraph->tp_rect.bottom - pgraph->tp_rect.top + 1))
	    {
	        sample_num = (pgraph->tp_rect.bottom - pgraph->tp_rect.top + 1);
	    } 
	}
	

	
	if(buffer[29]==0xFF && buffer[30]==0xFF && buffer[31]==0xFF && buffer[32]==0xFF)
	{
	    uint32_t size;
	    uint8_t i,byte;
		
		if(sample_depth == 0)
		{
			byte = 1;
		}
		else
		{
			byte = 2;
		}
		
	    size = sizeof(graph_info) + 8 * (sizeof(channel_info) + sample_num*byte);
	    pgraph->pgraph = (graph_info *) my_malloc(size);
	    pgraph->pgraph->channel_sta = 0;
	    pgraph->pgraph->sample_num = sample_num;
	    pgraph->pgraph->sample_depth = sample_depth;
		pgraph->pgraph->channel = (channel_info *)((uint8_t *) (pgraph->pgraph) + sizeof(graph_info));
		
	    for(i=0;i<pgraph->channel_num;i++)
	    {   
	        pgraph->pgraph->channel_sta |= (1 << i);
	        pgraph->pgraph->channel[i].color = (buffer[33+2*i] << 8) | (buffer[34+2*i]);
	        pgraph->pgraph->channel[i].len = 0;
	        pgraph->pgraph->channel[i].sample = (uint8_t *)pgraph->pgraph->channel + 8 * sizeof(channel_info) + i*sample_num*byte;
	        
	    }

	    buffer[29] = (uint32_t)(pgraph->pgraph)>>24;
	    buffer[30] = (uint32_t)(pgraph->pgraph)>>16;
	    buffer[31] = (uint32_t)(pgraph->pgraph)>>8;
	    buffer[32] = (uint32_t)(pgraph->pgraph) & (0xFF);

	    
	    
	}
	else
	{
	    pgraph->pgraph = (graph_info *)((buffer[29]<<24) | (buffer[30]<<16) | (buffer[31]<<8) | (buffer[32]));
	}

	ctrl_init((t_ctrl *) pgraph);
	
	return 0;
}




/***************************************************************************************
* End of File: graph.c

****************************************************************************************/
