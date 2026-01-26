/**
 * @file    main.c
 * @author  Dylan
 * @date    2026-01-15
 * @brief   基于CMSIS-RTOS2的应用入口
 *
 * @details 复用驱动与设备层，实现LED闪烁、串口回显和ADC采样任务。
 *          演示环形缓冲区在串口接收中的应用。
 */

// c语言标准库
#include <stdint.h>
#include <stdio.h>
#include <string.h>

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


// LED闪烁任务
static void BlinkTask(void *argument);
// 串口接收处理任务
static void UartRxTask(void *argument);
// // ADC采样打印任务，包含两级滤波
// static void AdcPrintTask(void *argument);


int main(void)
{
  // 系统初始化
  if(DRV_System_Init() != 0)
  {
    DRV_System_ErrorHandler();
  }

  // 外设初始化
  led_init(led1);
  relay_init(relay1);
  relay_on(relay1);

  // 初始化串口（带环形缓冲区）
  uart_init(uart2_rs485, Uart2_ringbuf_storage, sizeof(Uart2_ringbuf_storage));
  uart_init(uart1_rs232, Uart1_ringbuf_storage, sizeof(Uart1_ringbuf_storage));

  // 初始化ADC
  adc_init(adc1);
  adc_init(adc2);
  adc_start_dma(adc1);
  adc_start_dma(adc2);
  
  // 初始化RTOS内核
  osKernelInitialize();

  // 创建LED闪烁任务
  const osThreadAttr_t blinkTask_attributes =
  {
    .name = "BlinkTask",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityNormal,
  };
  osThreadNew(BlinkTask, NULL, &blinkTask_attributes);

  // 创建串口接收处理任务
  const osThreadAttr_t uartRxTask_attributes =
  {
    .name = "UartRxTask",
    .stack_size = 512 * 4,
    .priority = (osPriority_t)osPriorityNormal,
  };
  osThreadNew(UartRxTask, NULL, &uartRxTask_attributes);

  // 创建ADC打印任务（实时优先级）
  // const osThreadAttr_t adcPrintTask_attributes =
  // {
  //   .name = "AdcPrintTask",
  //   .stack_size = 512 * 4,
  //   .priority = (osPriority_t)osPriorityRealtime,
  // };
  // osThreadNew(AdcPrintTask, NULL, &adcPrintTask_attributes);

  // 启动RTOS调度器
  osKernelStart();

  // 注意：调度器启动后不应返回到此处
  while(1)
  {
  }
}

/**
 * @brief   LED闪烁任务
 *
 * @param[in]   argument  任务参数（未使用）
 *
 * @return  None
 */
static void BlinkTask(void *argument)
{
  (void)argument;

  while(1)
  {
    led_toggle(led1);
    osDelay(500);
  }
}

/**
 * @brief   串口接收处理任务
 *
 * @details 演示环形缓冲区的使用：
 *          1. 检查UART是否有数据
 *          2. 从环形缓冲区读取数据
 *          3. 直接回显数据
 *
 * @param[in]   argument  任务参数（未使用）
 *
 * @return  None
 */
static void UartRxTask(void *argument)
{
  uint8_t rx_buffer[128] = {0};
  uint32_t rx_len = 0;

  (void)argument;

  while(1)
  {
    // 检查UART1环形缓冲区是否有数据
    if(uart_get_available(uart1_rs232) > 0)
    {
      // 从环形缓冲区读取数据
      rx_len = uart_read(uart1_rs232, rx_buffer, sizeof(rx_buffer));

      if(rx_len > 0)
      {
        // 通过UART1回显接收到的数据
        uart_transmit(uart1_rs232, rx_buffer, rx_len, 1000);

        // 清空缓冲区
        memset(rx_buffer, 0, sizeof(rx_buffer));
      }
    }

    // 检查UART2环形缓冲区是否有数据
    if(uart_get_available(uart2_rs485) > 0)
    {
      // 从环形缓冲区读取数据
      rx_len = uart_read(uart2_rs485, rx_buffer, sizeof(rx_buffer));

      if(rx_len > 0)
      {
        // 通过UART2回显接收到的数据
        uart_transmit(uart2_rs485, rx_buffer, rx_len, 1000);

        // 清空缓冲区
        memset(rx_buffer, 0, sizeof(rx_buffer));
      }
    }

    // 延时10ms，避免CPU占用过高
    osDelay(10);
  }
}


// // 定义ADC滤波器
// static MAF_Handle_t s_adc_filter_1;
// static WMAF_Handle_t s_adc_filter_2;

// static void AdcPrintTask(void *argument)
// {
//   uint16_t adcx = 0;          /**< 一级滤波后的ADC值 */
//   uint16_t adcx2 = 0;         /**< 二级滤波后的ADC值 */
//   uint16_t *adc_buffer = NULL;

//   // uint16_t *adc2_buffer = NULL;

//   (void)argument;

//   while(1)
//   {
//      // 获取adc2的值
//     // adc2_buffer = adc_get_dma_buffer(adc2);
//     // printf("adc2: %d\n", adc2_buffer[0]);

//     //osDelay(1000);


//     // 获取DMA缓冲区数据
//     adc_buffer = adc_get_dma_buffer(adc1);
//     if(adc_buffer == NULL || adc_get_dma_length(adc1) == 0)
//     {
//       continue;
//     }

//     // 两级滤波处理：MAF -> WMAF
//     adcx = MAF_Update(&s_adc_filter_1, adc_buffer[0]);
//     adcx2 = WMAF_Update(&s_adc_filter_2, adcx);

//     printf("%d, %d, %d\n", adc_buffer[0], adcx, adcx2);
//   }
// }
