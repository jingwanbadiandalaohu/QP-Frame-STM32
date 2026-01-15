/**
 * @file    bsp_gpio.h
 * @author  Dylan
 * @date    2026-01-15
 * @brief   GPIO板级支持包头文件
 *
 * @details 定义板级GPIO引脚映射和便捷操作函数接口。
 */

#ifndef BSP_GPIO_H
#define BSP_GPIO_H

#include "bsp_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== 引脚定义（使用bsp_config.h中的配置） ==================== */

#define LED1_PORT                 BSP_LED1_PORT   /**< LED1端口 */
#define LED1_PIN                  BSP_LED1_PIN    /**< LED1引脚 */

#define Relay_PORT                BSP_RELAY_PORT  /**< 继电器端口 */
#define Relay_PIN                 BSP_RELAY_PIN   /**< 继电器引脚 */

/* ==================== 函数原型 ==================== */

/**
 * @brief   GPIO初始化
 *
 * @details 初始化LED和继电器引脚为推挽输出模式
 *
 * @param   None
 * @return  None
 */
void BSP_GPIO_Init(void);

/**
 * @brief   LED1状态翻转
 *
 * @param   None
 * @return  None
 */
void BSP_LED1_Toggle(void);

/**
 * @brief   LED1点亮（低电平有效）
 *
 * @param   None
 * @return  None
 */
void BSP_LED1_On(void);

/**
 * @brief   LED1熄灭
 *
 * @param   None
 * @return  None
 */
void BSP_LED1_Off(void);

/**
 * @brief   继电器吸合（高电平有效）
 *
 * @param   None
 * @return  None
 */
void BSP_Relay_On(void);

/**
 * @brief   继电器释放
 *
 * @param   None
 * @return  None
 */
void BSP_Relay_Off(void);

#ifdef __cplusplus
}
#endif

#endif /* BSP_GPIO_H */
