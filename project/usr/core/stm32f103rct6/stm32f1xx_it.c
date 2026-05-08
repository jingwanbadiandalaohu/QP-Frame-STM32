/**
 * @file    stm32f1xx_it.c
 * @author  Dylan
 * @date    2026-05-08
 * @brief   STM32F103RCT6中断处理函数实现
 */

#include "stm32f1xx_it.h"
#include "stm32f1xx.h"

/**
 * @brief   非屏蔽中断处理
 *
 * @param   None
 * @return  None
 */
void NMI_Handler(void)
{
  while(1)
  {
  }
}

/**
 * @brief   硬件错误中断处理
 *
 * @param   None
 * @return  None
 */
void HardFault_Handler(void)
{
  while(1)
  {
  }
}

/**
 * @brief   内存管理错误中断处理
 *
 * @param   None
 * @return  None
 */
void MemManage_Handler(void)
{
  while(1)
  {
  }
}

/**
 * @brief   总线错误中断处理
 *
 * @param   None
 * @return  None
 */
void BusFault_Handler(void)
{
  while(1)
  {
  }
}

/**
 * @brief   用法错误中断处理
 *
 * @param   None
 * @return  None
 */
void UsageFault_Handler(void)
{
  while(1)
  {
  }
}

// NOTE: SVC_Handler 和 PendSV_Handler 由 FreeRTOS 提供，不在此定义
// 参考 FreeRTOSConfig.h 中的映射：
//   #define vPortSVCHandler SVC_Handler
//   #define xPortPendSVHandler PendSV_Handler

/**
 * @brief   调试监视器中断处理
 *
 * @param   None
 * @return  None
 */
void DebugMon_Handler(void)
{
}

/**
 * @brief   系统滴答定时器中断处理
 *
 * @param   None
 * @return  None
 */
void SysTick_Handler(void)
{
  HAL_IncTick();
}
