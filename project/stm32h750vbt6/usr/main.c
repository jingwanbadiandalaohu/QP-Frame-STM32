/**
 * @file main.c
 * @author ZC (387646983@qq.com)
 * @brief 
 * @version 0.1
 * @date 2025-08-14
 * 
 * 
 */
#include "bsp.h"
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_uart.h"
#include <stdio.h>
#include <string.h>

extern UART_HandleTypeDef huart1;

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  BSP_Init();
  while (1)
  {
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

    // char str[128];
    // sprintf(str, "HELLO_H7!\r\n");
    // HAL_UART_Transmit(&huart1, (uint8_t *)str, strlen(str), 0xFFFF);

    printf("HELLO_H7!\r\n");

    HAL_Delay(1000);
  }
  /* USER CODE END 3 */
}
