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
#include "modbus.h"

// 驱动层
#include "drv_system.h"
#include "drv_uart.h"
#include "drv_adc.h"
#include "board.h"


// LED闪烁任务
static void BlinkTask(void *argument);
// Modbus从机任务
static void Modbus1Task(void *argument);
static void Modbus2Task(void *argument);


// // ADC采样打印任务，包含两级滤波
static void AdcPrintTask(void *argument);

// Modbus从机设备
static modbus_dev_t g_modbus_1;
static modbus_dev_t g_modbus_2;
// Modbus保持寄存器（100个）
static uint16_t g_modbus_regs[100] = {0};


int main(void)
{
  // 在系统初始化之前清零 AXI SRAM(D1)
  memset((void*)0x24000000, 0, 512 * 1024);  // 清零整个 AXI SRAM (512KB)
  
  // 清缓冲区
  memset(Uart1_dma_rx_buf, 0, sizeof(Uart1_dma_rx_buf));
  memset(Uart2_dma_rx_buf, 0, sizeof(Uart2_dma_rx_buf));

  // 系统初始化
  if(DRV_System_Init() != 0)
  {
    DRV_System_ErrorHandler();
  }

  // 外设初始化
  led_init(led1);
  relay_init(relay1);
  relay_on(relay1);

  // 初始化串口 Uart1/2_ringbuf_storage用于环形缓冲区存储
  uart_init(uart1_rs232, Uart1_ringbuf_storage, sizeof(Uart1_ringbuf_storage));
  uart_init(uart2_rs485, Uart2_ringbuf_storage, sizeof(Uart2_ringbuf_storage));

  // 初始化Modbus从机（地址145，寄存器地址100-199）
  modbus_init(&g_modbus_1, uart1_rs232, 145, g_modbus_regs, 100, 100);
  modbus_init(&g_modbus_2, uart2_rs485, 145, g_modbus_regs, 100, 100);
  
  modbus_set_byte_timeout(&g_modbus_1, 250);  //设置字节间超时
  modbus_set_byte_timeout(&g_modbus_2, 250);
  modbus_set_read_timeout(&g_modbus_1, 600);  //设置读总超时
  modbus_set_read_timeout(&g_modbus_2, 600);

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

  // 创建Modbus1从机任务
  const osThreadAttr_t modbus1Task_attributes =
  {
    .name = "Modbus1Task",
    .stack_size = 512 * 4,
    .priority = (osPriority_t)osPriorityNormal,
  };

  osThreadNew(Modbus1Task, NULL, &modbus1Task_attributes);

  // 创建Modbus2从机任务
  const osThreadAttr_t modbus2Task_attributes =
  {
    .name = "Modbus2Task",
    .stack_size = 512 * 4,
    .priority = (osPriority_t)osPriorityNormal,
  };

  osThreadNew(Modbus2Task, NULL, &modbus2Task_attributes);


  // 创建ADC打印任务（实时优先级）
  const osThreadAttr_t adcPrintTask_attributes =
  {
    .name = "AdcPrintTask",
    .stack_size = 512 * 4,
    .priority = (osPriority_t)osPriorityNormal,
  };
  osThreadNew(AdcPrintTask, NULL, &adcPrintTask_attributes);

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
    modbus_update_regs(g_modbus_regs);
    led_toggle(led1);
    osDelay(500);
  }
}

/**
 * @brief   Modbus从机任务
 *
 * @details 循环调用modbus_poll()处理Modbus请求，
 *          主机可通过功能码0x03读取保持寄存器
 *
 * @param[in]   argument  任务参数（未使用）
 *
 * @return  None
 */
static void Modbus1Task(void *argument)
{
  (void)argument;

  while(1)
  {
    // 处理Modbus请求（阻塞式，内部会等待接收）
    modbus_poll(&g_modbus_1);
  }
}

/**
 * @brief   Modbus从机任务
 *
 * @details 循环调用modbus_poll()处理Modbus请求，
 *          主机可通过功能码0x03读取保持寄存器
 *
 * @param[in]   argument  任务参数（未使用）
 *
 * @return  None
 */
static void Modbus2Task(void *argument)
{
  (void)argument;

  while(1)
  {
    // 处理Modbus请求（阻塞式，内部会等待接收）
    modbus_poll(&g_modbus_2);
  }
}

// 定义ADC滤波器
static MAF_Handle_t s_adc_filter_1;
static WMAF_Handle_t s_adc_filter_2;

static void AdcPrintTask(void *argument)
{
  uint16_t *adc_buffer = NULL; /**< ADC DMA缓冲区指针 */
  uint16_t adcx = 0;           /**< 一级滤波后的ADC值 */
  uint16_t adcx2 = 0;          /**< 二级滤波后的ADC值 */

  // uint16_t *adc2_buffer = NULL;

  (void)argument;

  while(1)
  {
     // 获取adc2的值
    // adc2_buffer = adc_get_dma_buffer(adc2);
    // printf("adc2: %d\n", adc2_buffer[0]);

    //osDelay(1000);

    // 获取DMA缓冲区数据
    adc_buffer = adc_get_dma_buffer(adc1);
    if(adc_buffer == NULL || adc_get_dma_length(adc1) == 0)
    {
      continue;
    }

    // 两级滤波处理：MAF -> WMAF
    adcx = MAF_Update(&s_adc_filter_1, adc_buffer[0]);
    printf("%d, %d\n", adc_buffer[0], adcx);
    //adcx2 = WMAF_Update(&s_adc_filter_2, adcx);

    //printf("%d, %d, %d\n", adc_buffer[0], adcx, adcx2);
  }
}
