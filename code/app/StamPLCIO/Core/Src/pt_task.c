#include "pt_task.h"
#include "i2c.h"
#include "gpio.h"
#include "iwdg.h"
#include "ina226.h"
#include "power.h"
#include "sys_utils.h"
#include "relay.h"

static INA226_config ina_temp;

struct pt main_pt;
struct pt stm_feed_wdt_handle_pt;
struct pt ina226_update_handle_pt;
struct pt relay_mode_update_handle_pt;
struct pt i2c_addr_io_handle_pt;
struct pt io_control_update_handle_pt;
struct pt jump_to_bootloader_setter_pt;
struct pt i2c_addr_change_handle_pt;
struct pt i2c_slave_timeout_handle_pt;
struct pt ina226_config_update_handle_pt;

void init_pt_task(void)
{
    PT_INIT(&main_pt);
    PT_INIT(&stm_feed_wdt_handle_pt);
    PT_INIT(&ina226_update_handle_pt);
    PT_INIT(&relay_mode_update_handle_pt);
    PT_INIT(&i2c_addr_io_handle_pt);
    PT_INIT(&io_control_update_handle_pt);
    PT_INIT(&jump_to_bootloader_setter_pt);
    PT_INIT(&i2c_addr_change_handle_pt);
    PT_INIT(&i2c_slave_timeout_handle_pt);
    PT_INIT(&ina226_config_update_handle_pt);
}

static PT_THREAD(ina226_update_handle(struct pt *pt))
{
    PT_BEGIN(pt);

    while (1) {
        if (ina226_ch1.Config.mInitialized) {
            if (INA226_CheckI2cAddress(&ina226_ch1.Config, INA226_CH1_I2C_ADDR) == OK) {
                peri_error.flags.ina226_ch1_init = 0;
                INA226_MeasureAll(&ina226_ch1);
            } else {
                peri_error.flags.ina226_ch1_init = 1;
            }
        }
        if (ina226_ch2.Config.mInitialized) {
            if (INA226_CheckI2cAddress(&ina226_ch2.Config, INA226_CH2_I2C_ADDR) == OK) {
                peri_error.flags.ina226_ch2_init = 0;
                INA226_MeasureAll(&ina226_ch2);
            } else {
                peri_error.flags.ina226_ch2_init = 1;
            }
        }
        PT_YIELD(pt);
    }

    PT_END(pt);
}

static PT_THREAD(ina226_config_update_handle(struct pt *pt))
{
    PT_BEGIN(pt);

    while (1) {
        PT_WAIT_UNTIL(pt, (g_events.ina226_update_0 == 1) || (g_events.ina226_update_1 == 1));
        g_events.ina226_update_0 = 0;
        g_events.ina226_update_1 = 0;

        while (config_queue_has_data()) {
            dequeue_config_request(&ina_temp);
            INA226_WriteRegister(&ina_temp, 0x00, ina_temp.mConfigRegister);
        }

        INA226_Debug_GetConfigRegister(&ina226_ch1.Config, &ina226_ch1.Config.mConfigRegister);
        INA226_Debug_GetConfigRegister(&ina226_ch2.Config, &ina226_ch2.Config.mConfigRegister);
    }

    PT_END(pt);
}

static PT_THREAD(i2c_addr_change_handle(struct pt *pt))
{
    PT_BEGIN(pt);

    while (1) {
        PT_WAIT_UNTIL(pt, g_events.change_i2c == 1);
        g_events.change_i2c = 0;

        if (i2c_addr_io_data.i2c_addr_7bit.flags.addr_reflash) {
            i2c2_reset();
            i2c_addr_io_data.i2c_addr_7bit.flags.addr_reflash = 0;
        }
    }

    PT_END(pt);
}

static PT_THREAD(relay_mode_update_handle(struct pt *pt))
{
    PT_BEGIN(pt);

    while (1) {
        PT_WAIT_UNTIL(pt, g_events.relay_mode_update == 1 || g_events.relay_freq_update == 1);

        if (g_events.relay_mode_update) {
            if (io_control.io_ctrl.flags.relay_mode) {
                relay_init();
                relay_status_update();
            } else {
                relay_deinit();
                relay_gpio_init();
            }
        }

        if (g_events.relay_freq_update && io_control.io_ctrl.flags.relay_mode) {
            relay_status_update();
        }

        g_events.relay_mode_update = 0;
        g_events.relay_freq_update = 0;
    }

    PT_END(pt);
}

static PT_THREAD(io_control_update_handle(struct pt *pt))
{
    PT_BEGIN(pt);

    while (1) {
        PT_WAIT_UNTIL(pt, g_events.io_control_update == 1);
        g_events.io_control_update = 0;

        if (io_control.io_ctrl.flags.relay_mode) {
            relay_status_update();
        } else {
            if (io_control.io_ctrl.flags.ex_ctr_1) {
                LL_GPIO_SetOutputPin(EX_CTR_1_GPIO_Port, EX_CTR_1_Pin);
            } else {
                LL_GPIO_ResetOutputPin(EX_CTR_1_GPIO_Port, EX_CTR_1_Pin);
            }

            if (io_control.io_ctrl.flags.ex_ctr_2) {
                LL_GPIO_SetOutputPin(EX_CTR_2_GPIO_Port, EX_CTR_2_Pin);
            } else {
                LL_GPIO_ResetOutputPin(EX_CTR_2_GPIO_Port, EX_CTR_2_Pin);
            }
        }

        if (io_control.io_ctrl.flags.ch1_pu_en) {
            LL_GPIO_SetOutputPin(CH1_PU_EN_GPIO_Port, CH1_PU_EN_Pin);
        } else {
            LL_GPIO_ResetOutputPin(CH1_PU_EN_GPIO_Port, CH1_PU_EN_Pin);
        }

        if (io_control.io_ctrl.flags.ch2_pu_en) {
            LL_GPIO_SetOutputPin(CH2_PU_EN_GPIO_Port, CH2_PU_EN_Pin);
        } else {
            LL_GPIO_ResetOutputPin(CH2_PU_EN_GPIO_Port, CH2_PU_EN_Pin);
        }

        if (io_control.io_ctrl.flags.relay_trig_0) {
            LL_GPIO_SetOutputPin(RELAY_TRIG_0_GPIO_Port, RELAY_TRIG_0_Pin);
        } else {
            LL_GPIO_ResetOutputPin(RELAY_TRIG_0_GPIO_Port, RELAY_TRIG_0_Pin);
        }
    }

    PT_END(pt);
}

static PT_THREAD(jump_to_bootloader_setter(struct pt *pt))
{
    PT_BEGIN(pt);
    while (1) {
        PT_WAIT_UNTIL(pt, g_events.jump_to_bootloader == 1);  // 等待设置触发信号
        g_events.jump_to_bootloader = 0;                      // 清除标志

        Jump_Bootloader();

        PT_YIELD(pt);  // 主动让出CPU
    }
    PT_END(pt);
}

static PT_THREAD(i2c_addr_io_handle(struct pt *pt))
{
    PT_BEGIN(pt);

    while (1) {
        PT_TIMER_DELAY(pt, PT_MILLIS_SECOND(1));

        if (!i2c_addr_io_data.i2c_addr_7bit.flags.addr_reflash) {
            i2c_addr_io_data.i2c_addr.value           = get_i2c_addr_gpio_in();
            i2c_addr_io_data.i2c_addr_7bit.flags.addr = i2c_addr_base + i2c_addr_io_data.i2c_addr.value;
        }
    }

    PT_END(pt);
}

static PT_THREAD(stm_feed_wdt_handle(struct pt *pt))
{
    PT_BEGIN(pt);

    while (1) {
        PT_TIMER_DELAY(pt, PT_MILLIS_SECOND(10));
        LL_IWDG_ReloadCounter(IWDG);
    }

    PT_END(pt);
}

static PT_THREAD(i2c_slave_timeout_handle(struct pt *pt))
{
    static uint32_t hal_get_tick_temp = 0;

    PT_BEGIN(pt);
    while (1) {
        if (i2c_slave_state != I2C_SLAVE_STATE_IDLE) {
            hal_get_tick_temp = HAL_GetTick();
            if ((hal_get_tick_temp > i2c_ex_timeout_start) &&
                (hal_get_tick_temp - i2c_ex_timeout_start > I2C_TIMEOUT_MS)) {
                i2c2_reset();
                i2c_slave_state = I2C_SLAVE_STATE_IDLE;
            }
        }
        PT_YIELD(pt);  // 主动让出CPU
    }
    PT_END(pt);
}

void main_scheduler(struct pt *pt)
{
    PT_BEGIN(pt);
    while (1) {
        // 按优先级顺序调度
        PT_SCHEDULE(stm_feed_wdt_handle(&stm_feed_wdt_handle_pt));
        PT_SCHEDULE(relay_mode_update_handle(&relay_mode_update_handle_pt));
        PT_SCHEDULE(io_control_update_handle(&io_control_update_handle_pt));
        PT_SCHEDULE(ina226_config_update_handle(&ina226_config_update_handle_pt));
        PT_SCHEDULE(ina226_update_handle(&ina226_update_handle_pt));
        PT_SCHEDULE(i2c_addr_io_handle(&i2c_addr_io_handle_pt));
        PT_SCHEDULE(i2c_addr_change_handle(&i2c_addr_change_handle_pt));
        PT_SCHEDULE(i2c_slave_timeout_handle(&i2c_slave_timeout_handle_pt));
        PT_SCHEDULE(jump_to_bootloader_setter(&jump_to_bootloader_setter_pt));
    }
    PT_END(pt);
}
