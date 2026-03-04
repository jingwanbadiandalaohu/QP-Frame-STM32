/**
 * @file    app_sysview.c
 * @author  Dylan
 * @date    2026-03-03
 * @brief   SystemView 运行时配置
 *
 * @details 提供 SEGGER_SYSVIEW_Conf()，将 FreeRTOS 跟踪接口接入 SystemView。
 */

#include "FreeRTOS.h"
#include "SEGGER_SYSVIEW.h"

/**
 * @brief FreeRTOS 到 SystemView 的跟踪 API
 */
extern const SEGGER_SYSVIEW_OS_API SYSVIEW_X_OS_TraceAPI;

#define SYSVIEW_APP_NAME        "Surface_Controller_V2"
#define SYSVIEW_DEVICE_NAME     "STM32H750VBT6"
#define SYSVIEW_TIMESTAMP_FREQ  (configCPU_CLOCK_HZ)
#define SYSVIEW_CPU_FREQ        (configCPU_CLOCK_HZ)
#define SYSVIEW_RAM_BASE        (0x20000000UL)

/**
 * @brief   发送 SystemView 系统描述字符串
 *
 * @param   None
 * @return  None
 */
static void AppSystemView_SendSystemDesc(void)
{
  SEGGER_SYSVIEW_SendSysDesc("N=" SYSVIEW_APP_NAME ",D=" SYSVIEW_DEVICE_NAME ",O=FreeRTOS");
  SEGGER_SYSVIEW_SendSysDesc("I#15=SysTick");
}

/**
 * @brief   初始化 SystemView 配置
 *
 * @param   None
 * @return  None
 */
void SEGGER_SYSVIEW_Conf(void)
{
  SEGGER_SYSVIEW_Init(
    SYSVIEW_TIMESTAMP_FREQ,
    SYSVIEW_CPU_FREQ,
    &SYSVIEW_X_OS_TraceAPI,
    AppSystemView_SendSystemDesc
  );

  SEGGER_SYSVIEW_SetRAMBase(SYSVIEW_RAM_BASE);
}
