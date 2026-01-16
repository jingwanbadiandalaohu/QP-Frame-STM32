#ifndef DRV_GPIO_H
#define DRV_GPIO_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct gpio_desc;
typedef struct gpio_desc *gpio_desc_t;

void gpio_init(gpio_desc_t gpio);
void gpio_set(gpio_desc_t gpio, bool state);
void gpio_on(gpio_desc_t gpio);
void gpio_off(gpio_desc_t gpio);
void gpio_toggle(gpio_desc_t gpio);
bool gpio_read(gpio_desc_t gpio);

#ifdef __cplusplus
}
#endif

#endif /* DRV_GPIO_H */
