// hal_port.c —— 芯片平台初始化入口（GD32F4xx 版）
//
// 作用：把当前芯片支持的所有底层实现（gpio/uart/dma/i2c...）一次性注册好。
// 换芯片时：这个文件里调用的 register 函数换成新芯片对应的实现即可。

#include "hal_port.h"
#include "hal_port_internal.h"

void hal_port_init(void)
{
    hal_gd32f4_gpio_register();   /* GPIO（已实现） */

    /* 下面这些还没实现，实现后取消注释即可： */
    /* hal_gd32f4_uart_register(); */
    /* hal_gd32f4_dma_register();  */
    /* hal_gd32f4_i2c_register();  */
    /* hal_gd32f4_irqn_register(); */
}
