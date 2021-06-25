/**********************************************************************
* $Id$		sdram_k4s561632j.h			2011-06-02
*//**
* @file		sdram_k4s561632j.h
* @brief	Contains all macro definitions for SAMSUNG K4S561632J
*			(supported on LPC1788 IAR Olimex Start Kit Rev.B)
* @version	1.0
* @date		02. June. 2011
* @author	NXP MCU SW Application Team
* 
* Copyright(C) 2011, NXP Semiconductor
* All rights reserved.
*
***********************************************************************
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
* NXP Semiconductors assumes no responsibility or liability for the
* use of the software, conveys no license or title under any patent,
* copyright, or mask work right to the product. NXP Semiconductors
* reserves the right to make changes in the software without
* notification. NXP Semiconductors also make no representation or
* warranty that such application will be suitable for the specified
* use without further testing or modification.
**********************************************************************/

/* Peripheral group ----------------------------------------------------------- */
/** @defgroup  Sdram_K4S561632J	Sdram K4S561632J
 * @ingroup LPC177x_8xCMSIS_Board_Support
 * @{
 */

#ifndef __SDRAM_W98N_32M_16bit_H_
#define __SDRAM_W98N_32M_16bit_H_


#include "fmc.h"
#include "r_typedefs.h"



#define SDRAM_BASE_ADDR		Bank6_SDRAM_ADDR
#define SDRAM_SIZE	        0x02000000   /* 16M 128Mbit 1024*1024*16 byte */                         



u8 SDRAM_Send_Cmd(u8 bankx,u8 cmd,u8 refresh,u16 regval);
void FMC_SDRAM_WriteBuffer(u8 *pBuffer,u32 WriteAddr,u32 n);
void FMC_SDRAM_ReadBuffer(u8 *pBuffer,u32 ReadAddr,u32 n);
void SDRAM_Initialization_Sequence(SDRAM_HandleTypeDef *hsdram);

#endif

/**
 * @}
 */

/*****************************************************************************
**                            End Of File
******************************************************************************/
