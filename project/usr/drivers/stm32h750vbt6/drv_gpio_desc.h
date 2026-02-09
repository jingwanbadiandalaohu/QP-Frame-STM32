#ifndef DRV_GPIO_DESC_H
#define DRV_GPIO_DESC_H

#include <stdbool.h>
#include <stdint.h>
#include "stm32h7xx_hal.h"

// - 类似于"类定义"
struct gpio_desc
{
  GPIO_TypeDef *port;     // 成员变量（属性）
  uint16_t pin;           
  GPIO_PinState on_state;   
  GPIO_PinState off_state;
};

//给"指向gpio_desc结构体指针"起了一个简短的别名gpio_desc_t
typedef struct gpio_desc *gpio_desc_t; // 类似于"对象指针"

#endif /* DRV_GPIO_DESC_H */
