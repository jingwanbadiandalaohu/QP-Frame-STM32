/**
 * @file    drv_system.c
 * @author  Dylan
 * @date    2026-05-08
 * @brief   STM32F103RCT6系统初始化驱动
 */

#include "drv_system.h"
#include "stm32f1xx_hal.h"

/**
 * @brief  系统时钟配置 (72MHz)
 *         使用 8MHz 外部晶振 (HSE)，通过 PLL 倍频到 72MHz
 * @param  None
 * @return None
 */
static void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** 初始化 RCC 振荡器 */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    DRV_System_ErrorHandler();
  }

  /** 初始化 CPU, AHB 和 APB 总线时钟 */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    DRV_System_ErrorHandler();
  }
}

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

  // 配置系统时钟（72MHz）
  SystemClock_Config();

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
