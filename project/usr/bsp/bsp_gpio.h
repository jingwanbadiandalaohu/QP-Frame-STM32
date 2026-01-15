/**
 * @file bsp_gpio.h
 * @brief GPIO 板级支持包
 */

#ifndef BSP_GPIO_H
#define BSP_GPIO_H

#include "bsp_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== 引脚定义 (使用 bsp_config.h 中的配置) ==================== */
#define LED1_PORT                 BSP_LED1_PORT
#define LED1_PIN                  BSP_LED1_PIN

#define Relay_PORT                BSP_RELAY_PORT
#define Relay_PIN                 BSP_RELAY_PIN

/* ==================== 函数原型 ==================== */

void BSP_GPIO_Init(void);

/* 使用驱动层接口的便捷函数 */
void BSP_LED1_Toggle(void);
void BSP_LED1_On(void);
void BSP_LED1_Off(void);
void BSP_Relay_On(void);
void BSP_Relay_Off(void);

#ifdef __cplusplus
}
#endif

#endif /* BSP_GPIO_H */
