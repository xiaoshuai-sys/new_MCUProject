#include "hal_gpio.h"

static const hal_gpio_ops_t *gpio_ops = 0;   /* 已注册的 GPIO 实现句柄 */

void hal_gpio_register_ops(const hal_gpio_ops_t *ops)
{
    gpio_ops = ops;
}

uint8_t hal_gpio_init(hal_gpio_t io, hal_gpio_mode_e mode)
{
    if(!gpio_ops || !gpio_ops->init)
    {
        return 0;
    }
    
    return gpio_ops->init(io,mode);
}    

uint8_t hal_gpio_write(hal_gpio_t io, hal_gpio_level_e level)
{
    if (!gpio_ops || !gpio_ops->write)
    {
        return 0;
    }
    return gpio_ops->write(io, level);
}


hal_gpio_level_e hal_gpio_read(hal_gpio_t io)
{
    if (!gpio_ops || !gpio_ops->read)
    {
        return HAL_GPIO_LEVEL_LOW;
    }
    return gpio_ops->read(io);
}

void hal_gpio_toggle(hal_gpio_t io)
{
    if (!gpio_ops || !gpio_ops->toggle)
    {
        return;
    }
    gpio_ops->toggle(io);
}

uint8_t hal_gpio_af_set(hal_gpio_t io, hal_gpio_af_e af)
{
    if (!gpio_ops || !gpio_ops->af_set)
    {
        return 0;
    }
    return gpio_ops->af_set(io, af);
}
