/**
 * @file    drv_system_impl.c
 * @author  Dylan
 * @date    2026-01-15
 * @brief   STM32H7平台系统驱动实现
 *
 * @details 实现系统初始化，包括HAL初始化和时钟配置。
 *          - 系统时钟: HSE 25MHz → PLL1 → 480MHz
 *          - ADC时钟: HSE 25MHz → PLL2 → 50MHz
 */

#include "drv_system.h"
#include "platform_config.h"

/* ==================== 私有函数 ==================== */

/**
 * @brief   系统时钟配置
 *
 * @details 配置系统时钟为480MHz：
 *          - HSE: 25MHz外部晶振
 *          - PLL1: 系统主时钟 480MHz
 *          - PLL2: ADC时钟 50MHz
 *          - AHB/APB分频配置
 *
 * @param   None
 * @return  DRV_OK成功，DRV_ERROR失败
 */
static int DRV_SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /* 供电与电压等级配置 */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  /* 等待电压调节器就绪 */
  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY))
  {
  }

  /* HSE + PLL1：系统主时钟配置 */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    return DRV_ERROR;
  }

  /* 配置总线分频与系统时钟源 */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 |
                                RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    return DRV_ERROR;
  }

  /* ADC 外设时钟由 PLL2 提供 */
  /* ADC Clock Configuration HSE 25MHz / PLL2M 5 = 5MHz
   * ADC kernel clock = 5MHz * PLL2N 40 = 200MHz / PLL2P/Q/R 4 = 50MHz
   */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInitStruct.PLL2.PLL2M = 5;
  PeriphClkInitStruct.PLL2.PLL2N = 40;
  PeriphClkInitStruct.PLL2.PLL2P = 4;
  PeriphClkInitStruct.PLL2.PLL2Q = 4;
  PeriphClkInitStruct.PLL2.PLL2R = 4;
  PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_2;
  PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
  PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
  PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_PLL2;
  if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    return DRV_ERROR;
  }

  return DRV_OK;
}

/* ==================== 公共接口函数 ==================== */

/**
 * @brief   系统初始化
 *
 * @details 执行HAL底层初始化和系统时钟配置
 *
 * @param   None
 * @return  DRV_OK成功，DRV_ERROR失败
 */
int DRV_System_Init(void)
{
  /* HAL 底层初始化：中断优先级、Systick 等 */
  if(HAL_Init() != HAL_OK)
  {
    return DRV_ERROR;
  }

  /* 系统时钟配置 */
  return DRV_SystemClock_Config();
}

/**
 * @brief   系统错误处理
 *
 * @details 关闭中断并进入无限循环，用于不可恢复的错误
 *
 * @param   None
 * @return  None（不返回）
 *
 * @warning 此函数不会返回
 */
void DRV_System_ErrorHandler(void)
{
  /* 发生致命错误后关闭中断并停机等待 */
  __disable_irq();
  while(1)
  {
  }
}
