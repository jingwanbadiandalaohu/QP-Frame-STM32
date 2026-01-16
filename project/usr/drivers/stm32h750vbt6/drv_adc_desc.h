#ifndef DRV_ADC_DESC_H
#define DRV_ADC_DESC_H

#include <stdint.h>
#include "stm32h7xx_hal.h"

struct adc_desc
{
  ADC_TypeDef *instance;
  uint32_t channel;
  uint16_t *dma_buffer;
  uint16_t buffer_len;
  ADC_HandleTypeDef hal_handle;
  DMA_HandleTypeDef dma_handle;
};

typedef struct adc_desc *adc_desc_t;

#endif /* DRV_ADC_DESC_H */
