# 实现计划: 可移植架构重构（函数指针封装）

## 概述

基于函数指针封装的可移植架构重构，采用 Linux 驱动风格的 Operations_Struct 和 Device_Object 设计，实现运行时多态和多平台支持。同时预留模拟器扩展接口。

## 任务

- [ ] 1. 重构Driver层接口为函数指针封装
  - [ ] 1.1 重构 `drivers/drv_gpio.h` 为函数指针封装
    - 定义 GPIO_Operations 结构体和 GPIO_Port 设备对象
    - 添加 inline 包装函数（gpio_init、gpio_write、gpio_read、gpio_toggle）
    - 声明平台端口实例指针（drv_gpioa、drv_gpiob等）
    - _需求: 8.2, 8.9_
  - [ ] 1.2 重构 `drivers/drv_uart.h` 为函数指针封装
    - 定义 UART_Operations 结构体和 UART_Device 设备对象
    - 添加 inline 包装函数（uart_transmit、uart_receive等）
    - 声明平台设备实例指针（drv_uart1、drv_uart2）
    - _需求: 8.3, 8.9_
  - [ ] 1.3 重构 `drivers/drv_adc.h` 为函数指针封装
    - 定义 ADC_Operations 结构体和 ADC_Device 设备对象
    - 添加 inline 包装函数（adc_read、adc_start_dma等）
    - 声明平台设备实例指针（drv_adc1、drv_adc2）
    - _需求: 8.4, 8.9_

- [ ] 2. 重构STM32H7平台实现
  - [ ] 2.1 重构 `drivers/stm32h7/drv_gpio_impl.c`
    - 定义 STM32H7 GPIO 操作函数（stm32h7_gpio_init、stm32h7_gpio_write等）
    - 创建 stm32h7_gpio_ops 操作函数集实例
    - 创建 GPIO_Port 设备对象实例（gpioa_device、gpiob_device等）
    - 导出设备指针（drv_gpioa = &gpioa_device）
    - 包含时钟使能逻辑
    - _需求: 8.6, 8.7_
  - [ ] 2.2 重构 `drivers/stm32h7/drv_uart_impl.c`
    - 定义 STM32H7_UART_Private_t 私有数据结构
    - 定义 STM32H7 UART 操作函数（stm32h7_uart_init、stm32h7_uart_transmit等）
    - 创建 stm32h7_uart_ops 操作函数集实例
    - 创建 UART_Device 设备对象实例（uart1_device、uart2_device）
    - 导出设备指针（drv_uart1 = &uart1_device）
    - 包含 MspInit 回调和时钟配置
    - _需求: 8.6, 8.7_
  - [ ] 2.3 重构 `drivers/stm32h7/drv_adc_impl.c`
    - 定义 STM32H7_ADC_Private_t 私有数据结构
    - 定义 STM32H7 ADC 操作函数（stm32h7_adc_init、stm32h7_adc_read等）
    - 创建 stm32h7_adc_ops 操作函数集实例
    - 创建 ADC_Device 设备对象实例（adc1_device、adc2_device）
    - 导出设备指针（drv_adc1 = &adc1_device）
    - 包含 MspInit 回调和 DMA 配置
    - _需求: 8.6, 8.7_

- [ ] 3. 更新BSP层使用新接口
  - [ ] 3.1 更新 `bsp/bsp_gpio.c`
    - 使用 gpio_init、gpio_write、gpio_toggle 等包装函数
    - 移除所有 DRV_GPIO_* 直接调用
    - 确保不包含任何 HAL 头文件
    - _需求: 8.7, 1.5_
  - [ ] 3.2 更新 `bsp/bsp_uart.c`
    - 使用 uart_transmit、uart_receive 等包装函数
    - 通过设备对象指针访问 UART（BSP_DEBUG_UART->ops->transmit）
    - 移除所有 DRV_UART_* 直接调用
    - _需求: 8.7, 1.5_
  - [ ] 3.3 更新 `bsp/bsp_adc.c`
    - 使用 adc_read、adc_start_dma 等包装函数
    - 通过设备对象指针访问 ADC
    - 移除所有 DRV_ADC_* 直接调用
    - _需求: 8.7, 1.5_
  - [ ] 3.4 更新 `bsp/bsp_config.h`
    - 将端口和设备定义改为设备对象指针（drv_gpioc、drv_uart1等）
    - 移除平台特定的宏定义
    - _需求: 3.1, 3.2_

- [ ] 4. 预留模拟器扩展接口
  - [ ] 4.1 创建模拟器目录结构
    - 创建 `project/simulator/` 目录
    - 创建 `project/simulator/drivers/simulator/` 子目录
    - 创建占位文件 `project/simulator/README.md`
    - _需求: 9.2_
  - [ ] 4.2 创建模拟器接口占位文件
    - 创建 `project/simulator/drivers/simulator/drv_gpio_sim.c`（空实现）
    - 创建 `project/simulator/drivers/simulator/drv_uart_sim.c`（空实现）
    - 创建 `project/simulator/drivers/simulator/drv_adc_sim.c`（空实现）
    - 添加注释说明这是预留扩展
    - _需求: 9.1, 9.3_

- [ ] 5. 更新构建系统
  - [ ] 5.1 更新 `usr/CMakeLists.txt`
    - 确保包含所有新的 drv_*_impl.c 文件
    - 添加条件编译支持（为将来多平台做准备）
    - _需求: 7.3_

- [ ] 6. 检查点 - 编译和功能验证
  - 确保项目能够成功编译
  - 验证 BSP 层通过函数指针调用 Driver 层
  - 验证设备对象和操作函数集正确绑定
  - 验证模拟器目录结构创建完成
  - 如有问题请询问用户

## 备注

- 任务按依赖顺序排列，建议按顺序执行
- 重构采用函数指针封装，实现运行时多态
- 每个任务完成后建议编译验证
- 关键验收标准：
  - BSP层通过函数指针间接调用Driver层
  - 设备对象和操作函数集正确绑定
  - 支持多平台实现共存（为将来扩展做准备）
  - 模拟器目录结构预留完成
