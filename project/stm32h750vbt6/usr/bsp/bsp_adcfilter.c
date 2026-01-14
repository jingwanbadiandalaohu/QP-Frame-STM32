#include "bsp_adcfilter.h"


/* ==================== 移动平均滤波 ==================== */

uint16_t MAF_Update(MovingAverageFilter *filter, uint16_t new_data)
{
  filter->sum -= filter->buffer[filter->index];
  filter->buffer[filter->index] = new_data;
  filter->sum += new_data;
  filter->index = (filter->index + 1) & MAF_WINDOW_MASK;
  return (uint16_t)(filter->sum >> 4);
}




/* ==================== 滑动加权滤波 ==================== */
/* 权重数组，线性递减 */
static const uint8_t WMAF_WEIGHTS[WMAF_WINDOW_SIZE] = {
  16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1
};

uint16_t WMAF_Update(WeightedMovingAverageFilter *filter, uint16_t new_data)
{
  filter->buffer[filter->index] = new_data;
  filter->index = (filter->index + 1) & WMAF_WINDOW_MASK;

  filter->weighted_sum = 0;
  for(uint8_t i = 0; i < WMAF_WINDOW_SIZE; i++)
  {
    int data_index = (filter->index - 1 - i + WMAF_WINDOW_SIZE) & WMAF_WINDOW_MASK;
    filter->weighted_sum += filter->buffer[data_index] * WMAF_WEIGHTS[i];
  }

  return (uint16_t)(filter->weighted_sum / WMAF_WEIGHT_SUM);
}
