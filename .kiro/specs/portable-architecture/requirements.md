# 需求文档

## 简介

本规范定义了 STM32 嵌入式项目的架构优化方案，重点关注可移植性设计。目标是将当前紧耦合的 BSP 层重构为分层架构，使代码能够轻松移植到不同的 MCU 平台（如 STM32F1、STM32F4、STM32H7 等），同时保持功能完整性和代码质量。

### 当前项目分析

- 目标芯片：STM32H750VBT6
- RTOS：FreeRTOS + CMSIS-RTOS2 接口
- 现有 BSP 模块：GPIO、UART、ADC、滤波器
- 构建系统：支持多工具链（Keil、GCC、EIDE）
- 中间件：Printf、FreeRTOS、CMSIS_5、CMSIS-FreeRTOS

### 现有目录结构

```
project/stm32h750vbt6/
├── ide/                    # 多工具链支持
│   ├── eide/              # Embedded IDE 配置
│   ├── gcc/               # ARM GCC 配置
│   └── keil/              # Keil MDK 配置
├── Middlewares/           # 中间件
│   └── Third_Party/
│       ├── CMSIS_5/       # CMSIS 核心
│       ├── CMSIS-FreeRTOS/# CMSIS-RTOS2 适配层
│       ├── FreeRTOS/      # FreeRTOS 内核
│       └── Printf/        # 轻量级 printf 实现
└── usr/                   # 用户代码
    ├── bsp/               # 板级支持包
    ├── core/              # 启动和中断处理
    └── inc/               # 配置头文件
```

## 术语表

- **HAL_Layer**: 硬件抽象层，直接与 MCU 寄存器和厂商 HAL 库交互的最底层
- **Driver_Layer**: 驱动层，提供与硬件无关的外设操作接口
- **BSP_Layer**: 板级支持包，针对特定开发板的配置和初始化
- **App_Layer**: 应用层，业务逻辑代码
- **Port_Interface**: 移植接口，定义平台相关代码必须实现的函数原型
- **Platform_Config**: 平台配置文件，包含 MCU 特定的宏定义和类型映射
- **CMSIS_RTOS2**: ARM 定义的 RTOS 标准接口，支持 FreeRTOS、RTX 等多种 RTOS 实现

## 需求

### 需求 1: 分层架构设计

**用户故事:** 作为嵌入式开发者，我希望代码按照清晰的层次结构组织，以便我可以独立修改某一层而不影响其他层。

#### 验收标准

1. 架构应定义四个独立的层次：HAL_Layer（硬件层）、Driver_Layer（抽象硬件层）、BSP_Layer（设备层）和 App_Layer（应用层）
2. 当上层调用下层时，架构应仅使用下层定义的公共接口
3. 架构应禁止 App_Layer 和 BSP_Layer 直接访问 HAL_Layer，所有硬件访问必须通过 Driver_Layer
4. 当修改 HAL_Layer 实现时，Driver_Layer 接口应保持不变
5. BSP_Layer 不应包含任何 HAL 库头文件（如 `stm32h7xx_hal.h`），仅通过 Driver_Layer 接口访问硬件
6. 所有 `HAL_*_MspInit()` 回调函数应位于 Driver_Layer 的平台实现文件中，不应出现在 BSP_Layer
7. 所有时钟使能宏（如 `__HAL_RCC_*_CLK_ENABLE()`）应位于 Driver_Layer 的平台实现文件中
8. Driver_Layer 应封装所有 HAL 类型（如 `UART_HandleTypeDef`），BSP_Layer 仅使用抽象句柄类型（如 `UART_Handle_t`）

### 需求 2: 平台抽象接口

**用户故事:** 作为嵌入式开发者，我希望硬件相关代码通过统一接口访问，以便移植到新平台时只需实现接口而不修改业务代码。

#### 验收标准

1. Port_Interface 应定义抽象类型：GPIO_Handle、UART_Handle、ADC_Handle 和 Timer_Handle
2. 当访问 GPIO 功能时，Driver_Layer 应使用平台无关的函数签名：`int DRV_GPIO_Init(GPIO_Config_t *config)`、`int DRV_GPIO_Write(GPIO_Handle_t handle, uint16_t pin, uint8_t state)`、`int DRV_GPIO_Read(GPIO_Handle_t handle, uint16_t pin, uint8_t *state)`
3. 当访问 UART 功能时，Driver_Layer 应使用平台无关的函数签名：`int DRV_UART_Init(UART_Config_t *config)`、`int DRV_UART_Transmit(UART_Handle_t handle, uint8_t *data, uint16_t len)`、`int DRV_UART_Receive(UART_Handle_t handle, uint8_t *data, uint16_t len)`
4. 当访问 ADC 功能时，Driver_Layer 应使用平台无关的函数签名：`int DRV_ADC_Init(ADC_Config_t *config)`、`int DRV_ADC_Read(ADC_Handle_t handle, uint16_t *value)`
5. Port_Interface 应定义简单的返回值约定：0 表示成功，负值表示失败

### 需求 3: 配置与实现分离

**用户故事:** 作为嵌入式开发者，我希望硬件配置参数与驱动实现分离，以便更换引脚或外设时只需修改配置文件。

#### 验收标准

1. BSP_Layer 应在专用配置头文件中定义所有引脚分配、时钟配置和外设映射
2. 当 GPIO 引脚分配变更时，修改应仅需更新配置文件而无需修改驱动实现
3. Platform_Config 应使用条件编译，基于单一平台宏选择 MCU 特定的定义
4. 当添加新 MCU 系列支持时，架构应仅需添加新的平台特定文件而无需修改现有可移植代码

### 需求 4: 目录结构规范

**用户故事:** 作为嵌入式开发者，我希望项目目录结构清晰反映分层架构，同时保持现有的多工具链和中间件组织方式。

#### 验收标准

1. 项目结构应保持现有的 `ide/` 目录组织，支持 Keil、GCC、EIDE 多工具链
2. 项目结构应保持现有的 `Middlewares/Third_Party/` 目录，包含 Printf、FreeRTOS、CMSIS 相关组件
3. `usr/` 目录应重构为：`drivers/` 用于 Driver_Layer 接口，`platform/` 用于平台实现，`bsp/` 用于 BSP_Layer，`app/` 用于 App_Layer，`common/` 用于平台无关模块（如滤波器）
4. 当为特定平台构建时，构建系统应仅包含相关的平台子目录

### 需求 5: 滤波器模块可移植性

**用户故事:** 作为嵌入式开发者，我希望 ADC 滤波器算法与硬件解耦，以便滤波器可以在不同平台复用。

#### 验收标准

1. Filter_Module 应仅依赖标准 C 类型（stdint.h），不包含任何硬件依赖
2. Filter_Module 应从 BSP 目录移动到 common 目录，保持现有接口不变

### 需求 6: RTOS 抽象层（基于 CMSIS-RTOS2）

**用户故事:** 作为嵌入式开发者，我希望使用 CMSIS-RTOS2 标准接口访问 RTOS 功能，以便我可以在 FreeRTOS、RTX 和其他兼容 RTOS 之间切换。

#### 验收标准

1. App_Layer 应使用 CMSIS-RTOS2 接口（cmsis_os2.h）进行线程、互斥锁、信号量和队列操作
2. 当创建线程时，App_Layer 应使用 CMSIS-RTOS2 接口：`osThreadNew(func, arg, &attr)` 而非直接调用 FreeRTOS API
3. 当使用延时时，App_Layer 应使用 CMSIS-RTOS2 接口：`osDelay(ms)` 而非直接调用 `vTaskDelay()`
4. RTOS 实现应可通过更换 CMSIS-RTOS2 适配层在编译时切换，无需修改应用代码
5. 项目应保留 FreeRTOSConfig.h 用于底层 RTOS 配置，但应用代码不应直接依赖 FreeRTOS 特定 API

### 需求 7: 构建系统支持

**用户故事:** 作为嵌入式开发者，我希望构建系统支持多平台和多工具链编译，以便我可以通过简单配置切换目标平台和开发环境。

#### 验收标准

1. 构建系统应支持通过配置变量选择目标平台（如 `PLATFORM=stm32h7`）
2. 构建系统应继续支持现有的多工具链配置（Keil、GCC、EIDE）
3. 当为 STM32H7 构建时，构建系统应自动包含 `platform/stm32h7/` 源文件并排除其他平台目录
4. 当添加新平台时，构建系统应仅需添加平台目录并更新相应的构建配置文件
