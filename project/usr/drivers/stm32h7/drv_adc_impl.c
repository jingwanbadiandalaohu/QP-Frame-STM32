/**
 * @file    drv_adc_impl.c
 * @author  Dylan
 * @date    2026-01-15
 * @brief   STM32H7 ADC驱动实现
 *
 * @details 实现ADC1和ADC2的HAL层封装，支持轮询读取和DMA连续采集。
 *          - ADC1: PB1 - ADC_CHANNEL_5
 *          - ADC2: PA6 - ADC_CHANNEL_3
 */

#include "drv_adc.h"
#include "platform_config.h"

/* ==================== ADC硬件配置 ==================== */

#define ADC1_INSTANCE                ADC1          /**< ADC1外设实例 */
#define ADC2_INSTANCE                ADC2          /**< ADC2外设实例 */

#define ADC1_CHANNEL                 ADC_CHANNEL_5 /**< ADC1通道: PB1 */
#define ADC2_CHANNEL                 ADC_CHANNEL_3 /**< ADC2通道: PA6 */

/* ==================== 私有数据结构 ==================== */

/**
 * @brief ADC私有数据结构
 */
typedef struct
{
  ADC_HandleTypeDef hal_handle;  /**< HAL ADC句柄 */
  DMA_HandleTypeDef dma_handle;  /**< HAL DMA句柄 */
  uint16_t *dma_buffer;          /**< DMA缓冲区指针 */
  uint16_t buffer_len;           /**< 缓冲区长度 */
  uint8_t initialized;           /**< 初始化标志 */
} ADC_Private_t;

static ADC_Private_t s_adc1_private = {0};  /**< ADC1私有数据 */
static ADC_Private_t s_adc2_private = {0};  /**< ADC2私有数据 */

/* ==================== 私有辅助函数 ==================== */

/**
 * @brief   从HAL句柄获取私有数据指针
 *
 * @param[in] hadc  HAL ADC句柄
 *
 * @return  私有数据指针，无效时返回NULL
 */
static ADC_Private_t *drv_adc_private_from_hal(ADC_HandleTypeDef *hadc)
{
  if(hadc == NULL)
  {
    return NULL;
  }

  if(hadc->Instance == ADC1_INSTANCE)
  {
    return &s_adc1_private;
  }
  if(hadc->Instance == ADC2_INSTANCE)
  {
    return &s_adc2_private;
  }

  return NULL;
}

/**
 * @brief   从设备指针获取私有数据指针
 *
 * @param[in] dev  ADC设备指针
 *
 * @return  私有数据指针，无效时返回NULL
 */
static ADC_Private_t *drv_adc_private_from_dev(ADC_Device_t *dev)
{
  if(dev == NULL)
  {
    return NULL;
  }

  return (ADC_Private_t *)dev->hw_handle;
}

/**
 * @brief   分辨率枚举转HAL值
 *
 * @param[in] resolution  驱动层分辨率枚举
 *
 * @return  HAL分辨率宏值
 */
static uint32_t drv_adc_map_resolution(DRV_ADC_Resolution_t resolution)
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

/* ==================== HAL MSP回调函数 ==================== */

/**
 * @brief   ADC MSP初始化回调
 *
 * @details 配置GPIO为模拟模式，初始化DMA通道
 *
 * @param[in] hadc  HAL ADC句柄
 *
 * @return  None
 */
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  ADC_Private_t *priv = drv_adc_private_from_hal(hadc);
  if(priv == NULL)
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
    priv->dma_handle.Instance = DMA1_Stream0;
    priv->dma_handle.Init.Request = DMA_REQUEST_ADC1;
    priv->dma_handle.Init.Direction = DMA_PERIPH_TO_MEMORY;
    priv->dma_handle.Init.PeriphInc = DMA_PINC_DISABLE;
    priv->dma_handle.Init.MemInc = DMA_MINC_ENABLE;
    priv->dma_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    priv->dma_handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    priv->dma_handle.Init.Mode = DMA_CIRCULAR;
    priv->dma_handle.Init.Priority = DMA_PRIORITY_HIGH;
    priv->dma_handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if(HAL_DMA_Init(&priv->dma_handle) == HAL_OK)
    {
      __HAL_LINKDMA(hadc, DMA_Handle, priv->dma_handle);
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
    priv->dma_handle.Instance = DMA1_Stream1;
    priv->dma_handle.Init.Request = DMA_REQUEST_ADC2;
    priv->dma_handle.Init.Direction = DMA_PERIPH_TO_MEMORY;
    priv->dma_handle.Init.PeriphInc = DMA_PINC_DISABLE;
    priv->dma_handle.Init.MemInc = DMA_MINC_ENABLE;
    priv->dma_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    priv->dma_handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    priv->dma_handle.Init.Mode = DMA_CIRCULAR;
    priv->dma_handle.Init.Priority = DMA_PRIORITY_HIGH;
    priv->dma_handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if(HAL_DMA_Init(&priv->dma_handle) == HAL_OK)
    {
      __HAL_LINKDMA(hadc, DMA_Handle, priv->dma_handle);
    }
  }
}

/**
 * @brief   ADC MSP反初始化回调
 *
 * @param[in] hadc  HAL ADC句柄
 *
 * @return  None
 */
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

/* ==================== 驱动操作函数实现 ==================== */

/**
 * @brief   ADC初始化
 *
 * @details 配置ADC参数、通道、执行校准
 *
 * @param[in] dev     设备指针
 * @param[in] config  配置参数
 *
 * @return  DRV_OK成功，DRV_ERROR失败
 */
static int stm32h7_adc_init(ADC_Device_t *dev, DRV_ADC_Config_t *config)
{
  ADC_ChannelConfTypeDef sConfig = {0};
  ADC_Private_t *priv = drv_adc_private_from_dev(dev);
  ADC_HandleTypeDef *hadc = NULL;
  uint32_t channel = 0;

  if(dev == NULL || config == NULL || priv == NULL)
  {
    return DRV_ERROR;
  }

  hadc = &priv->hal_handle;

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
  hadc->Init.Resolution = drv_adc_map_resolution(config->resolution);
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

  priv->initialized = 1U;
  dev->dma_buffer = NULL;

  return DRV_OK;
}

/**
 * @brief   ADC反初始化
 *
 * @param[in] dev  设备指针
 *
 * @return  DRV_OK成功，DRV_ERROR失败
 */
static int stm32h7_adc_deinit(ADC_Device_t *dev)
{
  ADC_Private_t *priv = drv_adc_private_from_dev(dev);

  if(priv == NULL)
  {
    return DRV_ERROR;
  }

  if(HAL_ADC_DeInit(&priv->hal_handle) != HAL_OK)
  {
    return DRV_ERROR;
  }

  return DRV_OK;
}

/**
 * @brief   ADC轮询读取
 *
 * @param[in]  dev    设备指针
 * @param[out] value  读取值存储指针
 *
 * @return  DRV_OK成功，DRV_TIMEOUT超时，DRV_ERROR失败
 */
static int stm32h7_adc_read(ADC_Device_t *dev, uint16_t *value)
{
  ADC_Private_t *priv = drv_adc_private_from_dev(dev);
  ADC_HandleTypeDef *hadc = NULL;

  if(priv == NULL || value == NULL)
  {
    return DRV_ERROR;
  }

  hadc = &priv->hal_handle;

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

/**
 * @brief   启动ADC DMA采集
 *
 * @param[in]  dev     设备指针
 * @param[out] buffer  DMA缓冲区
 * @param[in]  len     缓冲区长度
 *
 * @return  DRV_OK成功，DRV_ERROR失败
 */
static int stm32h7_adc_start_dma(ADC_Device_t *dev, uint16_t *buffer, uint16_t len)
{
  ADC_Private_t *priv = drv_adc_private_from_dev(dev);
  ADC_HandleTypeDef *hadc = NULL;

  if(priv == NULL || buffer == NULL || len == 0)
  {
    return DRV_ERROR;
  }

  hadc = &priv->hal_handle;
  priv->dma_buffer = buffer;
  priv->buffer_len = len;
  dev->dma_buffer = buffer;

  if(HAL_ADC_Start_DMA(hadc, (uint32_t *)buffer, len) != HAL_OK)
  {
    return DRV_ERROR;
  }

  return DRV_OK;
}

/**
 * @brief   停止ADC DMA采集
 *
 * @param[in] dev  设备指针
 *
 * @return  DRV_OK成功，DRV_ERROR失败
 */
static int stm32h7_adc_stop_dma(ADC_Device_t *dev)
{
  ADC_Private_t *priv = drv_adc_private_from_dev(dev);
  ADC_HandleTypeDef *hadc = NULL;

  if(priv == NULL)
  {
    return DRV_ERROR;
  }

  hadc = &priv->hal_handle;

  if(HAL_ADC_Stop_DMA(hadc) != HAL_OK)
  {
    return DRV_ERROR;
  }

  return DRV_OK;
}

/* ==================== 操作函数集与设备实例 ==================== */

/**
 * @brief STM32H7 ADC操作函数集
 */
static ADC_Ops_t stm32h7_adc_ops =
{
  .init = stm32h7_adc_init,
  .deinit = stm32h7_adc_deinit,
  .read = stm32h7_adc_read,
  .start_dma = stm32h7_adc_start_dma,
  .stop_dma = stm32h7_adc_stop_dma
};

/**
 * @brief ADC1设备实例
 */
static ADC_Device_t adc1_device =
{
  .name = "ADC1",
  .instance = DRV_ADC1,
  .hw_handle = &s_adc1_private,
  .dma_buffer = NULL,
  .ops = &stm32h7_adc_ops
};

/**
 * @brief ADC2设备实例
 */
static ADC_Device_t adc2_device =
{
  .name = "ADC2",
  .instance = DRV_ADC2,
  .hw_handle = &s_adc2_private,
  .dma_buffer = NULL,
  .ops = &stm32h7_adc_ops
};

ADC_Device_t *drv_adc1 = &adc1_device;  /**< ADC1全局设备指针 */
ADC_Device_t *drv_adc2 = &adc2_device;  /**< ADC2全局设备指针 */
