#ifndef DRV_GPIO_DESC_H
#define DRV_GPIO_DESC_H

#include <stdbool.h>
#include <stdint.h>
#include "stm32h7xx_hal.h"

struct gpio_desc
{
  GPIO_TypeDef *port;
  uint16_t pin;
  GPIO_PinState on_state;
  GPIO_PinState off_state;
};

typedef struct gpio_desc *gpio_desc_t;

#endif /* DRV_GPIO_DESC_H */
