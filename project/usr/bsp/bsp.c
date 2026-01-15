/**
 * @file    bsp.c
 * @author  Dylan
 * @date    2026-01-15
 * @brief   BSP初始化实现
 *
 * @details 实现板级支持包的统一初始化流程，按依赖顺序
 *          初始化各外设模块。
 */

#include "bsp.h"

/**
 * @brief   板级初始化入口
 *
 * @details 初始化顺序：
 *          1. 系统驱动（HAL、时钟）
 *          2. 串口（便于后续调试输出）
 *          3. GPIO
 *          4. ADC
 *
 * @param   None
 * @return  None
 *
 * @note    任何初始化失败都会进入错误处理
 */
void BSP_Init(void)
{
  /* 先初始化系统驱动，确保时钟与核心服务就绪 */
  if(DRV_System_Init() != DRV_OK)
  {
    DRV_System_ErrorHandler();
  }

  /* 先初始化串口，便于后续模块输出日志或通信 */
  if(BSP_UART_Init() != DRV_OK)
  {
    DRV_System_ErrorHandler();
  }

  /* 初始化不返回状态的板级外设 */
  BSP_GPIO_Init();
  BSP_ADC_Init();
}
