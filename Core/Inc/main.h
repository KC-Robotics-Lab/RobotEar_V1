/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define uart1_cli
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void uart_send_str(const char *s);
uint8_t CDC_Transmit_HS_Blocking(uint8_t *Buf, uint16_t Len, uint32_t timeout_ms);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define zero_deg_Pin GPIO_PIN_12
#define zero_deg_GPIO_Port GPIOF
#define LED_R_135_R_Pin GPIO_PIN_10
#define LED_R_135_R_GPIO_Port GPIOE
#define LED_180_R_Pin GPIO_PIN_12
#define LED_180_R_GPIO_Port GPIOE
#define LED_L_45_R_Pin GPIO_PIN_13
#define LED_L_45_R_GPIO_Port GPIOE
#define LED_L_135_R_Pin GPIO_PIN_14
#define LED_L_135_R_GPIO_Port GPIOE
#define LED_L_90_R_Pin GPIO_PIN_15
#define LED_L_90_R_GPIO_Port GPIOE
#define R90_Pin GPIO_PIN_10
#define R90_GPIO_Port GPIOD
#define LED_R_90_R_Pin GPIO_PIN_11
#define LED_R_90_R_GPIO_Port GPIOD
#define LED_R_45_R_Pin GPIO_PIN_12
#define LED_R_45_R_GPIO_Port GPIOD
#define LED_0_R_Pin GPIO_PIN_13
#define LED_0_R_GPIO_Port GPIOD
#define RST_MIC_L_Pin GPIO_PIN_2
#define RST_MIC_L_GPIO_Port GPIOG
#define RST_MIC_R_Pin GPIO_PIN_3
#define RST_MIC_R_GPIO_Port GPIOG
#define L90_Pin GPIO_PIN_4
#define L90_GPIO_Port GPIOG
#define L135_Pin GPIO_PIN_5
#define L135_GPIO_Port GPIOG
#define L45_Pin GPIO_PIN_6
#define L45_GPIO_Port GPIOG
#define Half_180_Pin GPIO_PIN_7
#define Half_180_GPIO_Port GPIOG
#define R135_Pin GPIO_PIN_8
#define R135_GPIO_Port GPIOG
#define R45_Pin GPIO_PIN_0
#define R45_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
