/**
 * @file    main.c
 * @author  Dylan
 * @date    2026-01-15
 * @brief   基于CMSIS-RTOS2的应用入口。
 *
 * @details 复用驱动与设备层，实现LED闪烁、串口回显和ADC采样任务。
 */

#include <stdint.h>
#include <stdio.h>

#include "cmsis_os2.h"
#include "drv_system.h"
#include "drv_uart.h"
#include "drv_adc.h"
#include "led.h"
#include "relay.h"
#include "board.h"
#include "filter.h"
#include "printf.h"

/**
 * @brief   LED闪烁任务。
 *
 * @param[in]   argument  任务参数（未使用）。
 * @return  无
 */
static void BlinkTask(void *argument);

/**
 * @brief   串口打印任务，回显接收的字节。
 *
 * @param[in]   argument  任务参数（未使用）。
 * @return  无
 */
static void PrintTask(void *argument);

/**
 * @brief   ADC采样打印任务，包含两级滤波。
 *
 * @param[in]   argument  任务参数（未使用）。
 * @return  无
 */
static void AdcPrintTask(void *argument);

/**
 * @brief   应用入口。
 *
 * @details 初始化系统外设并启动RTOS调度器。
 *
 * @param   无
 * @return  int 程序状态（正常情况下不返回）。
 */
int main(void)
{
  // 步骤1：系统初始化。
  if(DRV_System_Init() != 0)
  {
    DRV_System_ErrorHandler();
  }

  // 步骤2：外设初始化。
  led_init(led1);
  relay_init(relay1);
  relay_on(relay1);

  uart_init(debug_uart);
  uart_init(comm_uart);

  adc_init(adc1);
  adc_start_dma(adc1);

  // 步骤3：初始化RTOS内核。
  osKernelInitialize();

  // 步骤4：创建LED闪烁任务。
  const osThreadAttr_t blinkTask_attributes =
  {
    .name = "BlinkTask",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityNormal,
  };
  osThreadNew(BlinkTask, NULL, &blinkTask_attributes);

  // 步骤5：创建串口打印任务。
  const osThreadAttr_t printTask_attributes =
  {
    .name = "PrintTask",
    .stack_size = 512 * 4,
    .priority = (osPriority_t)osPriorityNormal,
  };
  osThreadNew(PrintTask, NULL, &printTask_attributes);

  // 步骤6：创建ADC打印任务（实时优先级）。
  const osThreadAttr_t adcPrintTask_attributes =
  {
    .name = "AdcPrintTask",
    .stack_size = 512 * 4,
    .priority = (osPriority_t)osPriorityRealtime,
  };
  osThreadNew(AdcPrintTask, NULL, &adcPrintTask_attributes);

  // 步骤7：启动RTOS调度器。
  osKernelStart();

  // 注意：调度器启动后不应返回到此处。
  while(1)
  {
  }
}

static void BlinkTask(void *argument)
{
  (void)argument;

  while(1)
  {
    led_toggle(led1);
    osDelay(1000);
  }
}

static void PrintTask(void *argument)
{
  char buffer[64] = {0};
  uint8_t byte = 0;
  int len = 0;

  (void)argument;

  while(1)
  {
    // 处理RS232调试串口数据。
    if(uart_receive(debug_uart, &byte, 1, 0) == 0)
    {
      len = snprintf(buffer, sizeof(buffer), "RS232_Uart1 %x\r\n", byte);
      uart_transmit(debug_uart, (uint8_t *)buffer, (uint16_t)len, 0xFFFF);
    }

    // 处理RS485通信串口数据。
    if(uart_receive(comm_uart, &byte, 1, 0) == 0)
    {
      len = snprintf(buffer, sizeof(buffer), "RS485_Uart2 %x\r\n", byte);
      uart_transmit(comm_uart, (uint8_t *)buffer, (uint16_t)len, 0xFFFF);
    }

    osDelay(1);
  }
}

/**
 * @brief ADC通道1移动平均滤波句柄。
 */
static MAF_Handle_t s_adc_filter_1;

/**
 * @brief ADC通道1加权移动平均滤波句柄。
 */
static WMAF_Handle_t s_adc_filter_2;

static void AdcPrintTask(void *argument)
{
  uint16_t adcx = 0;          /**< 一级滤波后的ADC值。 */
  uint16_t adcx2 = 0;         /**< 二级滤波后的ADC值。 */
  uint16_t *adc_buffer = NULL;

  (void)argument;

  while(1)
  {
    // 获取DMA缓冲区数据。
    adc_buffer = adc_get_dma_buffer(adc1);
    if(adc_buffer == NULL || adc_get_dma_length(adc1) == 0)
    {
      continue;
    }

    // 两级滤波处理：MAF -> WMAF。
    adcx = MAF_Update(&s_adc_filter_1, adc_buffer[0]);
    adcx2 = WMAF_Update(&s_adc_filter_2, adcx);

    printf("%d, %d, %d\n", adc_buffer[0], adcx, adcx2);
  }
}
