/**
 * @file    drv_uart.c
 * @author  Dylan
 * @date    2026-01-15
 * @brief   串口驱动实现。
 *
 * @details 提供UART初始化、收发接口与底层初始化配置。
 */

#include "drv_uart.h"
#include "drv_uart_desc.h"
#include "board.h"
#include "stm32h7xx_hal_dma.h"
#include "stm32h7xx_hal_uart.h"
#include "stm32h7xx_hal_uart_ex.h"

/**
 * @brief   初始化UART。
 *
 * @param[in]   uart  UART描述符。
 * @return  None
 */
void uart_init(uart_desc_t uart)
{
  if(uart == NULL)
  {
    return;
  }

  uart->hal_handle.Instance = uart->instance;
  uart->hal_handle.Init.BaudRate = uart->baudrate;
  uart->hal_handle.Init.WordLength = UART_WORDLENGTH_8B;
  uart->hal_handle.Init.StopBits = UART_STOPBITS_1;
  uart->hal_handle.Init.Parity = UART_PARITY_NONE;
  uart->hal_handle.Init.Mode = UART_MODE_TX_RX;
  uart->hal_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  uart->hal_handle.Init.OverSampling = UART_OVERSAMPLING_16;

  if(HAL_UART_Init(&uart->hal_handle) != HAL_OK)
  {
    return;
  }

  if(uart->instance == USART1)
  {
    // 使能空闲中断。
    __HAL_UART_ENABLE_IT(&uart->hal_handle, UART_IT_IDLE);

    // 启动DMA接收
    HAL_UART_Receive_DMA(&uart->hal_handle, (uint8_t *)Uart1_rx_buf, sizeof(Uart1_rx_buf));
  }
}

/**
 * @brief   UART底层初始化。
 *
 * @param[in]   huart  UART句柄。
 * @return  无
 */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  if(huart == NULL)
  {
    return;
  }

  if(huart->Instance == USART1)
  {
    static DMA_HandleTypeDef hdma_usart1_rx;

    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();


    GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 配置DMA,Uart1 - DMA1 Stream0
    hdma_usart1_rx.Instance = DMA1_Stream0;
    hdma_usart1_rx.Init.Request = DMA_REQUEST_USART1_RX;
    hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;               // 外设地址不递增
    hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;                   // 内存地址递增
    hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_rx.Init.Mode = DMA_CIRCULAR;
    hdma_usart1_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    HAL_DMA_Init(&hdma_usart1_rx);

    __HAL_LINKDMA(huart, hdmarx, hdma_usart1_rx);

    HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  }
  else if(huart->Instance == USART2)
  {
    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(USART2_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  }
}



/**
 * @brief   阻塞方式发送数据。
 *
 * @param[in]   uart     UART描述符。
 * @param[in]   data     发送数据指针。
 * @param[in]   len      发送长度。
 * @param[in]   timeout  超时计数。
 * @return  int 发送结果，0表示成功，-1表示失败。
 */
int uart_transmit(uart_desc_t uart, uint8_t *data, uint16_t len, uint32_t timeout)
{
  if(uart == NULL)
  {
    return -1;
  }

  return HAL_UART_Transmit(&uart->hal_handle, data, len, timeout) == HAL_OK ? 0 : -1;
}

/**
 * @brief   阻塞方式接收数据。
 *
 * @param[in]   uart     UART描述符。
 * @param[out]  data     接收数据指针。
 * @param[in]   len      接收长度。
 * @param[in]   timeout  超时计数。
 * @return  int 接收结果，0表示成功，-1表示失败。
 */
int uart_receive(uart_desc_t uart, uint8_t *data, uint16_t len, uint32_t timeout)
{
  if(uart == NULL)
  {
    return -1;
  }

  return HAL_UART_Receive(&uart->hal_handle, data, len, timeout) == HAL_OK ? 0 : -1;
}

/**
 * @brief   中断方式发送数据。
 *
 * @param[in]   uart  UART描述符。
 * @param[in]   data  发送数据指针。
 * @param[in]   len   发送长度。
 * @return  int 发送结果，0表示成功，-1表示失败。
 */
int uart_transmit_it(uart_desc_t uart, uint8_t *data, uint16_t len)
{
  if(uart == NULL)
  {
    return -1;
  }

  return HAL_UART_Transmit_IT(&uart->hal_handle, data, len) == HAL_OK ? 0 : -1;
}

/**
 * @brief   中断方式接收数据。
 *
 * @param[in]   uart  UART描述符。
 * @param[out]  data  接收数据指针。
 * @param[in]   len   接收长度。
 * @return  int 接收结果，0表示成功，-1表示失败。
 */
int uart_receive_it(uart_desc_t uart, uint8_t *data, uint16_t len)
{
  if(uart == NULL)
  {
    return -1;
  }

  return HAL_UART_Receive_IT(&uart->hal_handle, data, len) == HAL_OK ? 0 : -1;
}



/**
 * @brief   printf底层输出函数。
 *
 * @param[in]   character  需要输出的字符。
 * @return  None
 */
void _putchar(char character)
{
  if(uart2_rs485 != NULL)
  {
    HAL_UART_Transmit(&uart2_rs485->hal_handle, (uint8_t *)&character, 1, 0xFFFF);
  }
}


/**
 * @brief   USART1中断服务函数
 *
 * @param   None
 * @return  None
 */
void USART1_IRQHandler(void)
{
  // 空闲中断处理（必须在HAL_UART_IRQHandler之前）
  if(__HAL_UART_GET_FLAG(&uart1_rs232->hal_handle, UART_FLAG_IDLE) == SET)
  {
    __HAL_UART_CLEAR_IDLEFLAG(&uart1_rs232->hal_handle);

    // H7 必须处理 Cache 一致性，否则 CPU 读不到物理内存里的新数据
    SCB_InvalidateDCache_by_Addr((uint32_t *)Uart1_rx_buf, sizeof(Uart1_rx_buf));

    // 计算接收到的字节数
    volatile uint32_t recv_len = sizeof(Uart1_rx_buf) - __HAL_DMA_GET_COUNTER(uart1_rs232->hal_handle.hdmarx);

    if(recv_len > 0)
    {

    }

    // 重新启动DMA接收
    HAL_UART_Receive_DMA(&uart1_rs232->hal_handle, (uint8_t *)Uart1_rx_buf, sizeof(Uart1_rx_buf));
  }

  // 调用HAL库的中断处理函数
  HAL_UART_IRQHandler(&uart1_rs232->hal_handle);
}

