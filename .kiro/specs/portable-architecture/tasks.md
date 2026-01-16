# 实现计划: 可移植架构重构（描述符风格）

## 概述

基于面向对象描述符风格的可移植架构重构，采用结构体封装硬件属性，通过编译时多态实现多平台支持。参考 STM32F4_LEDOBJ 项目设计。

## 任务

- [-] 1. 创建驱动层公开接口
  - [ ] 1.1 创建 `drivers/drv_gpio.h`
    - 前向声明 `struct gpio_desc` 和 `gpio_desc_t` 类型
    - 声明 GPIO 操作函数（gpio_init、gpio_set、gpio_on、gpio_off、gpio_toggle、gpio_read）
    - 不包含任何平台相关头文件
  - [ ] 1.2 创建 `drivers/drv_uart.h`
    - 前向声明 `struct uart_desc` 和 `uart_desc_t` 类型
    - 声明 UART 操作函数（uart_init、uart_deinit、uart_transmit、uart_receive 等）
  - [ ] 1.3 创建 `drivers/drv_adc.h`
    - 前向声明 `struct adc_desc` 和 `adc_desc_t` 类型
    - 声明 ADC 操作函数（adc_init、adc_deinit、adc_read、adc_start_dma 等）
  - [ ] 1.4 创建 `drivers/drv_system.h`
    - 声明系统初始化函数 DRV_System_Init
    - 声明错误处理函数 DRV_System_ErrorHandler

- [ ] 2. 创建 STM32H750 平台实现
  - [ ] 2.1 创建 `drivers/stm32h750vbt6/drv_gpio_desc.h`
    - 定义 `struct gpio_desc` 结构体（port、pin、on_state、off_state）
    - 包含 `stm32h7xx_hal.h`
  - [ ] 2.2 创建 `drivers/stm32h750vbt6/drv_gpio.c`
    - 实现 gpio_init（时钟使能、引脚配置）
    - 实现 gpio_set、gpio_on、gpio_off、gpio_toggle、gpio_read
  - [ ] 2.3 创建 `drivers/stm32h750vbt6/drv_uart_desc.h`
    - 定义 `struct uart_desc` 结构体（instance、baudrate、hal_handle）
  - [ ] 2.4 创建 `drivers/stm32h750vbt6/drv_uart.c`
    - 实现 uart_init、uart_deinit
    - 实现 uart_transmit、uart_receive（阻塞模式）
    - 实现 uart_transmit_it、uart_receive_it（中断模式）
  - [ ] 2.5 创建 `drivers/stm32h750vbt6/drv_adc_desc.h`
    - 定义 `struct adc_desc` 结构体（instance、channel、dma_buffer、hal_handle）
  - [ ] 2.6 创建 `drivers/stm32h750vbt6/drv_adc.c`
    - 实现 adc_init、adc_deinit
    - 实现 adc_read（单次转换）
    - 实现 adc_start_dma、adc_stop_dma、adc_get_average
  - [ ] 2.7 创建 `drivers/stm32h750vbt6/drv_system.c`
    - 实现 DRV_System_Init（HAL_Init + 时钟配置）
    - 实现 DRV_System_ErrorHandler

- [ ] 3. 创建板级配置
  - [ ] 3.1 创建 `drivers/board.h`
    - 包含 drv_gpio.h、drv_uart.h、drv_adc.h
    - 声明设备实例指针（led1、relay1、debug_uart、adc1 等）
  - [ ] 3.2 创建 `drivers/stm32h750vbt6/board.c`
    - 包含 board.h 和各 drv_xxx_desc.h
    - 定义 LED 描述符实例（PC13，低电平点亮）
    - 定义继电器描述符实例
    - 定义 UART 描述符实例
    - 定义 ADC 描述符实例
    - 导出设备指针

- [ ] 4. 创建设备层
  - [ ] 4.1 创建 `device/led.h`
    - 包含 drv_gpio.h 和 board.h
    - 定义 LED 操作宏（led_init、led_on、led_off、led_toggle）
  - [ ] 4.2 创建 `device/relay.h`
    - 包含 drv_gpio.h 和 board.h
    - 定义继电器操作宏（relay_init、relay_on、relay_off）

- [ ] 5. 更新应用层
  - [ ] 5.1 更新 `app/main.c`
    - 包含 drv_system.h、led.h、relay.h
    - 调用 DRV_System_Init 初始化系统
    - 调用 led_init、relay_init 初始化设备
    - 实现 LED 闪烁测试

- [ ] 6. 更新构建配置
  - [ ] 6.1 更新 CMakeLists.txt
    - 添加 drivers/stm32h750vbt6/ 源文件
    - 添加头文件包含路径
    - 移除旧的 bsp 相关文件

- [ ] 7. 检查点 - 编译和功能验证
  - 确保项目能够成功编译
  - 验证 LED 闪烁功能正常
  - 验证设备层不包含任何 HAL 头文件
  - 如有问题请询问用户

## 备注

- 任务按依赖顺序排列，建议按顺序执行
- 采用描述符封装，编译时多态
- 每个任务完成后建议编译验证
- 关键验收标准：
  - 设备层（led.h、relay.h）不包含任何 `stm32*.h`
  - 所有平台相关代码集中在 `drivers/stm32h750vbt6/`
  - 换芯片只需新建 `drivers/{new_chip}/` 目录并实现
