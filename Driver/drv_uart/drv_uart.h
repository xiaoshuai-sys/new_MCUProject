// drv_uart.h
#ifndef __DRV_UART__H_
#define __DRV_UART__H_

#include "gd32f4xx.h"
#include "systick.h"
#include "circular_buffer.h"

/**
 * @brief 本驱动最多支持的 UART 实例数（USART0~USART5）
 */
#define DRV_UART_MAX_INSTANCES  6

/**
 * @brief UART 初始化配置结构体
 */
typedef struct
{
    uint32_t usart;
    uint32_t baudrate;
    uint32_t word_length;
    uint32_t stop_bits;
    uint32_t parity;
    IRQn_Type usart_irq;

    uint32_t tx_gpio;
    uint32_t tx_pin;
    uint32_t tx_af;

    uint32_t rx_gpio;
    uint32_t rx_pin;
    uint32_t rx_af;

    uint32_t tx_dma;
    dma_channel_enum tx_dma_ch;
    dma_subperipheral_enum tx_dma_sub;

    uint32_t rx_dma;
    dma_channel_enum rx_dma_ch;
    dma_subperipheral_enum rx_dma_sub;

} drv_usart_init_t;

/**
 * @brief RX 和 TX 缓冲区结构体
 */
typedef struct
{
    uint8_t *rx_dma_buf;
    uint8_t *tx_dma_buf;
    uint16_t rx_dma_buf_size;
    uint16_t tx_dma_buf_size;
} drv_buf_init_t;

/**
 * @brief 环形缓冲区结构体
 */
typedef struct
{
    uint8_t *cir_buf;
    uint16_t cir_buf_size;
    circular_buffer *cb;
} drv_cir_buf_init_t;

/**
 * @brief USART 运行时实例
 *
 * 保存一次 drv_usart_init() 的全部上下文，供中断服务函数按外设基地址
 * 在注册表中查找，并把 DMA 接收缓冲区里的新数据搬进环形缓冲区。
 */
typedef struct
{
    const drv_usart_init_t   *config;
    const drv_buf_init_t     *buf_config;
    const drv_cir_buf_init_t *cir_config;

    uint32_t rx_last_offset;   /**< 上次已搬移到的 DMA 偏移，避免重复搬移 */
    uint32_t rx_total_bytes;   /**< 累计接收字节数（含因缓冲满被丢弃的） */
    volatile uint8_t frame_ready; /**< IDLE 中断置 1，应用层读取后清 0  */
} drv_usart_dev_t;

/**
 * @brief 初始化 USART（DMA 收发 + HTF/FTF/IDLE 中断），并注册运行时实例
 */
void drv_usart_init(const drv_usart_init_t *config, const drv_buf_init_t *dma_buf_config, const drv_cir_buf_init_t *cir_buf_config);

/**
 * @brief 按外设基地址查找运行时实例
 *
 * @param usart USART 外设基地址
 * @return 实例指针，未找到返回 NULL
 */
drv_usart_dev_t *drv_usart_dev_get(uint32_t usart);

/*------------------------------- 发送 -------------------------------*/
void    drv_usart_send_byte(const drv_usart_init_t *config, uint8_t data);
void    drv_usart_send_string(const drv_usart_init_t *config, const char *str);
uint8_t drv_usart_send_byte_timeout(const drv_usart_init_t *config, uint8_t data, uint32_t timeout_ms);
uint8_t drv_usart_send_string_timeout(const drv_usart_init_t *config, const char *str, uint32_t timeout_ms);
uint8_t drv_usart_send_dma(const drv_usart_init_t *config, const drv_buf_init_t *buf_config, const uint8_t *data, uint16_t length);
uint8_t drv_usart_tx_busy(const drv_usart_init_t *config);

/*------------------------------- 接收 -------------------------------*/
size_t drv_usart_rx_length(const drv_cir_buf_init_t *cir_buf_config);
size_t drv_usart_rx_read_data(const drv_cir_buf_init_t *cir_buf_config, char *data, size_t length);

/**
 * @brief 把 DMA 接收缓冲区中尚未搬移的数据搬进环形缓冲区（中断/主循环都可调用）
 */
void drv_usart_drain_rx(drv_usart_dev_t *dev);

/**
 * @brief 接收数据处理（主循环调用，按实例）
 */
void drv_usart_process(drv_usart_dev_t *dev);

/*------------------------------- 中断入口 -------------------------------*/
/**
 * @brief USART 中断处理（IDLE：一帧结束）
 */
void drv_usart_irq_handler(uint32_t usart);

/**
 * @brief RX DMA 中断处理（HTF/FTF：及时把数据搬进环形缓冲区，防覆盖）
 */
void drv_usart_dma_rx_irq_handler(uint32_t dma, dma_channel_enum ch);

#endif /* __DRV_UART__H_ */
