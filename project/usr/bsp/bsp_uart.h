/**
 * @file bsp_uart.h
 * @brief UART 板级支持包
 */

#ifndef BSP_UART_H
#define BSP_UART_H

#include "bsp_config.h"
#include "drv_uart.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== 函数原型 ==================== */

int BSP_UART_Init(void);
int debug_uart1_read(uint8_t *out);
int debug_uart2_read(uint8_t *out);

/* 获取 UART 句柄 (供驱动层使用) */
UART_Handle_t BSP_GetUart1Handle(void);
UART_Handle_t BSP_GetUart2Handle(void);

#ifdef __cplusplus
}
#endif

#endif /* BSP_UART_H */

