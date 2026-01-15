/**
 * @file    drv_adc.h
 * @author  Dylan
 * @date    2026-01-15
 * @brief   ADC驱动抽象层接口定义
 *
 * @details 提供平台无关的ADC操作接口，支持轮询读取和DMA连续采集模式。
 *          通过操作函数指针实现不同平台的适配。
 */

#ifndef DRV_ADC_H
#define DRV_ADC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== 类型定义 ==================== */

typedef struct ADC_Device ADC_Device_t;  /**< ADC设备结构体前向声明 */

/**
 * @brief ADC实例枚举
 */
typedef enum
{
  DRV_ADC1,   /**< ADC1实例 */
  DRV_ADC2,   /**< ADC2实例 */
  DRV_ADC_MAX /**< ADC实例数量 */
} DRV_ADC_Instance_t;

/**
 * @brief ADC分辨率枚举
 */
typedef enum
{
  DRV_ADC_RESOLUTION_8BIT,   /**< 8位分辨率 */
  DRV_ADC_RESOLUTION_10BIT,  /**< 10位分辨率 */
  DRV_ADC_RESOLUTION_12BIT,  /**< 12位分辨率 */
  DRV_ADC_RESOLUTION_16BIT   /**< 16位分辨率 */
} DRV_ADC_Resolution_t;

/**
 * @brief ADC配置结构体
 */
typedef struct
{
  DRV_ADC_Instance_t instance;      /**< ADC实例 */
  uint8_t channel;                  /**< ADC通道号 */
  DRV_ADC_Resolution_t resolution;  /**< ADC分辨率 */
} DRV_ADC_Config_t;

/* ==================== 返回值定义 ==================== */

#ifndef DRV_OK
#define DRV_OK          0   /**< 操作成功 */
#define DRV_ERROR      -1   /**< 操作失败 */
#define DRV_BUSY       -2   /**< 设备忙 */
#define DRV_TIMEOUT    -3   /**< 操作超时 */
#endif

/**
 * @brief ADC操作函数集结构体
 */
typedef struct ADC_Operations
{
  int (*init)(ADC_Device_t *dev, DRV_ADC_Config_t *config);           /**< 初始化 */
  int (*deinit)(ADC_Device_t *dev);                                   /**< 反初始化 */
  int (*read)(ADC_Device_t *dev, uint16_t *value);                    /**< 轮询读取 */
  int (*start_dma)(ADC_Device_t *dev, uint16_t *buffer, uint16_t len);/**< 启动DMA */
  int (*stop_dma)(ADC_Device_t *dev);                                 /**< 停止DMA */
} ADC_Ops_t;

/**
 * @brief ADC设备结构体
 */
struct ADC_Device
{
  const char *name;               /**< 设备名称 */
  DRV_ADC_Instance_t instance;    /**< ADC实例 */
  void *hw_handle;                /**< 硬件句柄（平台相关） */
  uint16_t *dma_buffer;           /**< DMA缓冲区指针 */
  ADC_Ops_t *ops;                 /**< 操作函数集 */
};

/* ==================== 内联接口函数 ==================== */

/**
 * @brief   初始化ADC设备
 *
 * @param[in] dev     ADC设备指针
 * @param[in] config  配置参数指针
 *
 * @return  DRV_OK成功，其他失败
 */
static inline int adc_init(ADC_Device_t *dev, DRV_ADC_Config_t *config)
{
  if(dev && dev->ops && dev->ops->init)
  {
    return dev->ops->init(dev, config);
  }
  return DRV_ERROR;
}

/**
 * @brief   反初始化ADC设备
 *
 * @param[in] dev  ADC设备指针
 *
 * @return  DRV_OK成功，其他失败
 */
static inline int adc_deinit(ADC_Device_t *dev)
{
  if(dev && dev->ops && dev->ops->deinit)
  {
    return dev->ops->deinit(dev);
  }
  return DRV_ERROR;
}

/**
 * @brief   轮询读取ADC值
 *
 * @param[in]  dev    ADC设备指针
 * @param[out] value  读取值存储指针
 *
 * @return  DRV_OK成功，其他失败
 */
static inline int adc_read(ADC_Device_t *dev, uint16_t *value)
{
  if(dev && dev->ops && dev->ops->read)
  {
    return dev->ops->read(dev, value);
  }
  return DRV_ERROR;
}

/**
 * @brief   启动ADC DMA连续采集
 *
 * @param[in]  dev     ADC设备指针
 * @param[out] buffer  DMA缓冲区指针
 * @param[in]  len     缓冲区长度
 *
 * @return  DRV_OK成功，其他失败
 */
static inline int adc_start_dma(ADC_Device_t *dev, uint16_t *buffer, uint16_t len)
{
  if(dev && dev->ops && dev->ops->start_dma)
  {
    return dev->ops->start_dma(dev, buffer, len);
  }
  return DRV_ERROR;
}

/**
 * @brief   停止ADC DMA采集
 *
 * @param[in] dev  ADC设备指针
 *
 * @return  DRV_OK成功，其他失败
 */
static inline int adc_stop_dma(ADC_Device_t *dev)
{
  if(dev && dev->ops && dev->ops->stop_dma)
  {
    return dev->ops->stop_dma(dev);
  }
  return DRV_ERROR;
}

/* ==================== 平台设备实例声明 ==================== */

extern ADC_Device_t *drv_adc1;  /**< ADC1设备实例 */
extern ADC_Device_t *drv_adc2;  /**< ADC2设备实例 */

#ifdef __cplusplus
}
#endif

#endif /* DRV_ADC_H */
