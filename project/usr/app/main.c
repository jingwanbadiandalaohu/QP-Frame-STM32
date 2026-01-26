/**
 * @file    main.c
 * @author  Dylan
 * @date    2026-01-15
 * @brief   基于CMSIS-RTOS2的应用入口。
 *
 * @details 复用驱动与设备层，实现LED闪烁、串口回显和ADC采样任务。
 */

// c语言标准库
#include <stdint.h>
#include <stdio.h>

// 中间层
#include "cmsis_os2.h"
#include "printf.h"

// 组件
#include "filter.h"

// 设备层
#include "led.h"
#include "relay.h"

// 驱动层
#include "drv_system.h"
#include "drv_uart.h"
#include "drv_adc.h"
#include "board.h"


// LED闪烁任务。
static void BlinkTask(void *argument);
// // ADC采样打印任务，包含两级滤波。
// static void AdcPrintTask(void *argument);


int main(void){
  // 系统初始化。
  if(DRV_System_Init() != 0)
  {
    DRV_System_ErrorHandler();
  }

  // 外设初始化。
  led_init(led1);
  relay_init(relay1);
  relay_on(relay1);

  // 初始化串口。
  uart_init(uart2_rs485);
  uart_init(uart1_rs232);

  // 初始化ADC。
  adc_init(adc1);
  adc_init(adc2);
  adc_start_dma(adc1);
  adc_start_dma(adc2);
  
  // 初始化RTOS内核。
  osKernelInitialize();

  // 创建LED闪烁任务。
  const osThreadAttr_t blinkTask_attributes =
  {
    .name = "BlinkTask",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityNormal,
  };
  osThreadNew(BlinkTask, NULL, &blinkTask_attributes);


  // 创建ADC打印任务（实时优先级）。
  // const osThreadAttr_t adcPrintTask_attributes =
  // {
  //   .name = "AdcPrintTask",
  //   .stack_size = 512 * 4,
  //   .priority = (osPriority_t)osPriorityRealtime,
  // };
  // osThreadNew(AdcPrintTask, NULL, &adcPrintTask_attributes);

  // 启动RTOS调度器。
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
    osDelay(500);
  }
}


// // 定义ADC滤波器。
// static MAF_Handle_t s_adc_filter_1;
// static WMAF_Handle_t s_adc_filter_2;

// static void AdcPrintTask(void *argument)
// {
//   uint16_t adcx = 0;          /**< 一级滤波后的ADC值。 */
//   uint16_t adcx2 = 0;         /**< 二级滤波后的ADC值。 */
//   uint16_t *adc_buffer = NULL;

//   // uint16_t *adc2_buffer = NULL;

//   (void)argument;

//   while(1)
//   {
//      // 获取adc2的值。
//     // adc2_buffer = adc_get_dma_buffer(adc2);
//     // printf("adc2: %d\n", adc2_buffer[0]);

//     //osDelay(1000);


//     // 获取DMA缓冲区数据。
//     adc_buffer = adc_get_dma_buffer(adc1);
//     if(adc_buffer == NULL || adc_get_dma_length(adc1) == 0)
//     {
//       continue;
//     }

//     // 两级滤波处理：MAF -> WMAF。
//     adcx = MAF_Update(&s_adc_filter_1, adc_buffer[0]);
//     adcx2 = WMAF_Update(&s_adc_filter_2, adcx);

//     printf("%d, %d, %d\n", adc_buffer[0], adcx, adcx2);
//   }
// }
