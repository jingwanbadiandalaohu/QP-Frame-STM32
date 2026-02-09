#ifndef DRV_GPIO_H
#define DRV_GPIO_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// 前向声明：告诉编译器有这个结构体，但不暴露内部细节
struct gpio_desc;

// 定义指针类型：应用层只能使用指针，看不到结构体内部
// app和设备层只包含 drv_gpio.h，通过 gpio_desc_t 不透明指针操作串口！
typedef struct gpio_desc *gpio_desc_t;



// 类似于"成员方法"
void gpio_init(gpio_desc_t gpio);                // 构造函数
void gpio_set(gpio_desc_t gpio, bool state); 
void gpio_on(gpio_desc_t gpio);
void gpio_off(gpio_desc_t gpio);
void gpio_toggle(gpio_desc_t gpio);
bool gpio_read(gpio_desc_t gpio);

#ifdef __cplusplus
}
#endif

#endif /* DRV_GPIO_H */
