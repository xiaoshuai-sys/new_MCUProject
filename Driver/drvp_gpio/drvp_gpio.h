// drvp_gpio.h
#ifndef __DRVP_GPIO__H_
#define __DRVP_GPIO__H_

#include "drv_gpio.h"

/**
 * @brief GPIO 对外接口结构体（函数指针表）
 */
typedef struct
{
    void       (*init)(const drv_gpio_init_t *cfg);                      /**< 初始化引脚          */
    void       (*clock_enable)(uint32_t periph);                         /**< 使能端口时钟        */
    void       (*write_pin)(uint32_t periph, uint32_t pin, FlagStatus value); /**< 写入电平       */
    void       (*set_pin)(uint32_t periph, uint32_t pin);                /**< 置高                */
    void       (*reset_pin)(uint32_t periph, uint32_t pin);              /**< 置低                */
    void       (*toggle_pin)(uint32_t periph, uint32_t pin);             /**< 翻转                */
    FlagStatus (*read_pin)(uint32_t periph, uint32_t pin);               /**< 读取输入电平        */
} drvp_gpio_t;

/**
 * @brief 全局接口实例指针（供外部调用）
 */
extern drvp_gpio_t *drvp_gpio;

#endif /* __DRVP_GPIO__H_ */
