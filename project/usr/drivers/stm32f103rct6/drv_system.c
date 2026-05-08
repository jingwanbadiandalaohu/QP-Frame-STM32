/**
 * @file    drv_system.c
 * @author  Dylan
 * @date    2026-05-08
 * @brief   STM32F103RCT6系统初始化驱动
 */

#include "drv_system.h"
#include "stm32f1xx_hal.h"

/**
 * @brief   系统初始化
 *
 * @param   None
 * @return  0: 成功, -1: 失败
 */
int DRV_System_Init(void)
{
  // 复位所有外设，初始化Flash接口和SysTick
  HAL_Init();

  // TODO: 配置系统时钟（72MHz）

  return 0;
}

/**
 * @brief   系统错误处理
 *
 * @param   None
 * @return  None
 */
void DRV_System_ErrorHandler(void)
{
  __disable_irq();
  while(1)
  {
  }
}
