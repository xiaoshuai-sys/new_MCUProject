// drv_dma.c
#include "drv_dma.h"

void drv_dma_clock_enable(uint32_t dma)
{
    switch(dma)
    {
        case DMA0:
            rcu_periph_clock_enable(RCU_DMA0);
            break;
        case DMA1:
            rcu_periph_clock_enable(RCU_DMA1);
            break;
    }
}

void drv_rx_dma_init(const drv_rx_dma_init_t *config)
{
    dma_single_data_parameter_struct dma_init_struct = {0};
    if (config == NULL)
    {
        return;
    }
    // 使能 DMA 控制器时钟
    drv_dma_clock_enable(config->rx_dma);
    dma_deinit(config->rx_dma, config->rx_channel);
    dma_single_data_para_struct_init(&dma_init_struct);
    
    dma_init_struct.periph_addr         = config->rx_periph_addr;
    dma_init_struct.periph_inc          = config->rx_periph_inc;
    dma_init_struct.memory0_addr        = config->rx_memory0_addr;
    dma_init_struct.memory_inc          = config->rx_memory_inc;
    dma_init_struct.periph_memory_width = config->rx_width;
    dma_init_struct.circular_mode       = config->rx_circular_mode;
    dma_init_struct.direction           = config->rx_direction;
    dma_init_struct.number              = config->rx_number;
    dma_init_struct.priority            = config->rx_priority;
    
    dma_single_data_mode_init(config->rx_dma, config->rx_channel, &dma_init_struct);
    dma_channel_subperipheral_select(config->rx_dma, config->rx_channel, config->rx_subperiph);

    /* 注意：RX DMA 中断（HTF/FTF）的使能与 NVIC 由上层 UART 驱动按实际通道配置，
       不在此硬编码，保持本 DMA 驱动的通用性。 */
}


void drv_tx_dma_init(const drv_tx_dma_init_t *config)
{
    dma_single_data_parameter_struct dma_init_struct = {0};
    if (config == NULL)
    {
        return;
    }
    drv_dma_clock_enable(config->tx_dma);
    dma_deinit(config->tx_dma, config->tx_channel);
    dma_single_data_para_struct_init(&dma_init_struct);

    dma_init_struct.periph_addr         = config->tx_periph_addr;
    dma_init_struct.periph_inc          = config->tx_periph_inc;    
    dma_init_struct.memory0_addr        = config->tx_memory0_addr;
    dma_init_struct.memory_inc          = config->tx_memory_inc;
    dma_init_struct.periph_memory_width = config->tx_width;
    dma_init_struct.circular_mode       = config->tx_circular_mode;
    dma_init_struct.direction           = config->tx_direction;
    dma_init_struct.number              = config->tx_number;
    dma_init_struct.priority            = config->tx_priority;

    dma_single_data_mode_init(config->tx_dma, config->tx_channel, &dma_init_struct);
    dma_channel_subperipheral_select(config->tx_dma, config->tx_channel, config->tx_subperiph);
    /* 清除满传输完成标志。TX 完成采用轮询（transfer_number_get）判断，
       不使能 FTF 中断，避免无对应 IRQHandler 导致跳到默认弱中断死循环。 */
    drv_dma_flag_clear(config->tx_dma, config->tx_channel, DMA_INT_FLAG_FTF);
}


/**
 * @brief 以单数据模式初始化 DMA 通道
 *
 * @param cfg DMA 初始化配置结构体指针
 */
void drv_dma_init(const drv_rx_dma_init_t *rx_config, const drv_tx_dma_init_t *tx_config)
{
    if (rx_config != NULL)
    {
        drv_rx_dma_init(rx_config);
    }
    if (tx_config != NULL)
    {
        drv_tx_dma_init(tx_config);
    }
}

/**
 * @brief 使能 DMA 通道
 *
 * @param dma DMA 控制器
 * @param channel DMA 通道
 */
void drv_dma_channel_enable(uint32_t dma, dma_channel_enum channel)
{
    dma_channel_enable(dma, channel);
}

/**
 * @brief 禁止 DMA 通道
 *
 * @param dma DMA 控制器
 * @param channel DMA 通道
 */
void drv_dma_channel_disable(uint32_t dma, dma_channel_enum channel)
{
    dma_channel_disable(dma, channel);
}

/**
 * @brief 配置存储器 0 基地址
 *
 * @param dma DMA 控制器
 * @param channel DMA 通道
 * @param addr 存储器 0 基地址
 */
void drv_dma_memory_address_config(uint32_t dma, dma_channel_enum channel, uint32_t addr)
{
    dma_memory_address_config(dma, channel, DMA_MEMORY_0, addr);
}

/**
 * @brief 配置传输数量
 *
 * @param dma DMA 控制器
 * @param channel DMA 通道
 * @param number 传输数量
 */
void drv_dma_transfer_number_config(uint32_t dma, dma_channel_enum channel, uint32_t number)
{
    dma_transfer_number_config(dma, channel, number);
}

/**
 * @brief 获取剩余传输数量
 *
 * @param dma DMA 控制器
 * @param channel DMA 通道
 * @return 剩余传输数量
 */
uint32_t drv_dma_transfer_number_get(uint32_t dma, dma_channel_enum channel)
{
    return dma_transfer_number_get(dma, channel);
}

/**
 * @brief 使能 DMA 中断源
 *
 * @param dma DMA 控制器
 * @param channel DMA 通道
 * @param source 中断源
 */
void drv_dma_interrupt_enable(uint32_t dma, dma_channel_enum channel, uint32_t source)
{
    dma_interrupt_enable(dma, channel, source);
}

/**
 * @brief 禁止 DMA 中断源
 *
 * @param dma DMA 控制器
 * @param channel DMA 通道
 * @param source 中断源
 */
void drv_dma_interrupt_disable(uint32_t dma, dma_channel_enum channel, uint32_t source)
{
    dma_interrupt_disable(dma, channel, source);
}

/**
 * @brief 清除 DMA 标志
 *
 * @param dma DMA 控制器
 * @param channel DMA 通道
 * @param flag 待清除的标志
 */
void drv_dma_flag_clear(uint32_t dma, dma_channel_enum channel, uint32_t flag)
{
    dma_flag_clear(dma, channel, flag);
}

/**
 * @brief 获取 DMA 中断标志
 *
 * @param dma DMA 控制器
 * @param channel DMA 通道
 * @param int_flag 中断标志
 * @return 中断标志状态（SET/RESET）
 */
FlagStatus drv_dma_interrupt_flag_get(uint32_t dma, dma_channel_enum channel, uint32_t int_flag)
{
    return dma_interrupt_flag_get(dma, channel, int_flag);
}




//void DMA1_Channel7_IRQHandler(void)
//{
//    if (rv_dma_interrupt_flag_get(, USART0_TX_DMA_CH, DMA_INT_FLAG_FTF) != RESET) 
//    {
//        dma_interrupt_flag_clear(USART0_TX_DMA, USART0_TX_DMA_CH, DMA_INT_FLAG_FTF);
//        dma_channel_disable(USART0_TX_DMA, USART0_TX_DMA_CH);
//        uart0_tx_busy = 0;              /* 清除忙标志 */
//    
//    }
//    else if(dma_interrupt_flag_get(USART0_TX_DMA, USART0_TX_DMA_CH, DMA_INT_FLAG_HTF) != RESET)
//    {
//        dma_interrupt_flag_clear(USART0_TX_DMA, USART0_TX_DMA_CH, DMA_INT_FLAG_HTF);
//        dma_channel_disable(USART0_TX_DMA, USART0_TX_DMA_CH);
//        uart0_tx_busy = 0;              /* 清除忙标志 */
//    }
//}

