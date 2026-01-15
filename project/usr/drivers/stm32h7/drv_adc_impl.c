/**
 * @file drv_adc_impl.c
 * @brief STM32H7 平台 ADC 驱动实现
 */

#include "drv_adc.h"
#include "platform_config.h"

/* ==================== 平台 ADC 配置 ==================== */
#define ADC1_INSTANCE                ADC1
#define ADC2_INSTANCE                ADC2

#define ADC1_CHANNEL                 ADC_CHANNEL_5
#define ADC2_CHANNEL                 ADC_CHANNEL_3

/* ==================== 内部数据结构 ==================== */
typedef struct
{
  ADC_HandleTypeDef hal_handle;
  DMA_HandleTypeDef dma_handle;
  uint16_t *dma_buffer;
  uint16_t buffer_len;
  uint8_t initialized;
} ADC_Instance_t;

static ADC_Instance_t s_adc_instances[DRV_ADC_MAX] = {0};

static ADC_Instance_t *DRV_ADC_GetInstanceByHandle(ADC_HandleTypeDef *hadc)
{
  if(hadc == NULL)
  {
    return NULL;
  }

  if(hadc->Instance == ADC1_INSTANCE)
  {
    return &s_adc_instances[DRV_ADC1];
  }
  if(hadc->Instance == ADC2_INSTANCE)
  {
    return &s_adc_instances[DRV_ADC2];
  }

  return NULL;
}

static uint32_t DRV_ADC_MapResolution(DRV_ADC_Resolution_t resolution)
{
  switch(resolution)
  {
    case DRV_ADC_RESOLUTION_8BIT:
      return ADC_RESOLUTION_8B;
    case DRV_ADC_RESOLUTION_10BIT:
      return ADC_RESOLUTION_10B;
    case DRV_ADC_RESOLUTION_12BIT:
      return ADC_RESOLUTION_12B;
    case DRV_ADC_RESOLUTION_16BIT:
    default:
      return ADC_RESOLUTION_16B;
  }
}

/* ==================== HAL MSP 回调 ==================== */

void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  ADC_Instance_t *inst = DRV_ADC_GetInstanceByHandle(hadc);
  if(inst == NULL)
  {
    return;
  }

  if(hadc->Instance == ADC1_INSTANCE)
  {
    __HAL_RCC_ADC12_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    __HAL_RCC_DMA1_CLK_ENABLE();
    inst->dma_handle.Instance = DMA1_Stream0;
    inst->dma_handle.Init.Request = DMA_REQUEST_ADC1;
    inst->dma_handle.Init.Direction = DMA_PERIPH_TO_MEMORY;
    inst->dma_handle.Init.PeriphInc = DMA_PINC_DISABLE;
    inst->dma_handle.Init.MemInc = DMA_MINC_ENABLE;
    inst->dma_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    inst->dma_handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    inst->dma_handle.Init.Mode = DMA_CIRCULAR;
    inst->dma_handle.Init.Priority = DMA_PRIORITY_HIGH;
    inst->dma_handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if(HAL_DMA_Init(&inst->dma_handle) == HAL_OK)
    {
      __HAL_LINKDMA(hadc, DMA_Handle, inst->dma_handle);
    }
  }
  else if(hadc->Instance == ADC2_INSTANCE)
  {
    __HAL_RCC_ADC12_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    __HAL_RCC_DMA1_CLK_ENABLE();
    inst->dma_handle.Instance = DMA1_Stream1;
    inst->dma_handle.Init.Request = DMA_REQUEST_ADC2;
    inst->dma_handle.Init.Direction = DMA_PERIPH_TO_MEMORY;
    inst->dma_handle.Init.PeriphInc = DMA_PINC_DISABLE;
    inst->dma_handle.Init.MemInc = DMA_MINC_ENABLE;
    inst->dma_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    inst->dma_handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    inst->dma_handle.Init.Mode = DMA_CIRCULAR;
    inst->dma_handle.Init.Priority = DMA_PRIORITY_HIGH;
    inst->dma_handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if(HAL_DMA_Init(&inst->dma_handle) == HAL_OK)
    {
      __HAL_LINKDMA(hadc, DMA_Handle, inst->dma_handle);
    }
  }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef *hadc)
{
  if(hadc->Instance == ADC1_INSTANCE)
  {
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_1);
    HAL_DMA_DeInit(hadc->DMA_Handle);
  }
  else if(hadc->Instance == ADC2_INSTANCE)
  {
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_6);
    HAL_DMA_DeInit(hadc->DMA_Handle);
  }
}

/* ==================== 驱动接口实现 ==================== */

int DRV_ADC_Init(ADC_Config_t *config)
{
  ADC_ChannelConfTypeDef sConfig = {0};
  ADC_HandleTypeDef *hadc = NULL;
  uint32_t channel = 0;

  if(config == NULL || config->instance >= DRV_ADC_MAX)
  {
    return DRV_ERROR;
  }

  ADC_Instance_t *inst = &s_adc_instances[config->instance];
  hadc = &inst->hal_handle;

  if(config->instance == DRV_ADC1)
  {
    hadc->Instance = ADC1_INSTANCE;
    channel = ADC1_CHANNEL;
  }
  else if(config->instance == DRV_ADC2)
  {
    hadc->Instance = ADC2_INSTANCE;
    channel = ADC2_CHANNEL;
  }
  else
  {
    return DRV_ERROR;
  }

  hadc->Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc->Init.Resolution = DRV_ADC_MapResolution(config->resolution);
  hadc->Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc->Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc->Init.LowPowerAutoWait = DISABLE;
  hadc->Init.ContinuousConvMode = ENABLE;
  hadc->Init.NbrOfConversion = 1;
  hadc->Init.DiscontinuousConvMode = DISABLE;
  hadc->Init.NbrOfDiscConversion = 0;
  hadc->Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc->Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc->Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR;
  hadc->Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  hadc->Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
  hadc->Init.OversamplingMode = DISABLE;
  hadc->Init.Oversampling.Ratio = 0;
  hadc->Init.Oversampling.RightBitShift = 0;
  hadc->Init.Oversampling.TriggeredMode = 0;
  hadc->Init.Oversampling.OversamplingStopReset = 0;
  if(HAL_ADC_Init(hadc) != HAL_OK)
  {
    return DRV_ERROR;
  }

  sConfig.Channel = channel;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = (config->instance == DRV_ADC1) ?
                          ADC_SAMPLETIME_387CYCLES_5 :
                          ADC_SAMPLETIME_64CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if(HAL_ADC_ConfigChannel(hadc, &sConfig) != HAL_OK)
  {
    return DRV_ERROR;
  }

  if(HAL_ADCEx_Calibration_Start(hadc, ADC_CALIB_OFFSET,
                                 ADC_SINGLE_ENDED) != HAL_OK)
  {
    return DRV_ERROR;
  }

  inst->initialized = 1;

  return DRV_OK;
}

int DRV_ADC_DeInit(ADC_Handle_t handle)
{
  if(handle == NULL)
  {
    return DRV_ERROR;
  }

  if(HAL_ADC_DeInit((ADC_HandleTypeDef *)handle) != HAL_OK)
  {
    return DRV_ERROR;
  }

  return DRV_OK;
}

ADC_Handle_t DRV_ADC_GetHandle(DRV_ADC_Instance_t instance)
{
  if(instance >= DRV_ADC_MAX)
  {
    return NULL;
  }

  return (ADC_Handle_t)&s_adc_instances[instance].hal_handle;
}

int DRV_ADC_Read(ADC_Handle_t handle, uint16_t *value)
{
  if(handle == NULL || value == NULL)
  {
    return DRV_ERROR;
  }

  ADC_HandleTypeDef *hadc = (ADC_HandleTypeDef *)handle;

  if(HAL_ADC_Start(hadc) != HAL_OK)
  {
    return DRV_ERROR;
  }

  if(HAL_ADC_PollForConversion(hadc, 100) != HAL_OK)
  {
    HAL_ADC_Stop(hadc);
    return DRV_TIMEOUT;
  }

  *value = (uint16_t)HAL_ADC_GetValue(hadc);

  HAL_ADC_Stop(hadc);

  return DRV_OK;
}

int DRV_ADC_StartDMA(ADC_Handle_t handle, uint16_t *buffer, uint16_t len)
{
  if(handle == NULL || buffer == NULL || len == 0)
  {
    return DRV_ERROR;
  }

  ADC_HandleTypeDef *hadc = (ADC_HandleTypeDef *)handle;
  ADC_Instance_t *inst = DRV_ADC_GetInstanceByHandle(hadc);
  if(inst == NULL)
  {
    return DRV_ERROR;
  }

  inst->dma_buffer = buffer;
  inst->buffer_len = len;

  if(HAL_ADC_Start_DMA(hadc, (uint32_t *)buffer, len) != HAL_OK)
  {
    return DRV_ERROR;
  }

  return DRV_OK;
}

int DRV_ADC_StopDMA(ADC_Handle_t handle)
{
  if(handle == NULL)
  {
    return DRV_ERROR;
  }

  ADC_HandleTypeDef *hadc = (ADC_HandleTypeDef *)handle;

  if(HAL_ADC_Stop_DMA(hadc) != HAL_OK)
  {
    return DRV_ERROR;
  }

  return DRV_OK;
}

uint16_t *DRV_ADC_GetDMABuffer(DRV_ADC_Instance_t instance)
{
  if(instance >= DRV_ADC_MAX)
  {
    return NULL;
  }

  return s_adc_instances[instance].dma_buffer;
}
