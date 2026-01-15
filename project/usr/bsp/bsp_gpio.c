/**
 * @file bsp_gpio.c
 * @brief GPIO 板级支持包实现
 */

#include "bsp_gpio.h"
#include "drv_gpio.h"

/* ==================== 初始化函数 ==================== */

/**
 * @brief GPIO 初始化函数
 */
void BSP_GPIO_Init(void)
{
  /* 复用同一配置结构体，依次配置多个引脚 */
  GPIO_Config_t gpio_config = {0};

  /* LED1: PC13 推挽输出 */
  gpio_config.port = LED1_PORT;
  gpio_config.pin = LED1_PIN;
  gpio_config.mode = DRV_GPIO_MODE_OUTPUT_PP;
  gpio_config.pull = DRV_GPIO_PULL_NONE;
  gpio_config.speed = DRV_GPIO_SPEED_LOW;
  DRV_GPIO_Init(&gpio_config);

  /* 继电器: PE11 推挽输出 */
  gpio_config.port = Relay_PORT;
  gpio_config.pin = Relay_PIN;
  gpio_config.mode = DRV_GPIO_MODE_OUTPUT_PP;
  gpio_config.pull = DRV_GPIO_PULL_NONE;
  gpio_config.speed = DRV_GPIO_SPEED_LOW;
  DRV_GPIO_Init(&gpio_config);

  /* 默认状态: LED1 亮, 继电器打开 */
  DRV_GPIO_Write(LED1_PORT, LED1_PIN, 0);
  DRV_GPIO_Write(Relay_PORT, Relay_PIN, 1);
}

/* ==================== 便捷函数 ==================== */

void BSP_LED1_Toggle(void)
{
  /* LED1 状态翻转 */
  DRV_GPIO_Toggle(LED1_PORT, LED1_PIN);
}

void BSP_LED1_On(void)
{
  /* LED1 低电平点亮 */
  DRV_GPIO_Write(LED1_PORT, LED1_PIN, 0);
}

void BSP_LED1_Off(void)
{
  /* LED1 高电平熄灭 */
  DRV_GPIO_Write(LED1_PORT, LED1_PIN, 1);
}

void BSP_Relay_On(void)
{
  /* 继电器高电平吸合 */
  DRV_GPIO_Write(Relay_PORT, Relay_PIN, 1);
}

void BSP_Relay_Off(void)
{
  /* 继电器低电平释放 */
  DRV_GPIO_Write(Relay_PORT, Relay_PIN, 0);
}
