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
#include <string.h>

// 中间层
#include "SEGGER_SYSVIEW.h"

#include "cmsis_os2.h"
// #include "printf.h" // 开源printf库（暂未使用）

// 组件
// #include "filter.h" // 滤波器（暂未使用）

// 设备层
// #include "led.h"    // TODO: 待驱动实现后启用
// #include "modbus.h" // TODO: 待驱动实现后启用
// #include "relay.h"  // TODO: 待驱动实现后启用

// 驱动层
// #include "board.h"     // TODO: 待驱动实现后启用
// #include "drv_adc.h"   // TODO: 待驱动实现后启用
#include "drv_system.h"
#include "stm32f1xx_hal.h"
// #include "drv_uart.h"  // TODO: 待驱动实现后启用

// 应用层
// #include "app_digital_sample.h" // TODO: 待驱动实现后启用
// #include "app_modbus_map.h"     // TODO: 待驱动实现后启用

// 采集任务（暂时注释，待驱动实现后启用）
// static void AppCollectTask(void *argument);

// LED闪烁任务
static void BlinkTask(void *argument);

// Modbus从机任务（暂时注释，待驱动实现后启用）
// static void Modbus1Task(void *argument);
// static void Modbus2Task(void *argument);

// 采样参数（暂时注释，待驱动实现后启用）
// static Data_t g_data;

// Modbus从机设备（暂时注释，待驱动实现后启用）
// static modbus_dev_t g_modbus_1;
// static modbus_dev_t g_modbus_2;

// Modbus保持寄存器（100个）（暂时注释，待驱动实现后启用）
// static uint16_t g_modbus_regs[100]= {0};

// 任务运行状态测试计数器
uint32_t g_taskTestCounter= 0;

// 共享数据互斥锁（保护g_data与g_modbus_regs的一致性）（暂时注释，待驱动实现后启用）
// osMutexId_t g_modbusDataMutex= NULL;

int main(void)
{
  // NOTE: F103 暂时注释掉缺失的驱动调用，待驱动层实现后再启用

  // 在系统初始化之前清零 AXI SRAM(D1)
  // NOTE: F103 没有 AXI SRAM，注释掉
  // memset((void *)0x24000000, 0, 512 * 1024);

  // 清缓冲区
  // TODO: 待 board.c 定义 DMA 缓冲区后启用
  // memset(Uart1_dma_rx_buf, 0, sizeof(Uart1_dma_rx_buf));
  // memset(Uart2_dma_rx_buf, 0, sizeof(Uart2_dma_rx_buf));

  // 系统初始化
  if(DRV_System_Init() != 0)
  {
    DRV_System_ErrorHandler();
  }

  // 外设初始化
  // TODO: 待实现 drv_gpio.c 后启用
  // led_init(led1);
  // relay_init(relay1);
  // relay_on(relay1);

  // 初始化串口 Uart1/2_ringbuf_storage用于环形缓冲区存储
  // TODO: 待实现 drv_uart.c 后启用
  // uart_init(uart1_rs232, Uart1_ringbuf_storage, sizeof(Uart1_ringbuf_storage));
  // uart_init(uart2_rs485, Uart2_ringbuf_storage, sizeof(Uart2_ringbuf_storage));

  // 初始化Modbus从机（地址145，寄存器地址100-199）
  // TODO: 待 UART 驱动实现后启用
  // modbus_init(&g_modbus_1, uart1_rs232, 145, g_modbus_regs, 100, 100);
  // modbus_init(&g_modbus_2, uart2_rs485, 145, g_modbus_regs, 100, 100);
  // modbus_set_byte_timeout(&g_modbus_1, 250);
  // modbus_set_byte_timeout(&g_modbus_2, 250);
  // modbus_set_read_timeout(&g_modbus_1, 600);
  // modbus_set_read_timeout(&g_modbus_2, 600);

  // 初始化ADC
  // TODO: 待实现 drv_adc.c 后启用
  // adc_init(adc1);
  // adc_init(adc2);
  // adc_start_dma(adc1);
  // adc_start_dma(adc2);

  // 初始化RTOS内核
  osKernelInitialize();

  // 初始化SystemView并开启跟踪，便于分析任务切换和中断时序
  SEGGER_SYSVIEW_Conf();
  SEGGER_SYSVIEW_Start();

  // 创建共享数据互斥锁：后续所有共享数据读写都必须经过这把锁
  // TODO: 待驱动实现后启用
  // const osMutexAttr_t modbusDataMutex_attributes= {
  //   .name= "ModbusDataMutex",
  // };
  // g_modbusDataMutex= osMutexNew(&modbusDataMutex_attributes);
  // if(g_modbusDataMutex == NULL)
  // {
  //   DRV_System_ErrorHandler();
  // }

  // 创建LED闪烁任务
  const osThreadAttr_t blinkTask_attributes= {
    .name= "BlinkTask",
    .stack_size= 128 * 4,
    .priority= (osPriority_t)osPriorityNormal,
  };
  osThreadNew(BlinkTask, NULL, &blinkTask_attributes);

  // 创建采集任务
  // const osThreadAttr_t collectTask_attributes= {
  //   .name= "CollectTask",
  //   .stack_size= 512 * 4,
  //   .priority= (osPriority_t)osPriorityNormal,
  // };
  // osThreadNew(AppCollectTask, NULL, &collectTask_attributes);

  // 创建Modbus1从机任务
  // const osThreadAttr_t modbus1Task_attributes= {
  //   .name= "Modbus1Task",
  //   .stack_size= 512 * 4,
  //   .priority= (osPriority_t)osPriorityNormal,
  // };
  // osThreadNew(Modbus1Task, NULL, &modbus1Task_attributes);

  // 创建Modbus2从机任务
  // const osThreadAttr_t modbus2Task_attributes= {
  //   .name= "Modbus2Task",
  //   .stack_size= 512 * 4,
  //   .priority= (osPriority_t)osPriorityNormal,
  // };
  // osThreadNew(Modbus2Task, NULL, &modbus2Task_attributes);

  // 创建ADC打印任务（实时优先级）
  // const osThreadAttr_t adcPrintTask_attributes =
  // {
  //   .name = "AdcPrintTask",
  //   .stack_size = 512 * 4,
  //   .priority = (osPriority_t)osPriorityNormal,
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
 * @brief   LED闪烁任务（当前用于任务运行测试）
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
    // 自加计数器，用于在调试器中观察任务是否运行
    g_taskTestCounter++;

    // TODO: 待驱动实现后启用以下逻辑
    /*
    if(osMutexAcquire(g_modbusDataMutex, osWaitForever) == osOK)
    {
      app_modbus_update_regs(g_modbus_regs, &g_data);
      osMutexRelease(g_modbusDataMutex);
    }
    led_toggle(led1);
    */
    SEGGER_SYSVIEW_Print("Hello Dylan!\n");
    osDelay(1);
  }
}
//   }
// }

// /**
//  * @brief     采集任务
//  *
//  * @param[in]   argument  任务参数（未使用）
//  *
//  * @return  None
//  */
// void AppCollectTask(void *argument)
// {
//   (void)argument;
//
//   while(1)
//   {
//     Data_t local_data= g_data;
//     app_digital_sample_volty(&local_data);
//
//     if(osMutexAcquire(g_modbusDataMutex, osWaitForever) == osOK)
//     {
//       g_data= local_data;
//       osMutexRelease(g_modbusDataMutex);
//     }
//
//     osDelay(30);
//   }
// }

// /**
//  * @brief   Modbus从机任务
//  *
//  * @details 循环调用modbus_poll()处理Modbus请求，
//  *          主机可通过功能码0x03读取保持寄存器
//  *
//  * @param[in]   argument  任务参数（未使用）
//  *
//  * @return  None
//  */
// static void Modbus1Task(void *argument)
// {
//   (void)argument;
//
//   while(1)
//   {
//     modbus_poll(&g_modbus_1);
//     osDelay(1);
//   }
// }

// /**
//  * @brief   Modbus从机任务
//  *
//  * @details 循环调用modbus_poll()处理Modbus请求，
//  *          主机可通过功能码0x03读取保持寄存器
//  *
//  * @param[in]   argument  任务参数（未使用）
//  *
//  * @return  None
//  */
// static void Modbus2Task(void *argument)
// {
//   (void)argument;
//
//   while(1)
//   {
//     modbus_poll(&g_modbus_2);
//     osDelay(1);
//   }
// }

// 定义ADC滤波器
// static MAF_Handle_t s_adc_filter_1;
// static WMAF_Handle_t s_adc_filter_2;

// static void AdcPrintTask(void *argument)
// {
//   uint16_t *adc_buffer = NULL; /**< ADC DMA缓冲区指针 */
//   uint16_t adcx = 0;           /**< 一级滤波后的ADC值 */
//   uint16_t adcx2 = 0;          /**< 二级滤波后的ADC值 */

//   (void)argument;

//   while(1)
//   {
//     // 获取DMA缓冲区数据
//     adc_buffer = adc_get_dma_buffer(adc1);
//     if(adc_buffer == NULL || adc_get_dma_length(adc1) == 0)
//     {
//       continue;
//     }

//     // 两级滤波处理：MAF -> WMAF
//     adcx = MAF_Update(&s_adc_filter_1, adc_buffer[0]);
//     adcx2 = WMAF_Update(&s_adc_filter_2, adcx);

//     //printf("%d, %d, %d\n", adc_buffer[0], adcx, adcx2);
//     osDelay(1);
//   }
// }
