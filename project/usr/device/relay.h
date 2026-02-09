#ifndef RELAY_H
#define RELAY_H

#include "drv_gpio.h"
#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

#define relay_init(r)     gpio_init(r)
#define relay_on(r)       gpio_on(r)
#define relay_off(r)      gpio_off(r)

#ifdef __cplusplus
}
#endif

#endif /* RELAY_H */
