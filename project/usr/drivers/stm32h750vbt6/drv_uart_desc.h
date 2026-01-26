/**
 * @file    drv_uart_desc.h
 * @author  Dylan
 * @date    2026-01-15
 * @brief   串口描述符定义
 */

#ifndef DRV_UART_DESC_H
#define DRV_UART_DESC_H

#include <stdint.h>
#include "stm32h7xx_hal.h"
#include "ringbuffer.h"

/**
 * @brief 串口描述符结构体
 */
struct uart_desc
{
  USART_TypeDef *instance;            /**< 串口实例 */
  uint32_t baudrate;                  /**< 波特率 */
  UART_HandleTypeDef hal_handle;      /**< 串口HAL句柄 */
  RingBuffer_t rx_ringbuf;            /**< 接收环形缓冲区 */
};

typedef struct uart_desc *uart_desc_t;

#endif /* DRV_UART_DESC_H */
