/**
 * @file    modbus.h
 * @author  Dylan
 * @date    2026-01-28
 * @brief   Modbus从机设备层接口
 *
 * @details 基于nanoMODBUS库实现的Modbus RTU从机，
 *          适配DMA+IDLE+环形缓冲区的串口驱动，
 *          当前仅实现功能码0x03（读保持寄存器）
 */

#ifndef MODBUS_H
#define MODBUS_H

#include <stdint.h>
#include "nanomodbus.h"
#include "drv_uart.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Modbus从机设备描述符
 */
typedef struct
{
  nmbs_t nmbs;           /**< nanoMODBUS实例 */
  uart_desc_t uart;      /**< 串口描述符 */
  uint8_t slave_addr;    /**< 从机地址 */
  uint16_t *regs;        /**< 保持寄存器数组指针 */
  uint16_t regs_count;   /**< 保持寄存器数量 */
} modbus_dev_t;

/**
 * @brief   初始化Modbus从机
 *
 * @param[in]   dev         Modbus设备描述符指针
 * @param[in]   uart        串口描述符
 * @param[in]   slave_addr  从机地址（1-247）
 * @param[in]   regs        保持寄存器数组指针
 * @param[in]   regs_count  保持寄存器数量
 *
 * @return  None
 */
void modbus_init(modbus_dev_t *dev, uart_desc_t uart, uint8_t slave_addr,
                 uint16_t *regs, uint16_t regs_count);

/**
 * @brief   Modbus从机轮询处理函数
 *
 * @details 需要在任务中循环调用，处理接收到的Modbus请求
 *
 * @param[in]   dev  Modbus设备描述符指针
 *
 * @return  NMBS_ERROR_NONE 成功，其他值为错误码
 */
nmbs_error modbus_poll(modbus_dev_t *dev);

/**
 * @brief   设置读取超时时间
 *
 * @param[in]   dev         Modbus设备描述符指针
 * @param[in]   timeout_ms  超时时间（毫秒），<0表示无限等待
 *
 * @return  None
 */
void modbus_set_read_timeout(modbus_dev_t *dev, int32_t timeout_ms);

/**
 * @brief   设置字节间超时时间
 *
 * @param[in]   dev         Modbus设备描述符指针
 * @param[in]   timeout_ms  超时时间（毫秒），<0表示无限等待
 *
 * @return  None
 */
void modbus_set_byte_timeout(modbus_dev_t *dev, int32_t timeout_ms);

#ifdef __cplusplus
}
#endif

#endif /* MODBUS_H */
