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
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "stm32f4xx_hal.h"

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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Y_Pin GPIO_PIN_1
#define Y_GPIO_Port GPIOC
#define IN1_Pin GPIO_PIN_2
#define IN1_GPIO_Port GPIOC
#define IN2_Pin GPIO_PIN_3
#define IN2_GPIO_Port GPIOC
#define BATTERY_Pin GPIO_PIN_2
#define BATTERY_GPIO_Port GPIOA
#define PT100_Pin GPIO_PIN_3
#define PT100_GPIO_Port GPIOA
#define DI_SCK_Pin GPIO_PIN_5
#define DI_SCK_GPIO_Port GPIOA
#define DI_MISO_Pin GPIO_PIN_6
#define DI_MISO_GPIO_Port GPIOA
#define DI_MOSI_Pin GPIO_PIN_7
#define DI_MOSI_GPIO_Port GPIOA
#define DI_RES_Pin GPIO_PIN_4
#define DI_RES_GPIO_Port GPIOC
#define DI_DC_Pin GPIO_PIN_5
#define DI_DC_GPIO_Port GPIOC
#define DI_CS_Pin GPIO_PIN_0
#define DI_CS_GPIO_Port GPIOB
#define DI_LED_Pin GPIO_PIN_1
#define DI_LED_GPIO_Port GPIOB
#define VIB_Pin GPIO_PIN_10
#define VIB_GPIO_Port GPIOB
#define LED_Pin GPIO_PIN_12
#define LED_GPIO_Port GPIOB
#define V24ON1_Pin GPIO_PIN_13
#define V24ON1_GPIO_Port GPIOB
#define BUZZER_Pin GPIO_PIN_14
#define BUZZER_GPIO_Port GPIOB
#define V24ON2_Pin GPIO_PIN_15
#define V24ON2_GPIO_Port GPIOB
#define btn_up_Pin GPIO_PIN_8
#define btn_up_GPIO_Port GPIOD
#define btn_left_Pin GPIO_PIN_9
#define btn_left_GPIO_Port GPIOD
#define brn_enter_Pin GPIO_PIN_10
#define brn_enter_GPIO_Port GPIOD
#define btn_right_Pin GPIO_PIN_11
#define btn_right_GPIO_Port GPIOD
#define btn_down_Pin GPIO_PIN_12
#define btn_down_GPIO_Port GPIOD
#define TOUCH_Pin GPIO_PIN_10
#define TOUCH_GPIO_Port GPIOC
#define F_MISO_Pin GPIO_PIN_11
#define F_MISO_GPIO_Port GPIOC
#define F_MOSI_Pin GPIO_PIN_12
#define F_MOSI_GPIO_Port GPIOC
#define F_CS_Pin GPIO_PIN_7
#define F_CS_GPIO_Port GPIOD
#define F_SCK_Pin GPIO_PIN_3
#define F_SCK_GPIO_Port GPIOB
#define I_SCL_Pin GPIO_PIN_6
#define I_SCL_GPIO_Port GPIOB
#define I_SDA_Pin GPIO_PIN_7
#define I_SDA_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
