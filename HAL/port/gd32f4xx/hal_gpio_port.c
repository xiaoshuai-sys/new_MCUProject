// hal_gpio_port.c —— GD32F4xx 平台的 GPIO 具体实现
//
// 这是「唯一接触芯片寄存器/库」的文件。
// 换芯片时：只需重写本文件（+ 换底层库），hal_gpio.h / hal_gpio.c 和 App 层完全不用动。
//
// 关键点：
//   1. 底层函数全部用 static，名字不能和 hal_gpio.h 里的公共 API（hal_gpio_init 等）重名，
//      否则会和 hal_gpio.c 里的同名函数在链接时冲突（duplicate symbol）。
//   2. 通过 hal_gd32f4_gpio_register() 把一张函数指针表注册给 hal_gpio.c，
//      由 hal_gpio.c 统一分发，App 层始终只调 hal_gpio_xxx()。

#include "gd32f4xx.h"          /* 芯片相关头文件，只允许出现在 port 层 */
#include "hal_gpio.h"
#include "hal_port.h"
#include "hal_port_internal.h"


static const uint32_t s_port_base[] = 
{
     GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG, GPIOH,

};

static const rcu_periph_enum  s_port_rcu [] = 
{
    RCU_GPIOA, RCU_GPIOB, RCU_GPIOC, RCU_GPIOD,
    RCU_GPIOE, RCU_GPIOF, RCU_GPIOG, RCU_GPIOH,
};

static uint8_t gd32_gpio_init(hal_gpio_t io, hal_gpio_mode_e mode)
{
    uint8_t  p_idx = (uint8_t)(io / HAL_GPIO_PORT_STRIDE); /* 端口索引 -> 查端口基址/时钟表 */
    uint8_t  pin   = (uint8_t)(io % HAL_GPIO_PORT_STRIDE); /* 引脚号 0~15 */


    /* 越界保护：端口索引超出表范围就失败 */
    if (p_idx >= (sizeof(s_port_base) / sizeof(s_port_base[0])))
        return 0;


    /* 1. 使能该端口的总线时钟 */
    rcu_periph_clock_enable(s_port_rcu[p_idx]);

    /* 2. 按抽象模式翻译成 GD32 的模式 + 上下拉 */
    switch (mode)
    {
        case HAL_GPIO_MODE_INPUT:
            gpio_mode_set(s_port_base[p_idx], GPIO_MODE_INPUT, GPIO_PUPD_NONE, 1UL << pin);
            break;

        case HAL_GPIO_MODE_INPUT_PULLUP:
            gpio_mode_set(s_port_base[p_idx], GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, 1UL << pin);
            break;

        case HAL_GPIO_MODE_OUTPUT_PP:
            gpio_mode_set(s_port_base[p_idx], GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, 1UL << pin);
            gpio_output_options_set(s_port_base[p_idx], GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, 1UL << pin);
            break;

        case HAL_GPIO_MODE_OUTPUT_OD:
            gpio_mode_set(s_port_base[p_idx], GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, 1UL << pin);
            gpio_output_options_set(s_port_base[p_idx], GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, 1UL << pin);
            break;

        case HAL_GPIO_MODE_ANALOG:
            gpio_mode_set(s_port_base[p_idx], GPIO_MODE_ANALOG, GPIO_PUPD_NONE, 1UL << pin);
            break;

        case HAL_GPIO_MODE_AF:
            gpio_mode_set(s_port_base[p_idx], GPIO_MODE_AF, GPIO_PUPD_NONE, 1UL << pin);
            gpio_output_options_set(s_port_base[p_idx], GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, 1UL << pin);
            break;

        default:
            return 0;
    }

    return 1;
}
 
static uint8_t gd32_gpio_write(hal_gpio_t io, hal_gpio_level_e level)
{
    uint8_t  p_idx = (uint8_t)(io / HAL_GPIO_PORT_STRIDE); /* 端口索引 -> 查端口基址/时钟表 */
    uint8_t  pin   = (uint8_t)(io % HAL_GPIO_PORT_STRIDE); /* 引脚号 0~15 */


    /* 越界保护：参数错误就静默返回失败，卡死整块板子 */
    if (p_idx >= (sizeof(s_port_base) / sizeof(s_port_base[0])))
        return 0;

    if (level == HAL_GPIO_LEVEL_HIGH)
        gpio_bit_set  (s_port_base[p_idx], 1UL << pin);
    else
        gpio_bit_reset(s_port_base[p_idx], 1UL << pin);

    return 1;
}


static void gd32_gpio_toggle(hal_gpio_t io)
{
    uint8_t p_idx = (uint8_t)(io / HAL_GPIO_PORT_STRIDE);
    uint8_t pin   = (uint8_t)(io % HAL_GPIO_PORT_STRIDE);

    if (p_idx >= (sizeof(s_port_base) / sizeof(s_port_base[0])))
        return;

    gpio_bit_toggle(s_port_base[p_idx], 1UL << pin);
}

static hal_gpio_level_e gd32_gpio_read(hal_gpio_t io)
{
    uint8_t p_idx = (uint8_t)(io / HAL_GPIO_PORT_STRIDE);
    uint8_t pin   = (uint8_t)(io % HAL_GPIO_PORT_STRIDE);

    if (p_idx >= (sizeof(s_port_base) / sizeof(s_port_base[0])))
        return HAL_GPIO_LEVEL_LOW;

    if (gpio_input_bit_get(s_port_base[p_idx], 1UL << pin))
        return HAL_GPIO_LEVEL_HIGH;

    return HAL_GPIO_LEVEL_LOW;
}

static uint8_t gd32_gpio_af_set(hal_gpio_t io, hal_gpio_af_e af)
{
    uint8_t p_idx = (uint8_t)(io / HAL_GPIO_PORT_STRIDE);
    uint8_t pin   = (uint8_t)(io % HAL_GPIO_PORT_STRIDE);

    if (p_idx >= (sizeof(s_port_base) / sizeof(s_port_base[0])))
        return 0;

    /* 复用功能的标准流程：先设成 AF 模式，再选 AF 编号 */
    gpio_mode_set(s_port_base[p_idx], GPIO_MODE_AF, GPIO_PUPD_NONE, 1UL << pin);
    gpio_af_set(s_port_base[p_idx], (uint32_t)af, 1UL << pin);

    return 1;
}


/* ---------- 注册：把 4 个底层函数打包成一张 ops 表，交给 hal_gpio.c ---------- */

static const hal_gpio_ops_t s_gd32_gpio_ops =
{
    .init   = gd32_gpio_init,
    .write  = gd32_gpio_write,
    .read   = gd32_gpio_read,
    .toggle = gd32_gpio_toggle,
    .af_set = gd32_gpio_af_set,
};

void hal_gd32f4_gpio_register(void)
{
    hal_gpio_register_ops(&s_gd32_gpio_ops);
}
