/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LIS_IIC_Pin GPIO_PIN_8
#define LIS_IIC_GPIO_Port GPIOB
#define LED_G_Pin GPIO_PIN_14
#define LED_G_GPIO_Port GPIOG
#define BL_EN_Pin GPIO_PIN_7
#define BL_EN_GPIO_Port GPIOD
#define O_Pin GPIO_PIN_9
#define O_GPIO_Port GPIOB
#define LIS_IICB7_Pin GPIO_PIN_7
#define LIS_IICB7_GPIO_Port GPIOB
#define NAND_RB_Pin GPIO_PIN_6
#define NAND_RB_GPIO_Port GPIOD
#define NAND_RB_EXTI_IRQn EXTI9_5_IRQn
#define CTP_RST0_Pin GPIO_PIN_12
#define CTP_RST0_GPIO_Port GPIOA
#define USB_EN_Pin GPIO_PIN_11
#define USB_EN_GPIO_Port GPIOA
#define OD3_Pin GPIO_PIN_3
#define OD3_GPIO_Port GPIOD
#define OD2_Pin GPIO_PIN_2
#define OD2_GPIO_Port GPIOD
#define Encryption_Pin GPIO_PIN_9
#define Encryption_GPIO_Port GPIOC
#define EncryptionA8_Pin GPIO_PIN_8
#define EncryptionA8_GPIO_Port GPIOA
#define BL_PWM_Pin GPIO_PIN_7
#define BL_PWM_GPIO_Port GPIOC
#define TCP_Pin GPIO_PIN_4
#define TCP_GPIO_Port GPIOH
#define beep_Pin GPIO_PIN_6
#define beep_GPIO_Port GPIOC
#define TCPH5_Pin GPIO_PIN_5
#define TCPH5_GPIO_Port GPIOH
#define CT_Pin GPIO_PIN_3
#define CT_GPIO_Port GPIOG
#define CT_EXTI_IRQn EXTI3_IRQn
#define TFT_ENT_Pin GPIO_PIN_6
#define TFT_ENT_GPIO_Port GPIOA
#define TFT_DISP_Pin GPIO_PIN_3
#define TFT_DISP_GPIO_Port GPIOA
#define GS_INT1_Pin GPIO_PIN_0
#define GS_INT1_GPIO_Port GPIOB
#define GS_INT1_EXTI_IRQn EXTI0_IRQn
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
