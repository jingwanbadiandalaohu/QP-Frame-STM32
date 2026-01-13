#include "bsp.h"
#include "adc.h"
#include "bsp/adc.h"
#include "cmsis_os2.h"
#include "printf.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

void BlinkTask(void *argument);
void PrintTask(void *argument);
void AdcPrintTask(void *argument);

/**
 * @brief  The application entry point.
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

  }
}

void BlinkTask(void *argument)
{
  (void)argument;
  while(1)
  {
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    osDelay(1000);
  }
}

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;


void PrintTask(void *argument)
{
  char buffer[64];
  uint8_t byte;
  (void)argument;
  while(1)
  {
    if(debug_uart1_read(&byte))
    {
      int len = snprintf(buffer, sizeof(buffer), "RS232_Uart1 %x\r\n", byte);
      HAL_UART_Transmit(&huart1, (uint8_t *)buffer, (uint16_t)len, 0xFFFF);
    }
    if(debug_uart2_read(&byte))
    {
      int len = snprintf(buffer, sizeof(buffer), "RS485_Uart2 %x\r\n", byte);
      HAL_UART_Transmit(&huart2, (uint8_t *)buffer, (uint16_t)len, 0xFFFF);
    }
    osDelay(1);
  }
}

MovingAverageFilter adc_filter_1;
WeightedMovingAverageFilter adc_filter_2;

void AdcPrintTask(void *argument)
{
  uint16_t adcx,adcx2;

  (void)argument;
  // const float vref = 3.3f;
  // const float scale = vref / 65535.0f;
  while(1)
  {
    // uint16_t adc1_raw = g_adc1_dma_buffer[0];
    // uint16_t adc2_raw = g_adc2_dma_buffer[0];
    // float adc1_v = adc1_raw * scale;
    // float adc2_v = adc2_raw * scale;
    // printf("ADC1 raw=%u, V=%.3f; ADC2 raw=%u, V=%.3f\r\n",
    //        (unsigned)adc1_raw, adc1_v, (unsigned)adc2_raw, adc2_v);
    // osDelay(1000);
    adcx = MAF_Update(&adc_filter_1, g_adc1_dma_buffer[0]);
    adcx2 = WMAF_Update(&adc_filter_2, adcx);
    //printf("%d\n", adcx);
    printf("%d, %d, %d\n", g_adc1_dma_buffer[0], adcx, adcx2);       // 打印滤波前后波形
  }
}
