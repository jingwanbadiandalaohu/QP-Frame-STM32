/**
 * @file drv_gpio_impl.c
 * @brief STM32H7 GPIO driver implementation
 */

#include "drv_gpio.h"
#include "platform_config.h"

static void drv_gpio_enable_clock(GPIO_TypeDef *port)
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

static int stm32h7_gpio_init(GPIO_Port_t *port, DRV_GPIO_Config_t *config)
{
  GPIO_InitTypeDef gpio_init = {0};
  GPIO_TypeDef *hw_port = NULL;

  if(port == NULL || config == NULL)
  {
    return DRV_ERROR;
  }

  hw_port = (GPIO_TypeDef *)port->hw_base;
  gpio_init.Pin = config->pin;

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

  drv_gpio_enable_clock(hw_port);
  HAL_GPIO_Init(hw_port, &gpio_init);

  return DRV_OK;
}

static int stm32h7_gpio_deinit(GPIO_Port_t *port, uint16_t pin)
{
  if(port == NULL)
  {
    return DRV_ERROR;
  }

  HAL_GPIO_DeInit((GPIO_TypeDef *)port->hw_base, pin);

  return DRV_OK;
}

static int stm32h7_gpio_write(GPIO_Port_t *port, uint16_t pin, uint8_t state)
{
  if(port == NULL)
  {
    return DRV_ERROR;
  }

  HAL_GPIO_WritePin((GPIO_TypeDef *)port->hw_base, pin,
                    state ? GPIO_PIN_SET : GPIO_PIN_RESET);

  return DRV_OK;
}

static int stm32h7_gpio_read(GPIO_Port_t *port, uint16_t pin, uint8_t *state)
{
  if(port == NULL || state == NULL)
  {
    return DRV_ERROR;
  }

  *state = (uint8_t)HAL_GPIO_ReadPin((GPIO_TypeDef *)port->hw_base, pin);

  return DRV_OK;
}

static int stm32h7_gpio_toggle(GPIO_Port_t *port, uint16_t pin)
{
  if(port == NULL)
  {
    return DRV_ERROR;
  }

  HAL_GPIO_TogglePin((GPIO_TypeDef *)port->hw_base, pin);

  return DRV_OK;
}

static GPIO_Ops_t stm32h7_gpio_ops =
{
  .init = stm32h7_gpio_init,
  .deinit = stm32h7_gpio_deinit,
  .write = stm32h7_gpio_write,
  .read = stm32h7_gpio_read,
  .toggle = stm32h7_gpio_toggle
};

static GPIO_Port_t gpioa_device = {"GPIOA", GPIOA, &stm32h7_gpio_ops};
static GPIO_Port_t gpiob_device = {"GPIOB", GPIOB, &stm32h7_gpio_ops};
static GPIO_Port_t gpioc_device = {"GPIOC", GPIOC, &stm32h7_gpio_ops};
static GPIO_Port_t gpiod_device = {"GPIOD", GPIOD, &stm32h7_gpio_ops};
static GPIO_Port_t gpioe_device = {"GPIOE", GPIOE, &stm32h7_gpio_ops};

GPIO_Port_t *drv_gpioa = &gpioa_device;
GPIO_Port_t *drv_gpiob = &gpiob_device;
GPIO_Port_t *drv_gpioc = &gpioc_device;
GPIO_Port_t *drv_gpiod = &gpiod_device;
GPIO_Port_t *drv_gpioe = &gpioe_device;
