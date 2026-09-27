// drv_gpio.c
#include "drv_gpio.h"

/**
 * @brief 使能 GPIO 端口时钟
 *
 * @param periph GPIO 端口基地址
 */
void drv_gpio_clock_enable(uint32_t periph)
{
    switch (periph)
    {
    case GPIOA: rcu_periph_clock_enable(RCU_GPIOA); break;
    case GPIOB: rcu_periph_clock_enable(RCU_GPIOB); break;
    case GPIOC: rcu_periph_clock_enable(RCU_GPIOC); break;
    case GPIOD: rcu_periph_clock_enable(RCU_GPIOD); break;
    case GPIOE: rcu_periph_clock_enable(RCU_GPIOE); break;
    case GPIOF: rcu_periph_clock_enable(RCU_GPIOF); break;
    case GPIOG: rcu_periph_clock_enable(RCU_GPIOG); break;
    case GPIOH: rcu_periph_clock_enable(RCU_GPIOH); break;
    case GPIOI: rcu_periph_clock_enable(RCU_GPIOI); break;
    default: break;
    }
}

/**
 * @brief 初始化 GPIO 端口/引脚
 *
 * @param cfg GPIO 初始化配置结构体指针
 */
void drv_gpio_init(const drv_gpio_init_t *cfg)
{
    if (cfg == NULL)
    {
        return;
    }

    // 使能端口时钟（未使能时钟时后续寄存器配置不会生效）
    drv_gpio_clock_enable(cfg->periph);

    // 配置模式与上下拉
    gpio_mode_set(cfg->periph, cfg->mode, cfg->pull, cfg->pin);

    // 配置输出类型与速度（对输入/模拟引脚无影响）
    gpio_output_options_set(cfg->periph, cfg->otype, cfg->speed, cfg->pin);

    // 复用功能选择
    if (cfg->mode == GPIO_MODE_AF)
    {
        gpio_af_set(cfg->periph, cfg->af, cfg->pin);
    }
}

/**
 * @brief 向引脚写入电平
 *
 * @param periph GPIO 端口基地址
 * @param pin 引脚掩码
 * @param value 电平值（SET/RESET）
 */
void drv_gpio_write_pin(uint32_t periph, uint32_t pin, FlagStatus value)
{
    if (value == SET)
    {
        drv_gpio_set_pin(periph, pin);
    }
    else
    {
        drv_gpio_reset_pin(periph, pin);
    }
}


/**
 * @brief 将引脚置高
 *
 * @param periph GPIO 端口基地址
 * @param pin 引脚掩码
 */
void drv_gpio_set_pin(uint32_t periph, uint32_t pin)
{
    gpio_bit_set(periph, pin);
}

/**
 * @brief 将引脚置低
 *
 * @param periph GPIO 端口基地址
 * @param pin 引脚掩码
 */
void drv_gpio_reset_pin(uint32_t periph, uint32_t pin)
{
    gpio_bit_reset(periph, pin);
}

/**
 * @brief 翻转引脚电平
 *
 * @param periph GPIO 端口基地址
 * @param pin 引脚掩码
 */
void drv_gpio_toggle_pin(uint32_t periph, uint32_t pin)
{
    gpio_bit_toggle(periph, pin);
}

/**
 * @brief 读取引脚输入电平
 *
 * @param periph GPIO 端口基地址
 * @param pin 引脚掩码
 * @return 引脚输入电平（SET/RESET）
 */
FlagStatus drv_gpio_read_pin(uint32_t periph, uint32_t pin)
{
    return gpio_input_bit_get(periph, pin);
}
