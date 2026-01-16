/**
 * @file    drv_adc.c
 * @author  Dylan
 * @date    2026-01-15
 * @brief   ADC驱动实现。
 *
 * @details 完成ADC初始化、DMA采样、校准与数据读取。
 */

#include "drv_adc.h"
#include "drv_adc_desc.h"

// ADC1硬件资源定义。
#define ADC1_INSTANCE ADC1
#define ADC1_GPIO_PORT GPIOB
#define ADC1_GPIO_PIN GPIO_PIN_1
#define ADC1_DMA_INSTANCE DMA1_Stream0
#define ADC1_DMA_REQUEST DMA_REQUEST_ADC1

/**
 * @brief   ADC底层初始化。
 *
 * @param[in]   hadc  ADC句柄。
 * @return  无
 */
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  if(hadc == NULL || hadc->Instance != ADC1_INSTANCE)
  {
    return;
  }

  // 使能外设时钟。
  __HAL_RCC_ADC12_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();

  // 配置ADC输入引脚。
  GPIO_InitStruct.Pin = ADC1_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ADC1_GPIO_PORT, &GPIO_InitStruct);

  // 配置DMA参数。
  hadc->DMA_Handle->Instance = ADC1_DMA_INSTANCE;
  hadc->DMA_Handle->Init.Request = ADC1_DMA_REQUEST;
  hadc->DMA_Handle->Init.Direction = DMA_PERIPH_TO_MEMORY;
  hadc->DMA_Handle->Init.PeriphInc = DMA_PINC_DISABLE;
  hadc->DMA_Handle->Init.MemInc = DMA_MINC_ENABLE;
  hadc->DMA_Handle->Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
  hadc->DMA_Handle->Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
  hadc->DMA_Handle->Init.Mode = DMA_CIRCULAR;
  hadc->DMA_Handle->Init.Priority = DMA_PRIORITY_HIGH;
  hadc->DMA_Handle->Init.FIFOMode = DMA_FIFOMODE_DISABLE;

  if(HAL_DMA_Init(hadc->DMA_Handle) == HAL_OK)
  {
    __HAL_LINKDMA(hadc, DMA_Handle, *hadc->DMA_Handle);
  }
}

/**
 * @brief   ADC底层反初始化。
 *
 * @param[in]   hadc  ADC句柄。
 * @return  无
 */
void HAL_ADC_MspDeInit(ADC_HandleTypeDef *hadc)
{
  if(hadc == NULL || hadc->Instance != ADC1_INSTANCE)
  {
    return;
  }

  HAL_GPIO_DeInit(ADC1_GPIO_PORT, ADC1_GPIO_PIN);
  HAL_DMA_DeInit(hadc->DMA_Handle);
}

/**
 * @brief   初始化ADC。
 *
 * @param[in]   adc  ADC描述符。
 * @return  无
 */
void adc_init(adc_desc_t adc)
{
  ADC_ChannelConfTypeDef ch_config = {0};

  if(adc == NULL)
  {
    return;
  }

  adc->hal_handle.Instance = adc->instance;
  adc->hal_handle.DMA_Handle = &adc->dma_handle;
  adc->hal_handle.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV2;
  adc->hal_handle.Init.Resolution = ADC_RESOLUTION_16B;
  adc->hal_handle.Init.ScanConvMode = ADC_SCAN_DISABLE;
  adc->hal_handle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  adc->hal_handle.Init.LowPowerAutoWait = DISABLE;
  adc->hal_handle.Init.ContinuousConvMode = ENABLE;
  adc->hal_handle.Init.NbrOfConversion = 1;
  adc->hal_handle.Init.DiscontinuousConvMode = DISABLE;
  adc->hal_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  adc->hal_handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  adc->hal_handle.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR;
  adc->hal_handle.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  adc->hal_handle.Init.OversamplingMode = DISABLE;

  HAL_ADC_Init(&adc->hal_handle);
  HAL_ADCEx_Calibration_Start(&adc->hal_handle, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);

  ch_config.Channel = adc->channel;
  ch_config.Rank = ADC_REGULAR_RANK_1;
  ch_config.SamplingTime = ADC_SAMPLETIME_810CYCLES_5;
  ch_config.SingleDiff = ADC_SINGLE_ENDED;
  ch_config.OffsetNumber = ADC_OFFSET_NONE;
  ch_config.Offset = 0;
  HAL_ADC_ConfigChannel(&adc->hal_handle, &ch_config);
}

/**
 * @brief   反初始化ADC。
 *
 * @param[in]   adc  ADC描述符。
 * @return  无
 */
void adc_deinit(adc_desc_t adc)
{
  if(adc == NULL)
  {
    return;
  }

  HAL_ADC_DeInit(&adc->hal_handle);
}

/**
 * @brief   读取一次ADC值。
 *
 * @param[in]   adc  ADC描述符。
 * @return  uint16_t ADC采样值。
 */
uint16_t adc_read(adc_desc_t adc)
{
  if(adc == NULL)
  {
    return 0;
  }

  HAL_ADC_Start(&adc->hal_handle);
  HAL_ADC_PollForConversion(&adc->hal_handle, 100);
  return (uint16_t)HAL_ADC_GetValue(&adc->hal_handle);
}

/**
 * @brief   启动ADC DMA采样。
 *
 * @param[in]   adc  ADC描述符。
 * @return  无
 */
void adc_start_dma(adc_desc_t adc)
{
  if(adc == NULL)
  {
    return;
  }

  HAL_ADC_Start_DMA(&adc->hal_handle, (uint32_t *)adc->dma_buffer, adc->buffer_len);
}

/**
 * @brief   停止ADC DMA采样。
 *
 * @param[in]   adc  ADC描述符。
 * @return  无
 */
void adc_stop_dma(adc_desc_t adc)
{
  if(adc == NULL)
  {
    return;
  }

  HAL_ADC_Stop_DMA(&adc->hal_handle);
}

/**
 * @brief   计算DMA缓冲区平均值。
 *
 * @param[in]   adc  ADC描述符。
 * @return  uint16_t 平均值。
 */
uint16_t adc_get_average(adc_desc_t adc)
{
  uint32_t sum = 0;
  uint16_t i = 0;

  if(adc == NULL || adc->buffer_len == 0 || adc->dma_buffer == NULL)
  {
    return 0;
  }

  for(i = 0; i < adc->buffer_len; i++)
  {
    sum += adc->dma_buffer[i];
  }

  return (uint16_t)(sum / adc->buffer_len);
}

/**
 * @brief   获取DMA缓冲区指针。
 *
 * @param[in]   adc  ADC描述符。
 * @return  uint16_t* DMA缓冲区指针。
 */
uint16_t *adc_get_dma_buffer(adc_desc_t adc)
{
  if(adc == NULL)
  {
    return NULL;
  }

  return adc->dma_buffer;
}

/**
 * @brief   获取DMA缓冲区长度。
 *
 * @param[in]   adc  ADC描述符。
 * @return  uint16_t 缓冲区长度。
 */
uint16_t adc_get_dma_length(adc_desc_t adc)
{
  if(adc == NULL)
  {
    return 0;
  }

  return adc->buffer_len;
}

