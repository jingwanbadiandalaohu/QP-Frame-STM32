/**
 * @file drv_gpio.h
 * @brief GPIO 驱动层抽象接口
 * @note 平台无关的 GPIO 操作接口定义
 */

#ifndef DRV_GPIO_H
#define DRV_GPIO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 类型定义 */
typedef void *GPIO_Handle_t;

/* 平台端口定义（由平台实现提供） */
extern GPIO_Handle_t DRV_GPIOA;
extern GPIO_Handle_t DRV_GPIOB;
extern GPIO_Handle_t DRV_GPIOC;
extern GPIO_Handle_t DRV_GPIOD;
extern GPIO_Handle_t DRV_GPIOE;

/* 引脚定义 */
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

typedef enum
{
  DRV_GPIO_MODE_INPUT,
  DRV_GPIO_MODE_OUTPUT_PP,
  DRV_GPIO_MODE_OUTPUT_OD,
  DRV_GPIO_MODE_AF_PP,
  DRV_GPIO_MODE_AF_OD,
  DRV_GPIO_MODE_ANALOG
} GPIO_Mode_t;

typedef enum
{
  DRV_GPIO_PULL_NONE,
  DRV_GPIO_PULL_UP,
  DRV_GPIO_PULL_DOWN
} GPIO_Pull_t;

typedef enum
{
  DRV_GPIO_SPEED_LOW,
  DRV_GPIO_SPEED_MEDIUM,
  DRV_GPIO_SPEED_HIGH,
  DRV_GPIO_SPEED_VERY_HIGH
} GPIO_Speed_t;

typedef struct
{
  GPIO_Handle_t port;
  uint16_t pin;
  GPIO_Mode_t mode;
  GPIO_Pull_t pull;
  GPIO_Speed_t speed;
} GPIO_Config_t;

/* 返回值约定: 0 表示成功, 负值表示失败 */
#define DRV_OK          0
#define DRV_ERROR      -1
#define DRV_BUSY       -2
#define DRV_TIMEOUT    -3

/**
 * @brief 初始化 GPIO 引脚
 * @param config GPIO 配置结构体指针
 * @return 0 成功, 负值失败
 */
int DRV_GPIO_Init(GPIO_Config_t *config);

/**
 * @brief 反初始化 GPIO 引脚
 * @param port GPIO 端口句柄
 * @param pin GPIO 引脚号
 * @return 0 成功, 负值失败
 */
int DRV_GPIO_DeInit(GPIO_Handle_t port, uint16_t pin);

/**
 * @brief 写 GPIO 引脚状态
 * @param port GPIO 端口句柄
 * @param pin GPIO 引脚号
 * @param state 引脚状态 (0 或 1)
 * @return 0 成功, 负值失败
 */
int DRV_GPIO_Write(GPIO_Handle_t port, uint16_t pin, uint8_t state);

/**
 * @brief 读 GPIO 引脚状态
 * @param port GPIO 端口句柄
 * @param pin GPIO 引脚号
 * @param state 存储引脚状态的指针
 * @return 0 成功, 负值失败
 */
int DRV_GPIO_Read(GPIO_Handle_t port, uint16_t pin, uint8_t *state);

/**
 * @brief 翻转 GPIO 引脚状态
 * @param port GPIO 端口句柄
 * @param pin GPIO 引脚号
 * @return 0 成功, 负值失败
 */
int DRV_GPIO_Toggle(GPIO_Handle_t port, uint16_t pin);

#ifdef __cplusplus
}
#endif

#endif /* DRV_GPIO_H */
