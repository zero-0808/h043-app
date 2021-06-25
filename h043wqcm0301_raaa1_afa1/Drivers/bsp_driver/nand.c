



#include "nand.h"
#include "mem_my.h"
#include "string.h"
//#include "update.h"

uint32_t cnt1;
uint32_t cnt2;
u8 wait =0;
static void NandFlash_Init( void );
//static void NandFlash_WaitForReady( void );
static void get_block_information(uint16_t block, uint8_t * buffer);
static uint32_t NandFlash_ReadId( void );
static uint8_t create_lut(uint8_t flag);
//static void NandFlash_Reset( void );
static uint8_t NandFlash_Reset( void );
//static uint8_t NandFlash_BlockErase( uint32_t blockNum );
static uint8_t NandFlash_BlockErase(uint32_t BlockNum,uint32_t block_offset );
static uint8_t NandFlash_ReadStatus(uint32_t Cmd);
static uint8_t block_is_valid(uint32_t block);

///static uint8_t NandFlash_PageProgram( uint32_t blockNum, uint32_t pageNum, uint8_t *bufPtr,uint8_t bSpareProgram  );
static uint8_t NandFlash_PageProgram(uint32_t PageNum,uint16_t ColNum,uint8_t *pBuffer,uint16_t NumByteToWrite );
static uint8_t NandFlash_PageRead( u32 PageNum,u16 ColNum,u8 *pBuffer,u16 NumByteToRead);

static uint32_t NandFlash_PageReadFromAddr(uint32_t blockNum, uint32_t pageNum,
                                            uint32_t addrInPage, uint8_t* bufPtr, uint32_t size);
static uint32_t NandFlash_Read_Source(uint32_t block,uint32_t page,uint32_t addr,uint8_t * buf,uint32_t size);


static uint8_t NandFlash_BlockProgram(uint32_t block,uint32_t page,uint32_t page_offset,uint8_t * buf,uint32_t size);
static uint32_t NandFlash_Program(uint32_t addr,uint8_t * buf,uint32_t size);



u8 NAND_Reset(void);
u8 NAND_WaitForReady(void);
u32 NAND_ReadID(void);
u16 NAND_LBNToPBN(u32 LBNNum);



nand_attriute nand_dev =
{
	NandFlash_Init,
	create_lut,
	NAND_ReadID,
	NandFlash_BlockErase,
	NandFlash_PageProgram,
	NandFlash_Read_Source,
    NandFlash_Program,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,		
};

static volatile u8 waitrb;
void nandwaitrb(void)
{
	waitrb = 1;
}
void nandwaitrbdown(void)
{
	waitrb = 0;
}
static u8 nandgetwaitstate(void)
{
	return waitrb;
}
//NAND延时
void NAND_Delay(vu32 i)
{
	while(i>0)i--;
}
#if 0
static void NandFlash_WaitForReady( void )
{

    while( FIO2PIN & (1 << 21) );       /* from high to low once */

    while( !(FIO2PIN & (1 << 21)) );    /* from low to high once */


    return;
}
#endif
//读取NAND FLASH的ID
//不同的NAND略有不同，请根据自己所使用的NAND FALSH数据手册来编写函数
//返回值:NAND FLASH的ID值
u32 NAND_ReadID(void)
{
    u8 deviceid[5];
    u32 id;
    *(vu8*)(NAND_ADDRESS|NAND_CMD)=NAND_READID; //发送读取ID命令
    *(vu8*)(NAND_ADDRESS|NAND_ADDR)=0X00;

	//NOP指令E用于CPU空转延时，共60ns, tWHR
    HAL_Delay(NAND_TWHR_DELAY);

	//ID一共有5个字节
    deviceid[0]=*(vu8*)NAND_ADDRESS;
    deviceid[1]=*(vu8*)NAND_ADDRESS;
    deviceid[2]=*(vu8*)NAND_ADDRESS;
    deviceid[3]=*(vu8*)NAND_ADDRESS;
    deviceid[4]=*(vu8*)NAND_ADDRESS;
    //镁光的NAND FLASH的ID一共5个字节，但是为了方便我们只取4个字节组成一个32位的ID值
    //根据NAND FLASH的数据手册，只要是镁光的NAND FLASH，那么一个字节ID的第一个字节都是0X2C
    //所以我们就可以抛弃这个0X2C，只取后面四字节的ID值。
    id=((u32)deviceid[1])<<24|((u32)deviceid[2])<<16|((u32)deviceid[3])<<8|deviceid[4];
    if(NAND_WaitForReady()==NSTA_READY)return id;//成功
    else return 0xFFFFFFFF;
}

static void NandFlash_Init( void )
{
#if 0 /*lpc上使用，废弃*/
	 TIM_TIMERCFG_Type TIM_ConfigStruct;
	 EMC_STATIC_MEM_Config_Type config;

	config.CSn = 0;//1;
	config.AddressMirror = 0;
	config.ByteLane = 1;
	config.DataWidth = 8;
	config.ExtendedWait = 0;
	config.PageMode = 0;
	config.WaitWEn = 0;
	config.WaitOEn = 0;
	config.WaitWr = 0x2;
	config.WaitPage = 0x0;
	config.WaitRd = 0x2;
	config.WaitTurn = 0x0;


	StaticMem_Init(&config);
  // init timer
	TIM_ConfigStruct.PrescaleOption = TIM_PRESCALE_USVAL;
	TIM_ConfigStruct.PrescaleValue	= 1;

	// Set configuration for Tim_config and Tim_MatchConfig
	TIM_Init(LPC_TIM0, TIM_TIMER_MODE,&TIM_ConfigStruct);

  // wait 2ms
	TIM_Waitms(2);
#else
    NAND_Reset();       		        //复位NAND
    HAL_Delay(100);
    nand_dev.id=NAND_ReadID();	        //读取ID
    //printf("NAND ID:%#x\r\n",nand_dev.id);
	//NAND_ModeSet(4);			        //设置为MODE4,高速模式 ，这个到时看看需不需要设置
    nand_dev.page_totalsize=2112;
    nand_dev.page_mainsize=2048;
    nand_dev.page_sparesize=64;
    nand_dev.block_pagenum=64;
    nand_dev.plane_blocknum=1024;
    nand_dev.block_totalnum=2048;

#endif
}
//读NAND状态
//返回值:NAND状态值
//bit0:0,成功;1,错误(编程/擦除/READ)
//bit6:0,Busy;1,Ready
u8 NAND_ReadStatus(void)
{
    vu8 data=0;
    *(vu8*)(NAND_ADDRESS|NAND_CMD)=NAND_READSTA;//发送读状态命令
    HAL_Delay(NAND_TWHR_DELAY);		//等待tWHR,再读取状态寄存器
	data=*(vu8*)NAND_ADDRESS;			//读取状态值
    return data;
}

//复位NAND
//返回值:0,成功;
//    其他,失败
u8 NAND_Reset(void)
{
    *(vu8*)(NAND_ADDRESS|NAND_CMD)=NAND_RESET;	//复位NAND
	/* 镁光需要等100ns的tWB+100us的tRST */
    HAL_Delay(NAND_TRST_FIRST_DELAY);
    if(NAND_WaitForReady()==NSTA_READY)return 0;//复位成功
    else return 1;								//复位失败
}


static void get_block_information(uint16_t block, uint8_t * buffer)
{
	NandFlash_PageReadFromAddr(block, 0,2048,buffer,6);
}

//��������nand flash
void nand_erase(void)
{
    uint16_t i;
    for(i=0;i<nand_dev.block_totalnum; i++ )
    {
       // if(NandFlash_BlockErase(i) == 0)
        {
//            if(block_is_valid(i))
//            {
//                nand_dev.lut[n] = i;
//                n++;						
//            }
        }
    }
}
//等待NAND准备好
//返回值:NSTA_TIMEOUT 等待超时了
//      NSTA_READY    已经准备好
u8 NAND_WaitForReady(void)
{
    u8 status=0;
    vu32 time=0;
	while(1)						//等待ready
	{
		status=NAND_ReadStatus();	//获取状态值
		if(status&NSTA_READY)break;
		time++;
		if(time>=0X1FFFF)return NSTA_TIMEOUT;//超时
	}
    return NSTA_READY;//准备好
}
//读取spare区中的数据
//PageNum:要写入的页地址,范围:0~(block_pagenum*block_totalnum-1)
//ColNum:要写入的spare区地址(spare区中哪个地址),范围:0~(page_sparesize-1)
//pBuffer:接收数据缓冲区
//NumByteToRead:要读取的字节数(不大于page_sparesize)
//返回值:0,成功
//    其他,错误代码
u8 NAND_ReadSpare(u32 PageNum,u16 ColNum,u8 *pBuffer,u16 NumByteToRead , uint32_t block_offset)
{
    u8 temp=0;
    u8 remainbyte=0;
	PageNum = PageNum +block_offset * nand_dev.block_pagenum;
    remainbyte=nand_dev.page_sparesize-ColNum;
    if(NumByteToRead>remainbyte) NumByteToRead=remainbyte;  //确保要写入的字节数不大于spare剩余的大小
    temp=NandFlash_PageRead(PageNum,ColNum+nand_dev.page_mainsize,pBuffer,NumByteToRead);//读取数据
    return temp;
}

static uint8_t create_lut(uint8_t mode)
{
#if 0
	/*lpc上使用，废弃*/
	uint16_t i,n = 0,u = 0;
	uint8_t buf[6];
	uint8_t start_block;
	NandFlash_Reset();
	nand_dev.id = NandFlash_ReadId();
	if(nand_dev.id == 0XF1EC0095 || nand_dev.id == 0XECF19500 || nand_dev.id == 0x01F11D80 || nand_dev.id == 0x01F1801D \
		|| nand_dev.id == 0xEFDA9590)
	{
		if(nand_dev.id == 0xEFDA9590)
		{
			nand_dev.plane_blocknum = 1024;
			nand_dev.block_totalnum = 2048;
			nand_dev.page_mainsize = 2048;
			nand_dev.page_sparesize =64;
			nand_dev.page_totalsize = 2112;
			nand_dev.block_pagenum = 64;
		}
		else
		{
			nand_dev.plane_blocknum = 1024;
			nand_dev.block_totalnum = 1024;
			nand_dev.page_mainsize = 2048;
			nand_dev.page_sparesize =64;
			nand_dev.page_totalsize = 2112;
			nand_dev.block_pagenum = 64;
		}
		nand_dev.lut = (uint16_t *) my_malloc(nand_dev.block_totalnum*sizeof(2));

		if(nand_dev.lut == NULL)
		{
			return 1;
		}


		if(flag != 1)
		{
		    for ( i = 0; i < nand_dev.block_totalnum; i++ )
			{
				get_block_information(i,buf);

				if(buf[0] == 0xff)
				{
					nand_dev.lut[n] = i;
					n++;
					if(buf[1] == 0xee)
					{
						u++;
					}

				}
				nand_dev.good_blocknum = n;
				nand_dev.use_blocknum = u;
			}
		}

		EEPROM_Read(0,0,(void*)buf,MODE_8_BIT,6);

		if((buf[0] != 'n')||(buf[1] != 'a')||(buf[2] != 'n')||(buf[3] != 'd')||(flag))
		{
		    if(flag == 2)
		    {
		        start_block = nand_dev.lut[src_cfg.block_touch_addr];
		        n = src_cfg.block_touch_addr;     //����������
		    }
		    else
		    {
		        start_block = 0;
		        n = 0;
		    }
			for(i = start_block; i < nand_dev.block_totalnum; i++ )
			{
				if(NandFlash_BlockErase(i) == 0)
				{
					if(block_is_valid(i))
					{
						nand_dev.lut[n] = i;
						n++;
					}
				}
			}
			nand_dev.good_blocknum = n;
			nand_dev.use_blocknum = 0;
			EEPROM_Erase(0);
			buf[0] = 'n';
			buf[1] = 'a';
			buf[2] = 'n';
			buf[3] = 'd';
			EEPROM_Write(0,0,(void*)buf,MODE_8_BIT,6);
		}



	}
	return 0;
#else
	 	 u32 i , n = 0;
	 	u8 buf[4];
		nand_dev.lut=my_malloc((nand_dev.block_totalnum)*2); 	//��LUT�������ڴ�

		if(!nand_dev.lut)return 1;				//�ڴ�����ʧ��
		memset(nand_dev.lut,0,nand_dev.block_totalnum*2);			//ȫ������

		for(i=0;i<nand_dev.block_totalnum;i++)		//��λLUT����ʼ��Ϊ��Чֵ��Ҳ����0XFFFF
	    {
	        nand_dev.lut[i]=0XFFFF;
	    }
		nand_dev.good_blocknum=0;
		n = 0;
	    for(i=0;i<nand_dev.block_totalnum;i++)
	    {
			NAND_ReadSpare(i*nand_dev.block_pagenum,0,buf,4 , APP_NAND_START_BLOCK);	//��ȡ4���ֽ�
			if(buf[0]==0XFF&&mode)NAND_ReadSpare(i*nand_dev.block_pagenum+1,0,buf,1 , APP_NAND_START_BLOCK);//�ÿ�,����Ҫ���2�λ�����
			if(buf[0]==0XFF)//�Ǻÿ�
	        {
	            nand_dev.lut[n]=i;//找到nand flaash 0-2047块当中的好快 , 防止出现坏块，逻辑顺序编号0-2047，例如实际有坏块2，则物理上就会跳过这个坏块
				nand_dev.good_blocknum++;
				n++;
			}else
			{
				//printf("bad block index:%d\r\n",i);
			}
	    }
		nand_dev.valid_blocknum = nand_dev.good_blocknum;
	    if(nand_dev.valid_blocknum<100)return 2;	//��Ч����С��100,������.��Ҫ���¸�ʽ��
	    return 0;	//LUT�������
#endif
}


static uint8_t NandFlash_Reset( void )
{
#if 0
	/*lpc上使用，废弃*/
	volatile uint8_t *pCLE;

    /* Reset NAND FLASH  through NAND FLASH command */
    pCLE = K9F1G_CLE;
   *pCLE = K9FXX_RESET;//��λָ�һ������

    TIM_Waitms(2);
    return;
#else
    *(vu8*)(NAND_ADDRESS|NAND_CMD)=NAND_RESET;	//复位NAND
  	/* 镁光需要等100ns的tWB+100us的tRST */
      HAL_Delay(NAND_TRST_FIRST_DELAY);
      if(NAND_WaitForReady()==NSTA_READY)return 0;//复位成功
      else return 1;

#endif

}


static uint8_t NandFlash_ReadStatus(uint32_t Cmd)
{
#if 0
	/*lpc上使用，废弃*/
    volatile uint8_t *pCLE;
    volatile uint8_t *pDATA;
    uint8_t StatusData;

    pCLE  = K9F1G_CLE;
    pDATA = K9F1G_DATA;

    *pCLE = K9FXX_READ_STATUS;

#if (_CUR_USING_NANDFLASH == _RUNNING_NANDFLASH_K9F1G08U0C)
    while ( (*pDATA & 0xC0) != 0xC0 );
#else
    /* Wait until bit 5 and 6 are 1, READY, bit 7 should be 1 too, not protected */
    /* if ready bit not set, it gets stuck here */
    while ( (*pDATA & 0xE0) != 0xE0 );
#endif

    StatusData = *pDATA;

    switch (Cmd)
    {
        case K9FXX_BLOCK_PROGRAM_1:
        case K9FXX_BLOCK_ERASE_1:
            if (StatusData & 0x01)  /* Erase/Program failure(1) or pass(0) */
                return(1);
            else
                return(0);

        case K9FXX_READ_1:              /* bit 5 and 6, Read busy(0) or ready(1) */
            return(0);

        default:
            break;
    }

    return(FALSE);
#else
/**
* 读NAND状态
*	返回值:NAND状态值
*bit0:0,成功;1,错误(编程/擦除/READ)
*	bit6:0,Busy;1,Ready
*/
    vu8 data=0;
	*(vu8*)(NAND_ADDRESS|NAND_CMD)=Cmd;//NAND_READSTA;//发送读状态命令
	NAND_Delay(NAND_TWHR_DELAY);		//等待tWHR,再读取状态寄存器
	data=*(vu8*)NAND_ADDRESS;			//读取状态值
	return data;

#endif
}
/*lpc上使用，废弃*/
static uint32_t NandFlash_ReadId( void )
{
    uint8_t a, b, c, d;
    volatile uint8_t *pCLE;
    volatile uint8_t *pALE;
    volatile uint8_t *pDATA;

    pCLE  = K9F1G_CLE;
    pALE  = K9F1G_ALE;
    pDATA = K9F1G_DATA;

    *pCLE = K9FXX_READ_ID;
    *pALE = 0;

    a = *pDATA;
    b = *pDATA;
    d = *pDATA;
    c = *pDATA;

    return ((a << 24) | (b << 16) | (c << 8) | d);
}


static uint8_t NandFlash_BlockErase(uint32_t BlockNum,uint32_t block_offset )
{
#if 0 /*lpc上使用，废弃*/
    volatile uint8_t *pCLE;
    volatile uint8_t *pALE;
    uint32_t rowAddr;

    pCLE  = K9F1G_CLE;
    pALE  = K9F1G_ALE;

    rowAddr = blockNum*nand_dev.block_pagenum;

    *pCLE = K9FXX_BLOCK_ERASE_1;

    if(nand_dev.id == 0xEFDA9590)
	{
	    *pALE = (uint8_t)(rowAddr & 0x00FF);            /* column address low */
	    *pALE = (uint8_t)((rowAddr & 0xFF00) >> 8); /* column address high */
	    *pALE = (uint8_t)((rowAddr & 0x10000) >> 16); /* column address high */
    }
    else
    {
    	*pALE = (uint8_t)(rowAddr & 0x00FF);            /* column address low */
	    *pALE = (uint8_t)((rowAddr & 0xFF00) >> 8); /* column address high */
	    
    }

    *pCLE = K9FXX_BLOCK_ERASE_2;

    if(NAND_WaitForReady()== NSTA_READY)
    return(NandFlash_ReadStatus(K9FXX_BLOCK_ERASE_1));
#else
    	BlockNum = BlockNum + block_offset;

    	uint32_t max_pagenum;
    	max_pagenum = nand_dev.block_totalnum;
    	if(BlockNum > max_pagenum)
    	{//判断地址是否会超出范围
    		return 0;
    	}
    	BlockNum = BlockNum * nand_dev.block_pagenum;


        *(vu8*)(NAND_ADDRESS|NAND_CMD)=NAND_ERASE0;
        //发送块地址
        *(vu8*)(NAND_ADDRESS|NAND_ADDR)=(u8)BlockNum;
        *(vu8*)(NAND_ADDRESS|NAND_ADDR)=(u8)(BlockNum>>8);
        *(vu8*)(NAND_ADDRESS|NAND_ADDR)=(u8)(BlockNum>>16);
    	nandwaitrb();
        *(vu8*)(NAND_ADDRESS|NAND_CMD)=NAND_ERASE1;
    	HAL_Delay(NAND_TBERS_DELAY);		//等待擦除成功
    	cnt2++;
    	//NAND_Delay(NAND_TPROG_DELAY);	//等待tPROG

    	NAND_WaitRB(0 , NAND_RB_TBERS_TIME_OUT);			//等待RB=0
        //下面2行代码是真正判断NAND是否准备好的
    	NAND_WaitRB(1 , NAND_RB_TBERS_TIME_OUT);			//等待RB=1

    	if(NAND_WaitForReady()!=NSTA_READY)return NSTA_ERROR;//失败
        return 0;	//成功
#endif
}




static uint8_t block_is_valid(uint32_t block)
{
	uint32_t page;
	uint8_t data;
	for(page = 0; page < 2; page++)
	{
		 NandFlash_PageReadFromAddr(block, page, nand_dev.page_mainsize, &data, 1);
		if(data == 0XFF)
		{
			
			return 1;
			
		}
	}
	//PRINT("block : %d data: %d\r\n",block,data);
	return 0;
}
//在NAND一页中写入指定个字节的数据(main区和spare区都可以使用此函数)
//PageNum:要写入的页地址,范围:0~(block_pagenum*block_totalnum-1)
//ColNum:要写入的列开始地址(也就是页内地址),范围:0~(page_totalsize-1)
//pBbuffer:指向数据存储区
//NumByteToWrite:要写入的字节数，该值不能超过该页剩余字节数！！！
//返回值:0,成功
//    其他,错误代码
static uint8_t NandFlash_PageProgram(uint32_t PageNum,uint16_t ColNum,uint8_t *pBuffer,uint16_t NumByteToWrite )
{
#if 0/*lpc上使用，废弃*/
    volatile uint8_t *pCLE;
    volatile uint8_t *pALE;
    volatile uint8_t *pDATA;
    uint32_t i, curRow, curColumm;
    uint16_t programSize = nand_dev.page_mainsize;

    pCLE  = K9F1G_CLE;
    pALE  = K9F1G_ALE;
    pDATA = K9F1G_DATA;

    curColumm = 0;
    curRow = blockNum*nand_dev.block_pagenum + pageNum;
    
    if(bSpareProgram)
        programSize = nand_dev.page_totalsize;
    
    *pCLE = K9FXX_BLOCK_PROGRAM_1;
	
	if(nand_dev.id == 0XF1EC0095 || nand_dev.id == 0XECF19500 || nand_dev.id == 0x01F11D80 || nand_dev.id == 0x01F1801D)
	{
		*pALE =  (uint8_t)(curColumm & 0x000000FF);     /* column address low */

		*pALE = (uint8_t)((curColumm & 0x00000F00) >> 8);   /* column address high */

		*pALE = (uint8_t)((curRow & 0x00FF));    /* row address low */

		*pALE = (uint8_t)((curRow & 0xFF00) >> 8);    /* row address high */
	}
	else if(nand_dev.id == 0xEFDA9590)
	{
		*pALE =  (uint8_t)(curColumm & 0x000000FF);     /* column address low */

		*pALE = (uint8_t)((curColumm & 0x00000F00) >> 8);   /* column address high */

		*pALE = (uint8_t)((curRow & 0x00FF));    /* row address low */

		*pALE = (uint8_t)((curRow & 0xFF00) >> 8);    /* row address high */
		
		*pALE = (uint8_t)((curRow & 0x10000) >> 16);    /* row address high */
	}

    for ( i = 0; i < programSize; i++ )
    {
        *pDATA = *bufPtr++;
    }

    *pCLE = K9FXX_BLOCK_PROGRAM_2;
	
    if(NAND_WaitForReady()== NSTA_READY)
    return( NandFlash_ReadStatus( NAND_READSTA ) );
#else
    vu16 i=0;
    	uint32_t max_pagenum;
    	max_pagenum = nand_dev.block_pagenum;
    	max_pagenum = max_pagenum * nand_dev.block_totalnum - 1;
    	if(PageNum > max_pagenum)
    	{//判断地址是否会超出范围
    		return 0;
    	}

#ifdef rtos
    	uint8_t need_resum = 0;
    	if(xTaskGetSchedulerState() == taskSCHEDULER_RUNNING)
    	{
    		vTaskSuspendAll();
    		need_resume = 1;
    	}
    	else
    	{
    		need_resume = 0;
    	}
#endif
    	*(vu8*)(NAND_ADDRESS|NAND_CMD)=NAND_WRITE0;
        //发送地址
        *(vu8*)(NAND_ADDRESS|NAND_ADDR)=(u8)ColNum;
        *(vu8*)(NAND_ADDRESS|NAND_ADDR)=(u8)(ColNum>>8);
        *(vu8*)(NAND_ADDRESS|NAND_ADDR)=(u8)PageNum;
        *(vu8*)(NAND_ADDRESS|NAND_ADDR)=(u8)(PageNum>>8);
        *(vu8*)(NAND_ADDRESS|NAND_ADDR)=(u8)(PageNum>>16);
    	NAND_Delay(NAND_TADL_DELAY);			//等待tADL
    	for(i=0;i<NumByteToWrite;i++)		//写入数据
    	{
    		*(vu8*)NAND_ADDRESS=*(vu8*)pBuffer++;
    	}
    	nandwaitrb();
        *(vu8*)(NAND_ADDRESS|NAND_CMD)=NAND_WRITE_TURE1;
    	cnt2++;
    	//NAND_Delay(NAND_TPROG_DELAY);	//等待tPROG

    	NAND_WaitRB(0 , NAND_RB_WRITEPAGE_TIME_OUT);			//等待RB=0
        //下面2行代码是真正判断NAND是否准备好的
    	NAND_WaitRB(1 , NAND_RB_WRITEPAGE_TIME_OUT);			//等待RB=1

    	if(NAND_WaitForReady()!=NSTA_READY)return NSTA_ERROR;//失败
#if rtos
    	if(need_resume == 1)
    	{
    		xTaskResumeAll();
    	}
#endif
        return 0;//成功
#endif
}


static uint8_t NandFlash_PageRead(  u32 PageNum,u16 ColNum,u8 *pBuffer,u16 NumByteToRead )
{
#if 0

    return ((NandFlash_PageReadFromBeginning(blockNum, pageNum, bufPtr) != 0) ? TRUE:FALSE);
#else
    vu16 i=0;
	u8 res=0;
	u8 errsta=0;
//	if(getfirm_updatefile() == 1)
//	{
//	#if 0
//		for(i=0;i<NumByteToRead;i++)
//		{
//			*(vu8*)pBuffer++ = 0;//加这句为了防止返回值不定
//		}
//	#endif
//		return 0;
//	}
#ifdef rtos
	uint8_t need_resume = 0;

	if(xTaskGetSchedulerState() == taskSCHEDULER_RUNNING)
	{
		vTaskSuspendAll();
		need_resume = 1;
	}
	else
	{
		need_resume = 0;
	}
#endif
	wait = 0;
    *K9F1G_CLE=NAND_AREA_A;
    //发送地址
    *K9F1G_ALE=(u8)ColNum;
    *K9F1G_ALE=(u8)(ColNum>>8);
    *K9F1G_ALE=(u8)PageNum;
    *K9F1G_ALE=(u8)(PageNum>>8);
    *K9F1G_ALE=(u8)(PageNum>>16);
	nandwaitrb();
    *K9F1G_CLE=NAND_AREA_TRUE1;
	cnt2++;
    //下面两行代码是等待R/B引脚变为低电平，其实主要起延时作用的，等待NAND操作R/B引脚。因为我们是通过
    //将STM32的NWAIT引脚(NAND的R/B引脚)配置为普通IO，代码中通过读取NWAIT引脚的电平来判断NAND是否准备
    //就绪的。这个也就是模拟的方法，所以在速度很快的时候有可能NAND还没来得及操作R/B引脚来表示NAND的忙
    //闲状态，结果我们就读取了R/B引脚,这个时候肯定会出错的，事实上确实是会出错!大家也可以将下面两行
    //代码换成延时函数,只不过这里我们为了效率所以没有用延时函数。
	res=NAND_WaitRB(0 , NAND_RB_READPAGE_TIME_OUT);			//等待RB=0
    if(res)return NSTA_TIMEOUT;	//超时退出
    //下面2行代码是真正判断NAND是否准备好的
	res=NAND_WaitRB(1 , NAND_RB_READPAGE_TIME_OUT);			//等待RB=1
    if(res)return NSTA_TIMEOUT;	//超时退出
	//读取NAND FLASH中的值
	for(i=0;i<NumByteToRead;i++)
	{
		*(vu8*)pBuffer++ = *K9F1G_DATA;
	}
	if(wait == 1)
	{
		//printf("wait = %d\r\n" , wait);
		wait = 2;

	}

	if(NAND_WaitForReady()!=NSTA_READY)errsta=NSTA_ERROR;	//失败
#ifdef rtos
	if(need_resume == 1)
	{
		xTaskResumeAll();
	}
#endif
		return errsta;	//成功
#endif
}



static uint32_t NandFlash_PageReadFromAddr(uint32_t blockNum, uint32_t pageNum,
                                            uint32_t addrInPage, uint8_t* bufPtr, uint32_t size)
{
    volatile uint8_t *pCLE;
    volatile uint8_t *pALE;
    volatile uint8_t *pDATA;
    uint32_t i, curColumm, curRow;    

    i = 0;

    pCLE  = K9F1G_CLE;
    pALE  = K9F1G_ALE;
    pDATA = K9F1G_DATA;

    curColumm = addrInPage;
    curRow = blockNum*nand_dev.block_pagenum + pageNum;
	//vTaskSuspendAll();
	//taskENTER_CRITICAL();
    *pCLE = K9FXX_READ_1;
    if(nand_dev.id == 0XF1EC0095 || nand_dev.id == 0XECF19500 || nand_dev.id == 0x01F11D80 || nand_dev.id == 0x01F1801D)
	{
		*pALE =  (uint8_t)(curColumm & 0x000000FF);     /* column address low */

		*pALE = (uint8_t)((curColumm & 0x00000F00) >> 8);   /* column address high */

		*pALE = (uint8_t)((curRow & 0x00FF));    /* row address low */

		*pALE = (uint8_t)((curRow & 0xFF00) >> 8);    /* row address high */
	}
	else if(nand_dev.id == 0xEFDA9590)
	{
		*pALE =  (uint8_t)(curColumm & 0x000000FF);     /* column address low */

		*pALE = (uint8_t)((curColumm & 0x00000F00) >> 8);   /* column address high */

		*pALE = (uint8_t)((curRow & 0x00FF));    /* row address low */

		*pALE = (uint8_t)((curRow & 0xFF00) >> 8);    /* row address high */
		
		*pALE = (uint8_t)((curRow & 0x10000) >> 16);    /* row address high */
	}
    *pCLE = K9FXX_READ_2;
    if(NAND_WaitForReady()== NSTA_READY)

	//taskEXIT_CRITICAL();
	

    //Get data from the current address in the page til the end of the page
	
	
    for ( i = 0; i < (nand_dev.page_totalsize - curColumm); i++ )
    {
		
		
        *bufPtr = *pDATA;

        bufPtr++;

        if((i + 1) >= size)
        break;
		
		
		
		
    }
	//xTaskResumeAll();
	

    // Ok, return
    return i;
}

                                            
//��nand flash��ҳ���ݣ���ҳ                                            
static uint32_t NandFlash_Read_Source(uint32_t block,uint32_t page,uint32_t addr,uint8_t * buf,uint32_t size)
{
    uint32_t size1,size2,temp_size;
    uint16_t block1,page1,addr1;

    size1 = size;
    size2 = 0;
    uint8_t * p;
    block1 = block;
    page1 = page;
    addr1 = addr;
    

    while(size2 < size1)
    {
        p = buf + size2;

        temp_size = (size1 - size2 > nand_dev.page_mainsize - addr1) ? (nand_dev.page_mainsize - addr1) :(size1 - size2);
        //taskENTER_CRITICAL(); 
        NandFlash_PageReadFromAddr(nand_dev.lut[block1],page1,addr1,p,temp_size);
        //taskEXIT_CRITICAL();
        size2 += temp_size;
        page1++;
        if(page1 >= nand_dev.block_pagenum)
        {
            page1 = 0;
            block1++;
        }
        addr1 = 0;
    }
	
	return size2;
}

/*******************************************************************************
* Function     : NandFlash_BlockProgram
* Description  : ��nand flashд��������
* Input Para   : 
    -->block:flash�Ŀ��ַ
    -->page:flash��ҳ��ַ
    -->addr:flash��ҳƫ�Ƶ�ַ
    -->*buf:����ָ��
    -->size:���ݰ���С
* Output Para  : ��
* Return Value : 0���ɹ� 1������
*******************************************************************************/
static uint8_t NandFlash_BlockProgram(uint32_t block,uint32_t page,uint32_t addr,uint8_t * buf,uint32_t size)
{
    uint16_t page_size;
    uint32_t w_size,tmp_size,addr1,block_size,page1;
    uint16_t i;
    uint8_t* tmp_buf;
    uint8_t * p;
    uint8_t * block_buf;
  
    page_size = nand_dev.page_mainsize;
    block_size = nand_dev.block_pagenum*page_size; 
    w_size = 0;
    addr1 = addr;
    p = buf;
    page1 = page;
    if((addr+page*page_size+size)>block_size)   //���
    {
        return 1;
    }
    
    
    block_buf = my_malloc(block_size);//�黺��
    if(block_buf==NULL)
    {
        return 1;
    }
    memset(block_buf,0xff,block_size);
    tmp_buf = my_malloc(page_size);
    if(tmp_buf==NULL)
    {
        my_free(block_buf);
        return 1;
    }
    
    //block_num = (addr1)/(nand_dev.block_pagenum*page_size);//���ַ
    for(i=0;i<nand_dev.block_pagenum;i++)
    {
        NandFlash_Read_Source(NAND_LBNToPBN(block),i,0,&block_buf[i*page_size],page_size);    //������ǰ������
    }
    NandFlash_BlockErase(NAND_LBNToPBN(block),0);
    while(w_size<size)
    {
        //page_num = (addr1+w_size)/page_size;      
        //offset = (addr1+w_size)%page_size;       //��ҳ�ϵ�ƫ��
        //page1 = page_num+page;
        //p = buf + w_size;
        tmp_size = (size - w_size > page_size - addr1) ? (page_size - addr1) :(size - w_size);

        for(i=0;i<tmp_size;i++)
        {
            block_buf[page1*page_size+addr1+i] = p[w_size+i];
        }
        page1++;        
        w_size+=tmp_size;
        addr1 = 0;
    }
    //����д����������
    for(i=0;i<nand_dev.block_pagenum;i++)
    {
        //taskENTER_CRITICAL();
        NandFlash_PageProgram(NAND_LBNToPBN(block),i,&block_buf[i*page_size],0);//дһҳ����
        //taskEXIT_CRITICAL();
    }
    my_free(block_buf);
    my_free(tmp_buf);
    return 0;
}


/*******************************************************************************
* Function     : NandFlash_Program
* Description  : ��nand flashд����
* Input Para   : 
    -->addr:д���flash�ĵ�ַ
    -->*buf:����ָ��
    -->size:���ݰ���С
* Output Para  : ��
* Return Value : д���ֽ���
*******************************************************************************/
static uint32_t NandFlash_Program(uint32_t addr,uint8_t * buf,uint32_t size)
{
    uint16_t page_num,block_num;
    uint16_t page_size,offset,page_offset;
    uint32_t w_size,addr1,block_size,tmp_size,block_left;
    uint8_t * p;
  
    page_size = nand_dev.page_mainsize;
    block_size = nand_dev.block_pagenum*page_size;
    w_size = 0;
    addr1 = addr;
    p = buf;

    while(w_size<size)
    {
        p = buf+w_size;
        block_num = (addr1+w_size)/block_size;//���ַ
        page_num = (addr1+w_size)/page_size;      
        page_offset = page_num%(nand_dev.block_pagenum);//����ҳ��ַ
        offset = (addr1+w_size)%page_size;       //��ҳ�ϵ�ƫ��
        block_left = block_size - page_offset*page_size-offset;// 
        tmp_size = (size-w_size>block_left)?(block_left):(size-w_size);
        NandFlash_BlockProgram(block_num,page_offset,offset,p,tmp_size);
        w_size += tmp_size;
    }
    
    return w_size;
}

/*
* function: NAND_LBNToPBN,找到物理上对应的好快
* 逻辑是0-2047按着顺序，但是物理上可能第7块是坏块，所以逻辑对应的值是0-2046，实际物理逻辑块是1-2047，但已经踢除了第7块nand_dev.lut[6]的值为8
*/
u16 NAND_LBNToPBN(u32 LBNNum)
{
    u16 PBNNo=0;
    //���߼���Ŵ�����Ч������ʱ�򷵻�0XFFFF
    if(LBNNum>nand_dev.good_blocknum)return 0XFFFF;
    PBNNo=nand_dev.lut[LBNNum];
    return PBNNo;
}



//等待RB信号为某个电平,目前没有按超时后返回1值
//rb:0,等待RB==0
//   1,等待RB==1
//返回值:0,成功
//       1,超时
u8 NAND_WaitRB(vu8 rb , uint32_t timeout)
{
    vu32 time=0;
	while(1)
	{
		if(rb == 0)//只开启了下降沿中断
		{
			if((nandgetwaitstate() == 0) || NAND_RB==rb)
			{
				return 0;
			}
		}
		else
		{
			if(NAND_RB==rb)//读取高电平
			{
				return 0;
			}
		}
		time++;
		if(time > timeout)
		{
			wait = 1;
			return 0;

		}
	}
	return 0;
}



