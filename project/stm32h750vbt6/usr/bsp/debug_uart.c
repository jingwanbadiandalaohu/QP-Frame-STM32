/**
 * @file debug_uart.c
 * @author ZC (387646983@qq.com)
 * @brief 
 * @version 0.1
 * @date 2025-08-17
 * 
 * 
 */
/* ==================== [Includes] ========================================== */
#include "debug_uart.h"
#include "stdint.h"
#include <stdio.h>

/* ==================== [Defines] ========================================== */
#define USARTx                           USART1
#define USARTx_CLK_ENABLE()              __HAL_RCC_USART1_CLK_ENABLE()
#define USARTx_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOA_CLK_ENABLE()
#define USARTx_TX_PIN                    GPIO_PIN_9
#define USARTx_RX_PIN                    GPIO_PIN_10
#define USARTx_PORT                      GPIOA
#define USARTx_AF                        GPIO_AF7_USART1
/* ==================== [Macros] ============================================ */

/* ==================== [Typedefs] ========================================== */

/* ==================== [Static Prototypes] ========================================== */

/* ==================== [Static Variables] ========================================== */

/* ==================== [Static Functions] ================================== */

/* ==================== [Public Functions] ================================== */


UART_HandleTypeDef huart1;
/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
void elab_debug_uart_init(uint32_t baudrate)    
{
  huart1.Instance = USARTx;
  huart1.Init.BaudRate = baudrate;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&huart1);
}

void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  if (huart->Instance == USARTx)
  {
    USARTx_CLK_ENABLE();
    USARTx_GPIO_CLK_ENABLE();

    /* TX: PA9 */
    GPIO_InitStruct.Pin       = USARTx_TX_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = USARTx_AF;
    HAL_GPIO_Init(USARTx_PORT, &GPIO_InitStruct);

    /* RX: PA10 (same AF on H7) */
    GPIO_InitStruct.Pin       = USARTx_RX_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = USARTx_AF;
    HAL_GPIO_Init(USARTx_PORT, &GPIO_InitStruct);

    /* USER CODE BEGIN USARTx_MspInit 1 */
    /* USER CODE END USARTx_MspInit 1 */
  }
}

//printf 重定向

#if defined(__CC_ARM) // ARMCC5 (Keil MDK-ARM)
    #pragma import __use_no_semihosting
    void _sys_exit(int x)
    {
        (void)x;
    }
    struct __FILE { int handle; };
    FILE __stdout;

    int fputc(int ch, FILE *f)
    {
        (void)f;
        HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
        return ch;
    }
#elif defined(__ARMCC_VERSION) // Armclang/AC6
    __asm(".global __use_no_semihosting");
    void _sys_exit(int x)
    {
        (void)x;
    }
    FILE __stdout;

    int fputc(int ch, FILE *f)
    {
        (void)f;
        HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
        return ch;
    }
#elif defined(__GNUC__) // GCC Compiler
    int __io_putchar(int ch)
    {
        HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
        return ch;
    }

    int _write(int file, char *ptr, int len)
    {
        (void)file;
        for(int i=0;i<len;i++)
        {
           __io_putchar(*ptr++); 
        }
        return len;

    }
#endif
