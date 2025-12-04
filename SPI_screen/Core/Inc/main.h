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

#include "stm32h7xx_ll_system.h"
#include "stm32h7xx_ll_gpio.h"
#include "stm32h7xx_ll_exti.h"
#include "stm32h7xx_ll_bus.h"
#include "stm32h7xx_ll_cortex.h"
#include "stm32h7xx_ll_rcc.h"
#include "stm32h7xx_ll_utils.h"
#include "stm32h7xx_ll_pwr.h"
#include "stm32h7xx_ll_dma.h"

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
#define LED_Pin LL_GPIO_PIN_2
#define LED_GPIO_Port GPIOE
#define TS_CS_Pin LL_GPIO_PIN_4
#define TS_CS_GPIO_Port GPIOE
#define OSC32_IN_Pin LL_GPIO_PIN_14
#define OSC32_IN_GPIO_Port GPIOC
#define OSC32_OUT_Pin LL_GPIO_PIN_15
#define OSC32_OUT_GPIO_Port GPIOC
#define PH0_MCU_Pin LL_GPIO_PIN_0
#define PH0_MCU_GPIO_Port GPIOH
#define PH1_MCU_Pin LL_GPIO_PIN_1
#define PH1_MCU_GPIO_Port GPIOH
#define YM_Pin LL_GPIO_PIN_1
#define YM_GPIO_Port GPIOC
#define XP_Pin LL_GPIO_PIN_0
#define XP_GPIO_Port GPIOA
#define YP_Pin LL_GPIO_PIN_1
#define YP_GPIO_Port GPIOA
#define XM_Pin LL_GPIO_PIN_2
#define XM_GPIO_Port GPIOA
#define BAT_V_SENSE_Pin LL_GPIO_PIN_3
#define BAT_V_SENSE_GPIO_Port GPIOA
#define AUDIO_OUT_Pin LL_GPIO_PIN_4
#define AUDIO_OUT_GPIO_Port GPIOA
#define LCD_RD_Pin LL_GPIO_PIN_5
#define LCD_RD_GPIO_Port GPIOA
#define LCD_D0_Pin LL_GPIO_PIN_6
#define LCD_D0_GPIO_Port GPIOA
#define LCD_WR_Pin LL_GPIO_PIN_7
#define LCD_WR_GPIO_Port GPIOA
#define LCD_CS_Pin LL_GPIO_PIN_4
#define LCD_CS_GPIO_Port GPIOC
#define LCD_RST_Pin LL_GPIO_PIN_5
#define LCD_RST_GPIO_Port GPIOC
#define LCD_RS_Pin LL_GPIO_PIN_0
#define LCD_RS_GPIO_Port GPIOB
#define LCD_BL_Pin LL_GPIO_PIN_1
#define LCD_BL_GPIO_Port GPIOB
#define LCD_D2_Pin LL_GPIO_PIN_12
#define LCD_D2_GPIO_Port GPIOB
#define LCD_D1_Pin LL_GPIO_PIN_13
#define LCD_D1_GPIO_Port GPIOB
#define LCD_D4_Pin LL_GPIO_PIN_14
#define LCD_D4_GPIO_Port GPIOB
#define LCD_D3_Pin LL_GPIO_PIN_15
#define LCD_D3_GPIO_Port GPIOB
#define LCD_D5_Pin LL_GPIO_PIN_8
#define LCD_D5_GPIO_Port GPIOD
#define LCD_D6_Pin LL_GPIO_PIN_8
#define LCD_D6_GPIO_Port GPIOA
#define LCD_D7_Pin LL_GPIO_PIN_9
#define LCD_D7_GPIO_Port GPIOA
#define ROTP1_Pin LL_GPIO_PIN_0
#define ROTP1_GPIO_Port GPIOD
#define ROTB1_Pin LL_GPIO_PIN_1
#define ROTB1_GPIO_Port GPIOD
#define ROTA1_Pin LL_GPIO_PIN_2
#define ROTA1_GPIO_Port GPIOD
#define ROTP2_Pin LL_GPIO_PIN_3
#define ROTP2_GPIO_Port GPIOD
#define ROTB2_Pin LL_GPIO_PIN_4
#define ROTB2_GPIO_Port GPIOD
#define ROTA2_Pin LL_GPIO_PIN_5
#define ROTA2_GPIO_Port GPIOD
#define SW1_Pin LL_GPIO_PIN_6
#define SW1_GPIO_Port GPIOD
#define SW2_Pin LL_GPIO_PIN_7
#define SW2_GPIO_Port GPIOD
#define SW3_Pin LL_GPIO_PIN_9
#define SW3_GPIO_Port GPIOG
#define SW3G10_Pin LL_GPIO_PIN_10
#define SW3G10_GPIO_Port GPIOG
#define SW5_Pin LL_GPIO_PIN_11
#define SW5_GPIO_Port GPIOG
#define SLIDE_SWITCH_Pin LL_GPIO_PIN_12
#define SLIDE_SWITCH_GPIO_Port GPIOG
#define BAT_DIAG_Pin LL_GPIO_PIN_13
#define BAT_DIAG_GPIO_Port GPIOG
#define POWER_GOOD_Pin LL_GPIO_PIN_14
#define POWER_GOOD_GPIO_Port GPIOG

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
