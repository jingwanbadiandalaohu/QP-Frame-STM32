/**
 * @file    drv_uart.h
 * @author  Dylan
 * @date    2026-01-15
 * @brief   UART驱动抽象层接口定义
 *
 * @details 提供平台无关的UART操作接口，支持阻塞和中断两种收发模式。
 *          通过操作函数指针实现不同平台的适配。
 */

#ifndef DRV_UART_H
#define DRV_UART_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== 类型定义 ==================== */

typedef struct UART_Device UART_Device_t;  /**< UART设备结构体前向声明 */

/**
 * @brief UART实例枚举
 */
typedef enum
{
  DRV_UART1,    /**< UART1实例 */
  DRV_UART2,    /**< UART2实例 */
  DRV_UART_MAX  /**< UART实例数量 */
} DRV_UART_Instance_t;

/**
 * @brief UART校验位枚举
 */
typedef enum
{
  DRV_UART_PARITY_NONE,  /**< 无校验 */
  DRV_UART_PARITY_EVEN,  /**< 偶校验 */
  DRV_UART_PARITY_ODD    /**< 奇校验 */
} DRV_UART_Parity_t;

/**
 * @brief UART停止位枚举
 */
typedef enum
{
  DRV_UART_STOPBITS_1,  /**< 1位停止位 */
  DRV_UART_STOPBITS_2   /**< 2位停止位 */
} DRV_UART_StopBits_t;

/**
 * @brief UART配置结构体
 */
typedef struct
{
  DRV_UART_Instance_t instance;  /**< UART实例 */
  uint32_t baudrate;             /**< 波特率 */
  uint8_t data_bits;             /**< 数据位（通常为8） */
  DRV_UART_StopBits_t stop_bits; /**< 停止位 */
  DRV_UART_Parity_t parity;      /**< 校验位 */
} DRV_UART_Config_t;

/* ==================== 返回值定义 ==================== */

#ifndef DRV_OK
#define DRV_OK          0   /**< 操作成功 */
#define DRV_ERROR      -1   /**< 操作失败 */
#define DRV_BUSY       -2   /**< 设备忙 */
#define DRV_TIMEOUT    -3   /**< 操作超时 */
#endif

/**
 * @brief UART操作函数集结构体
 */
typedef struct UART_Operations
{
  int (*init)(UART_Device_t *dev, DRV_UART_Config_t *config);                       /**< 初始化 */
  int (*deinit)(UART_Device_t *dev);                                                /**< 反初始化 */
  int (*transmit)(UART_Device_t *dev, uint8_t *data, uint16_t len, uint32_t timeout);/**< 阻塞发送 */
  int (*receive)(UART_Device_t *dev, uint8_t *data, uint16_t len, uint32_t timeout); /**< 阻塞接收 */
  int (*transmit_it)(UART_Device_t *dev, uint8_t *data, uint16_t len);              /**< 中断发送 */
  int (*receive_it)(UART_Device_t *dev, uint8_t *data, uint16_t len);               /**< 中断接收 */
} UART_Ops_t;

/**
 * @brief UART设备结构体
 */
struct UART_Device
{
  const char *name;             /**< 设备名称 */
  DRV_UART_Instance_t instance; /**< UART实例 */
  void *hw_handle;              /**< 硬件句柄（平台相关） */
  UART_Ops_t *ops;              /**< 操作函数集 */
};

/* ==================== 内联接口函数 ==================== */

/**
 * @brief   初始化UART设备
 *
 * @param[in] dev     UART设备指针
 * @param[in] config  配置参数指针
 *
 * @return  DRV_OK成功，其他失败
 */
static inline int uart_init(UART_Device_t *dev, DRV_UART_Config_t *config)
{
  if(dev && dev->ops && dev->ops->init)
  {
    return dev->ops->init(dev, config);
  }
  return DRV_ERROR;
}

/**
 * @brief   反初始化UART设备
 *
 * @param[in] dev  UART设备指针
 *
 * @return  DRV_OK成功，其他失败
 */
static inline int uart_deinit(UART_Device_t *dev)
{
  if(dev && dev->ops && dev->ops->deinit)
  {
    return dev->ops->deinit(dev);
  }
  return DRV_ERROR;
}

/**
 * @brief   阻塞方式发送数据
 *
 * @param[in] dev      UART设备指针
 * @param[in] data     发送数据缓冲区
 * @param[in] len      数据长度
 * @param[in] timeout  超时时间（毫秒）
 *
 * @return  DRV_OK成功，DRV_TIMEOUT超时，其他失败
 */
static inline int uart_transmit(UART_Device_t *dev, uint8_t *data, uint16_t len, uint32_t timeout)
{
  if(dev && dev->ops && dev->ops->transmit)
  {
    return dev->ops->transmit(dev, data, len, timeout);
  }
  return DRV_ERROR;
}

/**
 * @brief   阻塞方式接收数据
 *
 * @param[in]  dev      UART设备指针
 * @param[out] data     接收数据缓冲区
 * @param[in]  len      期望接收长度
 * @param[in]  timeout  超时时间（毫秒）
 *
 * @return  DRV_OK成功，DRV_TIMEOUT超时，其他失败
 */
static inline int uart_receive(UART_Device_t *dev, uint8_t *data, uint16_t len, uint32_t timeout)
{
  if(dev && dev->ops && dev->ops->receive)
  {
    return dev->ops->receive(dev, data, len, timeout);
  }
  return DRV_ERROR;
}

/**
 * @brief   中断方式发送数据
 *
 * @param[in] dev   UART设备指针
 * @param[in] data  发送数据缓冲区
 * @param[in] len   数据长度
 *
 * @return  DRV_OK成功，DRV_BUSY设备忙，其他失败
 */
static inline int uart_transmit_it(UART_Device_t *dev, uint8_t *data, uint16_t len)
{
  if(dev && dev->ops && dev->ops->transmit_it)
  {
    return dev->ops->transmit_it(dev, data, len);
  }
  return DRV_ERROR;
}

/**
 * @brief   中断方式接收数据
 *
 * @param[in]  dev   UART设备指针
 * @param[out] data  接收数据缓冲区
 * @param[in]  len   期望接收长度
 *
 * @return  DRV_OK成功，DRV_BUSY设备忙，其他失败
 */
static inline int uart_receive_it(UART_Device_t *dev, uint8_t *data, uint16_t len)
{
  if(dev && dev->ops && dev->ops->receive_it)
  {
    return dev->ops->receive_it(dev, data, len);
  }
  return DRV_ERROR;
}

/* ==================== 扩展接口函数 ==================== */

/**
 * @brief   从接收缓冲区读取单字节
 *
 * @param[in]  dev  UART设备指针
 * @param[out] out  读取字节存储指针
 *
 * @return  1有数据，0无数据，负值错误
 */
int uart_read_byte(UART_Device_t *dev, uint8_t *out);

/**
 * @brief   UART中断处理函数
 *
 * @param[in] dev  UART设备指针
 *
 * @return  None
 *
 * @note    需在对应USARTx_IRQHandler中调用
 */
void drv_uart_irq_handler(UART_Device_t *dev);

/* ==================== 平台设备实例声明 ==================== */

extern UART_Device_t *drv_uart1;  /**< UART1设备实例 */
extern UART_Device_t *drv_uart2;  /**< UART2设备实例 */

#ifdef __cplusplus
}
#endif

#endif /* DRV_UART_H */
