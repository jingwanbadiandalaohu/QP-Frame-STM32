/**
 * @file    drv_adc.h
 * @author  Dylan
 * @date    2026-01-15
 * @brief   ADC驱动接口定义。
 */

#ifndef DRV_ADC_H
#define DRV_ADC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


struct adc_desc;
typedef struct adc_desc *adc_desc_t;

void adc_init(adc_desc_t adc);
uint16_t adc_read(adc_desc_t adc);
void adc_start_dma(adc_desc_t adc);
void adc_stop_dma(adc_desc_t adc);
uint16_t adc_get_average(adc_desc_t adc);
uint16_t *adc_get_dma_buffer(adc_desc_t adc);
uint16_t adc_get_dma_length(adc_desc_t adc);

#ifdef __cplusplus
}
#endif

#endif /* DRV_ADC_H */
