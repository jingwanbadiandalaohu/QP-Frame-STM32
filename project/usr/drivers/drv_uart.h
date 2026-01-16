#ifndef DRV_UART_H
#define DRV_UART_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct uart_desc;
typedef struct uart_desc *uart_desc_t;

void uart_init(uart_desc_t uart);
void uart_deinit(uart_desc_t uart);
int uart_transmit(uart_desc_t uart, uint8_t *data, uint16_t len, uint32_t timeout);
int uart_receive(uart_desc_t uart, uint8_t *data, uint16_t len, uint32_t timeout);
int uart_transmit_it(uart_desc_t uart, uint8_t *data, uint16_t len);
int uart_receive_it(uart_desc_t uart, uint8_t *data, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* DRV_UART_H */
