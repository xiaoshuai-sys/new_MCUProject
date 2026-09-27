// drv_gpio.h
#ifndef __DRV_GPIO__H_
#define __DRV_GPIO__H_

#include "gd32f4xx.h"

/**
 * @brief GPIO 初始化配置结构体
 */
typedef struct
{
    uint32_t periph;   /**< GPIO 端口基地址，例如 GPIOA / GPIOC          */
    uint32_t pin;      /**< GPIO 引脚掩码，例如 GPIO_PIN_6 / GPIO_PIN_ALL */
    uint32_t mode;     /**< GPIO 模式：输入 / 输出 / 复用 / 模拟          */
    uint32_t pull;     /**< 上下拉：无 / 上拉 / 下拉                       */
    uint8_t  otype;    /**< 输出类型：推挽 / 开漏                          */
    uint32_t speed;    /**< 输出速度：2MHz / 25MHz / 50MHz / 最大          */
    uint32_t af;       /**< 复用功能编号，仅在 mode 为复用模式时有效       */
} drv_gpio_init_t;


/* output mode definitions */
#define CTL_CLTR(regval)           (BITS(0,1) & ((uint32_t)(regval) << 0))
#define GPIO_MODE_INPUT            CTL_CLTR(0)               /*!< input mode */
#define GPIO_MODE_OUTPUT           CTL_CLTR(1)               /*!< output mode */
#define GPIO_MODE_AF               CTL_CLTR(2)               /*!< alternate function mode */
#define GPIO_MODE_ANALOG           CTL_CLTR(3)               /*!< analog mode */

/* pull-up/ pull-down definitions */
#define PUD_PUPD(regval)           (BITS(0,1) & ((uint32_t)(regval) << 0))
#define GPIO_PUPD_NONE             PUD_PUPD(0)               /*!< floating mode, no pull-up and pull-down resistors */
#define GPIO_PUPD_PULLUP           PUD_PUPD(1)               /*!< with pull-up resistor */
#define GPIO_PUPD_PULLDOWN         PUD_PUPD(2)               /*!< with pull-down resistor */

/* GPIO pin definitions */
#define GPIO_PIN_0                 BIT(0)                    /*!< GPIO pin 0 */
#define GPIO_PIN_1                 BIT(1)                    /*!< GPIO pin 1 */
#define GPIO_PIN_2                 BIT(2)                    /*!< GPIO pin 2 */
#define GPIO_PIN_3                 BIT(3)                    /*!< GPIO pin 3 */
#define GPIO_PIN_4                 BIT(4)                    /*!< GPIO pin 4 */
#define GPIO_PIN_5                 BIT(5)                    /*!< GPIO pin 5 */
#define GPIO_PIN_6                 BIT(6)                    /*!< GPIO pin 6 */
#define GPIO_PIN_7                 BIT(7)                    /*!< GPIO pin 7 */
#define GPIO_PIN_8                 BIT(8)                    /*!< GPIO pin 8 */
#define GPIO_PIN_9                 BIT(9)                    /*!< GPIO pin 9 */
#define GPIO_PIN_10                BIT(10)                   /*!< GPIO pin 10 */
#define GPIO_PIN_11                BIT(11)                   /*!< GPIO pin 11 */
#define GPIO_PIN_12                BIT(12)                   /*!< GPIO pin 12 */
#define GPIO_PIN_13                BIT(13)                   /*!< GPIO pin 13 */
#define GPIO_PIN_14                BIT(14)                   /*!< GPIO pin 14 */
#define GPIO_PIN_15                BIT(15)                   /*!< GPIO pin 15 */
#define GPIO_PIN_ALL               BITS(0,15)                /*!< GPIO pin all */


/* GPIO output type */
#define GPIO_OTYPE_PP              ((uint8_t)(0x00U))        /*!< push pull mode */
#define GPIO_OTYPE_OD              ((uint8_t)(0x01U))        /*!< open drain mode */

/* GPIO output max speed level */
#define OSPD_OSPD(regval)          (BITS(0,1) & ((uint32_t)(regval) << 0))
#define GPIO_OSPEED_LEVEL0         OSPD_OSPD(0)              /*!< output max speed level 0 */
#define GPIO_OSPEED_LEVEL1         OSPD_OSPD(1)              /*!< output max speed level 1 */
#define GPIO_OSPEED_LEVEL2         OSPD_OSPD(2)              /*!< output max speed level 2 */
#define GPIO_OSPEED_LEVEL3         OSPD_OSPD(3)              /*!< output max speed level 3 */

#define GPIO_OSPEED_2MHZ           GPIO_OSPEED_LEVEL0        /*!< output max speed 2MHz */
#define GPIO_OSPEED_25MHZ          GPIO_OSPEED_LEVEL1        /*!< output max speed 25MHz */
#define GPIO_OSPEED_50MHZ          GPIO_OSPEED_LEVEL2        /*!< output max speed 50MHz */
#define GPIO_OSPEED_MAX            GPIO_OSPEED_LEVEL3        /*!< GPIO very high output speed, max speed more than 50MHz */


/* GPIO alternate function */
#define AF(regval)                 (BITS(0,3) & ((uint32_t)(regval) << 0)) 
#define GPIO_AF_0                   AF(0)                    /*!< alternate function 0 selected */
#define GPIO_AF_1                   AF(1)                    /*!< alternate function 1 selected */
#define GPIO_AF_2                   AF(2)                    /*!< alternate function 2 selected */
#define GPIO_AF_3                   AF(3)                    /*!< alternate function 3 selected */
#define GPIO_AF_4                   AF(4)                    /*!< alternate function 4 selected */
#define GPIO_AF_5                   AF(5)                    /*!< alternate function 5 selected */
#define GPIO_AF_6                   AF(6)                    /*!< alternate function 6 selected */
#define GPIO_AF_7                   AF(7)                    /*!< alternate function 7 selected */
#define GPIO_AF_8                   AF(8)                    /*!< alternate function 8 selected */
#define GPIO_AF_9                   AF(9)                    /*!< alternate function 9 selected */
#define GPIO_AF_10                  AF(10)                   /*!< alternate function 10 selected */
#define GPIO_AF_11                  AF(11)                   /*!< alternate function 11 selected */
#define GPIO_AF_12                  AF(12)                   /*!< alternate function 12 selected */
#define GPIO_AF_13                  AF(13)                   /*!< alternate function 13 selected */
#define GPIO_AF_14                  AF(14)                   /*!< alternate function 14 selected */
#define GPIO_AF_15                  AF(15)                   /*!< alternate function 15 selected */


/**
 * @brief 使能 GPIO 端口时钟
 *
 * @param periph GPIO 端口基地址
 */
void drv_gpio_clock_enable(uint32_t periph);

/**
 * @brief 初始化 GPIO 端口/引脚
 *
 * @param cfg GPIO 初始化配置结构体指针
 */
void drv_gpio_init(const drv_gpio_init_t *cfg);

/**
 * @brief 向引脚写入电平
 *
 * @param periph GPIO 端口基地址
 * @param pin 引脚掩码
 * @param value 电平值（SET/RESET）
 */
void drv_gpio_write_pin(uint32_t periph, uint32_t pin, FlagStatus value);

/**
 * @brief 将引脚置高
 *
 * @param periph GPIO 端口基地址
 * @param pin 引脚掩码
 */
void drv_gpio_set_pin(uint32_t periph, uint32_t pin);

/**
 * @brief 将引脚置低
 *
 * @param periph GPIO 端口基地址
 * @param pin 引脚掩码
 */
void drv_gpio_reset_pin(uint32_t periph, uint32_t pin);

/**
 * @brief 翻转引脚电平
 *
 * @param periph GPIO 端口基地址
 * @param pin 引脚掩码
 */
void drv_gpio_toggle_pin(uint32_t periph, uint32_t pin);

/**
 * @brief 读取引脚输入电平
 *
 * @param periph GPIO 端口基地址
 * @param pin 引脚掩码
 * @return 引脚输入电平（SET/RESET）
 */
FlagStatus drv_gpio_read_pin(uint32_t periph, uint32_t pin);

#endif /* __DRV_GPIO__H_ */
