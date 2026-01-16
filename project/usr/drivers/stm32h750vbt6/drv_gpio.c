/**
 * @file    drv_gpio.c
 * @author  Dylan
 * @date    2026-01-15
 * @brief   GPIO驱动实现。
 */

#include "drv_gpio.h"
#include "drv_gpio_desc.h"

/**
 * @brief   初始化GPIO。
 *
 * @param[in]   gpio  GPIO描述符。
 * @return  无
 */
void gpio_init(gpio_desc_t gpio)
{
  GPIO_InitTypeDef init = {0};

  if(gpio == NULL)
  {
    return;
  }

  // 根据端口使能时钟。
  if(gpio->port == GPIOA) __HAL_RCC_GPIOA_CLK_ENABLE();
  else if(gpio->port == GPIOB) __HAL_RCC_GPIOB_CLK_ENABLE();
  else if(gpio->port == GPIOC) __HAL_RCC_GPIOC_CLK_ENABLE();
  else if(gpio->port == GPIOD) __HAL_RCC_GPIOD_CLK_ENABLE();
  else if(gpio->port == GPIOE) __HAL_RCC_GPIOE_CLK_ENABLE();

  init.Pin = gpio->pin;
  init.Mode = GPIO_MODE_OUTPUT_PP;
  init.Pull = GPIO_NOPULL;
  init.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(gpio->port, &init);

  HAL_GPIO_WritePin(gpio->port, gpio->pin, gpio->off_state);
}

/**
 * @brief   设置GPIO输出状态。
 *
 * @param[in]   gpio   GPIO描述符。
 * @param[in]   state  目标状态。
 * @return  无
 */
void gpio_set(gpio_desc_t gpio, bool state)
{
  if(gpio == NULL)
  {
    return;
  }

  HAL_GPIO_WritePin(gpio->port, gpio->pin,
                    state ? gpio->on_state : gpio->off_state);
}

/**
 * @brief   置位GPIO输出。
 *
 * @param[in]   gpio  GPIO描述符。
 * @return  无
 */
void gpio_on(gpio_desc_t gpio)
{
  if(gpio == NULL)
  {
    return;
  }

  HAL_GPIO_WritePin(gpio->port, gpio->pin, gpio->on_state);
}

/**
 * @brief   复位GPIO输出。
 *
 * @param[in]   gpio  GPIO描述符。
 * @return  无
 */
void gpio_off(gpio_desc_t gpio)
{
  if(gpio == NULL)
  {
    return;
  }

  HAL_GPIO_WritePin(gpio->port, gpio->pin, gpio->off_state);
}

/**
 * @brief   翻转GPIO输出。
 *
 * @param[in]   gpio  GPIO描述符。
 * @return  无
 */
void gpio_toggle(gpio_desc_t gpio)
{
  if(gpio == NULL)
  {
    return;
  }

  HAL_GPIO_TogglePin(gpio->port, gpio->pin);
}

/**
 * @brief   读取GPIO输入状态。
 *
 * @param[in]   gpio  GPIO描述符。
 * @return  bool 电平状态。
 */
bool gpio_read(gpio_desc_t gpio)
{
  if(gpio == NULL)
  {
    return false;
  }

  return HAL_GPIO_ReadPin(gpio->port, gpio->pin) == gpio->on_state;
}
