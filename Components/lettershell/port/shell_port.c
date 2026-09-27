//// shell_port.c
//#include "shell_port.h"
//#include "shell.h"
//#include "drvp_uart.h"
//#include "drvp_gpio.h"
//#include "gd32f4xx.h"
//#include "systick.h"

///*-----------------------------------------------------------------------------
//  资源分配：复用 USART0
//  PA9 = TX(AF7) / PA10 = RX(AF7)
//  RX = DMA1 CH2 外设4   TX = DMA1 CH7 外设4
// ----------------------------------------------------------------------------*/
//#define SHELL_UART_RX_DMA_BUF_SIZE   1024
//#define SHELL_UART_TX_DMA_BUF_SIZE   1024
//#define SHELL_UART_RX_CIR_BUF_SIZE   1024    /* 必须为 2 的幂次 */

///* DMA 接收缓冲区 */
//static uint8_t shell_rx_dma_buf[SHELL_UART_RX_DMA_BUF_SIZE];

///* DMA 发送缓冲区 */
//static uint8_t shell_tx_dma_buf[SHELL_UART_TX_DMA_BUF_SIZE];

///* 环形缓冲区（使用静态内存，避免 malloc） */
//static uint8_t         shell_rx_cir_buf[SHELL_UART_RX_CIR_BUF_SIZE];
//static circular_buffer shell_cir_buf;

///**
// * @brief USART0 初始化配置（shell 专用）
// */
//static const drv_usart_init_t shell_uart_config =
//{
//    .usart       = USART0,
//    .baudrate    = 115200,
//    .word_length = USART_WL_8BIT,
//    .stop_bits   = USART_STB_1BIT,
//    .parity      = USART_PM_NONE,
//    .usart_irq   = USART0_IRQn,

//    .tx_gpio = GPIOA, .tx_pin  = GPIO_PIN_9,  .tx_af = GPIO_AF_7,
//    .rx_gpio = GPIOA, .rx_pin  = GPIO_PIN_10, .rx_af = GPIO_AF_7,

//    .tx_dma     = DMA1, .tx_dma_ch  = DMA_CH7, .tx_dma_sub = DMA_SUBPERI4,
//    .rx_dma     = DMA1, .rx_dma_ch  = DMA_CH2, .rx_dma_sub = DMA_SUBPERI4,
//};

///**
// * @brief USART0 DMA 收发缓冲区配置
// */
//static const drv_buf_init_t shell_buf_config =
//{
//    .rx_dma_buf      = shell_rx_dma_buf,
//    .tx_dma_buf      = shell_tx_dma_buf,
//    .rx_dma_buf_size = SHELL_UART_RX_DMA_BUF_SIZE,
//    .tx_dma_buf_size = SHELL_UART_TX_DMA_BUF_SIZE,
//};

///**
// * @brief USART0 环形缓冲区配置
// */
//static const drv_cir_buf_init_t shell_cir_config =
//{
//    .cir_buf      = shell_rx_cir_buf,
//    .cir_buf_size = SHELL_UART_RX_CIR_BUF_SIZE,
//    .cb           = &shell_cir_buf,
//};

///**
// * @brief shell 实例与输入缓冲区
// */
//Shell shell;
//char  shell_buffer[512];


//signed short userShellWrite(char *data, unsigned short len)
//{
//    unsigned short i;
//    for (i = 0; i < len; i++)
//    {
//        drvp_uart->send_byte(&shell_uart_config, (uint8_t)data[i]);
//    }
//    return (signed short)len;
//}

//signed short userShellRead(char *data,unsigned short len)
//{
//    return drvp_uart->rx_read(&shell_cir_config,data,len);
//}

//void shell_port_init(void)
//{
//    drvp_uart->init(&shell_uart_config, &shell_buf_config, &shell_cir_config );
//    shell.write = userShellWrite;
//    shell.read  = userShellRead;
//    shellInit(&shell,shell_buffer,sizeof(shell_buffer));

//    /* 自检：用带超时发送测试字符串（不会死等）。
//       若串口助手能收到 "UART-TX-OK"，说明 USART 驱动发送链路正常，问题在 shell 层；
//       若收不到，说明问题在驱动/硬件配置层。 */
//    drvp_uart->send_string_timeout(&shell_uart_config, "UART-TX-OK\r\n", 1000);
//}


//void shell_port_task(void)
//{
//    char data;
//    
//    while (userShellRead(&data,1) == 1)
//    {
//        shellHandler(&shell, data);
//    }
//}

