#include "relay.h"

__IO uint16_t duty[2] = {0};

__IO uint8_t freq = 50;

void relay_gpio_init(void)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);

    LL_GPIO_ResetOutputPin(EX_CTR_1_GPIO_Port, EX_CTR_1_Pin);
    LL_GPIO_ResetOutputPin(EX_CTR_2_GPIO_Port, EX_CTR_2_Pin);

    GPIO_InitStruct.Pin        = EX_CTR_1_Pin;
    GPIO_InitStruct.Mode       = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull       = LL_GPIO_PULL_NO;
    LL_GPIO_Init(EX_CTR_1_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = EX_CTR_2_Pin;
    LL_GPIO_Init(EX_CTR_2_GPIO_Port, &GPIO_InitStruct);
}

void relay_init(void)
{
    MX_TIM3_Init();
    LL_TIM_EnableARRPreload(TIM3);
    LL_TIM_OC_EnablePreload(TIM3, LL_TIM_CHANNEL_CH1);
    LL_TIM_OC_EnablePreload(TIM3, LL_TIM_CHANNEL_CH2);
    LL_TIM_CC_EnableChannel(TIM3, LL_TIM_CHANNEL_CH1);
    LL_TIM_CC_EnableChannel(TIM3, LL_TIM_CHANNEL_CH2);
    LL_TIM_EnableCounter(TIM3);
}

void relay_deinit(void)
{
    LL_TIM_DisableCounter(TIM3);
    LL_TIM_CC_DisableChannel(TIM3, LL_TIM_CHANNEL_CH1);
    LL_TIM_CC_DisableChannel(TIM3, LL_TIM_CHANNEL_CH2);
    LL_TIM_DeInit(TIM3);
    LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_TIM3);
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_6, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_5, LL_GPIO_MODE_INPUT);
}

void relay_set_frequency(uint8_t freq)
{
    uint32_t psc = (16000000U / (freq * 1000U)) - 1;
    LL_TIM_SetPrescaler(TIM3, psc);
}

void relay_status_update(void)
{
    relay_set_frequency(freq);
    LL_TIM_OC_SetCompareCH1(TIM3, duty[0]);
    LL_TIM_OC_SetCompareCH2(TIM3, duty[1]);
}
