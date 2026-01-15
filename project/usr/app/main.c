/**
 * @file    main.c
 * @author  Dylan
 * @date    2026-01-15
 * @brief   应用层主程序入口
 *
 * @details 本文件实现FreeRTOS多任务应用框架，包含LED闪烁、
 *          串口通信和ADC数据采集三个独立任务。
 *          使用CMSIS-RTOS2接口和驱动层抽象接口实现平台无关性。
 */

#include "bsp.h"

/* ==================== 任务函数声明 ==================== */

void BlinkTask(void *argument);
void PrintTask(void *argument);
void AdcPrintTask(void *argument);

/**
 * @brief   应用程序入口点
 *
 * @details 完成BSP初始化、创建RTOS任务并启动调度器。
 *          正常情况下不会从osKernelStart()返回。
 *
 * @param   None
 * @return  int 正常情况下不返回
 */
int main(void)
{
  BSP_Init();
  osKernelInitialize();

  const osThreadAttr_t blinkTask_attributes =
  {
    .name = "BlinkTask",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityNormal,
  };
  osThreadNew(BlinkTask, NULL, &blinkTask_attributes);

  const osThreadAttr_t printTask_attributes =
  {
    .name = "PrintTask",
    .stack_size = 512 * 4,
    .priority = (osPriority_t)osPriorityNormal,
  };
  osThreadNew(PrintTask, NULL, &printTask_attributes);

  const osThreadAttr_t adcPrintTask_attributes =
  {
    .name = "AdcPrintTask",
    .stack_size = 1024 * 4,
    .priority = (osPriority_t)osPriorityRealtime,
  };
  osThreadNew(AdcPrintTask, NULL, &adcPrintTask_attributes);

  osKernelStart();

  while(1)
  {

  }
}

/**
 * @brief   LED闪烁任务
 *
 * @details 周期性翻转LED1状态，用于系统运行指示。
 *          闪烁周期为1秒。
 *
 * @param[in] argument  任务参数（未使用）
 *
 * @return  None（任务函数不返回）
 */
void BlinkTask(void *argument)
{
  (void)argument;
  while(1)
  {
    /* 使用驱动层接口替代直接 HAL 调用 */
    gpio_toggle(BSP_LED1_PORT, BSP_LED1_PIN);
    osDelay(1000);
  }
}

/**
 * @brief   UART串口打印任务
 *
 * @details 轮询检查UART1(RS232)和UART2(RS485)接收缓冲区，
 *          收到数据后格式化输出到对应串口。
 *
 * @param[in] argument  任务参数（未使用）
 *
 * @return  None（任务函数不返回）
 */
void PrintTask(void *argument)
{
  char buffer[64] = {0};
  uint8_t byte = 0;
  int len = 0;

  (void)argument;
  while(1)
  {
    if(debug_uart1_read(&byte))
    {
      len = snprintf(buffer, sizeof(buffer), "RS232_Uart1 %x\r\n", byte);
      uart_transmit(BSP_GetUart1Handle(), (uint8_t *)buffer,
                    (uint16_t)len, 0xFFFF);
    }
    if(debug_uart2_read(&byte))
    {
      len = snprintf(buffer, sizeof(buffer), "RS485_Uart2 %x\r\n", byte);
      uart_transmit(BSP_GetUart2Handle(), (uint8_t *)buffer,
                    (uint16_t)len, 0xFFFF);
    }
    osDelay(1);
  }
}

/* ==================== 滤波器实例（预留） ==================== */

// static MAF_Handle_t s_adc_filter_1;   /**< ADC1滑动平均滤波器 */
// static WMAF_Handle_t s_adc_filter_2;  /**< ADC1加权滑动平均滤波器 */

/**
 * @brief   ADC数据采集打印任务
 *
 * @details 周期性读取ADC DMA缓冲区数据，可选配滤波处理后输出。
 *          当前任务优先级为实时级别，确保数据采集的及时性。
 *
 * @param[in] argument  任务参数（未使用）
 *
 * @return  None（任务函数不返回）
 *
 * @note    滤波器功能已预留，取消注释即可启用
 */
void AdcPrintTask(void *argument)
{
  // uint16_t adcx = 0;
  // uint16_t adcx2 = 0;

  (void)argument;

  while(1)
  {
    osDelay(1000);

    // /* 使用滤波器处理 ADC 数据 */
    // adcx = MAF_Update(&s_adc_filter_1, g_adc1_dma_buffer[0]);
    // adcx2 = WMAF_Update(&s_adc_filter_2, adcx);

    // // /* 打印滤波前后波形 */
    // printf("%d, %d, %d\n", g_adc1_dma_buffer[0], adcx, adcx2);
  }
}
