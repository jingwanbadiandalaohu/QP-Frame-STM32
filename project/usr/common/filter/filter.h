/**
 * @file filter.h
 * @brief 平台无关的滤波器模块
 * @note 仅依赖标准 C 类型，无硬件依赖
 */

#ifndef FILTER_H
#define FILTER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== 移动平均滤波 (Moving Average Filter, MAF) ==================== */

#define MAF_WINDOW_SIZE   16
#define MAF_WINDOW_MASK   (MAF_WINDOW_SIZE - 1)

typedef struct
{
  uint16_t buffer[MAF_WINDOW_SIZE];
  uint8_t index;
  uint32_t sum;
} MAF_Handle_t;

/**
 * @brief 更新移动平均滤波器
 * @param filter 滤波器句柄
 * @param new_data 新数据
 * @return 滤波后的值
 */
uint16_t MAF_Update(MAF_Handle_t *filter, uint16_t new_data);

/* ==================== 滑动加权滤波 (Weighted Moving Average, WMA) ==================== */

#define WMAF_WINDOW_SIZE  16
#define WMAF_WINDOW_MASK  (WMAF_WINDOW_SIZE - 1)
#define WMAF_WEIGHT_SUM   136

typedef struct
{
  uint16_t buffer[WMAF_WINDOW_SIZE];
  uint8_t index;
  uint32_t weighted_sum;
} WMAF_Handle_t;

/**
 * @brief 更新加权移动平均滤波器
 * @param filter 滤波器句柄
 * @param new_data 新数据
 * @return 滤波后的值
 */
uint16_t WMAF_Update(WMAF_Handle_t *filter, uint16_t new_data);

/* 兼容旧类型名 */
typedef MAF_Handle_t MovingAverageFilter;
typedef WMAF_Handle_t WeightedMovingAverageFilter;

#ifdef __cplusplus
}
#endif

#endif /* FILTER_H */
