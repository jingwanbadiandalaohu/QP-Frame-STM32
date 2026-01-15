/**
 * @file main.c
 * @brief 应用层主程序
 * @note 使用 CMSIS-RTOS2 接口和驱动层抽象接口
 */

#include "bsp.h"

/* 任务函数声明 */
void BlinkTask(void *argument);
void PrintTask(void *argument);
void AdcPrintTask(void *argument);

/**
 * @brief 应用程序入口点
 * @retval int
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
    /* 不应到达此处 */
  }
}

/**
 * @brief LED 闪烁任务
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
 * @brief UART 打印任务
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
      /* 使用驱动层接口替代直接 HAL 调用 */
      uart_transmit(BSP_GetUart1Handle(), (uint8_t *)buffer,
                    (uint16_t)len, 0xFFFF);
    }
    if(debug_uart2_read(&byte))
    {
      len = snprintf(buffer, sizeof(buffer), "RS485_Uart2 %x\r\n", byte);
      /* 使用驱动层接口替代直接 HAL 调用 */
      uart_transmit(BSP_GetUart2Handle(), (uint8_t *)buffer,
                    (uint16_t)len, 0xFFFF);
    }
    osDelay(1);
  }
}

/* 滤波器实例 */
// static MAF_Handle_t s_adc_filter_1;
// static WMAF_Handle_t s_adc_filter_2;

/**
 * @brief ADC 数据打印任务
 */
void AdcPrintTask(void *argument)
{
  uint16_t adcx = 0;
  uint16_t adcx2 = 0;

  (void)argument;

  while(1)
  {
    osDelay(1000);

    /* 使用滤波器处理 ADC 数据 */
    // adcx = MAF_Update(&s_adc_filter_1, g_adc1_dma_buffer[0]);
    // adcx2 = WMAF_Update(&s_adc_filter_2, adcx);

    // /* 打印滤波前后波形 */
     printf("%d, %d, %d\n", g_adc1_dma_buffer[0], adcx, adcx2);
  }
}
