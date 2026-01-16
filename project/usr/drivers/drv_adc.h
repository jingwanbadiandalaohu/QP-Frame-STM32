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

/**
 * @brief ADC描述符类型。
 */
struct adc_desc;

typedef struct adc_desc *adc_desc_t;

/**
 * @brief   初始化ADC。
 *
 * @param[in]   adc  ADC描述符。
 * @return  无
 */
void adc_init(adc_desc_t adc);

/**
 * @brief   反初始化ADC。
 *
 * @param[in]   adc  ADC描述符。
 * @return  无
 */
void adc_deinit(adc_desc_t adc);

/**
 * @brief   读取一次ADC值。
 *
 * @param[in]   adc  ADC描述符。
 * @return  uint16_t ADC采样值。
 */
uint16_t adc_read(adc_desc_t adc);

/**
 * @brief   启动ADC DMA采样。
 *
 * @param[in]   adc  ADC描述符。
 * @return  无
 */
void adc_start_dma(adc_desc_t adc);

/**
 * @brief   停止ADC DMA采样。
 *
 * @param[in]   adc  ADC描述符。
 * @return  无
 */
void adc_stop_dma(adc_desc_t adc);

/**
 * @brief   计算DMA缓冲区平均值。
 *
 * @param[in]   adc  ADC描述符。
 * @return  uint16_t 平均值。
 */
uint16_t adc_get_average(adc_desc_t adc);

/**
 * @brief   获取DMA缓冲区指针。
 *
 * @param[in]   adc  ADC描述符。
 * @return  uint16_t* DMA缓冲区指针。
 */
uint16_t *adc_get_dma_buffer(adc_desc_t adc);

/**
 * @brief   获取DMA缓冲区长度。
 *
 * @param[in]   adc  ADC描述符。
 * @return  uint16_t 缓冲区长度。
 */
uint16_t adc_get_dma_length(adc_desc_t adc);

#ifdef __cplusplus
}
#endif

#endif /* DRV_ADC_H */
