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


void uart_init(uart_desc_t uart);
int uart_transmit(uart_desc_t uart, uint8_t *data, uint16_t len, uint32_t timeout);
int uart_receive(uart_desc_t uart, uint8_t *data, uint16_t len, uint32_t timeout);
int uart_transmit_it(uart_desc_t uart, uint8_t *data, uint16_t len);
int uart_receive_it(uart_desc_t uart, uint8_t *data, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* DRV_UART_H */
