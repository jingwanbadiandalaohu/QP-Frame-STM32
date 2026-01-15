/**
 * @file bsp_uart.c
 * @brief UART 板级支持包实现
 */

#include "bsp_uart.h"

/* ==================== 句柄获取函数 ==================== */

UART_Device_t *BSP_GetUart1Handle(void)
{
  return drv_uart1;
}

UART_Device_t *BSP_GetUart2Handle(void)
{
  return drv_uart2;
}

/* ==================== 读取函数 ==================== */

int debug_uart1_read(uint8_t *out)
{
  int ret = uart_read_byte(drv_uart1, out);
  return (ret > 0) ? 1 : 0;
}

int debug_uart2_read(uint8_t *out)
{
  int ret = uart_read_byte(drv_uart2, out);
  return (ret > 0) ? 1 : 0;
}

/* ==================== 初始化函数==================== */

int BSP_UART_Init(void)
{
  DRV_UART_Config_t config;

  config.instance = DRV_UART1;
  config.baudrate = BSP_UART1_BAUDRATE;
  config.data_bits = 8;
  config.stop_bits = DRV_UART_STOPBITS_1;
  config.parity = DRV_UART_PARITY_NONE;
  if(uart_init(drv_uart1, &config) != DRV_OK)
  {
    return DRV_ERROR;
  }

  config.instance = DRV_UART2;
  config.baudrate = BSP_UART2_BAUDRATE;
  config.data_bits = 8;
  config.stop_bits = DRV_UART_STOPBITS_1;
  config.parity = DRV_UART_PARITY_NONE;
  if(uart_init(drv_uart2, &config) != DRV_OK)
  {
    return DRV_ERROR;
  }

  return DRV_OK;
}
