// drv_dma.h
#ifndef __DRV_DMA__H_
#define __DRV_DMA__H_

#include "gd32f4xx.h"

/**
 * @brief DMA 初始化配置结构体（单数据模式）
 */
typedef struct
{
    uint32_t              rx_dma;          /**< DMA 控制器，DMA0 / DMA1             */
    dma_channel_enum      rx_channel;      /**< DMA 通道，DMA_CH0 ~ DMA_CH7         */
    dma_subperipheral_enum rx_subperiph;   /**< 外设选择，DMA_SUBPERI0 ~ DMA_SUBPERI7 */
    uint32_t              rx_periph_addr;  /**< 外设基地址                           */
    uint32_t              rx_periph_inc;   /**< 外设地址递增模式                     */
    uint32_t              rx_memory0_addr; /**< 存储器 0 基地址                      */
    uint32_t              rx_memory_inc;   /**< 存储器地址递增模式                   */
    uint32_t              rx_width;        /**< 传输宽度（外设与存储器一致）         */
    uint32_t              rx_circular_mode;/**< 循环模式使能/禁止                    */
    uint32_t              rx_direction;    /**< 传输方向                             */
    uint32_t              rx_number;       /**< 传输数量                             */
    uint32_t              rx_priority;     /**< 通道优先级                           */
} drv_rx_dma_init_t;


typedef struct
{
    uint32_t              tx_dma;          /**< DMA 控制器，DMA0 / DMA1             */
    dma_channel_enum      tx_channel;      /**< DMA 通道，DMA_CH0 ~ DMA_CH7         */
    dma_subperipheral_enum tx_subperiph;   /**< 外设选择，DMA_SUBPERI0 ~ DMA_SUBPERI7 */
    uint32_t              tx_periph_addr;  /**< 外设基地址                           */
    uint32_t              tx_periph_inc;   /**< 外设地址递增模式                     */
    uint32_t              tx_memory0_addr; /**< 存储器 0 基地址                      */
    uint32_t              tx_memory_inc;   /**< 存储器地址递增模式                   */
    uint32_t              tx_width;        /**< 传输宽度（外设与存储器一致）         */
    uint32_t              tx_circular_mode;/**< 循环模式使能/禁止                    */
    uint32_t              tx_direction;    /**< 传输方向                             */
    uint32_t              tx_number;       /**< 传输数量                             */
    uint32_t              tx_priority;     /**< 通道优先级                           */
} drv_tx_dma_init_t;

/**
 * @brief 使能 DMA 控制器时钟
 *
 * @param dma DMA 控制器（DMA0 / DMA1）
 */
void drv_dma_clock_enable(uint32_t dma);

/**
 * @brief 以单数据模式初始化 RX DMA 通道（循环模式，外设 -> 存储器）
 *
 * @param cfg RX DMA 初始化配置结构体指针
 */
void drv_rx_dma_init(const drv_rx_dma_init_t *cfg);

/**
 * @brief 以单数据模式初始化 TX DMA 通道（单次模式，存储器 -> 外设）
 *
 * @param cfg TX DMA 初始化配置结构体指针
 */
void drv_tx_dma_init(const drv_tx_dma_init_t *cfg);

/**
 * @brief 初始化一对 RX / TX DMA 通道（内部先后调用 rx / tx 两个初始化函数）
 *
 * @param rx_config RX DMA 初始化配置结构体指针
 * @param tx_config TX DMA 初始化配置结构体指针
 */
void drv_dma_init(const drv_rx_dma_init_t *rx_config, const drv_tx_dma_init_t *tx_config);

/**
 * @brief 使能 DMA 通道
 *
 * @param dma DMA 控制器
 * @param channel DMA 通道
 */
void drv_dma_channel_enable(uint32_t dma, dma_channel_enum channel);

/**
 * @brief 禁止 DMA 通道
 *
 * @param dma DMA 控制器
 * @param channel DMA 通道
 */
void drv_dma_channel_disable(uint32_t dma, dma_channel_enum channel);

/**
 * @brief 配置存储器 0 基地址
 *
 * @param dma DMA 控制器
 * @param channel DMA 通道
 * @param addr 存储器 0 基地址
 */
void drv_dma_memory_address_config(uint32_t dma, dma_channel_enum channel, uint32_t addr);

/**
 * @brief 配置传输数量
 *
 * @param dma DMA 控制器
 * @param channel DMA 通道
 * @param number 传输数量
 */
void drv_dma_transfer_number_config(uint32_t dma, dma_channel_enum channel, uint32_t number);

/**
 * @brief 获取剩余传输数量
 *
 * @param dma DMA 控制器
 * @param channel DMA 通道
 * @return 剩余传输数量
 */
uint32_t drv_dma_transfer_number_get(uint32_t dma, dma_channel_enum channel);

/**
 * @brief 使能 DMA 中断源
 *
 * @param dma DMA 控制器
 * @param channel DMA 通道
 * @param source 中断源
 */
void drv_dma_interrupt_enable(uint32_t dma, dma_channel_enum channel, uint32_t source);

/**
 * @brief 禁止 DMA 中断源
 *
 * @param dma DMA 控制器
 * @param channel DMA 通道
 * @param source 中断源
 */
void drv_dma_interrupt_disable(uint32_t dma, dma_channel_enum channel, uint32_t source);

/**
 * @brief 清除 DMA 标志
 *
 * @param dma DMA 控制器
 * @param channel DMA 通道
 * @param flag 待清除的标志
 */
void drv_dma_flag_clear(uint32_t dma, dma_channel_enum channel, uint32_t flag);

/**
 * @brief 获取 DMA 中断标志
 *
 * @param dma DMA 控制器
 * @param channel DMA 通道
 * @param int_flag 中断标志
 * @return 中断标志状态（SET/RESET）
 */
FlagStatus drv_dma_interrupt_flag_get(uint32_t dma, dma_channel_enum channel, uint32_t int_flag);

#endif /* __DRV_DMA__H_ */
