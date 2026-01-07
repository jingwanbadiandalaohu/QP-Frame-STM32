#include "bsp.h"
#include "cmsis_os2.h"
#include "printf.h"

void BlinkTask(void *argument);
void PrintTask(void *argument);

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
  BSP_Init();

  osKernelInitialize();

  const osThreadAttr_t blinkTask_attributes = {
      .name = "BlinkTask",
      .stack_size = 128 * 4,
      .priority = (osPriority_t)osPriorityNormal,
  };
  osThreadNew(BlinkTask, NULL, &blinkTask_attributes);

  const osThreadAttr_t printTask_attributes = {
      .name = "PrintTask",
      .stack_size = 512 * 4,
      .priority = (osPriority_t)osPriorityNormal,
  };
  osThreadNew(PrintTask, NULL, &printTask_attributes);

  osKernelStart();

  while (1) {
  }
}

void BlinkTask(void *argument) 
{
  (void)argument;
  while (1) {
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    osDelay(1000);
  }
}

void PrintTask(void *argument) 
{
  (void)argument;
  while (1) {
    printf("HELLO_H7 %f!\r\n", 3.1415926);
    osDelay(500);
  }
}
