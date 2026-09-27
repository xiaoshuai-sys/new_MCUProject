// drvp_gpio.c
#include "drvp_gpio.h"

/**
 * @brief 接口具体实例（映射到底层驱动 drv_gpio）
 */
static drvp_gpio_t drvp_gpio_impl =
{
    .init         = drv_gpio_init,
    .clock_enable = drv_gpio_clock_enable,
    .write_pin    = drv_gpio_write_pin,
    .set_pin      = drv_gpio_set_pin,
    .reset_pin    = drv_gpio_reset_pin,
    .toggle_pin   = drv_gpio_toggle_pin,
    .read_pin     = drv_gpio_read_pin,
};

/**
 * @brief 全局接口实例指针（供外部调用）
 */
drvp_gpio_t *drvp_gpio = &drvp_gpio_impl;
