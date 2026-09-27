// drvp_dma.h
#ifndef __DRVP_DMA__H_
#define __DRVP_DMA__H_

#include "drv_dma.h"

/**
 * @brief DMA 对外接口结构体（函数指针表）
 */
typedef struct
{
    void       (*init)(const drv_rx_dma_init_t *rx_config,const drv_tx_dma_init_t *tx_config);                              /**< 初始化通道          */
    void       (*clock_enable)(uint32_t dma);                                   /**< 使能控制器时钟      */
    void       (*channel_enable)(uint32_t dma, dma_channel_enum channel);       /**< 使能通道            */
    void       (*channel_disable)(uint32_t dma, dma_channel_enum channel);      /**< 禁止通道            */
    void       (*memory_address_config)(uint32_t dma, dma_channel_enum channel, uint32_t addr); /**< 配置存储器 0 基地址 */
    void       (*transfer_number_config)(uint32_t dma, dma_channel_enum channel, uint32_t number); /**< 配置传输数量 */
    uint32_t   (*transfer_number_get)(uint32_t dma, dma_channel_enum channel);  /**< 获取剩余传输数量    */
    void       (*interrupt_enable)(uint32_t dma, dma_channel_enum channel, uint32_t source);  /**< 使能中断源 */
    void       (*interrupt_disable)(uint32_t dma, dma_channel_enum channel, uint32_t source); /**< 禁止中断源 */
    void       (*flag_clear)(uint32_t dma, dma_channel_enum channel, uint32_t flag);          /**< 清除标志   */
    FlagStatus (*interrupt_flag_get)(uint32_t dma, dma_channel_enum channel, uint32_t int_flag); /**< 获取中断标志 */
} drvp_dma_t;

/**
 * @brief 全局接口实例指针（供外部调用）
 */
extern drvp_dma_t *drvp_dma;

#endif /* __DRVP_DMA__H_ */
