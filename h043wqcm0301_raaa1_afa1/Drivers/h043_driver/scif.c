
#include "message.h"
#include "usart.h"
#include "cmd.h"
#include "mem_my.h"
#include "usart.h"

extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart1;
SemaphoreHandle_t g_mutex_id_usart_tx;//串口发送互锁

#define USART_BUFFER_SIZE      2048

static uint8_t usart_receive(uint8_t * ch);

static uint8_t rx_buffer[USART_BUFFER_SIZE];
static uint16_t read = 0;
static uint16_t write = 0;
static uint8_t * pcmd;


uint32_t Baud_rate;
SemaphoreHandle_t  uart_rsem;
uint8_t send_compleate = 1;

//SemaphoreHandle_t  uart_ssem;


void take_g_mutex_id_usart_tx(void)
{
	if(g_mutex_id_usart_tx != NULL)
	xSemaphoreTake(g_mutex_id_usart_tx, portMAX_DELAY);
}
void give_g_mutex_id_usart_tx(void)
{
	if(g_mutex_id_usart_tx != NULL)
	xSemaphoreGive(g_mutex_id_usart_tx);

}



static uint16_t  plux( uint16_t  a )
{
	a++;
	a &= (USART_BUFFER_SIZE - 1); //if( a==USART_BUFFER_SIZE )a = 0;
	return a;
}


//static uint16_t minus(uint16_t x0, uint16_t x1)
//{
//    if(x0 >= x1)
//    {
//        return x0-x1;
//    }
//    else
//    {
//        return x0+USART_BUFFER_SIZE-x1;
//    }
//}





void UART0_IRQHandler(void)
{
#if 0
	uint32_t intsrc,tmp;
	BaseType_t temp;
	uint8_t ch,res;

	intsrc = UART_GetIntId(UART_0);
	tmp = intsrc & UART_IIR_INTID_MASK;

	if ((tmp == UART_IIR_INTID_RDA) || (tmp == UART_IIR_INTID_CTI))
	{
		while(1)
		{
			res = usart_receive(&ch);
			if(res)
			{
				xSemaphoreGiveFromISR(uart_rsem, &temp);
				break;
			}
			rx_buffer[write] = ch;
			write = plux(write);
		}

		//UART_IntConfig(UART_0,UART_INTCFG_RBR,DISABLE);
	}
#endif
	BaseType_t temp;
	uint8_t ch,res;
	unsigned int timeout = 0;
	unsigned int maxDelay = 0x1FFFF;

	if(RESET != __HAL_UART_GET_FLAG(&huart5, UART_FLAG_IDLE))
	{
		__HAL_UART_CLEAR_IDLEFLAG(&huart5);
		while (HAL_UART_GetState(&huart1)!=HAL_UART_STATE_READY)//等待就绪
		{
			timeout++;////超时处理
			if(timeout>maxDelay) break;
		}
		while(1)
		{
			res = HAL_UART_Receive_IT(&huart5, &ch, 1);
			if(res)
			{
				xSemaphoreGiveFromISR(uart_rsem, &temp);
				break;
			}
			rx_buffer[write] = ch;
			write = plux(write);
		}

		//UART_IntConfig(UART_0,UART_INTCFG_RBR,DISABLE);
	}


}

void usart_init(uint8_t baud_flag)
{
#if 0
	//lpc
	UART_CFG_Type UARTConfigStruct;
	UART_FIFO_CFG_Type UARTFIFOConfigStruct;


	PINSEL_ConfigPin (0, 2, 1);
	PINSEL_ConfigPin (0, 3, 1);


	UART_ConfigStructInit(&UARTConfigStruct);
	if(baud_flag == 0)
	{
	    UARTConfigStruct.Baud_rate = 1200;
	}
	else if(baud_flag == 1)
	{
	    UARTConfigStruct.Baud_rate = 2400;
	}
	else if(baud_flag == 2)
	{
	    UARTConfigStruct.Baud_rate = 4800;
	}
	else if(baud_flag == 3)
	{
	    UARTConfigStruct.Baud_rate = 9600;
	}
	else if(baud_flag == 4)
	{
	    UARTConfigStruct.Baud_rate = 19200;
	}
	else if(baud_flag == 5)
	{
	    UARTConfigStruct.Baud_rate = 28800;
	}
	else if(baud_flag == 6)
	{
	    UARTConfigStruct.Baud_rate = 38400;
	}
	else if(baud_flag == 7)
	{
	    UARTConfigStruct.Baud_rate = 57600;
	}
	else if(baud_flag == 8)
	{
	    UARTConfigStruct.Baud_rate = 62500;
	}
	else if(baud_flag == 9)
	{
	    UARTConfigStruct.Baud_rate = 76800;
	}
	else if(baud_flag == 10)
	{
	    UARTConfigStruct.Baud_rate = 115200;
	}
	else if(baud_flag == 11)
	{
	    UARTConfigStruct.Baud_rate = 218750;
	}
	else if(baud_flag == 12)
	{
	    UARTConfigStruct.Baud_rate = 230400;
	}
	else if(baud_flag == 13)
	{
	    UARTConfigStruct.Baud_rate = 250000;
	}
	else if(baud_flag == 14)
	{
	    UARTConfigStruct.Baud_rate = 460800;
	}
	else if(baud_flag == 15)
	{
	    UARTConfigStruct.Baud_rate = 921600;
	}
	else
	{
		UARTConfigStruct.Baud_rate = 115200;
	}

	Baud_rate = UARTConfigStruct.Baud_rate;
	UART_Init(UART_0, &UARTConfigStruct);

	UART_FIFOConfigStructInit(&UARTFIFOConfigStruct);
	UARTFIFOConfigStruct.FIFO_DMAMode = ENABLE;
	UARTFIFOConfigStruct.FIFO_Level = UART_FIFO_TRGLEV2;
	UART_FIFOConfig(UART_0, &UARTFIFOConfigStruct);

	UART_TxCmd(UART_0, ENABLE);

	uart_rsem = xSemaphoreCreateBinary();
	//uart_ssem = xSemaphoreCreateBinary();

	UART_IntConfig(UART_0,UART_INTCFG_RBR,ENABLE);
	//UART_IntConfig(UART_0, UART_INTCFG_RLS, ENABLE);
	NVIC_SetPriority(UART0_IRQn, 17);
	NVIC_EnableIRQ(UART0_IRQn);

	pcmd = (uint8_t *) my_malloc(USART_BUFFER_SIZE);
#endif
	uint32_t baud_remap[16] = {
		1200 , 2400 , 4800 , 9600 , 19200 , 28800 , 38400 , 57600 , 62500 , 76800 , 115200
		};
	UART_HandleTypeDef *huart;
	huart = &huart5;
	HAL_NVIC_DisableIRQ(UART5_IRQn);
	huart->Init.BaudRate = baud_remap[baud_flag];
	if (HAL_UART_Init(huart) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetTxFifoThreshold(huart, UART_TXFIFO_THRESHOLD_7_8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetRxFifoThreshold(huart, UART_RXFIFO_THRESHOLD_8_8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_EnableFifoMode(huart) != HAL_OK)
	{
		Error_Handler();
	}
	Baud_rate = huart->Init.BaudRate;
	__HAL_UART_ENABLE_IT(&huart5, UART_IT_IDLE);
	HAL_NVIC_EnableIRQ(UART5_IRQn);


	huart = &huart1;
	HAL_NVIC_DisableIRQ(USART1_IRQn);
	huart->Init.BaudRate = baud_remap[baud_flag];
	if (HAL_UART_Init(huart) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetTxFifoThreshold(huart, UART_TXFIFO_THRESHOLD_7_8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetRxFifoThreshold(huart, UART_RXFIFO_THRESHOLD_8_8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_EnableFifoMode(huart) != HAL_OK)
	{
		Error_Handler();
	}


	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
	HAL_NVIC_EnableIRQ(USART1_IRQn);

}






static uint8_t usart_receive(uint8_t * ch)
{
#if 0
	//lpc
    //必须先读lsr(线状态)寄存器，然后再读rbr(接收缓冲)寄存器
	__IO uint32_t *LSR = (__IO uint32_t *)&LPC_UART0->LSR;
	if ((*LSR & UART_LSR_RDR))
	{
		*ch = (LPC_UART0->RBR & UART_RBR_MASKBIT);
		return 0;
	}
	return 1;
#endif
	return 1;
}


uint8_t  usart_send_str(uint8_t * str,uint16_t len)
{
#if 0
	//lpc

	if(send_compleate)
	{
		send_compleate = 0;
		GPDMA_Channel_CFG_Type GPDMACfg;
		GPDMA_Init();
		NVIC_DisableIRQ (DMA_IRQn);
		NVIC_SetPriority(DMA_IRQn, 15);

		GPDMACfg.ChannelNum = 2;
		GPDMACfg.SrcMemAddr = (uint32_t)str;
		GPDMACfg.DstMemAddr = 0;
		GPDMACfg.TransferSize = len;
		GPDMACfg.TransferWidth = 0;
		GPDMACfg.TransferType = GPDMA_TRANSFERTYPE_M2P;
		GPDMACfg.SrcConn = 0;
		GPDMACfg.DstConn = GPDMA_CONN_UART0_Tx;
		GPDMACfg.DMALLI = 0;
		GPDMA_Setup(&GPDMACfg);
		NVIC_EnableIRQ (DMA_IRQn);
		GPDMA_ChannelCmd(2, ENABLE);
		return 0;
	}


	return 1;

#endif
	unsigned int time_out = 0;
	take_g_mutex_id_usart_tx();
	for(unsigned char i = 0 ;i < len ; i++)
		{
			USART1->TDR = (uint8_t) *(str + i);
			time_out = HAL_GetTick();
			while((USART1->ISR&0X40)==0)
			{
				if((HAL_GetTick() - time_out) > 500)
				{//100超时
					break;
				}
			}
		}
	give_g_mutex_id_usart_tx();
	return 0;
}


void usart_send(uint8_t ch)
{
#if 0
	//lpc
	__IO uint32_t *LSR = (__IO uint32_t *)&LPC_UART0->LSR;

	while (!(*LSR & UART_LSR_THRE));

	LPC_UART0->THR = ch & UART_THR_MASKBIT;
#endif
}




void usart_task(void *pvParameters)
{

	BaseType_t  result;
	uint16_t len,len1,i;
	uint16_t index;
	uint16_t tail[4];
	uint32_t time = portMAX_DELAY;
	uint8_t  loop = 0;
	uint32_t temp_time =  (1000*10*16/Baud_rate) + 1;
	g_mutex_id_usart_tx = xSemaphoreCreateMutex();

	while(1)
	{
		result = xSemaphoreTake(uart_rsem,time);

		if(result == pdTRUE)
		{
			while(1)
			{
				switch(loop)
				{
					case 0:
					{
						while(rx_buffer[read] != 0xEE && read != write)
						{
							read = plux(read);

						}

						if(read != write)  //找到命令头了
						{
							loop++;
						}
						else
						{
							break;
						}
					}
					case 1:
					{
						len = (write >= read) ? (write - read):(USART_BUFFER_SIZE - (read - write));
						if(len > 5)
						{
							index = plux(read);
							index = plux(index);
							loop++;
						}
						else
						{
							break;
						}
					}
					case 2:
					{


						len1 = (write >= index) ? (write - index):(USART_BUFFER_SIZE - (index - write));

						while(len1 >= 4)
						{
							tail[0] = index;
							tail[1] = plux(tail[0]);
							tail[2] = plux(tail[1]);
							tail[3] = plux(tail[2]);
							if(rx_buffer[tail[0]]!=0xFF || rx_buffer[tail[1]]!=0xFC || rx_buffer[tail[2]]!=0xFF || rx_buffer[tail[3]]!= 0xFF)
							{
								index = plux(index);
								len1 = (write >= index) ? (write - index):(USART_BUFFER_SIZE - (index - write));
							}
							else	//找到尾了
							{
								i = 0;
								index = plux(tail[3]);
								while(1)
								{
									if(read == index)
									{
										break;
									}
									pcmd[i] = rx_buffer[read];
									i++;
									read = plux(read);
								}
								cmd_dev.usart_cmd(pcmd,i);
								loop = 0; //从新获取下条命令
								break;
							}
						}


					}
					break;
					default:
					{
						read = write;
						loop = 0;
					}
					break;

				}

				if(loop)
				{
					time = temp_time;
					break;
				}
				else
				{
					if(read == write)
					{
						time = portMAX_DELAY;
						break;
					}
				}
			}
		}
		else
		{
			loop = 0;
			read = write;
			time = portMAX_DELAY;
		}
	}
}


















