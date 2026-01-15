/**
 * @file drv_gpio.h
 * @brief GPIO driver abstraction layer (platform-independent interface)
 */

#ifndef DRV_GPIO_H
#define DRV_GPIO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct GPIO_Port GPIO_Port_t;

typedef enum
{
  DRV_GPIO_MODE_INPUT,
  DRV_GPIO_MODE_OUTPUT_PP,
  DRV_GPIO_MODE_OUTPUT_OD,
  DRV_GPIO_MODE_AF_PP,
  DRV_GPIO_MODE_AF_OD,
  DRV_GPIO_MODE_ANALOG
} DRV_GPIO_Mode_t;

typedef enum
{
  DRV_GPIO_PULL_NONE,
  DRV_GPIO_PULL_UP,
  DRV_GPIO_PULL_DOWN
} DRV_GPIO_Pull_t;

typedef enum
{
  DRV_GPIO_SPEED_LOW,
  DRV_GPIO_SPEED_MEDIUM,
  DRV_GPIO_SPEED_HIGH,
  DRV_GPIO_SPEED_VERY_HIGH
} DRV_GPIO_Speed_t;

typedef struct
{
  uint16_t pin;
  DRV_GPIO_Mode_t mode;
  DRV_GPIO_Pull_t pull;
  DRV_GPIO_Speed_t speed;
} DRV_GPIO_Config_t;

/* Return codes */
#define DRV_OK          0
#define DRV_ERROR      -1
#define DRV_BUSY       -2
#define DRV_TIMEOUT    -3

/* Pin definitions */
#define DRV_PIN_0    (1U << 0)
#define DRV_PIN_1    (1U << 1)
#define DRV_PIN_2    (1U << 2)
#define DRV_PIN_3    (1U << 3)
#define DRV_PIN_4    (1U << 4)
#define DRV_PIN_5    (1U << 5)
#define DRV_PIN_6    (1U << 6)
#define DRV_PIN_7    (1U << 7)
#define DRV_PIN_8    (1U << 8)
#define DRV_PIN_9    (1U << 9)
#define DRV_PIN_10   (1U << 10)
#define DRV_PIN_11   (1U << 11)
#define DRV_PIN_12   (1U << 12)
#define DRV_PIN_13   (1U << 13)
#define DRV_PIN_14   (1U << 14)
#define DRV_PIN_15   (1U << 15)

typedef struct GPIO_Operations
{
  int (*init)(GPIO_Port_t *port, DRV_GPIO_Config_t *config);
  int (*deinit)(GPIO_Port_t *port, uint16_t pin);
  int (*write)(GPIO_Port_t *port, uint16_t pin, uint8_t state);
  int (*read)(GPIO_Port_t *port, uint16_t pin, uint8_t *state);
  int (*toggle)(GPIO_Port_t *port, uint16_t pin);
} GPIO_Ops_t;

struct GPIO_Port
{
  const char *name;
  void *hw_base;
  GPIO_Ops_t *ops;
};

static inline int gpio_init(GPIO_Port_t *port, DRV_GPIO_Config_t *config)
{
  if(port && port->ops && port->ops->init)
  {
    return port->ops->init(port, config);
  }
  return DRV_ERROR;
}

static inline int gpio_deinit(GPIO_Port_t *port, uint16_t pin)
{
  if(port && port->ops && port->ops->deinit)
  {
    return port->ops->deinit(port, pin);
  }
  return DRV_ERROR;
}

static inline int gpio_write(GPIO_Port_t *port, uint16_t pin, uint8_t state)
{
  if(port && port->ops && port->ops->write)
  {
    return port->ops->write(port, pin, state);
  }
  return DRV_ERROR;
}

static inline int gpio_read(GPIO_Port_t *port, uint16_t pin, uint8_t *state)
{
  if(port && port->ops && port->ops->read)
  {
    return port->ops->read(port, pin, state);
  }
  return DRV_ERROR;
}

static inline int gpio_toggle(GPIO_Port_t *port, uint16_t pin)
{
  if(port && port->ops && port->ops->toggle)
  {
    return port->ops->toggle(port, pin);
  }
  return DRV_ERROR;
}

/* Platform port instances */
extern GPIO_Port_t *drv_gpioa;
extern GPIO_Port_t *drv_gpiob;
extern GPIO_Port_t *drv_gpioc;
extern GPIO_Port_t *drv_gpiod;
extern GPIO_Port_t *drv_gpioe;

#ifdef __cplusplus
}
#endif

#endif /* DRV_GPIO_H */
