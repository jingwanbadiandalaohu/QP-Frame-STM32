/**
 * @file    drv_uart.h
 * @author  Dylan
 * @date    2026-01-15
 * @brief   串口驱动接口
 */

#ifndef DRV_UART_H
#define DRV_UART_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// 前向声明：告诉编译器有这个结构体，但不暴露内部细节
struct uart_desc;

// 定义指针类型：应用层只能使用指针，看不到结构体内部
// app和设备层只包含 drv_uart.h，通过 uart_desc_t 不透明指针操作串口！
typedef struct uart_desc *uart_desc_t;

/**
 * @brief   初始化UART
 *
 * @param[in]   uart            UART描述符
 * @param[in]   ringbuf_storage 环形缓冲区存储空间指针
 * @param[in]   ringbuf_size    环形缓冲区大小
 *
 * @return  None
 */
void uart_init(uart_desc_t uart, uint8_t *ringbuf_storage, uint32_t ringbuf_size);

/**
 * @brief   阻塞方式发送数据
 *
 * @param[in]   uart     UART描述符
 * @param[in]   data     发送数据指针
 * @param[in]   len      发送长度
 * @param[in]   timeout  超时计数
 *
 * @retval  0   成功
 * @retval  -1  失败
 */
int uart_transmit(uart_desc_t uart, uint8_t *data, uint16_t len, uint32_t timeout);

/**
 * @brief   阻塞方式接收数据
 *
 * @param[in]   uart     UART描述符
 * @param[out]  data     接收数据指针
 * @param[in]   len      接收长度
 * @param[in]   timeout  超时计数
 *
 * @retval  0   成功
 * @retval  -1  失败
 */
int uart_receive(uart_desc_t uart, uint8_t *data, uint16_t len, uint32_t timeout);

/**
 * @brief   中断方式发送数据
 *
 * @param[in]   uart  UART描述符
 * @param[in]   data  发送数据指针
 * @param[in]   len   发送长度
 *
 * @retval  0   成功
 * @retval  -1  失败
 */
int uart_transmit_it(uart_desc_t uart, uint8_t *data, uint16_t len);

/**
 * @brief   中断方式接收数据
 *
 * @param[in]   uart  UART描述符
 * @param[out]  data  接收数据指针
 * @param[in]   len   接收长度
 *
 * @retval  0   成功
 * @retval  -1  失败
 */
int uart_receive_it(uart_desc_t uart, uint8_t *data, uint16_t len);

/**
 * @brief   从环形缓冲区读取数据
 *
 * @param[in]   uart  UART描述符
 * @param[out]  data  接收数据指针
 * @param[in]   len   期望读取的长度
 *
 * @return  实际读取的字节数
 */
uint32_t uart_read(uart_desc_t uart, uint8_t *data, uint32_t len);

/**
 * @brief   获取环形缓冲区可用数据长度
 *
 * @param[in]   uart  UART描述符
 *
 * @return  可读取的字节数
 */
uint32_t uart_get_available(uart_desc_t uart);

/**
 * @brief   清空接收环形缓冲区
 *
 * @param[in]   uart  UART描述符
 *
 * @return  None
 */
void uart_flush_rx(uart_desc_t uart);

#ifdef __cplusplus
}
#endif

#endif /* DRV_UART_H */
