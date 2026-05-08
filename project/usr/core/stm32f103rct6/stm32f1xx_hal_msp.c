/**
 * @file    stm32f1xx_hal_msp.c
 * @author  Dylan
 * @date    2026-05-08
 * @brief   STM32F103RCT6 HAL MSP初始化
 */

#include "stm32f1xx_hal.h"

/**
 * @brief   HAL MSP初始化
 *
 * @param   None
 * @return  None
 */
void HAL_MspInit(void)
{
  // 配置中断优先级分组为4（抢占优先级4位，子优先级0位）
  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  // 使能AFIO时钟（用于GPIO重映射）
  __HAL_RCC_AFIO_CLK_ENABLE();
}
