/**
 * @file    bsp_config.h
 * @author  Dylan
 * @date    2026-01-15
 * @brief   板级硬件配置文件
 *
 * @details 集中定义所有引脚分配、外设配置参数，
 *          便于硬件移植和维护。
 */

#ifndef BSP_CONFIG_H
#define BSP_CONFIG_H

#include "drv_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== LED配置 ==================== */

#define BSP_LED1_PORT           drv_gpioc   /**< LED1端口: GPIOC */
#define BSP_LED1_PIN            DRV_PIN_13  /**< LED1引脚: PC13 */

/* ==================== 继电器配置 ==================== */

#define BSP_RELAY_PORT          drv_gpioe   /**< 继电器端口: GPIOE */
#define BSP_RELAY_PIN           DRV_PIN_11  /**< 继电器引脚: PE11 */

/* ==================== UART配置 ==================== */

#define BSP_UART1_BAUDRATE      115200      /**< UART1波特率 */
#define BSP_UART2_BAUDRATE      115200      /**< UART2波特率 */

/* UART1引脚配置 */
#define BSP_UART1_TX_PORT       drv_gpioa   /**< UART1 TX端口 */
#define BSP_UART1_TX_PIN        DRV_PIN_9   /**< UART1 TX引脚: PA9 */
#define BSP_UART1_RX_PORT       drv_gpioa   /**< UART1 RX端口 */
#define BSP_UART1_RX_PIN        DRV_PIN_10  /**< UART1 RX引脚: PA10 */

/* UART2引脚配置 */
#define BSP_UART2_TX_PORT       drv_gpioa   /**< UART2 TX端口 */
#define BSP_UART2_TX_PIN        DRV_PIN_2   /**< UART2 TX引脚: PA2 */
#define BSP_UART2_RX_PORT       drv_gpioa   /**< UART2 RX端口 */
#define BSP_UART2_RX_PIN        DRV_PIN_3   /**< UART2 RX引脚: PA3 */

/* ==================== ADC配置 ==================== */

#define BSP_ADC1_BUFFER_SIZE    64          /**< ADC1 DMA缓冲区大小 */
#define BSP_ADC2_BUFFER_SIZE    64          /**< ADC2 DMA缓冲区大小 */

/* ADC1引脚配置: PB1 - ADC_CHANNEL_5 */
#define BSP_ADC1_PORT           drv_gpiob   /**< ADC1端口 */
#define BSP_ADC1_PIN            DRV_PIN_1   /**< ADC1引脚: PB1 */

/* ADC2引脚配置: PA6 - ADC_CHANNEL_3 */
#define BSP_ADC2_PORT           drv_gpioa   /**< ADC2端口 */
#define BSP_ADC2_PIN            DRV_PIN_6   /**< ADC2引脚: PA6 */

/* ==================== 滤波器配置 ==================== */

#define BSP_FILTER_WINDOW_SIZE  16          /**< 滤波器窗口大小 */

#ifdef __cplusplus
}
#endif

#endif /* BSP_CONFIG_H */
