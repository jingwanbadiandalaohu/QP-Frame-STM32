#include "bsp.h"
#include "cmsis_os2.h"
#include "printf.h"
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

void PrintTask(void *argument) 
{
  char buffer[128];
  (void)argument;
  while (1) 
  {
    printf("RS485_Uart2 %f!\r\n", 3.1415926);
    sprintf(buffer, "RS232_Uart1 %f!\r\n", 3.1415926);
    HAL_UART_Transmit(&huart1, (uint8_t *)buffer, strlen(buffer), 0xFFFF);
    osDelay(1000);
  }
}
