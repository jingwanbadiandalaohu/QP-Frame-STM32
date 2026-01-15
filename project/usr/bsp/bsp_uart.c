/**
 * @file bsp_uart.c
 * @brief UART 板级支持包实现
 */

#include "bsp_uart.h"

/* ==================== 句柄获取函数 ==================== */

UART_Handle_t BSP_GetUart1Handle(void)
{
  return DRV_UART_GetHandle(DRV_UART1);
}

UART_Handle_t BSP_GetUart2Handle(void)
{
  return DRV_UART_GetHandle(DRV_UART2);
}

/* ==================== 读取函数 ==================== */

int debug_uart1_read(uint8_t *out)
{
  int ret = DRV_UART_ReadByte(DRV_UART1, out);
  return (ret > 0) ? 1 : 0;
}

int debug_uart2_read(uint8_t *out)
{
  int ret = DRV_UART_ReadByte(DRV_UART2, out);
  return (ret > 0) ? 1 : 0;
}

/* ==================== 初始化函数==================== */

int BSP_UART_Init(void)
{
  UART_Config_t config;

  config.instance = DRV_UART1;
  config.baudrate = BSP_UART1_BAUDRATE;
  config.data_bits = 8;
  config.stop_bits = DRV_UART_STOPBITS_1;
  config.parity = DRV_UART_PARITY_NONE;
  if(DRV_UART_Init(&config) != DRV_OK)
  {
    return DRV_ERROR;
  }

  config.instance = DRV_UART2;
  config.baudrate = BSP_UART2_BAUDRATE;
  config.data_bits = 8;
  config.stop_bits = DRV_UART_STOPBITS_1;
  config.parity = DRV_UART_PARITY_NONE;
  if(DRV_UART_Init(&config) != DRV_OK)
  {
    return DRV_ERROR;
  }

  return DRV_OK;
}
