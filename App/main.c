// main.c
#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "hal_gpio.h"
#include "hal_port.h"




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
    systick_config();
    hal_port_init();

    hal_gpio_init(HAL_GPIO_(HAL_GPIO_PORTC, 6), HAL_GPIO_MODE_OUTPUT_PP);
    hal_gpio_write(HAL_GPIO_(HAL_GPIO_PORTC, 6), HAL_GPIO_LEVEL_LOW);


    while (1)
    {
        hal_gpio_toggle(HAL_GPIO_(HAL_GPIO_PORTC, 6));
        delay_ms(500);
    }
}
