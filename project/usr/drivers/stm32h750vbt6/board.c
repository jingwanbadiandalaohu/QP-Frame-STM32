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
#include "stm32h7xx_hal_adc.h"

/**
 * @brief LED1 GPIO描述符。
 */
static struct gpio_desc s_led1 = {
  .port = GPIOC,
  .pin = GPIO_PIN_13,
  .on_state = GPIO_PIN_RESET,
  .off_state = GPIO_PIN_SET
};

// LED1描述符句柄。
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

// 继电器描述符句柄。
gpio_desc_t relay1 = &s_relay1;

/**
 * @brief UART1 DMA接收缓冲区（硬件DMA使用）
 * @note  32字节对齐确保cache一致性
 */
__attribute__((aligned(32))) __attribute__((section(".ram_d1"))) uint8_t Uart1_rx_buf[256] = {0};

/**
 * @brief UART2 DMA接收缓冲区（硬件DMA使用）
 * @note  32字节对齐确保cache一致性
 */
__attribute__((aligned(32))) __attribute__((section(".ram_d1"))) uint8_t Uart2_rx_buf[256] = {0};

/**
 * @brief UART1 环形缓冲区存储空间
 */
uint8_t Uart1_ringbuf_storage[2048] = {0};

/**
 * @brief UART2 环形缓冲区存储空间
 */
uint8_t Uart2_ringbuf_storage[2048] = {0};

/**
 * @brief ADC1 DMA缓冲区
 * @note  32字节对齐确保cache一致性
 */
__attribute__((aligned(32))) __attribute__((section(".ram_d1"))) uint16_t s_adc1_buffer[64] = {0};

/**
 * @brief ADC2 DMA缓冲区
 * @note  32字节对齐确保cache一致性
 */
__attribute__((aligned(32))) __attribute__((section(".ram_d1"))) uint16_t s_adc2_buffer[64] = {0};

/**
 * @brief 调试串口描述符。串口2-RS485
 */
static struct uart_desc s_uart2_rs485 = {
  .instance = USART2,
  .baudrate = 9600
};

// 调试串口句柄。
uart_desc_t uart2_rs485 = &s_uart2_rs485;


/**
 * @brief 通信串口描述符。串口1-RS232
 */
static struct uart_desc s_uart1_rs232 = {
  .instance = USART1,
  .baudrate = 9600
};

// 调试串口句柄。
uart_desc_t uart1_rs232 = &s_uart1_rs232;



/**
 * @brief ADC1描述符,ADC1 - PB1 ADC_CHANNEL_5 - DMA1_Stream2 - 采集下板数据
 */
static struct adc_desc s_adc1 = {
  .instance = ADC1,
  .channel = ADC_CHANNEL_5,
  .dma_buffer = s_adc1_buffer,
  .buffer_len = 64
  // hal_handle 和 dma_handle 没写 → 自动初始化为0
};
// ADC描述符句柄。
adc_desc_t adc1 = &s_adc1;



/**
 * @brief ADC2描述符。ADC2 - PA6 ADC_CHANNEL_3 - DMA1_Stream1 - 采集星电电压
 */
 static struct adc_desc s_adc2 = {
  .instance = ADC2,
  .channel = ADC_CHANNEL_3,
  .dma_buffer = s_adc2_buffer,
  .buffer_len = 64
  // hal_handle 和 dma_handle 没写 → 自动初始化为0
};
// ADC描述符句柄。
adc_desc_t adc2 = &s_adc2;

