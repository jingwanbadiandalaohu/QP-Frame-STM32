/**
 * @file    drv_system.c
 * @author  Dylan
 * @date    2026-01-15
 * @brief   系统初始化与时钟配置。
 *
 * @details 提供系统时钟与HAL基础初始化流程。
 */

#include "drv_system.h"
#include "stm32h7xx_hal.h"

/**
 * @brief   配置MPU（防止推测性访问）
 *
 * @details Cortex-M7推测性访问保护配置：
 *          - 配置整个4GB地址空间，通过SubRegionDisable选择性保护
 *          - SubRegionDisable = 0x87 (二进制: 10000111)
 *            * 4GB分为8个子区域，每个512MB
 *            * 位为1表示禁用该子区域的保护规则
 *          
 *          子区域划分：
 *          - 子区域0 (0x00000000-0x1FFFFFFF): 禁用保护，Flash/ITCM/DTCM区域
 *          - 子区域1 (0x20000000-0x3FFFFFFF): 禁用保护，SRAM/AHB SRAM区域
 *          - 子区域2 (0x40000000-0x5FFFFFFF): 禁用保护，外设区域
 *          - 子区域3 (0x60000000-0x7FFFFFFF): 启用保护，未使用区域
 *          - 子区域4 (0x80000000-0x9FFFFFFF): 启用保护，未使用区域
 *          - 子区域5 (0xA0000000-0xBFFFFFFF): 启用保护，未使用区域
 *          - 子区域6 (0xC0000000-0xDFFFFFFF): 启用保护，外部存储器区域（未使用）
 *          - 子区域7 (0xE0000000-0xFFFFFFFF): 禁用保护，系统区域
 *          
 *          保护策略：
 *          - 未使用的区域设为NO_ACCESS，防止推测性访问导致硬件错误
 *          - 已使用的区域禁用MPU保护，使用默认内存映射
 *
 * @param   None
 * @return  None
 *
 * @note    参考: STM32CubeMX生成的默认MPU配置
 */
static void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  // 禁用MPU
  HAL_MPU_Disable();

  // 配置整个4GB地址空间，通过SubRegionDisable选择性保护未使用区域
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;  // 禁用子区域0,1,2,7的保护
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;  // 未禁用的子区域禁止访问
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  // 使能MPU，使用默认内存映射作为背景区域
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

/**
 * @brief   配置系统时钟
 *
 * @return  int 配置结果，0表示成功，-1表示失败
 */
static int DRV_SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  // 电源与电压缩放配置
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY))
  {
  }

  // 配置主PLL时钟源
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
    return -1;
  }

  // 配置系统与总线时钟分频
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
    return -1;
  }

  // 配置ADC外设时钟
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
    return -1;
  }

  return 0;
}

/**
 * @brief   系统初始化
 *
 * @return  int 初始化结果，0表示成功，-1表示失败
 *
 * @note    不使用Cache时，MPU配置可选
 */
int DRV_System_Init(void)
{
  // Step 1: 配置MPU（不使用Cache时可选）
  MPU_Config();

  // Step 2: HAL初始化
  if(HAL_Init() != HAL_OK)
  {
    return -1;
  }

  // Step 3: 配置系统时钟
  return DRV_SystemClock_Config();
}

/**
 * @brief   系统错误处理
 *
 * @return  None
 */
void DRV_System_ErrorHandler(void)
{
  __disable_irq();
  while(1)
  {
  }
}
