/**
 * @file    bsp_uart.c
 * @author  Dylan
 * @date    2026-01-15
 * @brief   UART板级支持包实现
 *
 * @details 实现UART1(RS232)和UART2(RS485)的初始化及数据读取。
 */

#include "bsp_uart.h"

/* ==================== 句柄获取函数 ==================== */

/**
 * @brief   获取UART1设备句柄
 *
 * @param   None
 * @return  UART1设备指针
 */
UART_Device_t *BSP_GetUart1Handle(void)
{
  return drv_uart1;
}

/**
 * @brief   获取UART2设备句柄
 *
 * @param   None
 * @return  UART2设备指针
 */
UART_Device_t *BSP_GetUart2Handle(void)
{
  return drv_uart2;
}

/* ==================== 读取函数 ==================== */

/**
 * @brief   从UART1接收缓冲区读取单字节
 *
 * @param[out] out  读取字节存储指针
 *
 * @return  1有数据，0无数据
 */
int debug_uart1_read(uint8_t *out)
{
  int ret = uart_read_byte(drv_uart1, out);
  return (ret > 0) ? 1 : 0;
}

/**
 * @brief   从UART2接收缓冲区读取单字节
 *
 * @param[out] out  读取字节存储指针
 *
 * @return  1有数据，0无数据
 */
int debug_uart2_read(uint8_t *out)
{
  int ret = uart_read_byte(drv_uart2, out);
  return (ret > 0) ? 1 : 0;
}

/* ==================== 初始化函数 ==================== */

/**
 * @brief   UART初始化
 *
 * @details 初始化UART1和UART2，配置为115200波特率、8数据位、
 *          1停止位、无校验。
 *
 * @param   None
 * @return  DRV_OK成功，DRV_ERROR失败
 */
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
