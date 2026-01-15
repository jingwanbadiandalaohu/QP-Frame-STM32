/**
 * @file platform_config.h
 * @brief STM32H7 平台配置文件
 * @note 定义平台特定的宏和类型映射
 */

#ifndef PLATFORM_CONFIG_H
#define PLATFORM_CONFIG_H

#define PLATFORM_STM32H7

#include "stm32h7xx_hal.h"

/* GPIO 端口映射 */
#define PLATFORM_GPIOA    GPIOA
#define PLATFORM_GPIOB    GPIOB
#define PLATFORM_GPIOC    GPIOC
#define PLATFORM_GPIOD    GPIOD
#define PLATFORM_GPIOE    GPIOE

/* GPIO 引脚映射 */
#define PLATFORM_PIN_0    GPIO_PIN_0
#define PLATFORM_PIN_1    GPIO_PIN_1
#define PLATFORM_PIN_2    GPIO_PIN_2
#define PLATFORM_PIN_3    GPIO_PIN_3
#define PLATFORM_PIN_4    GPIO_PIN_4
#define PLATFORM_PIN_5    GPIO_PIN_5
#define PLATFORM_PIN_6    GPIO_PIN_6
#define PLATFORM_PIN_7    GPIO_PIN_7
#define PLATFORM_PIN_8    GPIO_PIN_8
#define PLATFORM_PIN_9    GPIO_PIN_9
#define PLATFORM_PIN_10   GPIO_PIN_10
#define PLATFORM_PIN_11   GPIO_PIN_11
#define PLATFORM_PIN_12   GPIO_PIN_12
#define PLATFORM_PIN_13   GPIO_PIN_13
#define PLATFORM_PIN_14   GPIO_PIN_14
#define PLATFORM_PIN_15   GPIO_PIN_15

#endif /* PLATFORM_CONFIG_H */
