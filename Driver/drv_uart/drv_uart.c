// drv_uart.c
#include "drv_uart.h"
#include "drvp_gpio.h"
#include "drvp_dma.h"
#include <string.h>

// 运行时实例注册表：把「USART 基地址 / DMA 通道」映射到对应实例，
// 使中断服务函数只需拿到基地址即可找到正确的配置与缓冲区。
static drv_usart_dev_t   g_dev[DRV_UART_MAX_INSTANCES];
static uint8_t           g_dev_count = 0;

/**
 * @brief 根据 USART 基地址查找对应的运行时实例
 *
 * @param usart USART 基地址
 * @return 对应实例指针，未找到时返回 NULL
 */


 /* drv_usart_dev_t dev;  drv_usart_dev_t *p; p = &dev   
    所以 drv_usart_dev_t *drv_usart_dev_get(uint32_t usart)  
    可以理解成 drv_usart_dev_t *p = drv_usart_dev_get(uint32_t usart)
    返回的是一个指针，指向对应的运行时实例*/
drv_usart_dev_t *drv_usart_dev_get(uint32_t usart)
{
    uint8_t i;
    for(i = 0;i < g_dev_count;i++)
    {
        if(g_dev[i].config != NULL && g_dev[i].config->usart == usart)
        {
            return &g_dev[i];
        }
    }
    return NULL;
}

/**
 * @brief 按 RX DMA 通道查找对应的运行时实例（供 DMA 半满/全满中断使用）
 *
 * @param dma DMA 控制器基地址
 * @param ch DMA 通道号
 * @return 对应实例指针，未找到时返回 NULL
 */
static drv_usart_dev_t *drv_usart_dev_find_dma(uint32_t dma, dma_channel_enum ch)
{
    uint8_t i;
	for(i = 0; i < g_dev_count;i++)
	{
		if(g_dev[i].config != NULL && g_dev[i].config->rx_dma == dma && g_dev[i].config->rx_dma_ch == ch)
		{
			return &g_dev[i];
		}
	}
	return NULL;
}

/**
 * @brief 由 DMA 控制器 + 通道映射到对应的中断号
 *
 * @note IRQn 非连续，需显式查表
 *
 * @param dma DMA 控制器基地址
 * @param ch DMA 通道号
 * @return 对应的 IRQn，无法匹配时返回 0
 */
static IRQn_Type drv_usart_dma_irqn(uint32_t dma, dma_channel_enum ch)
{
    if (dma == DMA0)
    {
        switch (ch)
        {
            case DMA_CH0: return DMA0_Channel0_IRQn;
            case DMA_CH1: return DMA0_Channel1_IRQn;
            case DMA_CH2: return DMA0_Channel2_IRQn;
            case DMA_CH3: return DMA0_Channel3_IRQn;
            case DMA_CH4: return DMA0_Channel4_IRQn;
            case DMA_CH5: return DMA0_Channel5_IRQn;
            case DMA_CH6: return DMA0_Channel6_IRQn;
            case DMA_CH7: return DMA0_Channel7_IRQn;
            default: break;
        }
    }
    else if (dma == DMA1)
    {
        switch (ch)
        {
            case DMA_CH0: return DMA1_Channel0_IRQn;
            case DMA_CH1: return DMA1_Channel1_IRQn;
            case DMA_CH2: return DMA1_Channel2_IRQn;
            case DMA_CH3: return DMA1_Channel3_IRQn;
            case DMA_CH4: return DMA1_Channel4_IRQn;
            case DMA_CH5: return DMA1_Channel5_IRQn;
            case DMA_CH6: return DMA1_Channel6_IRQn;
            case DMA_CH7: return DMA1_Channel7_IRQn;
            default: break;
        }
    }
    return (IRQn_Type)0;
}

/**
 * @brief 使能 USART 时钟
 *
 * @param usart USART 基地址
 */
static void drv_uart_clock_enable(uint32_t usart)
{
    switch (usart)
    {
        case USART0: rcu_periph_clock_enable(RCU_USART0); break;
        case USART1: rcu_periph_clock_enable(RCU_USART1); break;
        case USART2: rcu_periph_clock_enable(RCU_USART2); break;
        case UART3:  rcu_periph_clock_enable(RCU_UART3);  break;
        case UART4:  rcu_periph_clock_enable(RCU_UART4);  break;
        case USART5: rcu_periph_clock_enable(RCU_USART5); break;
        default: break;
    }
}

/**
 * @brief 把 DMA 缓冲区 [rx_last_offset, pos) 这段数据（可能跨回绕）整体搬进环形缓冲区
 *
 * @note 空间不足时整体丢弃（并推进游标），避免半包写入导致数据错位
 *
 * @param dev UART 运行时实例指针
 * @param pos DMA 当前写位置（相对缓冲区起始的偏移）
 */
static void drv_uart_rx_flush(drv_usart_dev_t *dev, uint32_t pos)
{
    uint32_t size = dev->buf_config->rx_dma_buf_size;
    uint32_t last = dev->rx_last_offset;
    const uint8_t *base = dev->buf_config->rx_dma_buf;
    circular_buffer *cb = dev->cir_config->cb;
    uint32_t n;

    if (pos == last)
    {
        return;
    }

    //数据长度
    n = (pos >= last) ? (pos - last) : (size - last + pos);

    /* 环形缓冲区有效容量为 size-1，判断能否整体放入 */
    if (circular_buffer_length(cb) + n < dev->cir_config->cir_buf_size)
    {
        if (pos >= last)
        {
            circular_buffer_write(cb, (const char *)&base[last], pos - last);
        }
        else
        {
            circular_buffer_write(cb, (const char *)&base[last], size - last);
            if (pos > 0)
            {
                circular_buffer_write(cb, (const char *)&base[0], pos);
            }
        }
    }

    dev->rx_total_bytes += n;   /* 含被丢弃的字节，便于上层统计丢包 */
    dev->rx_last_offset = pos;
}

/**
 * @brief 把 DMA 缓冲区中未搬运的数据搬进环形缓冲区（可随时调用）
 *
 * @param dev UART 运行时实例指针
 */
void drv_usart_drain_rx(drv_usart_dev_t *dev)
{
    uint32_t size, remain, pos;

    if (dev == NULL || dev->config == NULL || dev->buf_config == NULL)
    {
        return;
    }

    size = dev->buf_config->rx_dma_buf_size;
    if (size == 0)
    {
        return;
    }

    remain = drvp_dma->transfer_number_get(dev->config->rx_dma, dev->config->rx_dma_ch);
    pos = size - remain;
    if (pos >= size)            /* DMA 刚回绕到 0 的边界情况 */
    {
        pos = 0;
    }

    drv_uart_rx_flush(dev, pos);
}

/**
 * @brief 初始化 UART（GPIO、DMA、USART 外设及中断）
 *
 * @param config USART 初始化配置结构体指针
 * @param dma_buf_config DMA 收发缓冲区配置结构体指针
 * @param cir_buf_config 环形缓冲区配置结构体指针
 */
void drv_usart_init(const drv_usart_init_t *config, const drv_buf_init_t *dma_buf_config, const drv_cir_buf_init_t *cir_buf_config)
{
    drv_gpio_init_t   gpio;
    drv_rx_dma_init_t rx_dma;
    drv_tx_dma_init_t tx_dma;

    if (config == NULL || dma_buf_config == NULL || cir_buf_config == NULL)
    {
        return;
    }
    if (dma_buf_config->rx_dma_buf == NULL || dma_buf_config->tx_dma_buf == NULL ||
        cir_buf_config->cir_buf == NULL || cir_buf_config->cb == NULL)
    {
        return;
    }

    /* 申请环形缓冲区（大小必须为 2 的幂次） */
    if (!circular_buffer_attach(cir_buf_config->cb, (char *)cir_buf_config->cir_buf, cir_buf_config->cir_buf_size))
    {
        while (1);
    }

    /* 注册运行时实例（重复调用同一 USART 时复用原实例） */
    drv_usart_dev_t *dev = drv_usart_dev_get(config->usart);
    if (dev == NULL)
    {
        if (g_dev_count >= DRV_UART_MAX_INSTANCES)
        {
            return;
        }
        dev = &g_dev[g_dev_count++];
        memset(dev, 0, sizeof(*dev));
    }
    dev->config      = config;
    dev->buf_config  = dma_buf_config;
    dev->cir_config  = cir_buf_config;
    dev->rx_last_offset = 0;
    dev->rx_total_bytes = 0;
    dev->frame_ready = 0;

    drv_uart_clock_enable(config->usart);

    /* TX 引脚：复用推挽 */
    gpio.periph = config->tx_gpio;
    gpio.pin    = config->tx_pin;
    gpio.mode   = GPIO_MODE_AF;
    gpio.pull   = GPIO_PUPD_NONE;
    gpio.otype  = GPIO_OTYPE_PP;
    gpio.speed  = GPIO_OSPEED_50MHZ;
    gpio.af     = config->tx_af;
    drvp_gpio->init(&gpio);

    /* RX 引脚：复用输入，上拉 */
    gpio.periph = config->rx_gpio;
    gpio.pin    = config->rx_pin;
    gpio.mode   = GPIO_MODE_AF;
    gpio.pull   = GPIO_PUPD_PULLUP;
    gpio.otype  = GPIO_OTYPE_PP;
    gpio.speed  = GPIO_OSPEED_50MHZ;
    gpio.af     = config->rx_af;
    drvp_gpio->init(&gpio);

    /* RX DMA：循环模式，外设 -> 存储器 */
    rx_dma.rx_dma           = config->rx_dma;
    rx_dma.rx_channel       = config->rx_dma_ch;
    rx_dma.rx_subperiph     = config->rx_dma_sub;
    rx_dma.rx_periph_addr   = (uint32_t)&USART_DATA(config->usart);
    rx_dma.rx_periph_inc    = DMA_PERIPH_INCREASE_DISABLE;
    rx_dma.rx_memory0_addr  = (uint32_t)dma_buf_config->rx_dma_buf;
    rx_dma.rx_memory_inc    = DMA_MEMORY_INCREASE_ENABLE;
    rx_dma.rx_width         = DMA_PERIPH_WIDTH_8BIT;
    rx_dma.rx_circular_mode = DMA_CIRCULAR_MODE_ENABLE;
    rx_dma.rx_direction     = DMA_PERIPH_TO_MEMORY;
    rx_dma.rx_number        = dma_buf_config->rx_dma_buf_size;
    rx_dma.rx_priority      = DMA_PRIORITY_HIGH;

    /* TX DMA：单次模式，存储器 -> 外设（发送完成用轮询判断，不使能 FTF 中断） */
    tx_dma.tx_dma           = config->tx_dma;
    tx_dma.tx_channel       = config->tx_dma_ch;
    tx_dma.tx_subperiph     = config->tx_dma_sub;
    tx_dma.tx_periph_addr   = (uint32_t)&USART_DATA(config->usart);
    tx_dma.tx_periph_inc    = DMA_PERIPH_INCREASE_DISABLE;
    tx_dma.tx_memory0_addr  = (uint32_t)dma_buf_config->tx_dma_buf;
    tx_dma.tx_memory_inc    = DMA_MEMORY_INCREASE_ENABLE;
    tx_dma.tx_width         = DMA_PERIPH_WIDTH_8BIT;
    tx_dma.tx_circular_mode = DMA_CIRCULAR_MODE_DISABLE;
    tx_dma.tx_direction     = DMA_MEMORY_TO_PERIPH;
    tx_dma.tx_number        = 0;
    tx_dma.tx_priority      = DMA_PRIORITY_HIGH;

    /* USART 基本参数 */
    usart_deinit(config->usart);
    usart_baudrate_set(config->usart, config->baudrate);
    usart_word_length_set(config->usart, config->word_length);
    usart_stop_bit_set(config->usart, config->stop_bits);
    usart_parity_config(config->usart, config->parity);
    usart_receive_config(config->usart, USART_RECEIVE_ENABLE);
    usart_transmit_config(config->usart, USART_TRANSMIT_ENABLE);
    usart_enable(config->usart);

    /* ===== 硬件自检：直接写数据寄存器，绕过 TBE 轮询 =====
       若串口助手能收到 'H'，说明 USART/GPIO/时钟/波特率硬件链路全部正常；
       若收不到 'H'，问题在 USART 硬件配置（时钟/GPIO/波特率）。 */
    {
        volatile uint32_t t;
        for (t = 0; t < 200000U; t++) { __NOP(); }  /* 等待 USART 稳定 */
        usart_data_transmit(config->usart, (uint16_t)'H');
    }

    /* DMA 通道初始化 */
    drvp_dma->init(&rx_dma, &tx_dma);

    /* 清除 RX DMA 残留标志，并开 HTF/FTF 中断，实现连续数据不丢 */
    drvp_dma->flag_clear(config->rx_dma, config->rx_dma_ch, DMA_INT_FLAG_HTF);
    drvp_dma->flag_clear(config->rx_dma, config->rx_dma_ch, DMA_INT_FLAG_FTF);
    drvp_dma->interrupt_enable(config->rx_dma, config->rx_dma_ch, DMA_INT_HTF);
    drvp_dma->interrupt_enable(config->rx_dma, config->rx_dma_ch, DMA_INT_FTF);
    nvic_irq_enable(drv_usart_dma_irqn(config->rx_dma, config->rx_dma_ch), 2, 0);

    /* 使能 DMA 收发请求 */
    usart_dma_receive_config(config->usart, USART_RECEIVE_DMA_ENABLE);
    usart_dma_transmit_config(config->usart, USART_TRANSMIT_DMA_ENABLE);
    drvp_dma->channel_enable(config->rx_dma, config->rx_dma_ch);

    /* IDLE 中断：检测一帧结束 */
    nvic_irq_enable(config->usart_irq, 1, 1);
    usart_interrupt_enable(config->usart, USART_INT_IDLE);
}

/**
 * @brief 发送一个字节数据（轮询方式）
 *
 * @param config USART 初始化配置结构体指针
 * @param data 待发送的字节
 */
void drv_usart_send_byte(const drv_usart_init_t *config, uint8_t data)
{
    uint32_t timeout = 0;
    /* 带超时等待 TBE，避免 USART 未真正工作时永久卡死。
       超时后仍强行写入，保证上层（shell）不会因发送而挂死。 */
    while (usart_flag_get(config->usart, USART_FLAG_TBE) == RESET)
    {
        if (++timeout > 1000000U)
        {
            break;
        }
    }
    usart_data_transmit(config->usart, data);
}

/**
 * @brief 发送字符串（轮询方式）
 *
 * @param config USART 初始化配置结构体指针
 * @param str 待发送的以 '\0' 结尾的字符串
 */
void drv_usart_send_string(const drv_usart_init_t *config, const char *str)
{
    while (*str)
    {
        while (usart_flag_get(config->usart, USART_FLAG_TBE) == RESET);
        usart_data_transmit(config->usart, (uint8_t)*str++);
    }
    while (usart_flag_get(config->usart, USART_FLAG_TC) == RESET);
}

/**
 * @brief 发送一个字节数据（轮询方式，带超时）
 *
 * @param config USART 初始化配置结构体指针
 * @param data 待发送的字节
 * @param timeout_ms 超时时间（毫秒）
 * @return 0 成功，1 超时
 */
uint8_t drv_usart_send_byte_timeout(const drv_usart_init_t *config, uint8_t data, uint32_t timeout_ms)
{
    uint32_t start = systick_get_ms();

    while (usart_flag_get(config->usart, USART_FLAG_TBE) == RESET)
    {
        if ((systick_get_ms() - start) >= timeout_ms)
        {
            return 1;
        }
    }
    usart_data_transmit(config->usart, data);
    return 0;
}

/**
 * @brief 发送字符串（轮询方式，带超时）
 *
 * @param config USART 初始化配置结构体指针
 * @param str 待发送的以 '\0' 结尾的字符串
 * @param timeout_ms 超时时间（毫秒），为 0 时表示无限等待
 * @return 0 成功，1 超时
 */
uint8_t drv_usart_send_string_timeout(const drv_usart_init_t *config, const char *str, uint32_t timeout_ms)
{
    uint32_t start;

    while (*str)
    {
        if (drv_usart_send_byte_timeout(config, (uint8_t)*str++, timeout_ms) != 0)
        {
            return 1;
        }
    }

    start = systick_get_ms();
    while (usart_flag_get(config->usart, USART_FLAG_TC) == RESET)
    {
        if ((systick_get_ms() - start) >= timeout_ms)
        {
            return 1;
        }
    }
    return 0;
}

/**
 * @brief 查询发送 DMA 通道是否空闲
 *
 * @note 发送 DMA 通道忙时，不能再调用 drv_usart_send_dma，否则会丢数据。
 *       发送完成的判断依据是 DMA 剩余传输数量为 0。
 *
 * @param config USART 初始化配置结构体指针
 * @return 0 空闲，1 忙
 */
uint8_t drv_usart_tx_busy(const drv_usart_init_t *config)
{
    /* 传输完成后剩余计数为 0，据此判断发送是否仍在进行 */
    return (drvp_dma->transfer_number_get(config->tx_dma, config->tx_dma_ch) != 0) ? 1 : 0;
}

/**
 * @brief 发送数据（DMA 方式）
 *
 * @param config USART 初始化配置结构体指针
 * @param buf_config DMA 缓冲区配置结构体指针
 * @param data 待发送数据指针
 * @param length 待发送数据长度
 * @return 0 成功
 * @return 1 参数错误
 * @return 2 数据长度超过 DMA 缓冲区大小
 * @return 3 DMA 通道忙，正在发送中
 */
uint8_t drv_usart_send_dma(const drv_usart_init_t *config, const drv_buf_init_t *buf_config, const uint8_t *data, uint16_t length)
{
    if (data == NULL || length == 0)
        return 1;
    if (length > buf_config->tx_dma_buf_size)
        return 2;
    if (drv_usart_tx_busy(config))
        return 3;

    memcpy(buf_config->tx_dma_buf, data, length);

    drvp_dma->channel_disable(config->tx_dma, config->tx_dma_ch);
    drvp_dma->flag_clear(config->tx_dma, config->tx_dma_ch, DMA_INT_FLAG_HTF);
    drvp_dma->flag_clear(config->tx_dma, config->tx_dma_ch, DMA_INT_FLAG_FTF);
    drvp_dma->memory_address_config(config->tx_dma, config->tx_dma_ch, (uint32_t)buf_config->tx_dma_buf);
    drvp_dma->transfer_number_config(config->tx_dma, config->tx_dma_ch, length);
    drvp_dma->channel_enable(config->tx_dma, config->tx_dma_ch);

    return 0;
}

/**
 * @brief 获取环形缓冲区中的有效数据长度
 *
 * @param cir_buf_config 环形缓冲区配置结构体指针
 * @return 有效数据长度（字节）
 */
size_t drv_usart_rx_length(const drv_cir_buf_init_t *cir_buf_config)
{
    if (cir_buf_config == NULL || cir_buf_config->cb == NULL)
    {
        return 0;
    }
    return circular_buffer_length(cir_buf_config->cb);
}

/**
 * @brief 从环形缓冲区读取数据
 *
 * @note 如果环形缓冲区中没有足够的数据，返回值可能小于 length。
 *
 * @param cir_buf_config 环形缓冲区配置结构体指针
 * @param buf_data 读出数据存放的缓冲区指针
 * @param length 期望读取的字节数
 * @return 实际读取的字节数
 */
size_t drv_usart_rx_read_data(const drv_cir_buf_init_t *cir_buf_config, char *buf_data, size_t length)
{
    size_t available;
    size_t read_len;

    if (cir_buf_config == NULL || cir_buf_config->cb == NULL || buf_data == NULL || length == 0)
    {
        return 0;
    }

    available = circular_buffer_length(cir_buf_config->cb);
    if (available == 0)
    {
        return 0;
    }

    read_len = (available > length) ? length : available;

    if (!circular_buffer_read(cir_buf_config->cb, buf_data, read_len))
    {
        return 0;
    }
    return read_len;
}

/**
 * @brief 接收数据处理（主循环调用）
 *
 * @note 示例为原样回传。仅当发送空闲时才读取，避免 DMA 发送忙导致丢数据。
 *
 * @param dev UART 运行时实例指针
 */
void drv_usart_process(drv_usart_dev_t *dev)
{
    static char buf[1024];
    size_t n;

    if (dev == NULL || dev->config == NULL || dev->buf_config == NULL || dev->cir_config == NULL)
    {
        return;
    }

    if (drv_usart_tx_busy(dev->config))
    {
        return;                 /* 发送忙，本轮不取数据，等待发送完成 */
    }

    n = drv_usart_rx_length(dev->cir_config);
    if (n == 0)
    {
        return;
    }
    if (n > sizeof(buf))
    {
        n = sizeof(buf);
    }

    if (drv_usart_rx_read_data(dev->cir_config, buf, n) != n)
    {
        return;
    }

    drv_usart_send_dma(dev->config, dev->buf_config, (uint8_t *)buf, n);
}

/**
 * @brief RX DMA 中断处理（半满/全满时把数据搬进环形缓冲区）
 *
 * @param dma DMA 控制器基地址
 * @param ch DMA 通道号
 */
void drv_usart_dma_rx_irq_handler(uint32_t dma, dma_channel_enum ch)
{
    drv_usart_dev_t *dev = drv_usart_dev_find_dma(dma, ch);
    if (dev == NULL)
    {
        return;
    }

    if (drvp_dma->interrupt_flag_get(dma, ch, DMA_INT_FLAG_HTF) != RESET)
    {
        drvp_dma->flag_clear(dma, ch, DMA_INT_FLAG_HTF);
        drv_usart_drain_rx(dev);
    }
    if (drvp_dma->interrupt_flag_get(dma, ch, DMA_INT_FLAG_FTF) != RESET)
    {
        drvp_dma->flag_clear(dma, ch, DMA_INT_FLAG_FTF);
        drv_usart_drain_rx(dev);
    }
}

/**
 * @brief USART 中断处理（IDLE 中断检测一帧结束）
 *
 * @param usart USART 基地址
 */
void drv_usart_irq_handler(uint32_t usart)
{
    if (usart_interrupt_flag_get(usart, USART_INT_FLAG_IDLE) != RESET)
    {
        (void)USART_STAT0(usart);   /* 读 STAT0 再读 DATA 清除 IDLE 标志 */
        (void)USART_DATA(usart);

        drv_usart_dev_t *dev = drv_usart_dev_get(usart);
        if (dev != NULL)
        {
            drv_usart_drain_rx(dev);
            dev->frame_ready = 1;
        }
    }
}

/* USART 中断服务函数 */
void USART0_IRQHandler(void) { drv_usart_irq_handler(USART0); }
void USART1_IRQHandler(void) { drv_usart_irq_handler(USART1); }
void USART2_IRQHandler(void) { drv_usart_irq_handler(USART2); }
void UART3_IRQHandler(void)  { drv_usart_irq_handler(UART3);  }
void UART4_IRQHandler(void)  { drv_usart_irq_handler(UART4);  }
void USART5_IRQHandler(void) { drv_usart_irq_handler(USART5); }

/* RX DMA 中断服务函数：把对应通道的中断交给驱动统一处理。
   每新增一路 UART 的 RX DMA 通道，需在此补充对应的 IRQHandler。 */
void DMA1_Channel2_IRQHandler(void) { drv_usart_dma_rx_irq_handler(DMA1, DMA_CH2); }
void DMA1_Channel7_IRQHandler(void) { drv_usart_dma_rx_irq_handler(DMA1, DMA_CH7); }
