#ifndef __RELAY_H__
#define __RELAY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "tim.h"

#define EX_CTR_1_Pin       LL_GPIO_PIN_6
#define EX_CTR_1_GPIO_Port GPIOA

#define EX_CTR_2_Pin       LL_GPIO_PIN_5
#define EX_CTR_2_GPIO_Port GPIOB

#define PWM_FREQ_MIN (1)

#define PWM_FREQ_MAX (100)

#define PWM_DUTY_MAX (1000)

extern __IO uint16_t duty[2];

extern __IO uint8_t freq;

void relay_gpio_init(void);

void relay_init(void);

void relay_deinit(void);

void relay_set_frequency(uint8_t freq);

void relay_status_update(void);

#ifdef __cplusplus
}
#endif

#endif /* __RELAY_H__ */
