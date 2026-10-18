/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "OLED.h"
#include "menu.h"
#include "Key.h"
#include "MenuFSM.h"
#include "countDown.h"
#include "dino.h"

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

void isEnter_StandByMode(void);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Pin_KEY3_Pin GPIO_PIN_0
#define Pin_KEY3_GPIO_Port GPIOA
#define Pin_KEY2_Pin GPIO_PIN_6
#define Pin_KEY2_GPIO_Port GPIOA
#define Pin_KEY1_Pin GPIO_PIN_1
#define Pin_KEY1_GPIO_Port GPIOB
#define Pin_SI2C_SCL_Pin GPIO_PIN_10
#define Pin_SI2C_SCL_GPIO_Port GPIOB
#define Pin_SI2C_SDA_Pin GPIO_PIN_11
#define Pin_SI2C_SDA_GPIO_Port GPIOB
#define BAT_ADC_EN_Pin GPIO_PIN_11
#define BAT_ADC_EN_GPIO_Port GPIOA
#define CTL_Pin GPIO_PIN_12
#define CTL_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
