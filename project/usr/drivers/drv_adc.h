/**
 * @file drv_adc.h
 * @brief ADC 驱动层抽象接口
 * @note 平台无关的 ADC 操作接口定义
 */

#ifndef DRV_ADC_H
#define DRV_ADC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 类型定义 */
typedef void *ADC_Handle_t;

typedef enum
{
  DRV_ADC1,
  DRV_ADC2,
  DRV_ADC_MAX
} DRV_ADC_Instance_t;

typedef enum
{
  DRV_ADC_RESOLUTION_8BIT,
  DRV_ADC_RESOLUTION_10BIT,
  DRV_ADC_RESOLUTION_12BIT,
  DRV_ADC_RESOLUTION_16BIT
} DRV_ADC_Resolution_t;

typedef struct
{
  DRV_ADC_Instance_t instance;
  uint8_t channel;
  DRV_ADC_Resolution_t resolution;
} ADC_Config_t;

/* 返回值约定: 0 表示成功, 负值表示失败 */
#ifndef DRV_OK
#define DRV_OK          0
#define DRV_ERROR      -1
#define DRV_BUSY       -2
#define DRV_TIMEOUT    -3
#endif

/**
 * @brief 初始化 ADC
 * @param config ADC 配置结构体指针
 * @return 0 成功, 负值失败
 */
int DRV_ADC_Init(ADC_Config_t *config);

/**
 * @brief 反初始化 ADC
 * @param handle ADC 句柄
 * @return 0 成功, 负值失败
 */
int DRV_ADC_DeInit(ADC_Handle_t handle);

/**
 * @brief 获取 ADC 句柄
 * @param instance ADC 实例
 * @return ADC 句柄, NULL 表示无效
 */
ADC_Handle_t DRV_ADC_GetHandle(DRV_ADC_Instance_t instance);

/**
 * @brief 读取 ADC 值 (阻塞方式)
 * @param handle ADC 句柄
 * @param value 存储 ADC 值的指针
 * @return 0 成功, 负值失败
 */
int DRV_ADC_Read(ADC_Handle_t handle, uint16_t *value);

/**
 * @brief 启动 DMA 方式 ADC 采集
 * @param handle ADC 句柄
 * @param buffer DMA 缓冲区指针
 * @param len 缓冲区长度
 * @return 0 成功, 负值失败
 */
int DRV_ADC_StartDMA(ADC_Handle_t handle, uint16_t *buffer, uint16_t len);

/**
 * @brief 停止 DMA 方式 ADC 采集
 * @param handle ADC 句柄
 * @return 0 成功, 负值失败
 */
int DRV_ADC_StopDMA(ADC_Handle_t handle);

/**
 * @brief 获取 DMA 缓冲区
 * @param instance ADC 实例
 * @return DMA 缓冲区指针, NULL 表示无效
 */
uint16_t *DRV_ADC_GetDMABuffer(DRV_ADC_Instance_t instance);

#ifdef __cplusplus
}
#endif

#endif /* DRV_ADC_H */
