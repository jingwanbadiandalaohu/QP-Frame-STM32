/**
 * @file    drv_adc.c
 * @author  Dylan
 * @date    2026-01-15
 * @brief   ADC驱动实现
 *
 * @details 提供STM32H750VBT6的ADC初始化、DMA循环采样、校准与数据读取功能。
 *          支持16位分辨率、连续转换模式和DMA循环传输。
 *          
 *          ADC配置参数：
 *          - ADC时钟：50 MHz (PLL2，无预分频)
 *          - 分辨率：16位
 *          - 采样时间：387.5个时钟周期
 *          - 采样率：约126 kSPS
 *          - DMA模式：循环模式
 *          
 * @note    DMA缓冲区必须位于AXI SRAM (0x2400_0000 - 0x24FF_FFFF)
 * @warning 修改采样时间会影响采样率和信号稳定性
 */

#include "drv_adc.h"
#include "drv_adc_desc.h"

/**
 * @brief ADC1实例，ADC1: PB1 → ADC_CHANNEL_5 → DMA1_Stream0
 */
#define ADC1_INSTANCE ADC1
#define ADC1_GPIO_PORT GPIOB
#define ADC1_GPIO_PIN GPIO_PIN_1
#define ADC1_DMA_INSTANCE DMA1_Stream0
#define ADC1_DMA_REQUEST DMA_REQUEST_ADC1
/**
 * @brief ADC2实例，ADC2: PA6 → ADC_CHANNEL_3 → DMA1_Stream1
 */
#define ADC2_INSTANCE ADC2
#define ADC2_GPIO_PORT GPIOA
#define ADC2_GPIO_PIN GPIO_PIN_6
#define ADC2_DMA_INSTANCE DMA1_Stream1
#define ADC2_DMA_REQUEST DMA_REQUEST_ADC2


/**
 * @brief   初始化ADC
 *
 * @details 配置ADC工作参数并启动校准。
 *          
 *          ADC配置：
 *          - 时钟：50 MHz (无预分频)
 *          - 分辨率：16位
 *          - 转换模式：连续转换
 *          - 数据管理：DMA循环模式
 *          - 采样时间：387.5个时钟周期
 *          - 采样率：约126 kSPS
 *          
 *          初始化流程：
 *          1. 配置ADC基本参数
 *          2. 执行偏移校准
 *          3. 配置ADC通道参数
 *
 * @param[in]   adc  ADC描述符指针
 * 
 * @return  None
 * 
 * @note    调用前需确保adc描述符已正确初始化
 * @note    校准过程会自动执行，无需手动干预
 * @warning adc参数为NULL时函数直接返回
 */
void adc_init(adc_desc_t adc)
{
  ADC_ChannelConfTypeDef ch_config = {0};

  if(adc == NULL)
  {
    return;
  }

  // 配置ADC基本参数
  adc->hal_handle.Instance = adc->instance;
  adc->hal_handle.DMA_Handle = &adc->dma_handle;
  adc->hal_handle.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;        // 时钟不分频，50 MHz
  adc->hal_handle.Init.Resolution = ADC_RESOLUTION_16B;              // 16位分辨率
  adc->hal_handle.Init.ScanConvMode = ADC_SCAN_DISABLE;              // 禁用扫描模式
  adc->hal_handle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;           // 单次转换结束标志
  adc->hal_handle.Init.LowPowerAutoWait = DISABLE;                   // 禁用低功耗自动等待
  adc->hal_handle.Init.ContinuousConvMode = ENABLE;                  // 使能连续转换模式
  adc->hal_handle.Init.NbrOfConversion = 1;                          // 转换通道数量为1
  adc->hal_handle.Init.DiscontinuousConvMode = DISABLE;              // 禁用间断转换模式
  adc->hal_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;        // 软件触发转换
  adc->hal_handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;  // 无外部触发边沿
  adc->hal_handle.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR;  // DMA循环模式
  adc->hal_handle.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;           // 数据溢出时覆盖旧数据
  adc->hal_handle.Init.OversamplingMode = DISABLE;                   // 禁用过采样

  HAL_ADC_Init(&adc->hal_handle);
  
  // 执行ADC偏移校准
  HAL_ADCEx_Calibration_Start(&adc->hal_handle, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);

  // 配置ADC通道参数
  ch_config.Channel = adc->channel;                                  // 设置通道号
  ch_config.Rank = ADC_REGULAR_RANK_1;                               // 转换序列第1位
  ch_config.SamplingTime = ADC_SAMPLETIME_387CYCLES_5;               // 采样时间387.5周期，采样率126 kSPS
  ch_config.SingleDiff = ADC_SINGLE_ENDED;                           // 单端输入模式
  ch_config.OffsetNumber = ADC_OFFSET_NONE;                          // 不使用偏移
  ch_config.Offset = 0;                                              // 偏移值为0
  HAL_ADC_ConfigChannel(&adc->hal_handle, &ch_config);
}

/**
 * @brief   ADC底层初始化
 *
 * @details 配置ADC的GPIO、时钟和DMA资源。此函数由HAL库自动调用。
 *          
 *          配置内容：
 *          - 使能ADC12、GPIO和DMA1时钟
 *          - 配置GPIO为模拟输入模式
 *          - 配置DMA为循环模式，高优先级
 *          - 数据对齐方式：半字(16位)
 *
 * @param[in]   hadc  ADC句柄指针
 * 
 * @return  None
 * 
 * @note    此函数在HAL_ADC_Init()中被自动调用
 * @warning 支持ADC1和ADC2实例
 */
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  if(hadc == NULL)
  {
    return;
  }

  // 使能ADC12时钟和DMA1时钟
  __HAL_RCC_ADC12_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();

  // 根据ADC实例配置对应的GPIO和DMA
  if(hadc->Instance == ADC1_INSTANCE)
  {
    // 使能GPIOB时钟
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // 配置ADC1输入引脚为模拟模式
    GPIO_InitStruct.Pin = ADC1_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ADC1_GPIO_PORT, &GPIO_InitStruct);

    // 配置ADC1 DMA参数
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
  else if(hadc->Instance == ADC2_INSTANCE)
  {
    // 使能GPIOA时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // 配置ADC2输入引脚为模拟模式
    GPIO_InitStruct.Pin = ADC2_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ADC2_GPIO_PORT, &GPIO_InitStruct);

    // 配置ADC2 DMA参数
    hadc->DMA_Handle->Instance = ADC2_DMA_INSTANCE;
    hadc->DMA_Handle->Init.Request = ADC2_DMA_REQUEST;
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
}



/**
 * @brief   读取一次ADC值
 *
 * @details 启动ADC转换，等待转换完成并返回结果。
 *          此函数使用轮询方式，会阻塞直到转换完成或超时。
 *
 * @param[in]   adc  ADC描述符指针
 * 
 * @return  uint16_t ADC采样值 (0-65535)
 * @retval  0  adc参数为NULL
 * 
 * @note    此函数适用于单次采样场景
 * @note    超时时间设置为100ms
 * @warning 不建议在DMA模式运行时调用此函数
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
 * @brief   启动ADC DMA采样
 *
 * @details 启动ADC连续转换，并通过DMA将数据传输到缓冲区。
 *          DMA工作在循环模式，缓冲区填满后会自动从头开始覆盖。
 *
 * @param[in]   adc  ADC描述符指针
 * 
 * @return  None
 * 
 * @note    调用前需确保DMA缓冲区已分配且位于AXI SRAM
 * @note    DMA循环模式下无需手动重启，会持续采样
 * @warning adc参数为NULL时函数直接返回
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
 * @brief   停止ADC DMA采样
 *
 * @details 停止ADC转换和DMA传输。
 *
 * @param[in]   adc  ADC描述符指针
 * 
 * @return  None
 * 
 * @warning adc参数为NULL时函数直接返回
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
 * @brief   计算DMA缓冲区平均值
 *
 * @details 遍历DMA缓冲区所有采样点，计算算术平均值。
 *          适用于降低噪声、获取稳定测量值的场景。
 *
 * @param[in]   adc  ADC描述符指针
 * 
 * @return  uint16_t 平均值 (0-65535)
 * @retval  0  adc参数为NULL或缓冲区无效
 * 
 * @note    计算过程中使用32位累加器防止溢出
 * @warning 确保在DMA传输稳定后调用，避免读取不完整数据
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
 * @brief   获取DMA缓冲区指针
 *
 * @details 返回ADC DMA缓冲区的首地址，用于直接访问采样数据。
 *
 * @param[in]   adc  ADC描述符指针
 * 
 * @return  uint16_t* DMA缓冲区指针
 * @retval  NULL  adc参数为NULL
 * 
 * @note    返回的指针指向实际的DMA缓冲区，修改数据会影响采样结果
 * @warning 在DMA传输过程中访问缓冲区可能读取到不一致的数据
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
 * @brief   获取DMA缓冲区长度
 *
 * @details 返回DMA缓冲区的采样点数量。
 *
 * @param[in]   adc  ADC描述符指针
 * 
 * @return  uint16_t 缓冲区长度（采样点数）
 * @retval  0  adc参数为NULL
 * 
 * @note    返回值表示缓冲区可存储的uint16_t数据个数
 */
uint16_t adc_get_dma_length(adc_desc_t adc)
{
  if(adc == NULL)
  {
    return 0;
  }

  return adc->buffer_len;
}

