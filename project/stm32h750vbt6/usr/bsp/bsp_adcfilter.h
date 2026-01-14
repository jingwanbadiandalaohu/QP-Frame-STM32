#ifndef BSP_ADCFILTER_H
#define BSP_ADCFILTER_H

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== 移动平均滤波 (Moving Average Filter, MAF) ==================== */

#define MAF_WINDOW_SIZE 16
#define MAF_WINDOW_MASK (MAF_WINDOW_SIZE - 1)

typedef struct
{
  uint16_t buffer[MAF_WINDOW_SIZE];
  uint8_t index;
  uint32_t sum;
} MovingAverageFilter;

uint16_t MAF_Update(MovingAverageFilter *filter, uint16_t new_data);

/* ==================== 滑动加权滤波 (Weighted Moving Average, WMA) ==================== */

#define WMAF_WINDOW_SIZE 16
#define WMAF_WINDOW_MASK (WMAF_WINDOW_SIZE - 1)
#define WMAF_WEIGHT_SUM 136

typedef struct
{
  uint16_t buffer[WMAF_WINDOW_SIZE];
  uint8_t index;
  uint32_t weighted_sum;
} WeightedMovingAverageFilter;

uint16_t WMAF_Update(WeightedMovingAverageFilter *filter, uint16_t new_data);




#ifdef __cplusplus
}
#endif

#endif /* BSP_ADCFILTER_H */
