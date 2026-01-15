/**
 * @file bsp_adc.c
 * @brief ADC 板级支持包实现
 */

#include "bsp_adc.h"
#include "drv_system.h"

/* ADC1 - PB1 - ADC_CHANNEL_5 - DMA1_Stream0 - 采集下板数据 */
/* ADC2 - PA6 - ADC_CHANNEL_3 - DMA1_Stream1 - 采集星点电压 */

__attribute__((section(".ram_axi"), aligned(32)))
uint16_t g_adc1_dma_buffer[ADC1_DMA_BUFFER_LENGTH];
__attribute__((section(".ram_axi"), aligned(32)))
uint16_t g_adc2_dma_buffer[ADC2_DMA_BUFFER_LENGTH];

/* ==================== 句柄获取函数 ==================== */

ADC_Handle_t BSP_GetAdc1Handle(void)
{
  return DRV_ADC_GetHandle(DRV_ADC1);
}

ADC_Handle_t BSP_GetAdc2Handle(void)
{
  return DRV_ADC_GetHandle(DRV_ADC2);
}

/* ==================== 初始化函数==================== */

void BSP_ADC_Init(void)
{
  ADC_Config_t config;

  config.instance = DRV_ADC1;
  config.channel = 0;
  config.resolution = DRV_ADC_RESOLUTION_16BIT;
  if(DRV_ADC_Init(&config) != DRV_OK)
  {
    DRV_System_ErrorHandler();
  }

  config.instance = DRV_ADC2;
  config.channel = 0;
  config.resolution = DRV_ADC_RESOLUTION_16BIT;
  if(DRV_ADC_Init(&config) != DRV_OK)
  {
    DRV_System_ErrorHandler();
  }

  if(DRV_ADC_StartDMA(DRV_ADC_GetHandle(DRV_ADC1), g_adc1_dma_buffer,
                       ADC1_DMA_BUFFER_LENGTH) != DRV_OK)
  {
    DRV_System_ErrorHandler();
  }
  if(DRV_ADC_StartDMA(DRV_ADC_GetHandle(DRV_ADC2), g_adc2_dma_buffer,
                       ADC2_DMA_BUFFER_LENGTH) != DRV_OK)
  {
    DRV_System_ErrorHandler();
  }
}
