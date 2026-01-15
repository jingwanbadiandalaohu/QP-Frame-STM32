/**
 * @file drv_adc.h
 * @brief ADC driver abstraction layer (platform-independent interface)
 */

#ifndef DRV_ADC_H
#define DRV_ADC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ADC_Device ADC_Device_t;

typedef enum
{
  DRV_ADC1,
  DRV_ADC2,
  DRV_ADC_MAX
} DRV_ADC_Instance_t;

typedef enum
{
  DRV_ADC_RESOLUTION_8BIT,
  DRV_ADC_RESOLUTION_10BIT,
  DRV_ADC_RESOLUTION_12BIT,
  DRV_ADC_RESOLUTION_16BIT
} DRV_ADC_Resolution_t;

typedef struct
{
  DRV_ADC_Instance_t instance;
  uint8_t channel;
  DRV_ADC_Resolution_t resolution;
} DRV_ADC_Config_t;

/* Return codes */
#ifndef DRV_OK
#define DRV_OK          0
#define DRV_ERROR      -1
#define DRV_BUSY       -2
#define DRV_TIMEOUT    -3
#endif

typedef struct ADC_Operations
{
  int (*init)(ADC_Device_t *dev, DRV_ADC_Config_t *config);
  int (*deinit)(ADC_Device_t *dev);
  int (*read)(ADC_Device_t *dev, uint16_t *value);
  int (*start_dma)(ADC_Device_t *dev, uint16_t *buffer, uint16_t len);
  int (*stop_dma)(ADC_Device_t *dev);
} ADC_Ops_t;

struct ADC_Device
{
  const char *name;
  DRV_ADC_Instance_t instance;
  void *hw_handle;
  uint16_t *dma_buffer;
  ADC_Ops_t *ops;
};

static inline int adc_init(ADC_Device_t *dev, DRV_ADC_Config_t *config)
{
  if(dev && dev->ops && dev->ops->init)
  {
    return dev->ops->init(dev, config);
  }
  return DRV_ERROR;
}

static inline int adc_deinit(ADC_Device_t *dev)
{
  if(dev && dev->ops && dev->ops->deinit)
  {
    return dev->ops->deinit(dev);
  }
  return DRV_ERROR;
}

static inline int adc_read(ADC_Device_t *dev, uint16_t *value)
{
  if(dev && dev->ops && dev->ops->read)
  {
    return dev->ops->read(dev, value);
  }
  return DRV_ERROR;
}

static inline int adc_start_dma(ADC_Device_t *dev, uint16_t *buffer, uint16_t len)
{
  if(dev && dev->ops && dev->ops->start_dma)
  {
    return dev->ops->start_dma(dev, buffer, len);
  }
  return DRV_ERROR;
}

static inline int adc_stop_dma(ADC_Device_t *dev)
{
  if(dev && dev->ops && dev->ops->stop_dma)
  {
    return dev->ops->stop_dma(dev);
  }
  return DRV_ERROR;
}

/* Platform device instances */
extern ADC_Device_t *drv_adc1;
extern ADC_Device_t *drv_adc2;

#ifdef __cplusplus
}
#endif

#endif /* DRV_ADC_H */
