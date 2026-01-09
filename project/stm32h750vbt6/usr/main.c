#include "bsp.h"
#include "cmsis_os2.h"
#include "printf.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

void BlinkTask(void *argument);
void PrintTask(void *argument);

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) 
{
  BSP_Init();
  osKernelInitialize();

  const osThreadAttr_t blinkTask_attributes = 
  {
      .name = "BlinkTask",
      .stack_size = 128 * 4,
      .priority = (osPriority_t)osPriorityNormal,
  };
  osThreadNew(BlinkTask, NULL, &blinkTask_attributes);

  const osThreadAttr_t printTask_attributes = 
  {
      .name = "PrintTask",
      .stack_size = 512 * 4,
      .priority = (osPriority_t)osPriorityNormal,
  };
  osThreadNew(PrintTask, NULL, &printTask_attributes);

  osKernelStart();

  while (1)
  {

  }
}

void BlinkTask(void *argument) 
{
  (void)argument;
  while (1)
  {
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    osDelay(1000);
  }
}

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;


void PrintTask(void *argument) 
{
  char buffer[64];
  uint8_t byte;
  (void)argument;
  while (1) 
  {
    if (debug_uart1_read(&byte)) {
      int len = sprintf(buffer, "RS232_Uart1 %x\r\n", byte);
      HAL_UART_Transmit(&huart1, (uint8_t *)buffer, (uint16_t)len, 0xFFFF);
    }
    if (debug_uart2_read(&byte)) {
      int len = sprintf(buffer, "RS485_Uart2 %x\r\n", byte);
      HAL_UART_Transmit(&huart2, (uint8_t *)buffer, (uint16_t)len, 0xFFFF);
    }
    osDelay(1);
  }
}
