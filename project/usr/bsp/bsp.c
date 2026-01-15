/**
 * @file bsp.c
 * @brief BSP 初始化
 */

#include "bsp.h"

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
