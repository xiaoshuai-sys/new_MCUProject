// app_cmd.c
#include "shell.h"
#include "drv_gpio.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* 假设开发板 LED 接在 GPIOC 的 PIN6（GD32F450 官方板常见），按你的板子改 */
#define LED_PORT    GPIOC
#define LED_PIN     GPIO_PIN_6

/* LED 初始化：输出推挽 */
static void led_gpio_init(void)
{
    drv_gpio_clock_enable(LED_PORT);

    drv_gpio_init_t cfg;
    cfg.periph = LED_PORT;
    cfg.pin    = LED_PIN;
    cfg.mode   = GPIO_MODE_OUTPUT;
    cfg.pull   = GPIO_PUPD_NONE;
    cfg.otype  = GPIO_OTYPE_PP;
    cfg.speed  = GPIO_OSPEED_50MHZ;
    cfg.af     = GPIO_AF_0;
    drv_gpio_init(&cfg);
}

/**
 * @brief led 命令：led on / led off / led toggle
 */
static int led_cmd(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("usage: led <on|off|toggle>\r\n");
        return -1;
    }

    if (strcmp(argv[1], "on") == 0)
    {
        drv_gpio_set_pin(LED_PORT, LED_PIN);
        printf("led on\r\n");
    }
    else if (strcmp(argv[1], "off") == 0)
    {
        drv_gpio_reset_pin(LED_PORT, LED_PIN);
        printf("led off\r\n");
    }
    else if (strcmp(argv[1], "toggle") == 0)
    {
        drv_gpio_toggle_pin(LED_PORT, LED_PIN);
        printf("led toggle\r\n");
    }
    else
    {
        printf("unknown param: %s\r\n", argv[1]);
        return -1;
    }
    return 0;
}

/* 导出命令：串口输入 led on / led off / led toggle */
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
                 led, led_cmd, control led);
