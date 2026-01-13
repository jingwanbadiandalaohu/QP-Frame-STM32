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



// ================================  移动平均滤波 ================================
// =========================(Moving Average Filter, MAF)======================

#define MAF_WINDOW_SIZE 16 // 必须是 2 的幂（例如 4、8、16），以优化除法
#define MAF_WINDOW_MASK (MAF_WINDOW_SIZE - 1) // 用于快速模运算


typedef struct
{
  uint16_t buffer[MAF_WINDOW_SIZE]; // 存储 ADC 数据
  uint8_t index;                // 当前索引
  uint32_t sum;                 // 数据总和
} MovingAverageFilter;



uint16_t MAF_Update(MovingAverageFilter* filter, uint16_t new_data);



// =============================== 滑动加权滤波 ================================
// =========================(Weighted Moving Average, WMA)======================

#define WMAF_WINDOW_SIZE 16 // 必须是 2 的幂（例如 4、8、16），以优化除法
#define WMAF_WINDOW_MASK (WMAF_WINDOW_SIZE - 1) // 用于快速模运算

// 权重数组，线性递减，权重总和为 136
static const uint8_t WMAF_WEIGHTS[WMAF_WINDOW_SIZE] = {16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
#define WMAF_WEIGHT_SUM 136 // 权重总和，用于归一化

typedef struct
{
  uint16_t buffer[WMAF_WINDOW_SIZE]; // 存储 ADC 数据
  uint8_t index;                    // 当前索引
  uint32_t weighted_sum;            // 加权总和
} WeightedMovingAverageFilter;

uint16_t WMAF_Update(WeightedMovingAverageFilter* filter, uint16_t new_data);







#ifdef __cplusplus
}
#endif

#endif /* ADC_H */
