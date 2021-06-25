/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
* Copyright (C) 2012 - 2013 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/*******************************************************************************
* File Name : r_typedefs.h
* $Rev: 788 $
* $Date:: 2014-04-07 18:57:13 +0900#$
* Description : basic type definition
******************************************************************************/
#ifndef R_TYPEDEFS_H
#define R_TYPEDEFS_H

/******************************************************************************
Includes <System Includes> , "Project Includes"
******************************************************************************/
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "stm32h7xx_hal.h"

#if defined(__ARM_NEON__)

#include <arm_neon.h>

#else /* __ARM_NEON__ */

typedef float               float32_t;
typedef double              float64_t;

#endif /* __ARM_NEON__ */

/******************************************************************************
Typedef definitions
******************************************************************************/
typedef char                char_t;
typedef int                 bool_t;
typedef int                 int_t;
typedef long double         float128_t;
typedef signed long         long_t;
typedef unsigned long       ulong_t;

//定义一些常用的数据类型短关键字 
typedef unsigned int  s32;
typedef unsigned short int s16;
typedef unsigned char  s8;

typedef const unsigned int  sc32;  
typedef const unsigned short int sc16;  
typedef const unsigned char sc8;  

typedef volatile int  vs32;
typedef volatile short int  vs16;
typedef volatile char   vs8;

typedef volatile const int vsc32;  
typedef volatile const  short int  vsc16; 
typedef volatile const  char vsc8;   

typedef unsigned int  u32;
typedef unsigned short int  u16;
typedef unsigned char  u8;

typedef const unsigned int uc32;  
typedef const unsigned short int uc16;  
typedef const unsigned char uc8; 

typedef volatile unsigned int  vu32;
typedef volatile unsigned short int  vu16;
typedef volatile unsigned char  vu8;

typedef volatile const  unsigned int vuc32;  
typedef volatile const  unsigned short int  vuc16; 
typedef volatile const  unsigned char vuc8;  

#ifndef HAVE_BOOLEAN
#if defined FALSE || defined TRUE
/* Qt3 defines FALSE and TRUE as "const" variables in qglobal.h */
typedef int boolean;
#ifndef FALSE			/* in case these macros already exist */
#define FALSE	0		/* values of boolean */
#endif
#ifndef TRUE
#define TRUE	1
#endif
#else
typedef enum { FALSE = 0, TRUE = 1 } boolean;
#endif
#endif

#endif /* R_TYPEDEFS_H */

