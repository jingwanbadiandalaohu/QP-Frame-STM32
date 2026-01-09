/**
 * @file debug_uart.c
 * @author dylan
 * @brief 
 * @version 0.1
 * @date 2026-1-8
 * 
 * 
 */
/* ==================== [Includes] ========================================== */
#include "debug_uart.h"
#include "stdint.h"

/* ==================== [Defines] ========================================== */
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
/* ==================== [Macros] ============================================ */

/* ==================== [Typedefs] ========================================== */

/* ==================== [Static Prototypes] ========================================== */

/* ==================== [Static Variables] ========================================== */

/* ==================== [Static Functions] ================================== */

/* ==================== [Public Functions] ================================== */


UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
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

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
void elab_debug_uart2_init(uint32_t baudrate)
{
  huart2.Instance = USART2x;
  huart2.Init.BaudRate = baudrate;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&huart2);
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
  else if (huart->Instance == USART2x)
  {
    USART2x_CLK_ENABLE();
    USART2x_GPIO_CLK_ENABLE();

    /* TX: PA2 */
    GPIO_InitStruct.Pin       = USART2x_TX_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = USART2x_AF;
    HAL_GPIO_Init(USART2x_PORT, &GPIO_InitStruct);

    /* RX: PA3 */
    GPIO_InitStruct.Pin       = USART2x_RX_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = USART2x_AF;
    HAL_GPIO_Init(USART2x_PORT, &GPIO_InitStruct);

    /* USER CODE BEGIN USART2_MspInit 1 */
    /* USER CODE END USART2_MspInit 1 */
  }
}

void _putchar(char character)
{
  HAL_UART_Transmit(&huart2, (uint8_t *)&character, 1, 0xFFFF);
}
