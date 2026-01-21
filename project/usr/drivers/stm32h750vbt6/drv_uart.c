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

// UART1硬件资源定义。
#define UART1_INSTANCE USART1
#define UART1_GPIO_PORT GPIOA
#define UART1_GPIO_TX_PIN GPIO_PIN_9
#define UART1_GPIO_RX_PIN GPIO_PIN_10
#define UART1_GPIO_AF GPIO_AF7_USART1

// UART2硬件资源定义。
#define UART2_INSTANCE USART2
#define UART2_GPIO_PORT GPIOA
#define UART2_GPIO_TX_PIN GPIO_PIN_2
#define UART2_GPIO_RX_PIN GPIO_PIN_3
#define UART2_GPIO_AF GPIO_AF7_USART2

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

  if(huart->Instance == UART1_INSTANCE)
  {
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin = UART1_GPIO_TX_PIN | UART1_GPIO_RX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = UART1_GPIO_AF;
    HAL_GPIO_Init(UART1_GPIO_PORT, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  }
  else if(huart->Instance == UART2_INSTANCE)
  {
    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin = UART2_GPIO_TX_PIN | UART2_GPIO_RX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = UART2_GPIO_AF;
    HAL_GPIO_Init(UART2_GPIO_PORT, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(USART2_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  }
}

/**
 * @brief   UART底层反初始化。
 *
 * @param[in]   huart  UART句柄。
 * @return  无
 */
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
  if(huart == NULL)
  {
    return;
  }

  if(huart->Instance == UART1_INSTANCE)
  {
    __HAL_RCC_USART1_CLK_DISABLE();
    HAL_GPIO_DeInit(UART1_GPIO_PORT, UART1_GPIO_TX_PIN | UART1_GPIO_RX_PIN);
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  }
  else if(huart->Instance == UART2_INSTANCE)
  {
    __HAL_RCC_USART2_CLK_DISABLE();
    HAL_GPIO_DeInit(UART2_GPIO_PORT, UART2_GPIO_TX_PIN | UART2_GPIO_RX_PIN);
    HAL_NVIC_DisableIRQ(USART2_IRQn);
  }
}

/**
 * @brief   初始化UART。
 *
 * @param[in]   uart  UART描述符。
 * @return  无
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

  HAL_UART_Init(&uart->hal_handle);
}

/**
 * @brief   反初始化UART。
 *
 * @param[in]   uart  UART描述符。
 * @return  无
 */
void uart_deinit(uart_desc_t uart)
{
  if(uart == NULL)
  {
    return;
  }

  HAL_UART_DeInit(&uart->hal_handle);
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
 * @return  无
 */
void _putchar(char character)
{
  if(uart2_rs485 != NULL)
  {
    HAL_UART_Transmit(&uart2_rs485->hal_handle, (uint8_t *)&character, 1, 0xFFFF);
  }
}
