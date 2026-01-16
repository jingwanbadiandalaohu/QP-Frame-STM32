/**
 * @file    board.c
 * @author  Dylan
 * @date    2026-01-15
 * @brief   板级外设描述符定义。
 *
 * @details 定义GPIO、UART、ADC等外设的描述符实例。
 */

#include "board.h"
#include "drv_gpio_desc.h"
#include "drv_uart_desc.h"
#include "drv_adc_desc.h"

/**
 * @brief LED1 GPIO描述符。
 */
static struct gpio_desc s_led1 = {
  .port = GPIOC,
  .pin = GPIO_PIN_13,
  .on_state = GPIO_PIN_RESET,
  .off_state = GPIO_PIN_SET
};

/**
 * @brief LED1描述符句柄。
 */
gpio_desc_t led1 = &s_led1;

/**
 * @brief 继电器GPIO描述符。
 */
static struct gpio_desc s_relay1 = {
  .port = GPIOE,
  .pin = GPIO_PIN_11,
  .on_state = GPIO_PIN_SET,
  .off_state = GPIO_PIN_RESET
};

/**
 * @brief 继电器描述符句柄。
 */
gpio_desc_t relay1 = &s_relay1;

/**
 * @brief 调试串口描述符。
 */
static struct uart_desc s_debug_uart = {
  .instance = USART2,
  .baudrate = 115200
};

/**
 * @brief 通信串口描述符。
 */
static struct uart_desc s_comm_uart = {
  .instance = USART1,
  .baudrate = 115200
};

/**
 * @brief 调试串口句柄。
 */
uart_desc_t debug_uart = &s_debug_uart;

/**
 * @brief 通信串口句柄。
 */
uart_desc_t comm_uart = &s_comm_uart;

/**
 * @brief ADC1 DMA缓冲区（放置在AXI SRAM）。
 *
 * @note 通过链接脚本的 .ram_axi 段放到AXI SRAM。
 */
__attribute__((section(".ram_axi"))) static uint16_t s_adc1_buffer[64];

/**
 * @brief ADC1描述符。
 */
static struct adc_desc s_adc1 = {
  .instance = ADC1,
  .channel = ADC_CHANNEL_5,
  .dma_buffer = s_adc1_buffer,
  .buffer_len = 64
};

/**
 * @brief ADC1描述符句柄。
 */
adc_desc_t adc1 = &s_adc1;
