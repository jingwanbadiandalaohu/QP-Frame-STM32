/**
 * @file    bsp_adc.c
 * @author  Dylan
 * @date    2026-01-15
 * @brief   ADC板级支持包实现
 *
 * @details 实现ADC1和ADC2的初始化及DMA采集配置。
 *          - ADC1: PB1 - ADC_CHANNEL_5 - DMA1_Stream0 - 采集下板数据
 *          - ADC2: PA6 - ADC_CHANNEL_3 - DMA1_Stream1 - 采集星点电压
 */

#include "bsp_adc.h"
#include "drv_system.h"

/* ==================== DMA缓冲区定义 ==================== */

/**
 * @brief ADC1 DMA缓冲区
 * @note  放置在AXI SRAM，32字节对齐以满足DMA要求
 */
__attribute__((section(".ram_axi"), aligned(32)))
uint16_t g_adc1_dma_buffer[ADC1_DMA_BUFFER_LENGTH];

/**
 * @brief ADC2 DMA缓冲区
 * @note  放置在AXI SRAM，32字节对齐以满足DMA要求
 */
__attribute__((section(".ram_axi"), aligned(32)))
uint16_t g_adc2_dma_buffer[ADC2_DMA_BUFFER_LENGTH];

/* ==================== 句柄获取函数 ==================== */

/**
 * @brief   获取ADC1设备句柄
 *
 * @param   None
 * @return  ADC1设备指针
 */
ADC_Device_t *BSP_GetAdc1Handle(void)
{
  return drv_adc1;
}

/**
 * @brief   获取ADC2设备句柄
 *
 * @param   None
 * @return  ADC2设备指针
 */
ADC_Device_t *BSP_GetAdc2Handle(void)
{
  return drv_adc2;
}

/* ==================== 初始化函数 ==================== */

/**
 * @brief   ADC初始化
 *
 * @details 初始化ADC1和ADC2为16位分辨率，启动DMA循环采集模式。
 *          初始化失败时调用错误处理函数。
 *
 * @param   None
 * @return  None
 */
void BSP_ADC_Init(void)
{
  DRV_ADC_Config_t config;

  config.instance = DRV_ADC1;
  config.channel = 0;
  config.resolution = DRV_ADC_RESOLUTION_16BIT;
  if(adc_init(drv_adc1, &config) != DRV_OK)
  {
    DRV_System_ErrorHandler();
  }

  config.instance = DRV_ADC2;
  config.channel = 0;
  config.resolution = DRV_ADC_RESOLUTION_16BIT;
  if(adc_init(drv_adc2, &config) != DRV_OK)
  {
    DRV_System_ErrorHandler();
  }

  if(adc_start_dma(drv_adc1, g_adc1_dma_buffer,
                    ADC1_DMA_BUFFER_LENGTH) != DRV_OK)
  {
    DRV_System_ErrorHandler();
  }
  if(adc_start_dma(drv_adc2, g_adc2_dma_buffer,
                    ADC2_DMA_BUFFER_LENGTH) != DRV_OK)
  {
    DRV_System_ErrorHandler();
  }
}
