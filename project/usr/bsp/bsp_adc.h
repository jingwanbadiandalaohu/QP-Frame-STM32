/**
 * @file bsp_adc.h
 * @brief ADC 板级支持包
 */

#ifndef BSP_ADC_H
#define BSP_ADC_H

#include "bsp_config.h"
#include "drv_adc.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== ADC 缓冲区配置 ==================== */
#define ADC1_DMA_BUFFER_LENGTH  BSP_ADC1_BUFFER_SIZE
#define ADC2_DMA_BUFFER_LENGTH  BSP_ADC2_BUFFER_SIZE

extern uint16_t g_adc1_dma_buffer[ADC1_DMA_BUFFER_LENGTH];
extern uint16_t g_adc2_dma_buffer[ADC2_DMA_BUFFER_LENGTH];

/* ==================== 函数原型 ==================== */

void BSP_ADC_Init(void);

/* 获取 ADC 句柄 (供上层使用) */
ADC_Device_t *BSP_GetAdc1Handle(void);
ADC_Device_t *BSP_GetAdc2Handle(void);

#ifdef __cplusplus
}
#endif

#endif /* BSP_ADC_H */
