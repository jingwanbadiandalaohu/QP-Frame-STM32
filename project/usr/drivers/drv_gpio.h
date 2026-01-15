/**
 * @file    drv_gpio.h
 * @author  Dylan
 * @date    2026-01-15
 * @brief   GPIO驱动抽象层接口定义
 *
 * @details 提供平台无关的GPIO操作接口，支持输入/输出/复用等多种模式。
 *          通过操作函数指针实现不同平台的适配。
 */

#ifndef DRV_GPIO_H
#define DRV_GPIO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== 类型定义 ==================== */

typedef struct GPIO_Port GPIO_Port_t;  /**< GPIO端口结构体前向声明 */

/**
 * @brief GPIO模式枚举
 */
typedef enum
{
  DRV_GPIO_MODE_INPUT,      /**< 输入模式 */
  DRV_GPIO_MODE_OUTPUT_PP,  /**< 推挽输出 */
  DRV_GPIO_MODE_OUTPUT_OD,  /**< 开漏输出 */
  DRV_GPIO_MODE_AF_PP,      /**< 复用推挽 */
  DRV_GPIO_MODE_AF_OD,      /**< 复用开漏 */
  DRV_GPIO_MODE_ANALOG      /**< 模拟模式 */
} DRV_GPIO_Mode_t;

/**
 * @brief GPIO上下拉枚举
 */
typedef enum
{
  DRV_GPIO_PULL_NONE,  /**< 无上下拉 */
  DRV_GPIO_PULL_UP,    /**< 上拉 */
  DRV_GPIO_PULL_DOWN   /**< 下拉 */
} DRV_GPIO_Pull_t;

/**
 * @brief GPIO速度枚举
 */
typedef enum
{
  DRV_GPIO_SPEED_LOW,       /**< 低速 */
  DRV_GPIO_SPEED_MEDIUM,    /**< 中速 */
  DRV_GPIO_SPEED_HIGH,      /**< 高速 */
  DRV_GPIO_SPEED_VERY_HIGH  /**< 超高速 */
} DRV_GPIO_Speed_t;

/**
 * @brief GPIO配置结构体
 */
typedef struct
{
  uint16_t pin;            /**< 引脚掩码 */
  DRV_GPIO_Mode_t mode;    /**< 工作模式 */
  DRV_GPIO_Pull_t pull;    /**< 上下拉配置 */
  DRV_GPIO_Speed_t speed;  /**< 输出速度 */
} DRV_GPIO_Config_t;

/* ==================== 返回值定义 ==================== */

#define DRV_OK          0   /**< 操作成功 */
#define DRV_ERROR      -1   /**< 操作失败 */
#define DRV_BUSY       -2   /**< 设备忙 */
#define DRV_TIMEOUT    -3   /**< 操作超时 */

/* ==================== 引脚定义 ==================== */

#define DRV_PIN_0    (1U << 0)   /**< 引脚0 */
#define DRV_PIN_1    (1U << 1)   /**< 引脚1 */
#define DRV_PIN_2    (1U << 2)   /**< 引脚2 */
#define DRV_PIN_3    (1U << 3)   /**< 引脚3 */
#define DRV_PIN_4    (1U << 4)   /**< 引脚4 */
#define DRV_PIN_5    (1U << 5)   /**< 引脚5 */
#define DRV_PIN_6    (1U << 6)   /**< 引脚6 */
#define DRV_PIN_7    (1U << 7)   /**< 引脚7 */
#define DRV_PIN_8    (1U << 8)   /**< 引脚8 */
#define DRV_PIN_9    (1U << 9)   /**< 引脚9 */
#define DRV_PIN_10   (1U << 10)  /**< 引脚10 */
#define DRV_PIN_11   (1U << 11)  /**< 引脚11 */
#define DRV_PIN_12   (1U << 12)  /**< 引脚12 */
#define DRV_PIN_13   (1U << 13)  /**< 引脚13 */
#define DRV_PIN_14   (1U << 14)  /**< 引脚14 */
#define DRV_PIN_15   (1U << 15)  /**< 引脚15 */

/**
 * @brief GPIO操作函数集结构体
 */
typedef struct GPIO_Operations
{
  int (*init)(GPIO_Port_t *port, DRV_GPIO_Config_t *config);       /**< 初始化 */
  int (*deinit)(GPIO_Port_t *port, uint16_t pin);                  /**< 反初始化 */
  int (*write)(GPIO_Port_t *port, uint16_t pin, uint8_t state);    /**< 写电平 */
  int (*read)(GPIO_Port_t *port, uint16_t pin, uint8_t *state);    /**< 读电平 */
  int (*toggle)(GPIO_Port_t *port, uint16_t pin);                  /**< 翻转电平 */
} GPIO_Ops_t;

/**
 * @brief GPIO端口结构体
 */
struct GPIO_Port
{
  const char *name;   /**< 端口名称 */
  void *hw_base;      /**< 硬件基地址 */
  GPIO_Ops_t *ops;    /**< 操作函数集 */
};

/* ==================== 内联接口函数 ==================== */

/**
 * @brief   初始化GPIO引脚
 *
 * @param[in] port    GPIO端口指针
 * @param[in] config  配置参数指针
 *
 * @return  DRV_OK成功，其他失败
 */
static inline int gpio_init(GPIO_Port_t *port, DRV_GPIO_Config_t *config)
{
  if(port && port->ops && port->ops->init)
  {
    return port->ops->init(port, config);
  }
  return DRV_ERROR;
}

/**
 * @brief   反初始化GPIO引脚
 *
 * @param[in] port  GPIO端口指针
 * @param[in] pin   引脚掩码
 *
 * @return  DRV_OK成功，其他失败
 */
static inline int gpio_deinit(GPIO_Port_t *port, uint16_t pin)
{
  if(port && port->ops && port->ops->deinit)
  {
    return port->ops->deinit(port, pin);
  }
  return DRV_ERROR;
}

/**
 * @brief   写GPIO引脚电平
 *
 * @param[in] port   GPIO端口指针
 * @param[in] pin    引脚掩码
 * @param[in] state  电平状态（0低电平，非0高电平）
 *
 * @return  DRV_OK成功，其他失败
 */
static inline int gpio_write(GPIO_Port_t *port, uint16_t pin, uint8_t state)
{
  if(port && port->ops && port->ops->write)
  {
    return port->ops->write(port, pin, state);
  }
  return DRV_ERROR;
}

/**
 * @brief   读GPIO引脚电平
 *
 * @param[in]  port   GPIO端口指针
 * @param[in]  pin    引脚掩码
 * @param[out] state  电平状态存储指针
 *
 * @return  DRV_OK成功，其他失败
 */
static inline int gpio_read(GPIO_Port_t *port, uint16_t pin, uint8_t *state)
{
  if(port && port->ops && port->ops->read)
  {
    return port->ops->read(port, pin, state);
  }
  return DRV_ERROR;
}

/**
 * @brief   翻转GPIO引脚电平
 *
 * @param[in] port  GPIO端口指针
 * @param[in] pin   引脚掩码
 *
 * @return  DRV_OK成功，其他失败
 */
static inline int gpio_toggle(GPIO_Port_t *port, uint16_t pin)
{
  if(port && port->ops && port->ops->toggle)
  {
    return port->ops->toggle(port, pin);
  }
  return DRV_ERROR;
}

/* ==================== 平台端口实例声明 ==================== */

extern GPIO_Port_t *drv_gpioa;  /**< GPIOA端口实例 */
extern GPIO_Port_t *drv_gpiob;  /**< GPIOB端口实例 */
extern GPIO_Port_t *drv_gpioc;  /**< GPIOC端口实例 */
extern GPIO_Port_t *drv_gpiod;  /**< GPIOD端口实例 */
extern GPIO_Port_t *drv_gpioe;  /**< GPIOE端口实例 */

#ifdef __cplusplus
}
#endif

#endif /* DRV_GPIO_H */
