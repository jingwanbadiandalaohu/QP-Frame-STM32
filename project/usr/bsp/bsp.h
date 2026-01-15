/**
 * @file bsp.h
 * @brief 板级支持包主头文件
 */

#ifndef BSP_H
#define BSP_H

/* 驱动层接口 */
#include "drv_gpio.h"
#include "drv_uart.h"
#include "drv_adc.h"
#include "drv_system.h"

/* 板级配置 */
#include "bsp_config.h"

/* 板级驱动层 */
#include "bsp_uart.h"
#include "bsp_adc.h"
#include "bsp_gpio.h"

/* 平台无关模块 */
#include "filter.h"

/* 中间件 */
#include "cmsis_os2.h"
#include "printf.h"

/* C语言库 */
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 板级初始化入口 */
void BSP_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* BSP_H */
