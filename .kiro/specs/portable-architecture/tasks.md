# 实现计划: 可移植架构重构（修复分层）

## 概述

修复当前实现中的分层违规问题，将HAL相关代码从BSP层移到Driver层，实现真正的四层架构。

## 任务

- [x] 1. 重构Driver层目录结构
  - [x] 1.1 将 `platform/stm32h7/` 移动到 `drivers/stm32h7/`
    - 统一Driver层目录结构
    - _需求: 1.1, 4.3_
  - [x] 1.2 创建 `drivers/drv_system.h` 系统驱动接口
    - 定义 DRV_System_Init、DRV_System_ErrorHandler 函数原型
    - _需求: 2.1_

- [x] 2. 重构UART驱动实现
  - [x] 2.1 扩展 `drivers/stm32h7/drv_uart_impl.c`
    - 将 `bsp_uart.c` 中的 `MX_Uart1_Init`、`MX_Uart2_Init` 逻辑移入
    - 将 `HAL_UART_MspInit` 回调移入
    - 实现完整的 `DRV_UART_Init` 函数
    - 添加 `DRV_UART_GetHandle` 函数
    - _需求: 1.6, 1.7, 2.3_
  - [x] 2.2 简化 `bsp/bsp_uart.c`
    - 移除所有HAL类型和HAL调用
    - 仅保留BSP层封装函数（如 `BSP_UART_Init`、`debug_uart_read`）
    - 通过 `DRV_UART_*` 接口访问硬件
    - _需求: 1.3, 1.5, 1.8_
  - [x] 2.3 更新 `bsp/bsp_uart.h`
    - 移除 `#include "stm32h7xx_hal.h"`
    - 移除HAL相关宏定义（USARTx_CLK_ENABLE等）
    - _需求: 1.5_

- [x] 3. 重构ADC驱动实现
  - [x] 3.1 扩展 `drivers/stm32h7/drv_adc_impl.c`
    - 将 `bsp_adc.c` 中的 `MX_ADC1_Init`、`MX_ADC2_Init` 逻辑移入
    - 将 `HAL_ADC_MspInit`、`HAL_ADC_MspDeInit` 回调移入
    - 将DMA配置移入
    - 实现完整的 `DRV_ADC_Init` 函数
    - 添加 `DRV_ADC_GetHandle`、`DRV_ADC_GetDMABuffer` 函数
    - _需求: 1.6, 1.7, 2.4_
  - [x] 3.2 简化 `bsp/bsp_adc.c`
    - 移除所有HAL类型和HAL调用
    - 仅保留BSP层封装函数（如 `BSP_ADC_Init`）
    - 通过 `DRV_ADC_*` 接口访问硬件
    - _需求: 1.3, 1.5, 1.8_
  - [x] 3.3 更新 `bsp/bsp_adc.h`
    - 移除 `#include "stm32h7xx_hal.h"`
    - _需求: 1.5_

- [x] 4. 重构GPIO驱动实现
  - [x] 4.1 扩展 `drivers/stm32h7/drv_gpio_impl.c`
    - 添加GPIO端口变量定义（DRV_GPIOA等）
    - 在 `DRV_GPIO_Init` 中添加时钟使能逻辑
    - _需求: 1.7, 2.2_
  - [x] 4.2 简化 `bsp/bsp_gpio.c`
    - 移除 `__HAL_RCC_GPIOx_CLK_ENABLE()` 调用
    - 移除 `#include "stm32h7xx_hal.h"`
    - _需求: 1.5, 1.7_

- [x] 5. 重构系统初始化
  - [x] 5.1 创建 `drivers/stm32h7/drv_system_impl.c`
    - 将 `bsp.c` 中的 `SystemClock_Config` 移入
    - 实现 `DRV_System_Init`（包含HAL_Init和时钟配置）
    - 实现 `DRV_System_ErrorHandler`
    - _需求: 1.7_
  - [x] 5.2 简化 `bsp/bsp.c`
    - 移除 `SystemClock_Config` 函数
    - 移除 `Error_Handler` 函数（改用DRV_System_ErrorHandler）
    - `BSP_Init` 调用 `DRV_System_Init` 和各BSP模块初始化
    - _需求: 1.3, 1.5_

- [x] 6. 更新头文件依赖
  - [x] 6.1 更新 `bsp/bsp.h`
    - 移除HAL头文件包含
    - 包含Driver层头文件
    - _需求: 1.5_
  - [x] 6.2 更新 `bsp/bsp_config.h`
    - 使用Driver层定义的端口和引脚宏
    - 不包含平台特定头文件
    - _需求: 3.1, 3.2_

- [x] 7. 更新构建系统
  - [x] 7.1 更新 `usr/CMakeLists.txt`
    - 将 `platform/stm32h7/` 改为 `drivers/stm32h7/`
    - 添加 `drv_system_impl.c` 到源文件列表
    - _需求: 7.3_

- [ ] 8. 检查点 - 编译验证
  - 确保项目能够成功编译
  - 验证BSP层源文件不包含HAL头文件
  - 如有问题请询问用户

## 备注

- 任务按依赖顺序排列，建议按顺序执行
- 重构过程中保持现有功能不变
- 每个任务完成后建议编译验证
- 关键验收标准：BSP层源文件中不应出现 `#include "stm32h7xx_hal.h"`
