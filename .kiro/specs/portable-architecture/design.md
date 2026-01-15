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
    │   ├── drv_gpio.h          # GPIO抽象接口
    │   ├── drv_uart.h          # UART抽象接口
    │   ├── drv_adc.h           # ADC抽象接口
    │   └── stm32h7/            # STM32H7平台实现
    │       ├── drv_gpio_impl.c
    │       ├── drv_uart_impl.c # 包含MspInit、时钟配置
    │       ├── drv_adc_impl.c  # 包含MspInit、DMA配置
    │       └── drv_system.c    # 系统时钟配置
    └── inc/                    # 配置头文件（保持不变）
        ├── FreeRTOSConfig.h
        └── stm32h7xx_hal_conf.h
```

### GPIO 驱动接口 (drivers/drv_gpio.h)

```c
#ifndef DRV_GPIO_H
#define DRV_GPIO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 抽象句柄类型 */
typedef void *GPIO_Port_t;

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
  GPIO_Port_t port;
  uint16_t pin;
  DRV_GPIO_Mode_t mode;
  DRV_GPIO_Pull_t pull;
  DRV_GPIO_Speed_t speed;
} DRV_GPIO_Config_t;

/* 返回值: 0成功, 负值失败 */
#define DRV_OK       0
#define DRV_ERROR   -1

/* 平台端口定义（由平台实现提供） */
extern GPIO_Port_t DRV_GPIOA;
extern GPIO_Port_t DRV_GPIOB;
extern GPIO_Port_t DRV_GPIOC;
extern GPIO_Port_t DRV_GPIOD;
extern GPIO_Port_t DRV_GPIOE;

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

int DRV_GPIO_Init(DRV_GPIO_Config_t *config);
int DRV_GPIO_DeInit(GPIO_Port_t port, uint16_t pin);
int DRV_GPIO_Write(GPIO_Port_t port, uint16_t pin, uint8_t state);
int DRV_GPIO_Read(GPIO_Port_t port, uint16_t pin, uint8_t *state);
int DRV_GPIO_Toggle(GPIO_Port_t port, uint16_t pin);

#ifdef __cplusplus
}
#endif

#endif /* DRV_GPIO_H */
```

### UART 驱动接口 (drivers/drv_uart.h)

```c
#ifndef DRV_UART_H
#define DRV_UART_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 抽象句柄类型 */
typedef void *UART_Handle_t;

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

/* 初始化/反初始化 */
int DRV_UART_Init(DRV_UART_Config_t *config);
int DRV_UART_DeInit(DRV_UART_Instance_t instance);

/* 获取句柄 */
UART_Handle_t DRV_UART_GetHandle(DRV_UART_Instance_t instance);

/* 数据传输 */
int DRV_UART_Transmit(UART_Handle_t handle, uint8_t *data, uint16_t len,
                      uint32_t timeout);
int DRV_UART_Receive(UART_Handle_t handle, uint8_t *data, uint16_t len,
                     uint32_t timeout);
int DRV_UART_TransmitIT(UART_Handle_t handle, uint8_t *data, uint16_t len);
int DRV_UART_ReceiveIT(UART_Handle_t handle, uint8_t *data, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* DRV_UART_H */
```

### ADC 驱动接口 (drivers/drv_adc.h)

```c
#ifndef DRV_ADC_H
#define DRV_ADC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 抽象句柄类型 */
typedef void *ADC_Handle_t;

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

/* 初始化/反初始化 */
int DRV_ADC_Init(DRV_ADC_Config_t *config);
int DRV_ADC_DeInit(DRV_ADC_Instance_t instance);

/* 获取句柄 */
ADC_Handle_t DRV_ADC_GetHandle(DRV_ADC_Instance_t instance);

/* 数据读取 */
int DRV_ADC_Read(ADC_Handle_t handle, uint16_t *value);
int DRV_ADC_StartDMA(ADC_Handle_t handle, uint16_t *buffer, uint16_t len);
int DRV_ADC_StopDMA(ADC_Handle_t handle);

/* 获取DMA缓冲区 */
uint16_t *DRV_ADC_GetDMABuffer(DRV_ADC_Instance_t instance);

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

#include "drv_gpio.h"
#include "drv_uart.h"
#include "drv_adc.h"

/* ==================== LED 配置 ==================== */
#define BSP_LED1_PORT         DRV_GPIOC
#define BSP_LED1_PIN          DRV_PIN_13

/* ==================== 继电器配置 ==================== */
#define BSP_RELAY_PORT        DRV_GPIOE
#define BSP_RELAY_PIN         DRV_PIN_11

/* ==================== UART 配置 ==================== */
#define BSP_DEBUG_UART        DRV_UART2
#define BSP_COMM_UART         DRV_UART1
#define BSP_UART1_BAUDRATE    115200
#define BSP_UART2_BAUDRATE    115200

/* ==================== ADC 配置 ==================== */
#define BSP_ADC1_BUFFER_SIZE  64
#define BSP_ADC2_BUFFER_SIZE  64

#endif /* BSP_CONFIG_H */
```

## 数据模型

### Driver层内部数据结构（平台实现私有）

```c
/* drivers/stm32h7/drv_uart_impl.c 内部使用 */
typedef struct
{
  UART_HandleTypeDef hal_handle;
  uint8_t rx_byte;
  volatile uint8_t rx_ready;
} UART_Instance_t;

static UART_Instance_t s_uart_instances[DRV_UART_MAX];
```

```c
/* drivers/stm32h7/drv_adc_impl.c 内部使用 */
typedef struct
{
  ADC_HandleTypeDef hal_handle;
  DMA_HandleTypeDef dma_handle;
  uint16_t *dma_buffer;
  uint16_t buffer_len;
} ADC_Instance_t;

static ADC_Instance_t s_adc_instances[DRV_ADC_MAX];
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
