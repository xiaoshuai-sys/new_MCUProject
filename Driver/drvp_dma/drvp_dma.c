// drvp_dma.c
#include "drvp_dma.h"

/**
 * @brief 接口具体实例（映射到底层驱动 drv_dma）
 */
static drvp_dma_t drvp_dma_impl =
{
    .init                   = drv_dma_init,
    .clock_enable           = drv_dma_clock_enable,
    .channel_enable         = drv_dma_channel_enable,
    .channel_disable        = drv_dma_channel_disable,
    .memory_address_config  = drv_dma_memory_address_config,
    .transfer_number_config = drv_dma_transfer_number_config,
    .transfer_number_get    = drv_dma_transfer_number_get,
    .interrupt_enable       = drv_dma_interrupt_enable,
    .interrupt_disable      = drv_dma_interrupt_disable,
    .flag_clear             = drv_dma_flag_clear,
    .interrupt_flag_get     = drv_dma_interrupt_flag_get,
};

/**
 * @brief 全局接口实例指针（供外部调用）
 */
drvp_dma_t *drvp_dma = &drvp_dma_impl;
