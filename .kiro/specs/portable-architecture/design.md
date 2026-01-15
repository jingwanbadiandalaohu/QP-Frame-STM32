# 设计文档

## 概述

本设计文档描述了 STM32 嵌入式项目的可移植架构重构方案。参考 ELAB 框架的分层思想，通过引入清晰的四层架构和平台抽象接口，实现代码在不同 MCU 平台间的无缝移植。

### 当前问题

分析现有代码结构，发现以下问题：
1. BSP 层直接依赖 `stm32h7xx_hal.h`，与特定 MCU 紧耦合
2. BSP 层包含 `HAL_*_MspInit()` 回调和时钟配置代码
3. BSP 层直接操作 `UART_HandleTypeDef`、`ADC_HandleTypeDef` 等 HAL 类型
4. 滤波器模块虽然相对独立，但仍放置在 BSP 目录下

### 设计目标

- 实现四层架构：HAL_Layer → Driver_Layer → BSP_Layer → App_Layer
- Driver_Layer 包含抽象接口定义和平台实现
- BSP_Layer 完全不依赖 HAL，仅使用 Driver_Layer 接口
- 将滤波器等纯算法模块独立到 common 目录

## 架构

参考 ELAB 框架的分层设计：

```
┌─────────────────────────────────────────────────────────┐
│                    App_Layer (应用层)                    │
│              (业务逻辑、任务调度、协程)                    │
├─────────────────────────────────────────────────────────┤
│                   BSP_Layer (设备层)                     │
│           (LED、Relay、具体外设封装、板级配置)             │
├─────────────────────────────────────────────────────────┤
│                Driver_Layer (抽象硬件层)                  │
│    ┌─────────────────────────────────────────────────┐  │
│    │  接口定义: drv_gpio.h, drv_uart.h, drv_adc.h    │  │
│    ├─────────────────────────────────────────────────┤  │
│    │  平台实现: stm32h7/, stm32f4/, stm32f1/         │  │
│    │  (包含 MspInit、时钟配置、HAL调用)               │  │
│    └─────────────────────────────────────────────────┘  │
├─────────────────────────────────────────────────────────┤
│                   HAL_Layer (硬件层)                     │
│           (厂商 HAL 库：STM32H7xx_HAL_Driver)            │
└─────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────┐
│                   Common Modules (软件组件)              │
│         (Filter、环形缓冲区、状态机 - 平台无关)           │
└─────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────┐
│                 OS Abstraction (操作系统抽象)            │
│           (CMSIS-RTOS2: 锁、信号量、消息队列)            │
└─────────────────────────────────────────────────────────┘
```

### 层间调用规则

- App_Layer → BSP_Layer / Common Modules / CMSIS-RTOS2
- BSP_Layer → Driver_Layer（仅通过抽象接口）
- Driver_Layer 平台实现 → HAL_Layer
- **禁止**: App_Layer 和 BSP_Layer 直接调用 HAL_Layer

### 关键设计原则

1. **BSP层零HAL依赖**: BSP层不包含任何 `stm32h7xx_hal.h`，不使用 `UART_HandleTypeDef` 等HAL类型
2. **Driver层封装HAL**: 所有HAL调用、MspInit回调、时钟配置都在Driver层的平台实现中
3. **抽象句柄**: BSP层仅使用 `UART_Handle_t`、`ADC_Handle_t` 等抽象句柄类型

## 组件与接口

### 目录结构

```
project
├── ide/                        # 多工具链支持（保持不变）
├── Middlewares/                # 中间件（保持不变）
└── usr/                        # 用户代码（重构）
    ├── app/                    # 应用层
    │   └── main.c
    ├── bsp/                    # 设备层（无HAL依赖）
    │   ├── bsp_config.h        # 板级配置（引脚映射、波特率等）
    │   ├── bsp.h               # BSP统一头文件
    │   ├── bsp.c               # BSP初始化
    │   ├── bsp_led.c           # LED设备封装
    │   ├── bsp_uart.c          # UART设备封装
    │   └── bsp_adc.c           # ADC设备封装
    ├── common/                 # 软件组件（平台无关）
    │   └── filter/
    │       ├── filter.h
    │       └── filter.c
    ├── core/                   # 启动文件（保持不变）
    │   └── startup_stm32h750xx.s
    ├── drivers/                # 抽象硬件层
    │   ├── drv_gpio.h          # GPIO抽象接口（函数指针封装）
    │   ├── drv_uart.h          # UART抽象接口（函数指针封装）
    │   ├── drv_adc.h           # ADC抽象接口（函数指针封装）
    │   └── stm32h7/            # STM32H7平台实现
    │       ├── drv_gpio_impl.c
    │       ├── drv_uart_impl.c # 包含MspInit、时钟配置
    │       ├── drv_adc_impl.c  # 包含MspInit、DMA配置
    │       └── drv_system.c    # 系统时钟配置
    ├── simulator/              # 模拟器实现（预留扩展）
    │   ├── drivers/
    │   │   └── simulator/
    │   │       ├── drv_gpio_sim.c
    │   │       ├── drv_uart_sim.c
    │   │       └── drv_adc_sim.c
    │   └── main.c              # 模拟器主程序
    └── inc/                    # 配置头文件（保持不变）
        ├── FreeRTOSConfig.h
        └── stm32h7xx_hal_conf.h
```

### GPIO 驱动接口 (drivers/drv_gpio.h) - 函数指针封装

```c
#ifndef DRV_GPIO_H
#define DRV_GPIO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 前向声明 */
typedef struct GPIO_Port GPIO_Port_t;

/* GPIO 模式枚举 */
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

/* 返回值: 0成功, 负值失败 */
#define DRV_OK       0
#define DRV_ERROR   -1

/* 引脚定义 */
#define DRV_PIN_0   (1U << 0)
#define DRV_PIN_1   (1U << 1)
#define DRV_PIN_2   (1U << 2)
#define DRV_PIN_3   (1U << 3)
#define DRV_PIN_4   (1U << 4)
#define DRV_PIN_5   (1U << 5)
#define DRV_PIN_6   (1U << 6)
#define DRV_PIN_7   (1U << 7)
#define DRV_PIN_8   (1U << 8)
#define DRV_PIN_9   (1U << 9)
#define DRV_PIN_10  (1U << 10)
#define DRV_PIN_11  (1U << 11)
#define DRV_PIN_12  (1U << 12)
#define DRV_PIN_13  (1U << 13)
#define DRV_PIN_14  (1U << 14)
#define DRV_PIN_15  (1U << 15)

/* GPIO 操作函数集（类似 Linux file_operations） */
typedef struct GPIO_Operations
{
  int (*init)(GPIO_Port_t *port, DRV_GPIO_Config_t *config);
  int (*deinit)(GPIO_Port_t *port, uint16_t pin);
  int (*write)(GPIO_Port_t *port, uint16_t pin, uint8_t state);
  int (*read)(GPIO_Port_t *port, uint16_t pin, uint8_t *state);
  int (*toggle)(GPIO_Port_t *port, uint16_t pin);
} GPIO_Ops_t;

/* GPIO 端口设备对象 */
struct GPIO_Port
{
  const char *name;       // 端口名称 "GPIOA", "GPIOB"
  void *hw_base;          // 硬件基地址（平台私有）
  GPIO_Ops_t *ops;        // 操作函数集
};

/* 统一调用接口（包装函数，简化语法） */
static inline int gpio_init(GPIO_Port_t *port, DRV_GPIO_Config_t *config)
{
  if(port && port->ops && port->ops->init)
  {
    return port->ops->init(port, config);
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

/* 平台端口实例（由平台实现提供） */
extern GPIO_Port_t *drv_gpioa;
extern GPIO_Port_t *drv_gpiob;
extern GPIO_Port_t *drv_gpioc;
extern GPIO_Port_t *drv_gpiod;
extern GPIO_Port_t *drv_gpioe;

#ifdef __cplusplus
}
#endif

#endif /* DRV_GPIO_H */
```

### UART 驱动接口 (drivers/drv_uart.h) - 函数指针封装

```c
#ifndef DRV_UART_H
#define DRV_UART_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 前向声明 */
typedef struct UART_Device UART_Device_t;

/* UART 实例枚举 */
typedef enum
{
  DRV_UART1,
  DRV_UART2,
  DRV_UART_MAX
} DRV_UART_Instance_t;

typedef enum
{
  DRV_UART_PARITY_NONE,
  DRV_UART_PARITY_EVEN,
  DRV_UART_PARITY_ODD
} DRV_UART_Parity_t;

typedef enum
{
  DRV_UART_STOPBITS_1,
  DRV_UART_STOPBITS_2
} DRV_UART_StopBits_t;

typedef struct
{
  DRV_UART_Instance_t instance;
  uint32_t baudrate;
  uint8_t data_bits;
  DRV_UART_StopBits_t stop_bits;
  DRV_UART_Parity_t parity;
} DRV_UART_Config_t;

/* 返回值定义 */
#ifndef DRV_OK
#define DRV_OK       0
#define DRV_ERROR   -1
#define DRV_BUSY    -2
#define DRV_TIMEOUT -3
#endif

/* UART 操作函数集 */
typedef struct UART_Operations
{
  int (*init)(UART_Device_t *dev, DRV_UART_Config_t *config);
  int (*deinit)(UART_Device_t *dev);
  int (*transmit)(UART_Device_t *dev, uint8_t *data, uint16_t len, uint32_t timeout);
  int (*receive)(UART_Device_t *dev, uint8_t *data, uint16_t len, uint32_t timeout);
  int (*transmit_it)(UART_Device_t *dev, uint8_t *data, uint16_t len);
  int (*receive_it)(UART_Device_t *dev, uint8_t *data, uint16_t len);
} UART_Ops_t;

/* UART 设备对象 */
struct UART_Device
{
  const char *name;           // 设备名称 "UART1", "UART2"
  DRV_UART_Instance_t instance;
  void *hw_handle;            // 硬件句柄（平台私有）
  UART_Ops_t *ops;            // 操作函数集
};

/* 统一调用接口 */
static inline int uart_transmit(UART_Device_t *dev, uint8_t *data, uint16_t len, uint32_t timeout)
{
  if(dev && dev->ops && dev->ops->transmit)
  {
    return dev->ops->transmit(dev, data, len, timeout);
  }
  return DRV_ERROR;
}

static inline int uart_receive(UART_Device_t *dev, uint8_t *data, uint16_t len, uint32_t timeout)
{
  if(dev && dev->ops && dev->ops->receive)
  {
    return dev->ops->receive(dev, data, len, timeout);
  }
  return DRV_ERROR;
}

/* 平台设备实例 */
extern UART_Device_t *drv_uart1;
extern UART_Device_t *drv_uart2;

#ifdef __cplusplus
}
#endif

#endif /* DRV_UART_H */
```

### ADC 驱动接口 (drivers/drv_adc.h) - 函数指针封装

```c
#ifndef DRV_ADC_H
#define DRV_ADC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 前向声明 */
typedef struct ADC_Device ADC_Device_t;

/* ADC 实例枚举 */
typedef enum
{
  DRV_ADC1,
  DRV_ADC2,
  DRV_ADC_MAX
} DRV_ADC_Instance_t;

typedef enum
{
  DRV_ADC_RESOLUTION_8BIT,
  DRV_ADC_RESOLUTION_10BIT,
  DRV_ADC_RESOLUTION_12BIT,
  DRV_ADC_RESOLUTION_16BIT
} DRV_ADC_Resolution_t;

typedef struct
{
  DRV_ADC_Instance_t instance;
  uint8_t channel;
  DRV_ADC_Resolution_t resolution;
} DRV_ADC_Config_t;

/* 返回值定义 */
#ifndef DRV_OK
#define DRV_OK       0
#define DRV_ERROR   -1
#endif

/* ADC 操作函数集 */
typedef struct ADC_Operations
{
  int (*init)(ADC_Device_t *dev, DRV_ADC_Config_t *config);
  int (*deinit)(ADC_Device_t *dev);
  int (*read)(ADC_Device_t *dev, uint16_t *value);
  int (*start_dma)(ADC_Device_t *dev, uint16_t *buffer, uint16_t len);
  int (*stop_dma)(ADC_Device_t *dev);
} ADC_Ops_t;

/* ADC 设备对象 */
struct ADC_Device
{
  const char *name;           // 设备名称 "ADC1", "ADC2"
  DRV_ADC_Instance_t instance;
  void *hw_handle;            // 硬件句柄（平台私有）
  uint16_t *dma_buffer;       // DMA缓冲区
  ADC_Ops_t *ops;             // 操作函数集
};

/* 统一调用接口 */
static inline int adc_read(ADC_Device_t *dev, uint16_t *value)
{
  if(dev && dev->ops && dev->ops->read)
  {
    return dev->ops->read(dev, value);
  }
  return DRV_ERROR;
}

static inline int adc_start_dma(ADC_Device_t *dev, uint16_t *buffer, uint16_t len)
{
  if(dev && dev->ops && dev->ops->start_dma)
  {
    return dev->ops->start_dma(dev, buffer, len);
  }
  return DRV_ERROR;
}

/* 平台设备实例 */
extern ADC_Device_t *drv_adc1;
extern ADC_Device_t *drv_adc2;

#ifdef __cplusplus
}
#endif

#endif /* DRV_ADC_H */
```

### 系统驱动接口 (drivers/drv_system.h)

```c
#ifndef DRV_SYSTEM_H
#define DRV_SYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif

/* 系统初始化（HAL_Init + 时钟配置） */
int DRV_System_Init(void);

/* 错误处理 */
void DRV_System_ErrorHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* DRV_SYSTEM_H */
```

### BSP 配置 (bsp/bsp_config.h)

```c
#ifndef BSP_CONFIG_H
#define BSP_CONFIG_H

/* ==================== LED 配置 ==================== */
#define BSP_LED1_PORT         drv_gpioc
#define BSP_LED1_PIN          DRV_PIN_13

/* ==================== 继电器配置 ==================== */
#define BSP_RELAY_PORT        drv_gpioe
#define BSP_RELAY_PIN         DRV_PIN_11

/* ==================== UART 配置 ==================== */
#define BSP_DEBUG_UART        drv_uart2
#define BSP_COMM_UART         drv_uart1
#define BSP_UART1_BAUDRATE    115200
#define BSP_UART2_BAUDRATE    115200

/* ==================== ADC 配置 ==================== */
#define BSP_ADC1_DEVICE       drv_adc1
#define BSP_ADC2_DEVICE       drv_adc2
#define BSP_ADC1_BUFFER_SIZE  64
#define BSP_ADC2_BUFFER_SIZE  64

#endif /* BSP_CONFIG_H */
```

## 数据模型

### 函数指针封装架构

采用 Linux 驱动风格的函数指针封装，实现运行时多态：

```c
/* 设备对象通用结构 */
typedef struct Device_Object
{
  const char *name;         // 设备名称
  void *hw_private;         // 平台私有数据
  void *ops;                // 操作函数集指针
} Device_Object_t;

/* 操作函数集通用结构 */
typedef struct Operations_Struct
{
  int (*init)(Device_Object_t *dev, void *config);
  int (*deinit)(Device_Object_t *dev);
  // ... 其他操作函数指针
} Operations_t;
```

### Driver层内部数据结构（平台实现私有）

#### STM32H7 平台实现

```c
/* drivers/stm32h7/drv_uart_impl.c 内部使用 */
typedef struct
{
  UART_HandleTypeDef hal_handle;
  uint8_t rx_byte;
  volatile uint8_t rx_ready;
} STM32H7_UART_Private_t;

/* UART 设备实例 */
static STM32H7_UART_Private_t s_uart1_private;
static STM32H7_UART_Private_t s_uart2_private;

static UART_Device_t uart1_device = {
  .name = "UART1",
  .instance = DRV_UART1,
  .hw_handle = &s_uart1_private,
  .ops = &stm32h7_uart_ops,
};

static UART_Device_t uart2_device = {
  .name = "UART2", 
  .instance = DRV_UART2,
  .hw_handle = &s_uart2_private,
  .ops = &stm32h7_uart_ops,
};

/* 导出给上层使用 */
UART_Device_t *drv_uart1 = &uart1_device;
UART_Device_t *drv_uart2 = &uart2_device;
```

```c
/* drivers/stm32h7/drv_adc_impl.c 内部使用 */
typedef struct
{
  ADC_HandleTypeDef hal_handle;
  DMA_HandleTypeDef dma_handle;
  uint16_t *dma_buffer;
  uint16_t buffer_len;
} STM32H7_ADC_Private_t;

static STM32H7_ADC_Private_t s_adc1_private;
static STM32H7_ADC_Private_t s_adc2_private;

static ADC_Device_t adc1_device = {
  .name = "ADC1",
  .instance = DRV_ADC1,
  .hw_handle = &s_adc1_private,
  .dma_buffer = NULL,  // 运行时分配
  .ops = &stm32h7_adc_ops,
};
```

#### 模拟器平台实现（预留）

```c
/* project/simulator/drivers/drv_gpio_sim.c */
typedef struct
{
  uint16_t pin_states;      // 模拟引脚状态
  char port_name[8];        // 端口名称用于日志
} Simulator_GPIO_Private_t;

static Simulator_GPIO_Private_t s_gpioc_private = {
  .pin_states = 0,
  .port_name = "GPIOC",
};

static GPIO_Port_t gpioc_sim_device = {
  .name = "GPIOC_SIM",
  .hw_base = &s_gpioc_private,
  .ops = &simulator_gpio_ops,  // 指向模拟器实现
};

GPIO_Port_t *drv_gpioc = &gpioc_sim_device;
```

## 错误处理

所有驱动函数返回统一的错误码：
- `DRV_OK (0)`: 操作成功
- `DRV_ERROR (-1)`: 一般错误
- `DRV_BUSY (-2)`: 设备忙
- `DRV_TIMEOUT (-3)`: 操作超时

## 正确性属性

*正确性属性是指在系统所有有效执行中都应保持为真的特征或行为。*

### 分层隔离属性

1. **BSP层HAL隔离**: BSP层源文件不包含任何 `stm32*.h` 头文件
2. **类型封装**: BSP层不使用 `UART_HandleTypeDef`、`ADC_HandleTypeDef` 等HAL类型
3. **接口稳定性**: 更换平台时，Driver层接口（.h文件）保持不变

### 函数指针封装属性

4. **运行时多态**: 通过函数指针实现运行时绑定，支持多平台实现共存
5. **操作函数集完整性**: 每个外设的 Operations_Struct 包含该外设所有必要操作
6. **设备对象一致性**: 所有平台实现使用相同的 Device_Object 结构，仅 ops 指针不同
7. **调用安全性**: inline 包装函数进行空指针检查，防止运行时错误

### 模拟器支持属性

8. **接口兼容性**: 模拟器实现与真实硬件使用相同的 Driver_Layer 接口
9. **代码隔离性**: 模拟器代码位于独立目录，不影响嵌入式项目结构
