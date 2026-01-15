/**
 * @file bsp_config.h
 * @brief 板级硬件配置文件
 * @note 集中定义所有引脚分配、外设配置参数
 */

#ifndef BSP_CONFIG_H
#define BSP_CONFIG_H

#include "drv_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== LED 配置 ==================== */
#define BSP_LED1_PORT           drv_gpioc
#define BSP_LED1_PIN            DRV_PIN_13

/* ==================== 继电器配置 ==================== */
#define BSP_RELAY_PORT          drv_gpioe
#define BSP_RELAY_PIN           DRV_PIN_11

/* ==================== UART 配置 ==================== */
#define BSP_UART1_BAUDRATE      115200
#define BSP_UART2_BAUDRATE      115200

/* UART1 引脚配置 */
#define BSP_UART1_TX_PORT       drv_gpioa
#define BSP_UART1_TX_PIN        DRV_PIN_9
#define BSP_UART1_RX_PORT       drv_gpioa
#define BSP_UART1_RX_PIN        DRV_PIN_10

/* UART2 引脚配置 */
#define BSP_UART2_TX_PORT       drv_gpioa
#define BSP_UART2_TX_PIN        DRV_PIN_2
#define BSP_UART2_RX_PORT       drv_gpioa
#define BSP_UART2_RX_PIN        DRV_PIN_3

/* ==================== ADC 配置 ==================== */
#define BSP_ADC1_BUFFER_SIZE    64
#define BSP_ADC2_BUFFER_SIZE    64

/* ADC1 引脚配置: PB1 - ADC_CHANNEL_5 */
#define BSP_ADC1_PORT           drv_gpiob
#define BSP_ADC1_PIN            DRV_PIN_1

/* ADC2 引脚配置: PA6 - ADC_CHANNEL_3 */
#define BSP_ADC2_PORT           drv_gpioa
#define BSP_ADC2_PIN            DRV_PIN_6

/* ==================== 滤波器配置 ==================== */
#define BSP_FILTER_WINDOW_SIZE  16

#ifdef __cplusplus
}
#endif

#endif /* BSP_CONFIG_H */
