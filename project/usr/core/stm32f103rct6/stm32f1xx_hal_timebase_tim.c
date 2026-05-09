/**
 * @file    stm32f1xx_hal_timebase_tim.c
 * @author  Dylan
 * @date    2026-05-09
 * @brief   基于硬件定时器TIM4的HAL时基实现
 */

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_tim.h"

TIM_HandleTypeDef htim4;

/**
 * @brief  配置TIM4作为HAL时基源
 *         该函数会被HAL_Init()自动调用
 * @param  TickPriority: 中断优先级
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
  RCC_ClkInitTypeDef clkconfig;
  uint32_t uwTimclock, uwAPB1Prescaler;
  uint32_t uwPrescalerValue;
  uint32_t pFLatency;

  // 配置TIM4中断优先级
  if(TickPriority < (1UL << __NVIC_PRIO_BITS))
  {
    HAL_NVIC_SetPriority(TIM4_IRQn, TickPriority, 0);
    HAL_NVIC_EnableIRQ(TIM4_IRQn);
    uwTickPrio = TickPriority;
  }
  else
  {
    return HAL_ERROR;
  }

  // 使能TIM4时钟
  __HAL_RCC_TIM4_CLK_ENABLE();

  // 获取时钟配置
  HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);
  uwAPB1Prescaler = clkconfig.APB1CLKDivider;

  // 计算TIM4输入时钟
  // F1系列中，如果APB1预分频不为1，则定时器时钟为PCLK1的2倍
  if(uwAPB1Prescaler == RCC_HCLK_DIV1)
  {
    uwTimclock = HAL_RCC_GetPCLK1Freq();
  }
  else
  {
    uwTimclock = 2UL * HAL_RCC_GetPCLK1Freq();
  }

  // 计算预分频值，使计数频率为1MHz
  uwPrescalerValue = (uint32_t)((uwTimclock / 1000000U) - 1U);

  // 初始化TIM4
  htim4.Instance = TIM4;
  htim4.Init.Period = (1000000U / 1000U) - 1U; // 1ms周期
  htim4.Init.Prescaler = uwPrescalerValue;
  htim4.Init.ClockDivision = 0;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;

  if(HAL_TIM_Base_Init(&htim4) == HAL_OK)
  {
    return HAL_TIM_Base_Start_IT(&htim4);
  }

  return HAL_ERROR;
}

/**
 * @brief  定时器周期到达回调（非阻塞模式）
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim->Instance == TIM4)
  {
    HAL_IncTick();
  }
}

/**
 * @brief  暂停Tick累加
 */
void HAL_SuspendTick(void)
{
  __HAL_TIM_DISABLE_IT(&htim4, TIM_IT_UPDATE);
}

/**
 * @brief  恢复Tick累加
 */
void HAL_ResumeTick(void)
{
  __HAL_TIM_ENABLE_IT(&htim4, TIM_IT_UPDATE);
}
