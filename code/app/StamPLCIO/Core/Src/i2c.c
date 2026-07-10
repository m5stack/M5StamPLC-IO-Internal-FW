/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    i2c.c
  * @brief   This file provides code for the configuration
  *          of the I2C instances.
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
/* Includes ------------------------------------------------------------------*/
#include "i2c.h"

/* USER CODE BEGIN 0 */
#include "i2c_ex.h"
#include "gpio.h"
/* USER CODE END 0 */

I2C_HandleTypeDef hi2c1;

/* I2C1 init function */
void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x0010061A;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}
/* I2C2 init function */
void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  LL_I2C_InitTypeDef I2C_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
  /**I2C2 GPIO Configuration
  PA11 [PA9]   ------> I2C2_SCL
  PA12 [PA10]   ------> I2C2_SDA
  */
  GPIO_InitStruct.Pin = ISO_SCL_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_6;
  LL_GPIO_Init(ISO_SCL_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = ISO_SDA_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_6;
  LL_GPIO_Init(ISO_SDA_GPIO_Port, &GPIO_InitStruct);

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C2);

  /* I2C2 interrupt Init */
  NVIC_SetPriority(I2C2_IRQn, 0);
  NVIC_EnableIRQ(I2C2_IRQn);

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */

  /** I2C Initialization
  */
  I2C_InitStruct.PeripheralMode = LL_I2C_MODE_I2C;
  I2C_InitStruct.Timing = 0x0010061A;
  I2C_InitStruct.AnalogFilter = LL_I2C_ANALOGFILTER_ENABLE;
  I2C_InitStruct.DigitalFilter = 0;
  I2C_InitStruct.OwnAddress1 = 0x20<<1;
  I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK;
  I2C_InitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
  LL_I2C_Init(I2C2, &I2C_InitStruct);
  LL_I2C_EnableAutoEndMode(I2C2);
  LL_I2C_SetOwnAddress2(I2C2, 0, LL_I2C_OWNADDRESS2_NOMASK);
  LL_I2C_DisableOwnAddress2(I2C2);
  LL_I2C_DisableGeneralCall(I2C2);
  LL_I2C_EnableClockStretching(I2C2);
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(i2cHandle->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspInit 0 */

  /* USER CODE END I2C1_MspInit 0 */

  /** Initializes the peripherals clocks
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
    PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**I2C1 GPIO Configuration
    PB8     ------> I2C1_SCL
    PB9     ------> I2C1_SDA
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF6_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* I2C1 clock enable */
    __HAL_RCC_I2C1_CLK_ENABLE();
  /* USER CODE BEGIN I2C1_MspInit 1 */

  /* USER CODE END I2C1_MspInit 1 */
  }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* i2cHandle)
{

  if(i2cHandle->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspDeInit 0 */

  /* USER CODE END I2C1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C1_CLK_DISABLE();

    /**I2C1 GPIO Configuration
    PB8     ------> I2C1_SCL
    PB9     ------> I2C1_SDA
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_8);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_9);

  /* USER CODE BEGIN I2C1_MspDeInit 1 */

  /* USER CODE END I2C1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void user_i2c_init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  LL_I2C_InitTypeDef I2C_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
  /**I2C2 GPIO Configuration
  PA11 [PA9]   ------> I2C2_SCL
  PA12 [PA10]   ------> I2C2_SDA
  */
  GPIO_InitStruct.Pin = ISO_SCL_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_6;
  LL_GPIO_Init(ISO_SCL_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = ISO_SDA_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_6;
  LL_GPIO_Init(ISO_SDA_GPIO_Port, &GPIO_InitStruct);

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C2);

  /* I2C2 interrupt Init */
  NVIC_SetPriority(I2C2_IRQn, 0);
  NVIC_EnableIRQ(I2C2_IRQn);

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */

  /** I2C Initialization
  */
  I2C_InitStruct.PeripheralMode = LL_I2C_MODE_I2C;
  I2C_InitStruct.Timing = 0x0010061A;
  I2C_InitStruct.AnalogFilter = LL_I2C_ANALOGFILTER_ENABLE;
  I2C_InitStruct.DigitalFilter = 0;
  I2C_InitStruct.OwnAddress1 = ((i2c_addr_io_data.i2c_addr_7bit.flags.addr)<<1);
  I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK;
  I2C_InitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
  LL_I2C_Init(I2C2, &I2C_InitStruct);
  LL_I2C_EnableAutoEndMode(I2C2);
  LL_I2C_SetOwnAddress2(I2C2, 0, LL_I2C_OWNADDRESS2_NOMASK);
  LL_I2C_DisableOwnAddress2(I2C2);
  LL_I2C_DisableGeneralCall(I2C2);
  LL_I2C_EnableClockStretching(I2C2);
  /* USER CODE BEGIN I2C2_Init 2 */
  set_i2c_slave_address(i2c_addr_io_data.i2c_addr_7bit.flags.addr);
  /* USER CODE END I2C2_Init 2 */

}

void I2C1_Start(void)
{
  LL_I2C_Enable(I2C1);
}

static void I2C1_ClearCR(void)
{
  (I2C1->CR2 &= (uint32_t)~((uint32_t)(I2C_CR2_SADD | I2C_CR2_HEAD10R | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_RD_WRN)));
}

uint8_t I2C1_TransmitData(uint8_t device_id ,uint8_t *pdata, uint8_t size, uint16_t timeout)
{
  // busyフラグをチェック
  while(LL_I2C_IsActiveFlag_BUSY(I2C1) == SET) {
    if(LL_SYSTICK_IsActiveCounterFlag())
    {
        if (timeout-- == 0)
        {
            I2C1_ClearCR();
            return 1;
        }
    }       
  }

  LL_I2C_HandleTransfer(I2C1, device_id, LL_I2C_ADDRSLAVE_7BIT, size, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_WRITE);

  for(uint8_t i = 0; i < size; i++){
    while(LL_I2C_IsActiveFlag_TXE(I2C1)== RESET) {
        if(LL_SYSTICK_IsActiveCounterFlag())
        {
            if (timeout-- == 0)
            {
                I2C1_ClearCR();
                return 1;
            }
        }         
    }
    LL_I2C_TransmitData8(I2C1, *pdata++);
  }

  while(LL_I2C_IsActiveFlag_STOP(I2C1)==RESET) {
    if(LL_SYSTICK_IsActiveCounterFlag())
    {
        if (timeout-- == 0)
        {
            I2C1_ClearCR();
            return 1;
        }
    }       
  }
  LL_I2C_ClearFlag_STOP(I2C1);
  
  // 設定をリセットする
  I2C1_ClearCR();

  return 0;
}

uint8_t I2C1_TransmitData_RepeatedStart(uint8_t device_id ,uint8_t *pdata, uint8_t size, uint16_t timeout)
{
  // busyフラグをチェック
  while(LL_I2C_IsActiveFlag_BUSY(I2C1) == SET) {
    if(LL_SYSTICK_IsActiveCounterFlag())
    {
        if (timeout-- == 0)
        {
            I2C1_ClearCR();
            return 1;
        }
    }
  }

  LL_I2C_HandleTransfer(I2C1, device_id, LL_I2C_ADDRSLAVE_7BIT, size, LL_I2C_MODE_SOFTEND, LL_I2C_GENERATE_START_WRITE);

  for(uint8_t i = 0; i < size; i++){
    while(LL_I2C_IsActiveFlag_TXE(I2C1)== RESET) {
        if(LL_SYSTICK_IsActiveCounterFlag())
        {
            if (timeout-- == 0)
            {
                I2C1_ClearCR();
                return 1;
            }
        }        
    }
    LL_I2C_TransmitData8(I2C1, *pdata++);
  }
  while(LL_I2C_IsActiveFlag_TC(I2C1)==RESET) {
    if(LL_SYSTICK_IsActiveCounterFlag())
    {
        if (timeout-- == 0)
        {
            I2C1_ClearCR();
            return 1;
        }
    }
  }
  
  return 0;
}

uint8_t I2C1_ReceiveData(uint8_t device_id , uint8_t *pdata, uint8_t size, uint16_t timeout)
{
  // 初期設定 
  LL_I2C_HandleTransfer(I2C1, device_id, LL_I2C_ADDRSLAVE_7BIT, size, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_READ);

  for(uint8_t i = 0; i < size; i++){
    while(LL_I2C_IsActiveFlag_RXNE(I2C1) == RESET) {
        if(LL_SYSTICK_IsActiveCounterFlag())
        {
            if (timeout-- == 0)
            {
                I2C1_ClearCR();
                return 1;
            }
        }        
    }
    *pdata++ = LL_I2C_ReceiveData8(I2C1);
  }
  
  while(LL_I2C_IsActiveFlag_STOP(I2C1)==RESET) {
    if(LL_SYSTICK_IsActiveCounterFlag())
    {
        if (timeout-- == 0)
        {
            I2C1_ClearCR();
            return 1;
        }
    }      
  }
  LL_I2C_ClearFlag_STOP(I2C1);
  
  // 設定をリセットする
  I2C1_ClearCR();

  return 0;
}
/* USER CODE END 1 */
