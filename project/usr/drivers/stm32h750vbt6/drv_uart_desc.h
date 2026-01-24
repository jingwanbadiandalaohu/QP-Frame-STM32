#ifndef DRV_UART_DESC_H
#define DRV_UART_DESC_H

#include <stdint.h>
#include "stm32h7xx_hal.h"

struct uart_desc
{
  USART_TypeDef *instance;
  uint32_t baudrate;
  UART_HandleTypeDef hal_handle;      // 串口HAL句柄，替代UART_HandleTypeDef huart1;
};


typedef struct uart_desc *uart_desc_t;

#endif /* DRV_UART_DESC_H */
