/**
 * @file    drv_uart.c
 * @author  Dylan
 * @date    2026-01-15
 * @brief   串口驱动实现
 *
 * @details 提供STM32H750VBT6的UART初始化、收发接口与底层初始化配置。
 *          支持阻塞/中断/DMA三种传输模式，以及DMA+IDLE中断的不定长接收。
 *          
 *          UART配置参数：
 *          - 波特率：9600 (可配置)
 *          - 数据位：8位
 *          - 停止位：1位
 *          - 校验位：无
 *          - 流控：无
 *          - 过采样：16倍
 *          
 *          硬件配置：
 *          - UART1 (RS232): PA9(TX), PA10(RX) → DMA1_Stream3 (接收)
 *          - UART2 (RS485): PA2(TX), PA3(RX)  → DMA1_Stream4 (接收)
 *          
 *          DMA+IDLE+环形缓冲区接收机制：
 *          - DMA工作在循环模式，持续接收数据到临时缓冲区
 *          - IDLE中断触发时，将DMA缓冲区数据写入环形缓冲区
 *          - 应用层通过uart_read_ringbuf从环形缓冲区读取数据
 *          - 环形缓冲区避免数据丢失，支持连续高速接收
 *          
 * @note    UART DMA缓冲区需确保32字节对齐
 * @note    printf输出通过UART2实现，需实现_putchar函数
 * @warning 不同UART必须使用不同的DMA Stream，避免冲突
 */

#include "drv_uart.h"
#include "drv_uart_desc.h"
#include "board.h"
#include "ringbuffer.h"
#include <string.h>
#include <stdbool.h>

/**
 * @brief   初始化UART
 *
 * @param[in]   uart            UART描述符
 * @param[in]   ringbuf_storage 环形缓冲区存储空间指针
 * @param[in]   ringbuf_size    环形缓冲区大小
 *
 * @return  None
 */
void uart_init(uart_desc_t uart, uint8_t *ringbuf_storage, uint32_t ringbuf_size)
{
  if(uart == NULL || ringbuf_storage == NULL || ringbuf_size == 0)
  {
    return;
  }

  // 初始化环形缓冲区
  RingBuffer_Init(&uart->rx_ringbuf, ringbuf_storage, ringbuf_size);

  uart->hal_handle.Instance = uart->instance;
  uart->hal_handle.Init.BaudRate = uart->baudrate;
  uart->hal_handle.Init.WordLength = UART_WORDLENGTH_8B;
  uart->hal_handle.Init.StopBits = UART_STOPBITS_1;
  uart->hal_handle.Init.Parity = UART_PARITY_NONE;
  uart->hal_handle.Init.Mode = UART_MODE_TX_RX;
  uart->hal_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  uart->hal_handle.Init.OverSampling = UART_OVERSAMPLING_16;

  // HAL_UART_Init会调用HAL_UART_MspInit初始化DMA
  if(HAL_UART_Init(&uart->hal_handle) != HAL_OK)
  {
    return;
  }

  // DMA初始化完成后，再启动接收和使能IDLE中断
  if(uart->instance == USART1)
  {
    // 清除可能存在的IDLE标志
    __HAL_UART_CLEAR_IDLEFLAG(&uart->hal_handle);
    
    // 使能空闲中断
    __HAL_UART_ENABLE_IT(&uart->hal_handle, UART_IT_IDLE);

    // 启动DMA循环接收
    HAL_UART_Receive_DMA(&uart->hal_handle, Uart1_dma_rx_buf, sizeof(Uart1_dma_rx_buf));
  }
  else if(uart->instance == USART2)
  {
    // 清除可能存在的IDLE标志
    __HAL_UART_CLEAR_IDLEFLAG(&uart->hal_handle);
    
    // 使能空闲中断
    __HAL_UART_ENABLE_IT(&uart->hal_handle, UART_IT_IDLE);

    // 启动DMA循环接收
    HAL_UART_Receive_DMA(&uart->hal_handle, Uart2_dma_rx_buf, sizeof(Uart2_dma_rx_buf));
  }
}

/**
 * @brief   UART底层初始化
 *
 * @param[in]   huart  UART句柄
 *
 * @return  None
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
    static DMA_HandleTypeDef hdma_usart1_tx;

    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();


    GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 配置DMA接收,Uart1 - DMA1 Stream3
    hdma_usart1_rx.Instance = DMA1_Stream3;
    hdma_usart1_rx.Init.Request = DMA_REQUEST_USART1_RX;
    hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_rx.Init.Mode = DMA_CIRCULAR;
    hdma_usart1_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    HAL_DMA_Init(&hdma_usart1_rx);

    __HAL_LINKDMA(huart, hdmarx, hdma_usart1_rx);

    // 配置DMA发送,Uart1 - DMA1 Stream5
    hdma_usart1_tx.Instance = DMA1_Stream5;
    hdma_usart1_tx.Init.Request = DMA_REQUEST_USART1_TX;
    hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_tx.Init.Mode = DMA_NORMAL;
    hdma_usart1_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    HAL_DMA_Init(&hdma_usart1_tx);

    __HAL_LINKDMA(huart, hdmatx, hdma_usart1_tx);

    // 使能UART中断（用于IDLE中断接收）
    HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  }
  else if(huart->Instance == USART2)
  {
    static DMA_HandleTypeDef hdma_usart2_rx;
    static DMA_HandleTypeDef hdma_usart2_tx;

    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 配置DMA接收,Uart2 - DMA1 Stream4
    hdma_usart2_rx.Instance = DMA1_Stream4;
    hdma_usart2_rx.Init.Request = DMA_REQUEST_USART2_RX;
    hdma_usart2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart2_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart2_rx.Init.Mode = DMA_CIRCULAR;
    hdma_usart2_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart2_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    HAL_DMA_Init(&hdma_usart2_rx);

    __HAL_LINKDMA(huart, hdmarx, hdma_usart2_rx);

    // 配置DMA发送,Uart2 - DMA1 Stream6
    hdma_usart2_tx.Instance = DMA1_Stream6;
    hdma_usart2_tx.Init.Request = DMA_REQUEST_USART2_TX;
    hdma_usart2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart2_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart2_tx.Init.Mode = DMA_NORMAL;
    hdma_usart2_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart2_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    HAL_DMA_Init(&hdma_usart2_tx);

    __HAL_LINKDMA(huart, hdmatx, hdma_usart2_tx);

    // 使能UART中断（用于IDLE中断接收）
    HAL_NVIC_SetPriority(USART2_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  }
}



/**
 * @brief   阻塞方式发送数据
 *
 * @param[in]   uart     UART描述符
 * @param[in]   data     发送数据指针
 * @param[in]   len      发送长度
 * @param[in]   timeout  超时计数
 *
 * @retval  0   成功
 * @retval  -1  失败
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
 * @brief   阻塞方式接收数据
 *
 * @param[in]   uart     UART描述符
 * @param[out]  data     接收数据指针
 * @param[in]   len      接收长度
 * @param[in]   timeout  超时计数
 *
 * @retval  0   成功
 * @retval  -1  失败
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
 * @brief   中断方式发送数据
 *
 * @param[in]   uart  UART描述符
 * @param[in]   data  发送数据指针
 * @param[in]   len   发送长度
 *
 * @retval  0   成功
 * @retval  -1  失败
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
 * @brief   中断方式接收数据
 *
 * @param[in]   uart  UART描述符
 * @param[out]  data  接收数据指针
 * @param[in]   len   接收长度
 *
 * @retval  0   成功
 * @retval  -1  失败
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
 * @brief   DMA方式发送数据（非阻塞）
 *
 * @param[in]   uart  UART描述符
 * @param[in]   data  发送数据指针
 * @param[in]   len   发送长度
 *
 * @retval  0   成功
 * @retval  -1  失败（DMA忙或参数错误）
 */
int uart_transmit_dma(uart_desc_t uart, uint8_t *data, uint16_t len)
{
  if(uart == NULL || data == NULL || len == 0)
  {
    return -1;
  }

  return HAL_UART_Transmit_DMA(&uart->hal_handle, data, len) == HAL_OK ? 0 : -1;
}

/**
 * @brief   检查UART发送是否空闲
 *
 * @param[in]   uart  UART描述符
 *
 * @retval  true   发送空闲（可以启动新的发送）
 * @retval  false  发送忙（DMA正在发送）
 */
bool uart_is_tx_idle(uart_desc_t uart)
{
  if(uart == NULL)
  {
    return true;
  }

  // 检查UART状态：只要不是正在发送就认为空闲
  HAL_UART_StateTypeDef state = HAL_UART_GetState(&uart->hal_handle);
  
  // HAL_UART_STATE_READY: 完全空闲
  // HAL_UART_STATE_BUSY_RX: 只在接收，发送空闲
  // HAL_UART_STATE_BUSY_TX: 正在发送，不空闲
  // HAL_UART_STATE_BUSY_TX_RX: 收发都忙，不空闲
  return (state != HAL_UART_STATE_BUSY_TX) && (state != HAL_UART_STATE_BUSY_TX_RX);
}

/**
 * @brief   从环形缓冲区读取数据（非阻塞）
 *
 * @param[in]   uart  UART描述符
 * @param[out]  data  接收数据指针
 * @param[in]   len   期望读取的长度
 *
 * @return  实际读取的字节数
 */
uint32_t uart_read_ringbuf(uart_desc_t uart, uint8_t *data, uint32_t len)
{
  if(uart == NULL || data == NULL || len == 0)
  {
    return 0;
  }

  return RingBuffer_Read(&uart->rx_ringbuf, data, len);
}

/**
 * @brief   获取环形缓冲区可用数据长度
 *
 * @param[in]   uart  UART描述符
 *
 * @return  可读取的字节数
 */
uint32_t uart_get_available(uart_desc_t uart)
{
  if(uart == NULL)
  {
    return 0;
  }

  return RingBuffer_GetAvailable(&uart->rx_ringbuf);
}

/**
 * @brief   清空接收环形缓冲区
 *
 * @param[in]   uart  UART描述符
 *
 * @return  None
 */
void uart_flush_rx(uart_desc_t uart)
{
  if(uart == NULL)
  {
    return;
  }

  RingBuffer_Reset(&uart->rx_ringbuf);
}


/**
 * @brief   printf底层输出函数
 *
 * @param[in]   character  需要输出的字符
 *
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
 * @details 【生产者角色】在生产者-消费者模型中作为数据生产者
 *          
 *          工作流程：
 *          1. 检测IDLE中断（串口空闲，表示一帧数据接收完成）
 *          2. 计算DMA接收到的数据长度
 *          3. 将数据从DMA缓冲区写入环形缓冲区（生产数据）
 *          4. 重启DMA接收，准备下一帧
 *
 *          生产特点：
 *          - 运行在中断上下文（高优先级）
 *          - 快速写入，立即返回
 *          - 不关心消费者是否读取
 *          - 满了自动覆盖旧数据（覆盖模式）
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

    // 计算接收到的字节数
    uint32_t recv_len = sizeof(Uart1_dma_rx_buf) - 
                        __HAL_DMA_GET_COUNTER(uart1_rs232->hal_handle.hdmarx);

    if(recv_len > 0 && recv_len <= sizeof(Uart1_dma_rx_buf))
    {
      // 将数据写入环形缓冲区
      RingBuffer_Write(&uart1_rs232->rx_ringbuf, Uart1_dma_rx_buf, recv_len);

      // 停止DMA接收
      HAL_UART_DMAStop(&uart1_rs232->hal_handle);

      // 重新启动DMA接收
      HAL_UART_Receive_DMA(&uart1_rs232->hal_handle, Uart1_dma_rx_buf, sizeof(Uart1_dma_rx_buf));
    }
  }

  // 调用HAL库的中断处理函数
  HAL_UART_IRQHandler(&uart1_rs232->hal_handle);
}

/**
 * @brief   USART2中断服务函数
 *
 * @details 【生产者角色】在生产者-消费者模型中作为数据生产者
 *          
 *          工作流程：
 *          1. 检测IDLE中断（串口空闲，表示一帧数据接收完成）
 *          2. 计算DMA接收到的数据长度
 *          3. 将数据从DMA缓冲区写入环形缓冲区（生产数据）
 *          4. 重启DMA接收，准备下一帧
 *
 *          生产特点：
 *          - 运行在中断上下文（高优先级）
 *          - 快速写入，立即返回
 *          - 不关心消费者是否读取
 *          - 满了自动覆盖旧数据（覆盖模式）
 *
 * @param   None
 * @return  None
 */
void USART2_IRQHandler(void)
{
  // 空闲中断处理（必须在HAL_UART_IRQHandler之前）
  if(__HAL_UART_GET_FLAG(&uart2_rs485->hal_handle, UART_FLAG_IDLE) == SET)
  {
    __HAL_UART_CLEAR_IDLEFLAG(&uart2_rs485->hal_handle);

    // 计算接收到的字节数
    uint32_t recv_len = sizeof(Uart2_dma_rx_buf) - 
                        __HAL_DMA_GET_COUNTER(uart2_rs485->hal_handle.hdmarx);

    if(recv_len > 0 && recv_len <= sizeof(Uart2_dma_rx_buf))
    {
      // 将数据写入环形缓冲区
      RingBuffer_Write(&uart2_rs485->rx_ringbuf, Uart2_dma_rx_buf, recv_len);

      // 停止DMA接收
      HAL_UART_DMAStop(&uart2_rs485->hal_handle);

      // 重新启动DMA接收
      HAL_UART_Receive_DMA(&uart2_rs485->hal_handle, Uart2_dma_rx_buf, sizeof(Uart2_dma_rx_buf));
    }
  }

  // 调用HAL库的中断处理函数
  HAL_UART_IRQHandler(&uart2_rs485->hal_handle);
}

