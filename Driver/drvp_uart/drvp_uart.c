// drvp_uart.c
#include "drvp_uart.h"

/**
 * @brief 接口具体实例（映射到底层驱动 drv_uart）
 */
const drvp_uart_t drvp_uart_inst =
{
    .init               = drv_usart_init,
    .get_dev            = drv_usart_dev_get,

    .send_byte          = drv_usart_send_byte,
    .send_string        = drv_usart_send_string,
    .send_byte_timeout  = drv_usart_send_byte_timeout,
    .send_string_timeout= drv_usart_send_string_timeout,
    .send_dma           = drv_usart_send_dma,
    .tx_busy            = drv_usart_tx_busy,

    .rx_length          = drv_usart_rx_length,
    .rx_read            = drv_usart_rx_read_data,

    .process            = drv_usart_process,
};

/**
 * @brief 全局接口实例指针（供外部调用）
 */
drvp_uart_t *drvp_uart = (drvp_uart_t *)&drvp_uart_inst;
