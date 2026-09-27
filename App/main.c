// main.c
#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "hal_gpio.h"
#include "hal_port.h"


/* ==================== 半主机重定向（让 printf 走串口） ====================
 * ARMCC 去掉 semihosting、并把 printf 重定向到 fputc 的标准写法。
 * 注意：现在还没接 UART，printf 暂无实际输出目标；
 *       等 hal_uart 写好、在 fputc 里用 hal_uart 发送后，printf 才能用。
 */
#pragma import(__use_no_semihosting_swi)
#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;
};

FILE __stdout;

void _sys_exit(int x)
{
    (void)x;
}

void _ttywrch(int ch)
{
    (void)ch;
}

int _sys_open(const char *name, int openmode)
{
    (void)name;
    (void)openmode;
    return -1;
}


static void delay_ms(uint32_t ms)
{
    delay_1ms(ms);
}


/* ==================== 主函数 ==================== */
int main(void)
{
    /* 1. 系统滴答定时器：一切延时的基准 */
    systick_config();
    hal_port_init();

    /* 3. 把 PC6（你板子上的 LED）配成推挽输出，初始熄灭 */
    hal_gpio_init(HAL_GPIO_(HAL_GPIO_PORTC, 6), HAL_GPIO_MODE_OUTPUT_PP);
    hal_gpio_write(HAL_GPIO_(HAL_GPIO_PORTC, 6), HAL_GPIO_LEVEL_LOW);

    /* 4. 主循环：每隔 500ms 翻转一次 LED，产生闪烁 */
    while (1)
    {
        hal_gpio_toggle(HAL_GPIO_(HAL_GPIO_PORTC, 6));
        delay_ms(500);
    }
}
