#ifndef BSP_UART_H
#define BSP_UART_H

#include "stm32h7xx_hal.h"


#ifdef __cplusplus
extern "C" {
#endif

#define USARTx                           USART1
#define USARTx_CLK_ENABLE()              __HAL_RCC_USART1_CLK_ENABLE()
#define USARTx_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOA_CLK_ENABLE()
#define USARTx_TX_PIN                    GPIO_PIN_9
#define USARTx_RX_PIN                    GPIO_PIN_10
#define USARTx_PORT                      GPIOA
#define USARTx_AF                        GPIO_AF7_USART1

#define USART2x                          USART2
#define USART2x_CLK_ENABLE()             __HAL_RCC_USART2_CLK_ENABLE()
#define USART2x_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE()
#define USART2x_TX_PIN                   GPIO_PIN_2
#define USART2x_RX_PIN                   GPIO_PIN_3
#define USART2x_PORT                     GPIOA
#define USART2x_AF                       GPIO_AF7_USART2


void MX_Uart1_Init(uint32_t baudrate);
void MX_Uart2_Init(uint32_t baudrate);
int debug_uart1_read(uint8_t *out);
int debug_uart2_read(uint8_t *out);


#ifdef __cplusplus
}
#endif

#endif /* BSP_UART_H */

