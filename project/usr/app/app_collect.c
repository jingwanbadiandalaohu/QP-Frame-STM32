/**
 * @file    main.c
 * @author  Dylan
 * @date    2026-01-15
 * @brief   基于CMSIS-RTOS2的应用入口
 *
 * @details 复用驱动与设备层，实现LED闪烁、串口回显和ADC采样任务。
 *          演示环形缓冲区在串口接收中的应用。
 */
#include "app_collect.h"
#include <stdint.h>
#include <stdio.h>

/**
 * @brief   采集并打印星点电压
 *
 * @details 每次调用先进行5次ADC平均值滤波，再将滤波结果累计。
 *          当累计到200次后，计算平均ADC值并换算电压后打印。
 *
 * @param   None
 * @return  None
 */
void app_collect_VoltY(Data_t *Data)
{
  static uint16_t s_adc_count = 0;
  static uint32_t s_adc_sum = 0U;
  uint32_t five_filter_sum = 0U;
  uint16_t adc_filtered = 0U;
  uint16_t i = 0U;
  float avg_volt = 0.0f;

  if(adc2 == NULL || adc_get_dma_length(adc2) == 0U)
  {
    return;
  }

  // 先做5次平均值滤波，降低瞬时噪声影响
  for(i = 0U; i < 5U; i++)
  {
    five_filter_sum += adc_get_average(adc2);
  }
  adc_filtered = (uint16_t)(five_filter_sum / 5U);

  // 累计滤波结果用于计算长期平均电压
  s_adc_sum += adc_filtered;
  s_adc_count++;

  // 每200次计算一次平均值
  if(s_adc_count >= 200U)
  {
    uint16_t avg_adc = (uint16_t)(s_adc_sum / 200U);

    // ADC为16位分辨率，按3.3V参考电压换算
    avg_volt = ((float)avg_adc * 3.3f) / 65535.0f;
    //printf("VoltY avg_adc=%u, avg_volt=%.4fV\r\n", avg_adc, avg_volt);
    // 换算成星点电压,放大10倍保留一位小数
    Data->VoltY = avg_volt *10 *84.60f;    
    //printf("VoltY %dV\r\n", Data->VoltY);

    s_adc_count = 0U;
    s_adc_sum = 0U;
  }
}
