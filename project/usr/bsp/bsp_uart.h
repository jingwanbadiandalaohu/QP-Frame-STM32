/**
 * @file    bsp_uart.h
 * @author  Dylan
 * @date    2026-01-15
 * @brief   UART板级支持包头文件
 *
 * @details 定义UART初始化和调试读取函数接口。
 *          UART1用于RS232通信，UART2用于RS485通信。
 */

#ifndef BSP_UART_H
#define BSP_UART_H

#include "bsp_config.h"
#include "drv_uart.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== 函数原型 ==================== */

/**
 * @brief   UART初始化
 *
 * @details 初始化UART1和UART2，配置波特率、数据位等参数
 *
 * @param   None
 * @return  DRV_OK成功，其他失败
 */
int BSP_UART_Init(void);

/**
 * @brief   从UART1读取单字节
 *
 * @param[out] out  读取字节存储指针
 *
 * @return  1有数据，0无数据
 */
int debug_uart1_read(uint8_t *out);

/**
 * @brief   从UART2读取单字节
 *
 * @param[out] out  读取字节存储指针
 *
 * @return  1有数据，0无数据
 */
int debug_uart2_read(uint8_t *out);

/**
 * @brief   获取UART1设备句柄
 *
 * @param   None
 * @return  UART1设备指针
 */
UART_Device_t *BSP_GetUart1Handle(void);

/**
 * @brief   获取UART2设备句柄
 *
 * @param   None
 * @return  UART2设备指针
 */
UART_Device_t *BSP_GetUart2Handle(void);

#ifdef __cplusplus
}
#endif

#endif /* BSP_UART_H */

