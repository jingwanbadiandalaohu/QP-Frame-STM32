/**
 * @file    bsp_adc.h
 * @author  Dylan
 * @date    2026-01-15
 * @brief   ADC板级支持包头文件
 *
 * @details 定义ADC DMA缓冲区和初始化函数接口。
 *          ADC1采集下板数据，ADC2采集星点电压。
 */

#ifndef BSP_ADC_H
#define BSP_ADC_H

#include "bsp_config.h"
#include "drv_adc.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== ADC缓冲区配置 ==================== */

#define ADC1_DMA_BUFFER_LENGTH  BSP_ADC1_BUFFER_SIZE  /**< ADC1 DMA缓冲区长度 */
#define ADC2_DMA_BUFFER_LENGTH  BSP_ADC2_BUFFER_SIZE  /**< ADC2 DMA缓冲区长度 */

extern uint16_t g_adc1_dma_buffer[ADC1_DMA_BUFFER_LENGTH];  /**< ADC1 DMA缓冲区 */
extern uint16_t g_adc2_dma_buffer[ADC2_DMA_BUFFER_LENGTH];  /**< ADC2 DMA缓冲区 */

/* ==================== 函数原型 ==================== */

/**
 * @brief   ADC初始化
 *
 * @details 初始化ADC1和ADC2，配置为16位分辨率，启动DMA连续采集
 *
 * @param   None
 * @return  None
 *
 * @note    初始化失败会进入错误处理
 */
void BSP_ADC_Init(void);

/**
 * @brief   获取ADC1设备句柄
 *
 * @param   None
 * @return  ADC1设备指针
 */
ADC_Device_t *BSP_GetAdc1Handle(void);

/**
 * @brief   获取ADC2设备句柄
 *
 * @param   None
 * @return  ADC2设备指针
 */
ADC_Device_t *BSP_GetAdc2Handle(void);

#ifdef __cplusplus
}
#endif

#endif /* BSP_ADC_H */
