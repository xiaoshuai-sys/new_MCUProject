// drvp_uart.h
#ifndef __DRVP_UART__H_
#define __DRVP_UART__H_

#include "drv_uart.h"

/**
 * @brief UART 对外接口结构体（函数指针表）
 */
typedef struct
{
    void    (*init)(const drv_usart_init_t *config, const drv_buf_init_t *dma_buf_config, const drv_cir_buf_init_t *cir_buf_config); /**< 初始化 USART */
    drv_usart_dev_t *(*get_dev)(uint32_t usart);                                   /**< 按基地址获取实例  */

    void    (*send_byte)(const drv_usart_init_t *config, uint8_t data);                 /**< 阻塞式发送一个字节 */
    void    (*send_string)(const drv_usart_init_t *config, const char *str);            /**< 阻塞式发送字符串   */
    uint8_t (*send_byte_timeout)(const drv_usart_init_t *config, uint8_t data, uint32_t timeout_ms);      /**< 带超时发送一个字节 */
    uint8_t (*send_string_timeout)(const drv_usart_init_t *config, const char *str, uint32_t timeout_ms); /**< 带超时发送字符串   */
    uint8_t (*send_dma)(const drv_usart_init_t *config, const drv_buf_init_t *buf_config, const uint8_t *data, uint16_t length); /**< DMA 发送 */
    uint8_t (*tx_busy)(const drv_usart_init_t *config);                                 /**< 查询 DMA 发送是否忙 */

    size_t  (*rx_length)(const drv_cir_buf_init_t *cir_buf_config);                     /**< 环形缓冲区可读字节数 */
    size_t  (*rx_read)(const drv_cir_buf_init_t *cir_buf_config, char *data, size_t length); /**< 从环形缓冲区读取数据 */

    void    (*process)(drv_usart_dev_t *dev);                                          /**< 接收数据处理（主循环调用） */
} drvp_uart_t;

/**
 * @brief 全局接口实例指针（供外部调用）
 */
extern drvp_uart_t *drvp_uart;

#endif /* __DRVP_UART__H_ */
