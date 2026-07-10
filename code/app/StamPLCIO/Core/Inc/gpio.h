/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.h
  * @brief   This file contains all the function prototypes for
  *          the gpio.c file
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
#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
typedef struct {
  union {
      struct {
          uint8_t ex_ctr_1       : 1;
          uint8_t ex_ctr_2       : 1;
          uint8_t ch1_pu_en      : 1;
          uint8_t ch2_pu_en      : 1;
          uint8_t relay_trig_0   : 1;
          uint8_t relay_mode     : 1;
          uint8_t rsvd           : 2;  // 保留位
      } flags;
      uint8_t value;  // 整体访问接口
  } io_ctrl;  
} io_control_t;

typedef struct {
  union {
      struct {
          uint8_t addr0       : 1;
          uint8_t addr1       : 1;
          uint8_t addr2       : 1;
          uint8_t addr3       : 1;
          uint8_t rsvd        : 4;  // 保留位
      } flags;
      uint8_t value;  // 整体访问接口
  } i2c_addr;  
  union {
      struct {
          uint8_t addr                : 7;
          uint8_t addr_reflash        : 1; 
      } flags;
      uint8_t value;  // 整体访问接口
  } i2c_addr_7bit;  
} i2c_addr_io_data_t;

extern volatile io_control_t io_control;
extern volatile i2c_addr_io_data_t i2c_addr_io_data;
/* USER CODE END Private defines */

void MX_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */
uint8_t get_i2c_addr_gpio_in(void);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

