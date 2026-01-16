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
extern uart_desc_t debug_uart;

/**
 * @brief 通信串口描述符。
 */
extern uart_desc_t comm_uart;

/**
 * @brief ADC1描述符。
 */
extern adc_desc_t adc1;

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
