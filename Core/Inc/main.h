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
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "cmsis_os.h"
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
#ifdef __cplusplus
extern "C" {
#endif

void Application_main(void);

#ifdef __cplusplus
}
#endif
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define CONVEYOR_IN1_Pin GPIO_PIN_5
#define CONVEYOR_IN1_GPIO_Port GPIOA
#define CONVEYOR_IN2_Pin GPIO_PIN_6
#define CONVEYOR_IN2_GPIO_Port GPIOA
#define SENSOR1_Pin GPIO_PIN_8
#define SENSOR1_GPIO_Port GPIOA
#define SENSOR2_Pin GPIO_PIN_9
#define SENSOR2_GPIO_Port GPIOA
#define DEST0_Pin GPIO_PIN_10
#define DEST0_GPIO_Port GPIOA
#define DEST1_Pin GPIO_PIN_11
#define DEST1_GPIO_Port GPIOA
#define DEST2_Pin GPIO_PIN_12
#define DEST2_GPIO_Port GPIOA
#define SORTER_IN1_Pin GPIO_PIN_3
#define SORTER_IN1_GPIO_Port GPIOB
#define SORTER_IN2_Pin GPIO_PIN_4
#define SORTER_IN2_GPIO_Port GPIOB
#define SORTER_IN3_Pin GPIO_PIN_5
#define SORTER_IN3_GPIO_Port GPIOB
#define SORTER_IN4_Pin GPIO_PIN_6
#define SORTER_IN4_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim2;
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
