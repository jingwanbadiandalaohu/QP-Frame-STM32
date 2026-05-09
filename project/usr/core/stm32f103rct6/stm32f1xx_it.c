/**
 * @file    stm32f1xx_it.c
 * @author  Dylan
 * @date    2026-05-08
 * @brief   STM32F103RCT6中断处理函数实现
 */

#include "stm32f1xx_it.h"
#include "stm32f1xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stm32f1xx_hal_tim.h"

extern TIM_HandleTypeDef htim4;

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

extern void xPortSysTickHandler(void);

/**
 * @brief   系统滴答定时器中断处理
 *
 * @param   None
 * @return  None
 */
void SysTick_Handler(void)
{
  // traceISR_ENTER(); // SystemView 记录中断进入

  // 确保FreeRTOS调度器启动后才调用其Tick处理函数
  if(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
  {
    xPortSysTickHandler();
  }

  // traceISR_EXIT();  // SystemView 记录中断退出
}

/**
 * @brief   TIM4全局中断处理
 */
void TIM4_IRQHandler(void)
{
  // traceISR_ENTER(); // SystemView 记录中断进入
  HAL_TIM_IRQHandler(&htim4);
  // traceISR_EXIT();  // SystemView 记录中断退出
}
