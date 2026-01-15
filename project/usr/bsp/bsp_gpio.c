/**
 * @file    bsp_gpio.c
 * @author  Dylan
 * @date    2026-01-15
 * @brief   GPIO板级支持包实现
 *
 * @details 实现LED和继电器的初始化及控制函数。
 */

#include "bsp_gpio.h"
#include "drv_gpio.h"

/* ==================== 初始化函数 ==================== */

/**
 * @brief   GPIO初始化函数
 *
 * @details 配置LED1(PC13)和继电器(PE11)为推挽输出模式，
 *          默认LED1点亮、继电器吸合。
 *
 * @param   None
 * @return  None
 */
void BSP_GPIO_Init(void)
{
  /* 复用同一配置结构体，依次配置多个引脚 */
  DRV_GPIO_Config_t gpio_config = {0};

  /* LED1: PC13 推挽输出 */
  gpio_config.pin = LED1_PIN;
  gpio_config.mode = DRV_GPIO_MODE_OUTPUT_PP;
  gpio_config.pull = DRV_GPIO_PULL_NONE;
  gpio_config.speed = DRV_GPIO_SPEED_LOW;
  gpio_init(LED1_PORT, &gpio_config);

  /* 继电器: PE11 推挽输出 */
  gpio_config.pin = Relay_PIN;
  gpio_config.mode = DRV_GPIO_MODE_OUTPUT_PP;
  gpio_config.pull = DRV_GPIO_PULL_NONE;
  gpio_config.speed = DRV_GPIO_SPEED_LOW;
  gpio_init(Relay_PORT, &gpio_config);

  /* 默认状态: LED1 亮, 继电器打开 */
  gpio_write(LED1_PORT, LED1_PIN, 0);
  gpio_write(Relay_PORT, Relay_PIN, 1);
}

/* ==================== 便捷函数 ==================== */

/**
 * @brief   LED1状态翻转
 *
 * @param   None
 * @return  None
 */
void BSP_LED1_Toggle(void)
{
  gpio_toggle(LED1_PORT, LED1_PIN);
}

/**
 * @brief   LED1点亮
 *
 * @param   None
 * @return  None
 *
 * @note    LED1低电平点亮
 */
void BSP_LED1_On(void)
{
  gpio_write(LED1_PORT, LED1_PIN, 0);
}

/**
 * @brief   LED1熄灭
 *
 * @param   None
 * @return  None
 *
 * @note    LED1高电平熄灭
 */
void BSP_LED1_Off(void)
{
  gpio_write(LED1_PORT, LED1_PIN, 1);
}

/**
 * @brief   继电器吸合
 *
 * @param   None
 * @return  None
 *
 * @note    继电器高电平吸合
 */
void BSP_Relay_On(void)
{
  gpio_write(Relay_PORT, Relay_PIN, 1);
}

/**
 * @brief   继电器释放
 *
 * @param   None
 * @return  None
 *
 * @note    继电器低电平释放
 */
void BSP_Relay_Off(void)
{
  gpio_write(Relay_PORT, Relay_PIN, 0);
}
