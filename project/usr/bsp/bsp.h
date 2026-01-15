/**
 * @file    bsp.h
 * @author  Dylan
 * @date    2026-01-15
 * @brief   板级支持包主头文件
 *
 * @details 统一包含所有BSP模块头文件，提供板级初始化入口。
 *          应用层只需包含此文件即可访问所有板级功能。
 */

#ifndef BSP_H
#define BSP_H

/* ==================== 驱动层接口 ==================== */
#include "drv_gpio.h"
#include "drv_uart.h"
#include "drv_adc.h"
#include "drv_system.h"

/* ==================== 板级配置 ==================== */
#include "bsp_config.h"

/* ==================== 板级驱动层 ==================== */
#include "bsp_uart.h"
#include "bsp_adc.h"
#include "bsp_gpio.h"

/* ==================== 平台无关模块 ==================== */
#include "filter.h"

/* ==================== 中间件 ==================== */
#include "cmsis_os2.h"
#include "printf.h"

/* ==================== C语言库 ==================== */
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   板级初始化入口
 *
 * @details 按顺序初始化系统时钟、串口、GPIO、ADC等外设。
 *          初始化失败时调用错误处理函数。
 *
 * @param   None
 * @return  None
 */
void BSP_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* BSP_H */
