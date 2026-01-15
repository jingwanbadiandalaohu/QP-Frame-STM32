/**
 * @file drv_gpio_impl.c
 * @brief STM32H7 平台 GPIO 驱动实现
 */

#include "drv_gpio.h"
#include "platform_config.h"

GPIO_Handle_t DRV_GPIOA = (GPIO_Handle_t)GPIOA;
GPIO_Handle_t DRV_GPIOB = (GPIO_Handle_t)GPIOB;
GPIO_Handle_t DRV_GPIOC = (GPIO_Handle_t)GPIOC;
GPIO_Handle_t DRV_GPIOD = (GPIO_Handle_t)GPIOD;
GPIO_Handle_t DRV_GPIOE = (GPIO_Handle_t)GPIOE;

static void DRV_GPIO_EnableClock(GPIO_TypeDef *port)
{
  if(port == GPIOA)
  {
    __HAL_RCC_GPIOA_CLK_ENABLE();
  }
  else if(port == GPIOB)
  {
    __HAL_RCC_GPIOB_CLK_ENABLE();
  }
  else if(port == GPIOC)
  {
    __HAL_RCC_GPIOC_CLK_ENABLE();
  }
  else if(port == GPIOD)
  {
    __HAL_RCC_GPIOD_CLK_ENABLE();
  }
  else if(port == GPIOE)
  {
    __HAL_RCC_GPIOE_CLK_ENABLE();
  }
}

/**
 * @brief 初始化 GPIO 引脚
 */
int DRV_GPIO_Init(GPIO_Config_t *config)
{
  if(config == NULL)
  {
    return DRV_ERROR;
  }

  GPIO_TypeDef *port = (GPIO_TypeDef *)config->port;
  GPIO_InitTypeDef gpio_init = {0};

  gpio_init.Pin = config->pin;

  /* 模式映射 */
  switch(config->mode)
  {
    case DRV_GPIO_MODE_INPUT:
      gpio_init.Mode = GPIO_MODE_INPUT;
      break;
    case DRV_GPIO_MODE_OUTPUT_PP:
      gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
      break;
    case DRV_GPIO_MODE_OUTPUT_OD:
      gpio_init.Mode = GPIO_MODE_OUTPUT_OD;
      break;
    case DRV_GPIO_MODE_AF_PP:
      gpio_init.Mode = GPIO_MODE_AF_PP;
      break;
    case DRV_GPIO_MODE_AF_OD:
      gpio_init.Mode = GPIO_MODE_AF_OD;
      break;
    case DRV_GPIO_MODE_ANALOG:
      gpio_init.Mode = GPIO_MODE_ANALOG;
      break;
    default:
      return DRV_ERROR;
  }

  /* 上下拉映射 */
  switch(config->pull)
  {
    case DRV_GPIO_PULL_NONE:
      gpio_init.Pull = GPIO_NOPULL;
      break;
    case DRV_GPIO_PULL_UP:
      gpio_init.Pull = GPIO_PULLUP;
      break;
    case DRV_GPIO_PULL_DOWN:
      gpio_init.Pull = GPIO_PULLDOWN;
      break;
    default:
      gpio_init.Pull = GPIO_NOPULL;
      break;
  }

  /* 速度映射 */
  switch(config->speed)
  {
    case DRV_GPIO_SPEED_LOW:
      gpio_init.Speed = GPIO_SPEED_FREQ_LOW;
      break;
    case DRV_GPIO_SPEED_MEDIUM:
      gpio_init.Speed = GPIO_SPEED_FREQ_MEDIUM;
      break;
    case DRV_GPIO_SPEED_HIGH:
      gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;
      break;
    case DRV_GPIO_SPEED_VERY_HIGH:
      gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
      break;
    default:
      gpio_init.Speed = GPIO_SPEED_FREQ_LOW;
      break;
  }

  DRV_GPIO_EnableClock(port);
  HAL_GPIO_Init(port, &gpio_init);

  return DRV_OK;
}

/**
 * @brief 反初始化 GPIO 引脚
 */
int DRV_GPIO_DeInit(GPIO_Handle_t port, uint16_t pin)
{
  if(port == NULL)
  {
    return DRV_ERROR;
  }

  HAL_GPIO_DeInit((GPIO_TypeDef *)port, pin);

  return DRV_OK;
}

/**
 * @brief 写 GPIO 引脚状态
 */
int DRV_GPIO_Write(GPIO_Handle_t port, uint16_t pin, uint8_t state)
{
  if(port == NULL)
  {
    return DRV_ERROR;
  }

  HAL_GPIO_WritePin((GPIO_TypeDef *)port, pin,
                    state ? GPIO_PIN_SET : GPIO_PIN_RESET);

  return DRV_OK;
}

/**
 * @brief 读 GPIO 引脚状态
 */
int DRV_GPIO_Read(GPIO_Handle_t port, uint16_t pin, uint8_t *state)
{
  if(port == NULL || state == NULL)
  {
    return DRV_ERROR;
  }

  *state = (uint8_t)HAL_GPIO_ReadPin((GPIO_TypeDef *)port, pin);

  return DRV_OK;
}

/**
 * @brief 翻转 GPIO 引脚状态
 */
int DRV_GPIO_Toggle(GPIO_Handle_t port, uint16_t pin)
{
  if(port == NULL)
  {
    return DRV_ERROR;
  }

  HAL_GPIO_TogglePin((GPIO_TypeDef *)port, pin);

  return DRV_OK;
}
