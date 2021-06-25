#include "message.h"
#include "usart.h"
#include "cfg.h"
#include "cmd.h"
#include "mem_my.h"
#include "usart.h"
#include "textbox.h"
//#include "keyboard.h"

#define CMD_BUFFER_SIZE       (2048)




static void cmd_init(void);
static void get_cmd(uint32_t addr,uint16_t len,uint8_t flag);
static void notify_cmd(uint8_t *p,uint16_t len);
static void usart_cmd(uint8_t *p,uint16_t len);
static uint32_t get_seamphore(uint32_t time,uint8_t flag);
static uint32_t give_seamphore(uint8_t flag);
static void cmd_send_str(uint8_t * str,uint16_t len);
extern  uint8_t cmd_handle_func(uint8_t * p,uint16_t len);






static uint8_t internal_cmd[CMD_BUFFER_SIZE];
static uint16_t iwrite,iread;
static uint8_t external_cmd[CMD_BUFFER_SIZE];
static uint16_t ewrite,eread;













cmd_device cmd_dev =
{
	cmd_init,
	get_cmd,
	notify_cmd,
	usart_cmd,
	get_seamphore,
	give_seamphore,


};

static void cmd_init(void)
{

	cmd_dev.cmd_sem = xSemaphoreCreateCounting(10,0);
	cmd_dev.send_sem= xSemaphoreCreateBinary();
}

static uint16_t  plux( uint16_t   a )
{
	a++;
	a &= (CMD_BUFFER_SIZE - 1);
	return a;
}

//static uint16_t plus(uint16_t x0,uint16_t x1)
//{
//    x0 += x1;
//    x0 &= (CMD_BUFFER_SIZE - 1);
//    return x0;
//}

static uint16_t minus(uint16_t x0, uint16_t x1)
{
    if(x0 >= x1)
    {
        return x0-x1;
    }
    else
    {
        return x0+CMD_BUFFER_SIZE-x1;
    }
}




static uint32_t get_seamphore(uint32_t time,uint8_t flag)
{

	if(flag==0)
	{
		return xSemaphoreTake(cmd_dev.cmd_sem,time);
	}
	else if(flag==1)
	{
		return xSemaphoreTake(cmd_dev.send_sem,time);
	}

	return 0;

};


static uint32_t give_seamphore(uint8_t flag)
{
	if(flag==0)
	{
		return xSemaphoreGive(cmd_dev.cmd_sem);
	}
	else if(flag==1)
	{
		return xSemaphoreGive(cmd_dev.send_sem);
	}

	return 0;

}


static void notify_cmd(uint8_t *p,uint16_t len)
{

	uint16_t len1;


    len1 = ((ewrite+len) > (CMD_BUFFER_SIZE)) ? ((CMD_BUFFER_SIZE) - ewrite) :len;
	mem_copy(&external_cmd[ewrite],p,len1);
	ewrite += len1;

    if(ewrite >= CMD_BUFFER_SIZE)
	{
        ewrite -= CMD_BUFFER_SIZE;
	}

	if(len1 < len)
	{
        ewrite = 0;
		len1 = len - len1;
		mem_copy(&external_cmd[ewrite],&p[len - len1],len1);
		ewrite += len1;
	}

    cmd_dev.give_seamphore(1);



}


static void cmd_send_str(uint8_t * str,uint16_t len)
{
    uint16_t len1;
    len1=((len+iwrite) > CMD_BUFFER_SIZE)?(CMD_BUFFER_SIZE-iwrite):len;
    mem_copy(&internal_cmd[iwrite], str,len1);

    iwrite += len1;
	if(iwrite >= CMD_BUFFER_SIZE)
	{
		iwrite -= CMD_BUFFER_SIZE;
	}

    if(len1 < len)
    {
        iwrite = 0;
        len1 = len - len1;
        mem_copy(&internal_cmd[iwrite], &str[len-len1],len1);
        iwrite += len1;
    }
}



static void usart_cmd(uint8_t *p,uint16_t len)
{

	cmd_send_str(p,len);

	cmd_dev.give_seamphore(0);


}


uint32_t get_next_cmd_addr(uint32_t addr)
{
    uint8_t *p = &src_cfg.p_cmd[addr];
    uint16_t i = 0;


    while(1)
    {
        if(p[i] == ';')
        {
            i++;
            break;
        }
        i++;
    }

    return (addr+i);
}

static void get_cmd(uint32_t addr,uint16_t len,uint8_t flag)
{


        uint16_t len1;
        uint16_t addr1;

        if(flag==1)
        {
            len1 = ((ewrite+len) > (CMD_BUFFER_SIZE)) ? ((CMD_BUFFER_SIZE) - ewrite) :len;
            addr1 = addr;
            mem_copy(&external_cmd[ewrite],&src_cfg.p_cmd[addr1],len1);
            ewrite += len1;
			if(ewrite >= CMD_BUFFER_SIZE)
			{
				ewrite -= CMD_BUFFER_SIZE;
			}
            if(len1 < len)
            {
                ewrite = 0;
                addr1 += len1;
                len1 = len -len1;
                mem_copy((void*)&external_cmd[ewrite],(void*)&src_cfg.p_cmd[addr1],len1);
                ewrite += len1;
            }

        }
        else if(flag == 0)
        {
            len = len - 1;
            len1=((len+iwrite) > CMD_BUFFER_SIZE)?(CMD_BUFFER_SIZE-iwrite):len;
            addr1 = addr;
            mem_copy(&internal_cmd[iwrite],&src_cfg.p_cmd[addr1],len1);
            iwrite += len1;
			if(iwrite >= CMD_BUFFER_SIZE)
			{
				iwrite -= CMD_BUFFER_SIZE;
			}
            if(len1 < len)
            {
                iwrite = 0;
                addr1 += len1;
                len1 = len -len1;
                mem_copy((void *)&internal_cmd[iwrite],(void *)&src_cfg.p_cmd[addr1],len1);
                iwrite += len1;
            }
        }


	cmd_dev.give_seamphore(flag);



}







/*
*����:�õ�ָ�������
*flag:
*		1:�ⲿָ��
*		0:�ڲ�ָ��
*/

uint16_t get_cmd_num(uint32_t addr,uint16_t len)
{
	uint16_t num,len0;
	uint8_t * p;


	p = &src_cfg.p_cmd[addr];

	num = 0;
	len0 = 0;

	while(len0 < len)
	{
		if(p[len0] == ';')
		{
			num++;
		}

		len0++;
	}

	return num;
}





void external_cmd_task(void *pvParameters)
{
	uint16_t len;
	uint16_t index,i;
	uint8_t * p = NULL;



	while(1)
	{
wait_cmd1:
		cmd_dev.get_seamphore(portMAX_DELAY,1);

		if(eread == ewrite)
		{
		    goto wait_cmd1;
		}

		index = minus(ewrite, 1);

		if(external_cmd[index] != 0x3b)
		{
		    eread = ewrite;
		    goto wait_cmd1;
		}

		len = (ewrite > eread)?(ewrite-eread):(CMD_BUFFER_SIZE - eread + ewrite);

		if(len == 1)
		{
		    eread = ewrite;
		    goto wait_cmd1;
		}

		if(p)
		{
		    my_free((void *) p);
		    p=NULL;
		}
		i = 0;
		p = my_malloc(len-1);

		while(1)
		{
		    p[i]=external_cmd[eread];
		    eread = plux(eread);
		    if(eread == index)
		    {
		        break;
		    }
		    i++;

		}

		usart_send_str(p,len-1);
		eread = ewrite;

	}


}






#if 0

void cmd_prase_task(void *pvParameters)
{
    uint16_t index,i;
    uint16_t len;
    uint8_t  tail[4];
    uint8_t * p;

    while(1)
    {
 wait_cmd:

       cmd_dev.get_seamphore(portMAX_DELAY,0);

		//xSemaphoreTake(kb_info.hide_sem,portMAX_DELAY);

//       if(iread == iwrite)
//        {
//            goto wait_cmd;
//        }
        len = (iwrite > iread)?(iwrite-iread):(CMD_BUFFER_SIZE - iread + iwrite);
        if(len < 6)
        {
            iread = iwrite;
            goto wait_cmd;
        }
        if(internal_cmd[iread]!=0xEE)
        {
            iread = iwrite;
            goto wait_cmd;
        }

        index = minus(iwrite,1);
        tail[3] = internal_cmd[index];

				index = minus(iwrite,2);
        tail[2] = internal_cmd[index];

				index = minus(iwrite,3);
        tail[1] = internal_cmd[index];


        index = minus(iwrite,4);
        tail[0] = internal_cmd[index];

        if(tail[0] !=0xff || tail[1] !=0xfc ||tail[2] !=0xff || tail[3] != 0xff)
        {
            iread = iwrite;
            goto wait_cmd;
        }

		//xSemaphoreTake(kb_info.hide_sem,portMAX_DELAY);

        i= 0;

        p = (uint8_t *)my_malloc(len - 5);



        while(1)
        {
            iread = plux(iread);

           if(iread == index)
           {
                break;
           }
           p[i]=internal_cmd[iread];
           i++;
        }

        cmd_handle_func(p,len-5);
		iread = iwrite;
		my_free(p);
		//xSemaphoreGive(kb_info.hide_sem);




    }

}

#else

#if 0
void cmd_prase_task(void *pvParameters)
{
    uint16_t cnt,head,i,len,index;
    uint8_t  tail[4];
    uint8_t * p;

    while(1)
    {
 wait_cmd:

       cmd_dev.get_seamphore(portMAX_DELAY,0);

		while(internal_cmd[iread] != 0xEE && iread != iwrite)
		{
			iread = plux(iread);
		}

		if(iread != iwrite)
		{
			head = iread;
			iread = plux(iread);
			cnt = 0;
			len = (iwrite >= iread) ? (iwrite - iread):(CMD_BUFFER_SIZE - (iread - iwrite));
			while(len > 3)
			{
				cnt++;
				index = plux(iread);
				tail[0] = internal_cmd[index];
				index = plux(index);
				tail[1] = internal_cmd[index];
				index = plux(index);
				tail[2] = internal_cmd[index];
				index = plux(index);
				tail[3] = internal_cmd[index];

				iread = plux(iread);

				if(tail[0] == 0xFF && tail[1] == 0xFC && tail[2] == 0xFF && tail[3] == 0xFF)
				{
					p = (uint8_t *)my_malloc(cnt);
					for(i=0;i<cnt;i++)
					{
						head = plux(head);
						p[i] = internal_cmd[head];
					}
					cmd_handle_func(p,cnt);
					my_free(p);
					break;

				}

				len = (iwrite >= iread) ? (iwrite - iread):(CMD_BUFFER_SIZE - (iread - iwrite));
			}

			if(iread < 4)
			{
				iread = iwrite;
				goto wait_cmd;
			}



		}
		else
		{
			goto wait_cmd;
		}




    }




}

#else


#if 0

void cmd_prase_task(void *pvParameters)
{
	uint16_t cnt,head,len,index,i;
	uint8_t step = 0;
	uint8_t tail[4];
	 uint8_t * p;

	while(1)
	{

		cmd_dev.get_seamphore(portMAX_DELAY,0);
		len = (iwrite >= iread) ? (iwrite - iread):(CMD_BUFFER_SIZE - (iread - iwrite));
		if(len >= 6)
		{
			while(1)
			{
				switch(step)
				{
					case 0:
					{
						while(internal_cmd[iread] != 0xEE && iread != iwrite)
						{
							iread = plux(iread);
						}

						if(iread != iwrite)
						{
							head = iread;
							iread = plux(iread);
							cnt = 0;
							step++;
						}
						else
						{
							break;
						}
					}
					case 1:
					{
						len = (iwrite >= iread) ? (iwrite - iread):(CMD_BUFFER_SIZE - (iread - iwrite));
						while(len > 3)
						{
							cnt++;
							index = plux(iread);
							tail[0] = internal_cmd[index];
							index = plux(index);
							tail[1] = internal_cmd[index];
							index = plux(index);
							tail[2] = internal_cmd[index];
							index = plux(index);
							tail[3] = internal_cmd[index];

							if(tail[0] != 0xFF || tail[1] != 0xFC || tail[2] != 0xFF || tail[3] != 0xFF)
							{
								iread = plux(iread);
								len = (iwrite >= iread) ? (iwrite - iread):(CMD_BUFFER_SIZE - (iread - iwrite));
							}
							else
							{
								p = (uint8_t *)my_malloc(cnt);

								for(i=0;i<cnt;i++)
								{
									head = plux(head);
									p[i] = internal_cmd[head];
								}
								cmd_handle_func(p,cnt);
								iread = plux(index);
								len = (iwrite >= iread) ? (iwrite - iread):(CMD_BUFFER_SIZE - (iread - iwrite));
								my_free(p);
								break;
							}
						}
						step = 0;
						break;

					}
					default:
					{
						len = 0;
						step = 0;
						break;
					}
				}

				if(len < 6)
				{
					iread = iwrite;
					break;
				}
			}
		}
		else
		{
			iread = iwrite;
			step = 0;
		}
	}
}

#else



void cmd_prase_task(void *pvParameters)
{
	uint16_t cnt,head,len,index,i;
	uint8_t tail[4];
	uint8_t * p;

	while(1)
	{

		cmd_dev.get_seamphore(portMAX_DELAY,0);
		len = (iwrite >= iread) ? (iwrite - iread):(CMD_BUFFER_SIZE - (iread - iwrite));

		if(len >= 6)
		{
			while(1)
			{
				while(internal_cmd[iread] != 0xEE && iread != iwrite)
				{
					iread = plux(iread);
				}

				if(iread != iwrite)
				{
					head = iread;
					iread = plux(iread);
					cnt = 0;
				}

				len = (iwrite >= iread) ? (iwrite - iread):(CMD_BUFFER_SIZE - (iread - iwrite));

				while(len > 3)
				{
					cnt++;
					index = plux(iread);
					tail[0] = internal_cmd[index];
					index = plux(index);
					tail[1] = internal_cmd[index];
					index = plux(index);
					tail[2] = internal_cmd[index];
					index = plux(index);
					tail[3] = internal_cmd[index];

					if(tail[0] != 0xFF || tail[1] != 0xFC || tail[2] != 0xFF || tail[3] != 0xFF)
					{
						iread = plux(iread);
						len = (iwrite >= iread) ? (iwrite - iread):(CMD_BUFFER_SIZE - (iread - iwrite));
					}
					else
					{
						p = (uint8_t *)my_malloc(cnt);

						for(i=0;i<cnt;i++)
						{
							head = plux(head);
							p[i] = internal_cmd[head];
						}
						cmd_handle_func(p,cnt);
						iread = plux(index);
						len = (iwrite >= iread) ? (iwrite - iread):(CMD_BUFFER_SIZE - (iread - iwrite));
						my_free(p);
						break;
					}

				}

				if(len < 6)
				{
					iread = iwrite;
					break;
				}
			}
		}
		else
		{
			iread = iwrite;
		}

	}
}


#endif






#endif








#endif





