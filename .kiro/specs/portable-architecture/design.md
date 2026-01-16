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

- 实现四层架构：HAL_Layer → Driver_Layer → Device_Layer → App_Layer
- Driver_Layer 包含抽象接口定义和平台实现
- Device_Layer 完全不依赖 HAL，仅使用 Driver_Layer 接口
- 将滤波器等纯算法模块独立到 common 目录

## 架构

参考 ELAB 框架的分层设计：

```
┌─────────────────────────────────────────────────────────┐
│                    App_Layer (应用层)                    │
│         (定时调度、直接调用、任务/线程、协程)              │
├─────────────────────────────────────────────────────────┤
│                  Device_Layer (设备层)                   │
│              (LED、Motor、IMU、Relay 等)                 │
├─────────────────────────────────────────────────────────┤
│                Driver_Layer (抽象硬件层)                  │
│    ┌─────────────────────────────────────────────────┐  │
│    │  接口定义: GPIO、UART、ADC、PWM、IIC、SPI      │  │
│    ├─────────────────────────────────────────────────┤  │
│    │  平台实现: stm32h750vbt6/, stm32f103rct6/, etc.  │  │
│    │  (包含 MspInit、时钟配置、HAL调用)               │  │
│    └─────────────────────────────────────────────────┘  │
├─────────────────────────────────────────────────────────┤
│                   HAL_Layer (硬件层)                     │
│    (STM32F1、STM32H7、TC264D、MSPM0G3507 等厂商库)       │
└─────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────┐
│                   Common Modules (软件组件)              │
│    (链表、队列、哈希表、环形缓冲区、状态机 - 平台无关)     │
└─────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────┐
│                 OS Abstraction (操作系统抽象API)         │
│       (锁、信号量、软件定时器、消息队列 - CMSIS-RTOS2)     │
└─────────────────────────────────────────────────────────┘
```

### 层间调用规则

- App_Layer → Device_Layer / Common Modules / CMSIS-RTOS2
- Device_Layer → Driver_Layer（仅通过抽象接口）
- Driver_Layer 平台实现 → HAL_Layer
- **禁止**: App_Layer 和 Device_Layer 直接调用 HAL_Layer

### 关键设计原则

1. **Device层零HAL依赖**: Device层公开头文件不包含任何 `stm32*.h`
2. **描述符封装**: 硬件属性封装在描述符结构体中
3. **平台隔离**: 所有平台相关代码集中在 `drivers/{chip}/` 子目录

## 组件与接口

### 目录结构

```
mcu/                            # 厂商HAL库（按芯片区分）
├── stm32h750vbt6/
│   ├── CMSIS/
│   └── STM32H7xx_HAL_Driver/
└── stm32f103rct6/              # （预留）

project
├── ide/                        # 多工具链支持（保持不变）
├── Middlewares/                # 中间件（保持不变）
└── usr/                        # 用户代码
    ├── app/                    # 应用层
    │   └── main.c
    ├── device/                 # 设备层（平台无关）
    │   ├── led.h               # LED 接口
    │   ├── led.c               # LED 实现
    │   ├── relay.h             # 继电器接口
    │   └── relay.c             # 继电器实现
    ├── drivers/                # 驱动层
    │   ├── drv_gpio.h          # GPIO 公开接口（平台无关）
    │   ├── drv_uart.h          # UART 公开接口
    │   ├── drv_adc.h           # ADC 公开接口
    │   ├── drv_system.h        # 系统接口
    │   ├── board.h             # 设备实例声明（平台无关）
    │   ├── stm32h750vbt6/      # H750 平台实现
    │   │   ├── drv_gpio_desc.h # GPIO 描述符定义
    │   │   ├── drv_gpio.c      # GPIO 实现
    │   │   ├── drv_uart_desc.h
    │   │   ├── drv_uart.c
    │   │   ├── drv_adc_desc.h
    │   │   ├── drv_adc.c
    │   │   ├── drv_system.c
    │   │   └── board.c         # 设备实例定义（引脚配置）
    │   └── stm32f103rct6/      # F103 平台实现（预留）
    │       └── ...
    ├── common/                 # 软件组件（平台无关）
    │   └── filter/
    │       ├── filter.h
    │       └── filter.c
    ├── core/                   # 启动文件和中断处理（按芯片区分）
    │   ├── stm32h750vbt6/
    │   │   ├── stm32h7xx_hal_msp.c
    │   │   ├── stm32h7xx_hal_timebase_tim.c
    │   │   ├── stm32h7xx_it.c
    │   │   ├── stm32h7xx_it.h
    │   │   └── syscalls.c
    │   └── stm32f103rct6/      # （预留）
    └── inc/                    # 配置头文件（按芯片区分）
        ├── stm32h750vbt6/
        │   ├── FreeRTOSConfig.h
        │   ├── stm32h7xx_hal_conf.h
        │   └── RTE_Components.h
        └── stm32f103rct6/      # （预留）
```

### 芯片相关文件汇总

| 目录 | 说明 | 迁移时需要 |
|------|------|-----------|
| `mcu/{chip}/` | 厂商HAL库、CMSIS | 添加新芯片HAL库 |
| `usr/core/{chip}/` | 启动文件、中断处理、HAL MSP | 新建并实现 |
| `usr/drivers/{chip}/` | 驱动层平台实现 | 新建并实现 |
| `usr/inc/{chip}/` | HAL配置、FreeRTOS配置 | 新建并配置 |

### GPIO 驱动接口 (drivers/drv_gpio.h) - 面向对象描述符风格

参考 STM32F4_LEDOBJ 项目的设计，采用描述符结构体封装引脚属性，简化驱动实现。

**设计理念：**
- 描述符包含引脚的所有属性（端口、引脚号、极性等）
- 驱动函数接收描述符指针，直接操作
- 板级配置在 device 层定义具体实例
- 无需函数指针表，代码更简洁

```c
/* drivers/drv_gpio.h - 公开接口（约30行） */
#ifndef DRV_GPIO_H
#define DRV_GPIO_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 前向声明，隐藏实现细节 */
struct gpio_desc;
typedef struct gpio_desc *gpio_desc_t;

/* GPIO 操作函数 */
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
```

```c
/* drivers/stm32h750vbt6/drv_gpio_desc.h - 描述符定义（平台相关，约25行） */
#ifndef DRV_GPIO_DESC_H
#define DRV_GPIO_DESC_H

#include <stdbool.h>
#include <stdint.h>
#include "stm32h7xx_hal.h"  /* 平台相关 */

/* GPIO 描述符结构体 */
struct gpio_desc
{
  GPIO_TypeDef *port;     /**< GPIO端口 */
  uint16_t pin;           /**< 引脚号 */
  GPIO_PinState on_state; /**< 有效电平 */
  GPIO_PinState off_state;/**< 无效电平 */
};

typedef struct gpio_desc *gpio_desc_t;

#endif /* DRV_GPIO_DESC_H */
```

```c
/* drivers/stm32h750vbt6/drv_gpio.c - GPIO 实现（约40行） */
#include "drv_gpio.h"
#include "drv_gpio_desc.h"

void gpio_init(gpio_desc_t gpio)
{
  GPIO_InitTypeDef init = {0};
  
  /* 使能时钟 */
  if(gpio->port == GPIOA) __HAL_RCC_GPIOA_CLK_ENABLE();
  else if(gpio->port == GPIOB) __HAL_RCC_GPIOB_CLK_ENABLE();
  else if(gpio->port == GPIOC) __HAL_RCC_GPIOC_CLK_ENABLE();
  else if(gpio->port == GPIOD) __HAL_RCC_GPIOD_CLK_ENABLE();
  else if(gpio->port == GPIOE) __HAL_RCC_GPIOE_CLK_ENABLE();
  
  init.Pin = gpio->pin;
  init.Mode = GPIO_MODE_OUTPUT_PP;
  init.Pull = GPIO_NOPULL;
  init.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(gpio->port, &init);
  
  HAL_GPIO_WritePin(gpio->port, gpio->pin, gpio->off_state);
}

void gpio_set(gpio_desc_t gpio, bool state)
{
  HAL_GPIO_WritePin(gpio->port, gpio->pin, 
                    state ? gpio->on_state : gpio->off_state);
}

void gpio_on(gpio_desc_t gpio)
{
  HAL_GPIO_WritePin(gpio->port, gpio->pin, gpio->on_state);
}

void gpio_off(gpio_desc_t gpio)
{
  HAL_GPIO_WritePin(gpio->port, gpio->pin, gpio->off_state);
}

void gpio_toggle(gpio_desc_t gpio)
{
  HAL_GPIO_TogglePin(gpio->port, gpio->pin);
}

bool gpio_read(gpio_desc_t gpio)
{
  return HAL_GPIO_ReadPin(gpio->port, gpio->pin) == gpio->on_state;
}
```

### 板级配置 (drivers/board.h, drivers/stm32h750vbt6/board.c)

```c
/* drivers/board.h - 设备实例声明（平台无关） */
#ifndef BOARD_H
#define BOARD_H

#include "drv_gpio.h"
#include "drv_uart.h"
#include "drv_adc.h"

#ifdef __cplusplus
extern "C" {
#endif

/* GPIO 设备实例 */
extern gpio_desc_t led1;
extern gpio_desc_t relay1;

/* UART 设备实例 */
extern uart_desc_t debug_uart;
extern uart_desc_t comm_uart;

/* ADC 设备实例 */
extern adc_desc_t adc1;

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
```

```c
/* drivers/stm32h750vbt6/board.c - 设备实例定义（平台相关） */
#include "board.h"
#include "drv_gpio_desc.h"
#include "drv_uart_desc.h"
#include "drv_adc_desc.h"

/* LED1: PC13, 低电平点亮 */
static struct gpio_desc s_led1 = {
  .port = GPIOC,
  .pin = GPIO_PIN_13,
  .on_state = GPIO_PIN_RESET,
  .off_state = GPIO_PIN_SET
};
gpio_desc_t led1 = &s_led1;

/* 继电器: PE11, 高电平吸合 */
static struct gpio_desc s_relay1 = {
  .port = GPIOE,
  .pin = GPIO_PIN_11,
  .on_state = GPIO_PIN_SET,
  .off_state = GPIO_PIN_RESET
};
gpio_desc_t relay1 = &s_relay1;

/* 调试串口: UART2, 115200 */
static struct uart_desc s_debug_uart = {
  .instance = USART2,
  .baudrate = 115200
};
uart_desc_t debug_uart = &s_debug_uart;

/* 通信串口: UART1, 115200 */
static struct uart_desc s_comm_uart = {
  .instance = USART1,
  .baudrate = 115200
};
uart_desc_t comm_uart = &s_comm_uart;

/* ADC1: PB1, 通道5 */
static uint16_t s_adc1_buffer[64];
static struct adc_desc s_adc1 = {
  .instance = ADC1,
  .channel = ADC_CHANNEL_5,
  .dma_buffer = s_adc1_buffer,
  .buffer_len = 64
};
adc_desc_t adc1 = &s_adc1;
```

### 新旧设计对比

| 对比项 | 旧设计（函数指针） | 新设计（描述符） |
|--------|-------------------|-----------------|
| 代码行数 | ~300行 | ~100行 |
| 抽象层次 | 3层（drv_gpio.h → impl → bsp） | 2层（drv_gpio.h → device） |
| 枚举转换 | 需要 Mode/Pull/Speed 转换 | 直接使用 HAL 定义 |
| 函数指针表 | 需要 GPIO_Ops_t | 不需要 |
| 极性处理 | BSP 层硬编码 | 描述符配置 on_state/off_state |
| 新增引脚 | 修改多个文件 | 只需添加描述符实例 |
| 可读性 | 较复杂 | 简洁直观 |

### UART 驱动接口 (drivers/drv_uart.h) - 面向对象描述符风格

```c
/* drivers/drv_uart.h - 公开接口（约25行） */
#ifndef DRV_UART_H
#define DRV_UART_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 前向声明 */
struct uart_desc;
typedef struct uart_desc *uart_desc_t;

/* UART 操作函数 */
void uart_init(uart_desc_t uart);
void uart_deinit(uart_desc_t uart);
int uart_transmit(uart_desc_t uart, uint8_t *data, uint16_t len, uint32_t timeout);
int uart_receive(uart_desc_t uart, uint8_t *data, uint16_t len, uint32_t timeout);
int uart_transmit_it(uart_desc_t uart, uint8_t *data, uint16_t len);
int uart_receive_it(uart_desc_t uart, uint8_t *data, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* DRV_UART_H */
```

```c
/* drivers/stm32h750vbt6/drv_uart_desc.h - 描述符定义（平台相关） */
#ifndef DRV_UART_DESC_H
#define DRV_UART_DESC_H

#include <stdint.h>
#include "stm32h7xx_hal.h"

struct uart_desc
{
  USART_TypeDef *instance;        /**< UART外设基地址 */
  uint32_t baudrate;              /**< 波特率 */
  UART_HandleTypeDef hal_handle;  /**< HAL句柄（内部使用） */
};

typedef struct uart_desc *uart_desc_t;

#endif /* DRV_UART_DESC_H */
```

```c
/* drivers/stm32h750vbt6/drv_uart.c - UART 实现（约60行） */
#include "drv_uart.h"
#include "drv_uart_desc.h"

void uart_init(uart_desc_t uart)
{
  uart->hal_handle.Instance = uart->instance;
  uart->hal_handle.Init.BaudRate = uart->baudrate;
  uart->hal_handle.Init.WordLength = UART_WORDLENGTH_8B;
  uart->hal_handle.Init.StopBits = UART_STOPBITS_1;
  uart->hal_handle.Init.Parity = UART_PARITY_NONE;
  uart->hal_handle.Init.Mode = UART_MODE_TX_RX;
  uart->hal_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  uart->hal_handle.Init.OverSampling = UART_OVERSAMPLING_16;
  
  HAL_UART_Init(&uart->hal_handle);
}

void uart_deinit(uart_desc_t uart)
{
  HAL_UART_DeInit(&uart->hal_handle);
}

int uart_transmit(uart_desc_t uart, uint8_t *data, uint16_t len, uint32_t timeout)
{
  return HAL_UART_Transmit(&uart->hal_handle, data, len, timeout) == HAL_OK ? 0 : -1;
}

int uart_receive(uart_desc_t uart, uint8_t *data, uint16_t len, uint32_t timeout)
{
  return HAL_UART_Receive(&uart->hal_handle, data, len, timeout) == HAL_OK ? 0 : -1;
}

int uart_transmit_it(uart_desc_t uart, uint8_t *data, uint16_t len)
{
  return HAL_UART_Transmit_IT(&uart->hal_handle, data, len) == HAL_OK ? 0 : -1;
}

int uart_receive_it(uart_desc_t uart, uint8_t *data, uint16_t len)
{
  return HAL_UART_Receive_IT(&uart->hal_handle, data, len) == HAL_OK ? 0 : -1;
}
```

### ADC 驱动接口 (drivers/drv_adc.h) - 面向对象描述符风格

```c
/* drivers/drv_adc.h - 公开接口（约20行） */
#ifndef DRV_ADC_H
#define DRV_ADC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 前向声明 */
struct adc_desc;
typedef struct adc_desc *adc_desc_t;

/* ADC 操作函数 */
void adc_init(adc_desc_t adc);
void adc_deinit(adc_desc_t adc);
uint16_t adc_read(adc_desc_t adc);
void adc_start_dma(adc_desc_t adc);
void adc_stop_dma(adc_desc_t adc);
uint16_t adc_get_average(adc_desc_t adc);

#ifdef __cplusplus
}
#endif

#endif /* DRV_ADC_H */
```

```c
/* drivers/stm32h750vbt6/drv_adc_desc.h - 描述符定义（平台相关） */
#ifndef DRV_ADC_DESC_H
#define DRV_ADC_DESC_H

#include <stdint.h>
#include "stm32h7xx_hal.h"

struct adc_desc
{
  ADC_TypeDef *instance;          /**< ADC外设基地址 */
  uint32_t channel;               /**< ADC通道 */
  uint16_t *dma_buffer;           /**< DMA缓冲区指针 */
  uint16_t buffer_len;            /**< 缓冲区长度 */
  ADC_HandleTypeDef hal_handle;   /**< HAL句柄（内部使用） */
  DMA_HandleTypeDef dma_handle;   /**< DMA句柄（内部使用） */
};

typedef struct adc_desc *adc_desc_t;

#endif /* DRV_ADC_DESC_H */
```

```c
/* drivers/stm32h750vbt6/drv_adc.c - ADC 实现（约80行） */
#include "drv_adc.h"
#include "drv_adc_desc.h"

void adc_init(adc_desc_t adc)
{
  /* ADC 配置 */
  adc->hal_handle.Instance = adc->instance;
  adc->hal_handle.Init.Resolution = ADC_RESOLUTION_16B;
  adc->hal_handle.Init.ScanConvMode = ADC_SCAN_DISABLE;
  adc->hal_handle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  adc->hal_handle.Init.ContinuousConvMode = ENABLE;
  adc->hal_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  
  HAL_ADC_Init(&adc->hal_handle);
  HAL_ADCEx_Calibration_Start(&adc->hal_handle, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
  
  /* 通道配置 */
  ADC_ChannelConfTypeDef ch_config = {0};
  ch_config.Channel = adc->channel;
  ch_config.Rank = ADC_REGULAR_RANK_1;
  ch_config.SamplingTime = ADC_SAMPLETIME_810CYCLES_5;
  HAL_ADC_ConfigChannel(&adc->hal_handle, &ch_config);
}

void adc_deinit(adc_desc_t adc)
{
  HAL_ADC_DeInit(&adc->hal_handle);
}

uint16_t adc_read(adc_desc_t adc)
{
  HAL_ADC_Start(&adc->hal_handle);
  HAL_ADC_PollForConversion(&adc->hal_handle, 100);
  return (uint16_t)HAL_ADC_GetValue(&adc->hal_handle);
}

void adc_start_dma(adc_desc_t adc)
{
  HAL_ADC_Start_DMA(&adc->hal_handle, (uint32_t *)adc->dma_buffer, adc->buffer_len);
}

void adc_stop_dma(adc_desc_t adc)
{
  HAL_ADC_Stop_DMA(&adc->hal_handle);
}

uint16_t adc_get_average(adc_desc_t adc)
{
  uint32_t sum = 0;
  for(uint16_t i = 0; i < adc->buffer_len; i++)
  {
    sum += adc->dma_buffer[i];
  }
  return (uint16_t)(sum / adc->buffer_len);
}
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

### LED 设备层 (device/led.h, led.c)

```c
/* device/led.h - LED 接口（平台无关） */
#ifndef LED_H
#define LED_H

#include "drv_gpio.h"
#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/* LED 操作宏 */
#define led_init(led)     gpio_init(led)
#define led_on(led)       gpio_on(led)
#define led_off(led)      gpio_off(led)
#define led_toggle(led)   gpio_toggle(led)

#ifdef __cplusplus
}
#endif

#endif /* LED_H */
```

```c
/* device/led.c - LED 实现（平台无关，可选） */
#include "led.h"

/* 简单的开关操作用宏即可，不需要 .c 文件 */
/* 如果需要闪烁、呼吸灯等复杂功能，在这里实现 */
```

### 继电器设备层 (device/relay.h, relay.c)

```c
/* device/relay.h - 继电器接口（平台无关） */
#ifndef RELAY_H
#define RELAY_H

#include "drv_gpio.h"
#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 继电器操作宏 */
#define relay_init(r)     gpio_init(r)
#define relay_on(r)       gpio_on(r)
#define relay_off(r)      gpio_off(r)

#ifdef __cplusplus
}
#endif

#endif /* RELAY_H */
```

```c
/* device/relay.c - 继电器实现（平台无关，可选） */
#include "relay.h"

/* 简单的开关操作用宏即可，不需要 .c 文件 */
```

### 应用层使用示例 (app/main.c)

```c
#include "drv_system.h"
#include "led.h"
#include "relay.h"

int main(void)
{
  DRV_System_Init();
  
  led_init(led1);
  relay_init(relay1);
  
  relay_on(relay1);
  
  while(1)
  {
    led_toggle(led1);
    HAL_Delay(500);
  }
}
```

## 数据模型

### 描述符封装架构

采用面向对象的描述符结构体封装，每个外设实例用一个描述符表示：

```c
/* 描述符通用设计模式 */
struct xxx_desc
{
  /* 硬件属性 */
  void *hw_port;          // 硬件端口/外设基地址
  uint32_t hw_config;     // 硬件配置参数
  
  /* 行为属性 */
  xxx_state_t on_state;   // 有效状态
  xxx_state_t off_state;  // 无效状态
};
typedef struct xxx_desc *xxx_desc_t;
```

### GPIO 描述符

```c
struct gpio_desc
{
  GPIO_TypeDef *port;       /**< GPIO端口基地址 */
  uint16_t pin;             /**< 引脚号 (GPIO_PIN_x) */
  GPIO_PinState on_state;   /**< 有效电平 */
  GPIO_PinState off_state;  /**< 无效电平 */
};
```

### UART 描述符

```c
struct uart_desc
{
  USART_TypeDef *instance;  /**< UART外设基地址 */
  uint32_t baudrate;        /**< 波特率 */
  /* 内部使用 */
  UART_HandleTypeDef hal_handle;
};
```

### ADC 描述符

```c
struct adc_desc
{
  ADC_TypeDef *instance;    /**< ADC外设基地址 */
  uint32_t channel;         /**< ADC通道 */
  uint16_t *dma_buffer;     /**< DMA缓冲区 */
  uint16_t buffer_len;      /**< 缓冲区长度 */
  /* 内部使用 */
  ADC_HandleTypeDef hal_handle;
  DMA_HandleTypeDef dma_handle;
};
```

### 层间依赖关系

```
┌─────────────────────────────────────────────────────────┐
│  app/main.c                                             │
│  └── #include "led.h", "relay.h"                        │
├─────────────────────────────────────────────────────────┤
│  device/led.h, device/relay.h                           │
│  └── #include "drv_gpio.h", "board.h" (平台无关)        │
├─────────────────────────────────────────────────────────┤
│  drivers/board.h                                        │
│  └── #include "drv_gpio.h" (平台无关)                   │
├─────────────────────────────────────────────────────────┤
│  drivers/stm32h750vbt6/board.c                          │
│  └── #include "drv_gpio_desc.h" (平台相关)              │
├─────────────────────────────────────────────────────────┤
│  drivers/stm32h750vbt6/drv_gpio.c                       │
│  └── #include "drv_gpio_desc.h", "stm32h7xx_hal.h"      │
└─────────────────────────────────────────────────────────┘
```

**换芯片时只需修改：**
- `drivers/stm32xxx/` 目录下的所有文件
- `core/stm32xxx/` 目录下的启动和中断文件
- `inc/stm32xxx/` 目录下的配置文件

## 错误处理

所有驱动函数返回统一的错误码：
- `DRV_OK (0)`: 操作成功
- `DRV_ERROR (-1)`: 一般错误
- `DRV_BUSY (-2)`: 设备忙
- `DRV_TIMEOUT (-3)`: 操作超时

## 正确性属性

*正确性属性是指在系统所有有效执行中都应保持为真的特征或行为。*

### 分层隔离属性

1. **Device层HAL隔离**: Device层公开头文件（led.h、relay.h）不包含任何 `stm32*.h` 头文件
2. **描述符封装**: 描述符定义文件（drv_xxx_desc.h）包含平台相关类型，但仅在 .c 文件中引用
3. **接口稳定性**: 更换平台时，Driver层公开接口（drv_xxx.h）保持不变

### 描述符封装属性

4. **属性完整性**: 每个描述符包含该外设操作所需的全部硬件属性
5. **极性配置**: GPIO 描述符通过 on_state/off_state 支持不同极性的外设
6. **实例独立性**: 每个外设实例对应一个独立的描述符，互不影响

### 代码简洁性属性

7. **最小抽象**: 驱动函数直接操作描述符，无中间转换层
8. **平台隔离**: 所有平台相关代码集中在 `drivers/{chip}/` 子目录
9. **代码量可控**: GPIO 驱动总代码量不超过 100 行
