/**
 * @file drv_uart.h
 * @brief UART 驱动层抽象接口
 * @note 平台无关的 UART 操作接口定义
 */

#ifndef DRV_UART_H
#define DRV_UART_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 类型定义 */
typedef void *UART_Handle_t;

/* UART 实例枚举 */
typedef enum
{
  DRV_UART1,
  DRV_UART2,
  DRV_UART_MAX
} DRV_UART_Instance_t;

typedef enum
{
  DRV_UART_PARITY_NONE,
  DRV_UART_PARITY_EVEN,
  DRV_UART_PARITY_ODD
} UART_Parity_t;

typedef enum
{
  DRV_UART_STOPBITS_1,
  DRV_UART_STOPBITS_2
} UART_StopBits_t;

typedef struct
{
  DRV_UART_Instance_t instance;
  uint32_t baudrate;
  uint8_t data_bits;
  UART_StopBits_t stop_bits;
  UART_Parity_t parity;
} UART_Config_t;

/* 返回值约定: 0 表示成功, 负值表示失败 */
#ifndef DRV_OK
#define DRV_OK          0
#define DRV_ERROR      -1
#define DRV_BUSY       -2
#define DRV_TIMEOUT    -3
#endif

/**
 * @brief 初始化 UART
 * @param config UART 配置结构体指针
 * @return 0 成功, 负值失败
 */
int DRV_UART_Init(UART_Config_t *config);

/**
 * @brief 反初始化 UART
 * @param instance UART 实例
 * @return 0 成功, 负值失败
 */
int DRV_UART_DeInit(DRV_UART_Instance_t instance);

/**
 * @brief 获取 UART 句柄
 * @param instance UART 实例
 * @return UART 句柄, NULL 表示无效
 */
UART_Handle_t DRV_UART_GetHandle(DRV_UART_Instance_t instance);

/**
 * @brief 阻塞方式发送数据
 * @param handle UART 句柄
 * @param data 数据缓冲区指针
 * @param len 数据长度
 * @param timeout 超时时间(ms)
 * @return 0 成功, 负值失败
 */
int DRV_UART_Transmit(UART_Handle_t handle, uint8_t *data, uint16_t len,
                      uint32_t timeout);

/**
 * @brief 阻塞方式接收数据
 * @param handle UART 句柄
 * @param data 数据缓冲区指针
 * @param len 数据长度
 * @param timeout 超时时间(ms)
 * @return 0 成功, 负值失败
 */
int DRV_UART_Receive(UART_Handle_t handle, uint8_t *data, uint16_t len,
                     uint32_t timeout);

/**
 * @brief 中断方式发送数据
 * @param handle UART 句柄
 * @param data 数据缓冲区指针
 * @param len 数据长度
 * @return 0 成功, 负值失败
 */
int DRV_UART_TransmitIT(UART_Handle_t handle, uint8_t *data, uint16_t len);

/**
 * @brief 中断方式接收数据
 * @param handle UART 句柄
 * @param data 数据缓冲区指针
 * @param len 数据长度
 * @return 0 成功, 负值失败
 */
int DRV_UART_ReceiveIT(UART_Handle_t handle, uint8_t *data, uint16_t len);

/**
 * @brief 读取接收到的单字节数据
 * @param instance UART 实例
 * @param out 输出缓冲区
 * @return 1 有新数据, 0 无数据, 负值失败
 */
int DRV_UART_ReadByte(DRV_UART_Instance_t instance, uint8_t *out);

#ifdef __cplusplus
}
#endif

#endif /* DRV_UART_H */
