#ifndef LED_H
#define LED_H

#include "drv_gpio.h"
#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

#define led_init(led)     gpio_init(led)
#define led_on(led)       gpio_on(led)
#define led_off(led)      gpio_off(led)
#define led_toggle(led)   gpio_toggle(led)

#ifdef __cplusplus
}
#endif

#endif /* LED_H */
