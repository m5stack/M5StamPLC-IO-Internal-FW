/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "i2c.h"
#include "iwdg.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "pt_task.h"
#include "ina226.h"
#include <string.h>
#include "sys_utils.h"
#include "relay.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
volatile uint8_t i2c_addr_base = 0x20;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
volatile PeriError_TypeDef peri_error = {0};
volatile EventFlags_t g_events = {0};
volatile uint8_t fm_version = FIRMWARE_VERSION;

static volatile INA226_config ina226_config_temp[2] = {0};

uint32_t millis(void) 
{
  return HAL_GetTick();
}

__STATIC_INLINE uint32_t GXT_SYSTICK_IsActiveCounterFlag(void)
{
  return ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == (SysTick_CTRL_COUNTFLAG_Msk));
}

static uint32_t getCurrentMicros(void)
{
  /* Ensure COUNTFLAG is reset by reading SysTick control and status register */
  GXT_SYSTICK_IsActiveCounterFlag();
  uint32_t m = HAL_GetTick();
  const uint32_t tms = SysTick->LOAD + 1;
  __IO uint32_t u = tms - SysTick->VAL;
  if (GXT_SYSTICK_IsActiveCounterFlag()) {
    m = HAL_GetTick();
    u = tms - SysTick->VAL;
  }
  return (m * 1000 + (u * 1000) / tms);
}

//获取系统时间，单位us
uint32_t micros(void)
{
  return getCurrentMicros();
}

void Slave_Complete_Callback(uint8_t *rx_data, uint16_t len)
{
  uint8_t i2c_handle_tx_buf[128] = {0};
  uint8_t rx_buf[128] = {0};
  uint8_t rx_mark[128] = {0}; 

  if (len == 1) {
    if (rx_data[0] <= 0x2F) {
      int16_t voltage_temp;
      int32_t current_temp;
      voltage_temp = ina226_ch1.Result.BusVoltage_uV / 1000;
      current_temp = ina226_ch1.Result.Current_uA;
      memcpy(&i2c_handle_tx_buf[0], (uint8_t *)&voltage_temp, 2);
      memcpy(&i2c_handle_tx_buf[2], (uint8_t *)&current_temp, 4);    
      voltage_temp = ina226_ch2.Result.BusVoltage_uV / 1000;
      current_temp = ina226_ch2.Result.Current_uA;
      memcpy(&i2c_handle_tx_buf[6], (uint8_t *)&voltage_temp, 2);
      memcpy(&i2c_handle_tx_buf[8], (uint8_t *)&current_temp, 4); 

      i2c_handle_tx_buf[0x10] = io_control.io_ctrl.value;
      i2c_handle_tx_buf[0x11] = i2c_addr_io_data.i2c_addr.value;

      memcpy(&i2c_handle_tx_buf[0x20], (uint8_t *)&ina226_ch1.Config.mConfigRegister, 2);
      memcpy(&i2c_handle_tx_buf[0x22], (uint8_t *)&ina226_ch2.Config.mConfigRegister, 2);
      i2c2_set_send_data((uint8_t *)&i2c_handle_tx_buf[rx_data[0]-0], 0x2F-rx_data[0]+1);
    }
    else if((rx_data[0] >= 0x30) && (rx_data[0] <= 0x34)){
      i2c_handle_tx_buf[0] = freq;
      i2c_handle_tx_buf[1] = duty[0] & 0xFF;
      i2c_handle_tx_buf[2] = (duty[0] >> 8) & 0xFF;
      i2c_handle_tx_buf[3] = duty[1] & 0xFF;
      i2c_handle_tx_buf[4] = (duty[1] >> 8) & 0xFF;
      i2c2_set_send_data((uint8_t *)&i2c_handle_tx_buf[rx_data[0]-0x30], 0x34-rx_data[0]+1);
    }
    else if ((rx_data[0] >= 0xFB) && (rx_data[0] <= 0xFF))
    {
      i2c_handle_tx_buf[0] = peri_error.value;
      i2c_handle_tx_buf[1] = (*(uint8_t*)BOOTLOADER_VER_ADDR);
      i2c_handle_tx_buf[2] = 0;
      i2c_handle_tx_buf[3] = fm_version;
      i2c_handle_tx_buf[4] = i2c_addr_io_data.i2c_addr_7bit.value;
      i2c2_set_send_data((uint8_t *)&i2c_handle_tx_buf[rx_data[0]-0xFB], 0xFF-rx_data[0]+1);
    }
  }
  else if (len > 1) {
    if (rx_data[0] <= 0x2F) {
      for(int i = 0; i < len - 1; i++) {
        uint8_t rx_index = rx_data[0]+i;
        
        rx_buf[rx_index] = rx_data[1+i];
        rx_mark[rx_index] = 1;
      }

      if (rx_mark[0x10] == 1) {
        uint8_t relay_mode = io_control.io_ctrl.flags.relay_mode;
        io_control.io_ctrl.value = rx_buf[0x10];
        g_events.io_control_update = 1;
        if (relay_mode != io_control.io_ctrl.flags.relay_mode) {
          g_events.relay_mode_update = 1;
        }
      }
      // 初始化临时配置
      ina226_config_temp[0] = ina226_ch1.Config;
      uint16_t temp_config = ina226_config_temp[0].mConfigRegister;

      // 更新低字节 (地址0x20)
      if (rx_mark[0x20] == 1) {
          // 保留高字节 + 更新低字节
          temp_config = (temp_config & 0xFF00) | rx_buf[0x20];
      }

      // 更新高字节 (地址0x21)
      if (rx_mark[0x21] == 1) {
          // 保留低字节 + 更新高字节
          temp_config = (temp_config & 0x00FF) | (rx_buf[0x21] << 8);
      }

      // 统一应用保护位（如果任一字节被更新）
      if (rx_mark[0x20] || rx_mark[0x21]) {
          const uint16_t PROTECT_MASK = 0xF807;
          const uint16_t WRITE_MASK = 0x07F8;
          
          // 保留原始保护位 + 更新可写位
          uint16_t original_config = ina226_ch1.Config.mConfigRegister;
          temp_config = (original_config & PROTECT_MASK) | 
                        (temp_config & WRITE_MASK);
          
          ina226_config_temp[0].mConfigRegister = temp_config;
          g_events.ina226_update_0 = 1;
          enqueue_config_request(ina226_config_temp[0]);
      }

      ina226_config_temp[1] = ina226_ch2.Config;
      uint16_t temp_config_1 = ina226_config_temp[1].mConfigRegister;

      if (rx_mark[0x22] == 1) {
          temp_config_1 = (temp_config_1 & 0xFF00) | rx_buf[0x22];
      }

      if (rx_mark[0x23] == 1) {
          temp_config_1 = (temp_config_1 & 0x00FF) | (rx_buf[0x23] << 8);
      }

      if (rx_mark[0x22] || rx_mark[0x23]) {
          const uint16_t PROTECT_MASK = 0xF807;
          const uint16_t WRITE_MASK = 0x07F8;
          
          uint16_t original_config = ina226_ch2.Config.mConfigRegister;
          temp_config_1 = (original_config & PROTECT_MASK) | 
                          (temp_config_1 & WRITE_MASK);
          
          ina226_config_temp[1].mConfigRegister = temp_config_1;
          g_events.ina226_update_1 = 1;
          enqueue_config_request(ina226_config_temp[1]);
      }
    }
    else if(rx_data[0] >= 0x30 && rx_data[0] <= 0x34){
        for(int i = 0; i < len - 1; i++) {
            uint8_t rx_index = rx_data[0]+i;
        
            rx_buf[rx_index] = rx_data[1+i];
            rx_mark[rx_index] = 1;
        }

        if(rx_mark[0x30] == 1){
          if(rx_buf[0x30] >= 1 && rx_buf[0x30] <= 100){
            freq = rx_buf[0x30];
            g_events.relay_freq_update = 1;
          }
        }

        if(rx_mark[0x31] == 1 || rx_mark[0x32] == 1){
            uint16_t data = (rx_buf[0x32] << 8) | rx_buf[0x31];
            if(data <= 1000){
                duty[0] = data;
                g_events.io_control_update = 1;
            }
        }

        if(rx_mark[0x33] == 1 || rx_mark[0x34] == 1){
            uint16_t data = (rx_buf[0x34] << 8) | rx_buf[0x33];
            if(data <= 1000){
                duty[1] = data;
                g_events.io_control_update = 1;
            }
        }

    }
    else if (rx_data[0] == 0xFF) {
      i2c_addr_io_data_t i2c_addr_io_data_temp = {0};
      i2c_addr_io_data_temp.i2c_addr_7bit.value = rx_data[1];
      if (!i2c_addr_io_data.i2c_addr_7bit.flags.addr_reflash) {
        if (i2c_addr_io_data_temp.i2c_addr_7bit.flags.addr_reflash && 
           (i2c_addr_io_data_temp.i2c_addr_7bit.flags.addr == i2c_addr_io_data.i2c_addr_7bit.flags.addr)) {
          g_events.change_i2c = 1;
          i2c_addr_io_data.i2c_addr_7bit.flags.addr_reflash = 1;
        }
      }
    }
    else if (rx_data[0] == 0xFD) {
      if (rx_data[1] == 1) {
        g_events.jump_to_bootloader = 1;
      }
    }
  }  
}
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  // MX_I2C2_Init();
  MX_IWDG_Init();
  // MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  relay_gpio_init();
  LL_IWDG_ReloadCounter(IWDG);
  peri_error.flags.ina226_ch1_init = INA226_Init(&ina226_ch1, NULL, INA226_CH1_I2C_ADDR, 3, 0.0273);
  peri_error.flags.ina226_ch2_init = INA226_Init(&ina226_ch2, NULL, INA226_CH2_I2C_ADDR, 3, 0.0273);
  LL_IWDG_ReloadCounter(IWDG);
  i2c_addr_io_data.i2c_addr.value = get_i2c_addr_gpio_in();
  i2c_addr_io_data.i2c_addr_7bit.value = i2c_addr_base + i2c_addr_io_data.i2c_addr.value;
  user_i2c_init();
  i2c2_it_enable();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    main_scheduler(&main_pt);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
    HAL_NVIC_SystemReset();
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
