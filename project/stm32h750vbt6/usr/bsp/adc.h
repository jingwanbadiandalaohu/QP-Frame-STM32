#ifndef ADC_H
#define ADC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ADC1_DMA_BUFFER_LENGTH 64U
#define ADC2_DMA_BUFFER_LENGTH 64U

extern uint16_t g_adc1_dma_buffer[ADC1_DMA_BUFFER_LENGTH];
extern uint16_t g_adc2_dma_buffer[ADC2_DMA_BUFFER_LENGTH];

void BSP_ADC_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* ADC_H */
