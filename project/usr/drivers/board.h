/**
 * @file    board.h
 * @author  Dylan
 * @date    2026-01-15
 * @brief   板级资源声明。
 */

#ifndef BOARD_H
#define BOARD_H

#include "drv_gpio.h"
#include "drv_uart.h"
#include "drv_adc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief LED1描述符。
 */
extern gpio_desc_t led1;

/**
 * @brief 继电器描述符。
 */
extern gpio_desc_t relay1;

/**
 * @brief 调试串口描述符。
 */
extern uart_desc_t uart2_rs485;

/**
 * @brief 通信串口描述符。
 */
extern uart_desc_t uart1_rs232;

/**
 * @brief UART1 DMA接收缓冲区。
 */
extern volatile uint8_t Uart1_rx_buf[64];

/**
 * @brief ADC描述符。
 */
extern adc_desc_t adc1;
extern adc_desc_t adc2;


#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
