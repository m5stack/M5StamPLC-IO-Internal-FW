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
#include "stm32g0xx_hal.h"

#include "stm32g0xx_ll_i2c.h"
#include "stm32g0xx_ll_iwdg.h"
#include "stm32g0xx_ll_tim.h"
#include "stm32g0xx_ll_system.h"
#include "stm32g0xx_ll_gpio.h"
#include "stm32g0xx_ll_exti.h"
#include "stm32g0xx_ll_bus.h"
#include "stm32g0xx_ll_cortex.h"
#include "stm32g0xx_ll_rcc.h"
#include "stm32g0xx_ll_utils.h"
#include "stm32g0xx_ll_pwr.h"
#include "stm32g0xx_ll_dma.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef union {
    struct {
        uint8_t ina226_ch1_init : 1;  // aw32001初始化错误
        uint8_t ina226_ch2_init : 1;  // aw32001设置错误
        uint8_t flash_err       : 1;  // flash设置错误
        uint8_t rsvd            : 5;  // 保留位
    } flags;
    uint8_t value;  // 整体访问接口
} __attribute__((packed)) PeriError_TypeDef;

typedef struct {
    volatile uint8_t io_control_update    : 1; 
    volatile uint8_t jump_to_bootloader   : 1;
    volatile uint8_t change_i2c           : 1;
    volatile uint8_t ina226_update_0      : 1;
    volatile uint8_t ina226_update_1      : 1;
    volatile uint8_t relay_mode_update    : 1; 
    volatile uint8_t relay_freq_update    : 1;  
    volatile uint8_t reserved             : 2;  // 预留位
} EventFlags_t;

extern volatile EventFlags_t g_events;
extern volatile PeriError_TypeDef peri_error;
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
uint32_t millis(void);
uint32_t micros(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define INA_I2C_SCL_Pin LL_GPIO_PIN_8
#define INA_I2C_SCL_GPIO_Port GPIOB
#define INA_I2C_SDA_Pin LL_GPIO_PIN_9
#define INA_I2C_SDA_GPIO_Port GPIOB
#define CH1_PU_EN_Pin LL_GPIO_PIN_0
#define CH1_PU_EN_GPIO_Port GPIOA
#define CH2_PU_EN_Pin LL_GPIO_PIN_1
#define CH2_PU_EN_GPIO_Port GPIOA
#define ADDR_1_Pin LL_GPIO_PIN_2
#define ADDR_1_GPIO_Port GPIOA
#define ADDR_0_Pin LL_GPIO_PIN_3
#define ADDR_0_GPIO_Port GPIOA
#define ADDR_3_Pin LL_GPIO_PIN_4
#define ADDR_3_GPIO_Port GPIOA
#define ADDR_2_Pin LL_GPIO_PIN_5
#define ADDR_2_GPIO_Port GPIOA
#define RELAY_TRIG_0_Pin LL_GPIO_PIN_7
#define RELAY_TRIG_0_GPIO_Port GPIOA
#define ISO_SCL_Pin LL_GPIO_PIN_11
#define ISO_SCL_GPIO_Port GPIOA
#define ISO_SDA_Pin LL_GPIO_PIN_12
#define ISO_SDA_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */
#define FIRMWARE_VERSION 0x01

#define INA226_CH1_I2C_ADDR 0x40
#define INA226_CH2_I2C_ADDR 0x45

#define APPLICATION_ADDRESS     ((uint32_t)0x08002000)
#define BOOTLOADER_ADDRESS     ((uint32_t)0x08000000) 
#define BOOTLOADER_VER_ADDR ((uint32_t)0x08002000 - 1)
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
