#ifndef BSP_GPIO_H
#define BSP_GPIO_H

#include "stm32h7xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

/* LED1: PC13 */
#define LED1_PIN                  GPIO_PIN_13
#define LED1_PORT                 GPIOC

/* 继电器: PE11 */
#define Relay_PIN                  GPIO_PIN_11
#define Relay_PORT                 GPIOE

/* ==================== [Global Prototypes] ================================= */

void MX_GPIO_Init(void);



#ifdef __cplusplus
}
#endif

#endif /* BSP_GPIO_H */
