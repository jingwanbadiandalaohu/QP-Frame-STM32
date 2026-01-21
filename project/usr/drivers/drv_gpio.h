#ifndef DRV_GPIO_H
#define DRV_GPIO_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// 前向声明,让应用层可以使用gpio_desc_t指针类型,但看不到结构体内部（信息隐藏），前向声明不是必须的，但加上是好习惯！
struct gpio_desc;
typedef struct gpio_desc *gpio_desc_t;// 定义指针类型


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
