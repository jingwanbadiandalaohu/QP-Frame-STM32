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
#include "printf.h"

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
    printf("HELLO_H7 %f!\r\n", 3.1415926);

    HAL_Delay(1000);
  }
  /* USER CODE END 3 */
}
