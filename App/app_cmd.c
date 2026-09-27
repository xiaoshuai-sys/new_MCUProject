//// app_cmd.c
//#include "shell.h"
////#include "drv_gpio.h"
//#include <stdio.h>
//#include <string.h>
//#include <stdlib.h>


//#define LED_PORT    GPIOC
//#define LED_PIN     GPIO_PIN_6


//static int blink_count = 0;


///**
// * @brief led 命令：led <on|off|toggle>
// *
// * 用法示例：
// *   led on      点亮 LED
// *   led off     熄灭 LED
// *   led toggle  翻转 LED
// *
// * @return 0 成功，-1 参数错误
// */
//static int led_cmd(int argc, char *argv[])
//{
//    if (argc != 2)
//    {
//        printf("usage: led <on|off|toggle>\r\n");
//        return -1;
//    }

//    if (strcmp(argv[1], "on") == 0)
//    {
//        drv_gpio_set_pin(LED_PORT, LED_PIN);
//        printf("led on\r\n");
//    }
//    else if (strcmp(argv[1], "off") == 0)
//    {
//        drv_gpio_reset_pin(LED_PORT, LED_PIN);
//        printf("led off\r\n");
//    }
//    else if (strcmp(argv[1], "toggle") == 0)
//    {
//        drv_gpio_toggle_pin(LED_PORT, LED_PIN);
//        printf("led toggle\r\n");
//    }
//    else
//    {
//        printf("unknown action: %s\r\n", argv[1]);
//        return -1;
//    }
//    return 0;
//}

///* 导出 led 命令（CMD_MAIN 类型，匹配 led_cmd 的 argc/argv 签名） */
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), led_switch , led_cmd, control led on/off/toggle);



