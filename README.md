# Codex-STM32

基于 STM32H7/F1 的嵌入式应用开发框架，面向表面面板控制器应用场景。采用分层架构设计（HAL → Driver → Device → App），使用 FreeRTOS + CMSIS-RTOS2 实时操作系统，实现业务逻辑与硬件的完全解耦。

## 项目特性

- **多平台支持**：STM32H750VBT6（Cortex-M7, 480MHz）已完成 / STM32F103RCT6（Cortex-M3, 72MHz）移植中
- **分层架构**：驱动层 → 设备层 → 应用层，禁止跨层调用 HAL
- **RTOS**：FreeRTOS + CMSIS-RTOS2 抽象接口，互斥锁保护共享数据
- **实时调试**：SEGGER SystemView 任务切换与中断时序可视化（J-Link RTT）
- **多构建系统**：Keil MDK (AC6) / GCC + CMake / EIDE (VSCode)
- **通信协议**：Modbus RTU 从机（基于 nanoMODBUS）
- **外设驱动**：GPIO、UART（DMA + 环形缓冲区 + 空闲中断）、ADC（DMA 连续采样 + 数字滤波）
- **通用组件**：环形缓冲区、数字滤波器（MAF / WMAF）

## 平台支持状态

| 平台 | 芯片 | 内核 | 驱动层 | 设备层 | 应用层 |
|------|------|------|--------|--------|--------|
| STM32H750VBT6 | Cortex-M7 | 480MHz | 完成 | 完成 | 完成 |
| STM32F103RCT6 | Cortex-M3 | 72MHz | 移植中 | 待启用 | 待启用 |

> F103 当前仅完成系统初始化与 SystemView 集成，外设驱动（GPIO/UART/ADC）待实现。

## 目录结构

```
.
├── mcu/                          # MCU HAL 库与启动文件
│   ├── stm32h750vbt6/            # H750 平台支持包（HAL, CMSIS, 链接脚本, 启动文件）
│   └── stm32f103rct6/            # F103 平台支持包
├── project/
│   ├── ide/                      # IDE 工程文件
│   │   ├── keil/                 # Keil MDK 工程 (Template.uvprojx)
│   │   ├── gcc/scripts/cmake/    # GCC 交叉编译工具链文件
│   │   └── eide/                 # EIDE (VSCode) 工程
│   ├── Middlewares/Third_Party/  # 第三方中间件
│   │   ├── FreeRTOS/             # FreeRTOS 内核（heap_4）
│   │   ├── CMSIS-FreeRTOS/       # CMSIS-RTOS2 适配层
│   │   ├── CMSIS_5/              # CMSIS 5 头文件
│   │   ├── SystemView/           # SEGGER 实时跟踪
│   │   ├── nanoMODBUS/           # Modbus RTU 协议栈
│   │   └── Printf/              # 轻量级 printf 库
│   └── usr/                      # 用户应用代码
│       ├── app/                  # 应用层（任务入口, 采样, Modbus 映射）
│       ├── device/               # 设备抽象层（LED, Relay, Modbus）
│       ├── drivers/              # 硬件驱动抽象接口与平台实现
│       │   ├── stm32h750vbt6/    # H750 驱动实现
│       │   └── stm32f103rct6/    # F103 驱动实现（移植中）
│       ├── common/               # 通用组件（滤波器, 环形缓冲区）
│       ├── core/                 # 系统核心（中断向量, HAL MSP, syscalls）
│       └── inc/                  # 平台配置头文件（FreeRTOSConfig 等）
└── .vscode/                      # VSCode 调试与构建配置
```

## 快速开始

### 环境要求

| 构建方式 | 工具 |
|---------|------|
| Keil MDK | MDK 5.36+, ARM Compiler 6, J-Link 或 ST-Link |
| GCC/CMake | CMake 3.20+, arm-none-eabi-gcc, OpenOCD 或 J-Link GDB Server |
| EIDE | VSCode + EIDE 插件, arm-none-eabi-gcc |

### 编译

#### Keil MDK

1. 打开 `project/ide/keil/Template.uvprojx`
2. 选择目标平台（STM32H750 或 STM32F103）
3. 编译并下载

#### CMake + GCC

```bash
cd project/usr
mkdir build && cd build

# 默认编译 H750
cmake ..
make -j4

# 编译 F103
cmake -DPLATFORM=stm32f103rct6 ..
make -j4
```

生成文件位于 `build/output/`：
- `Template.elf` — ELF 可执行文件
- `Template.hex` — HEX 烧录文件
- `Template.bin` — BIN 烧录文件
- `Template.map` — 链接映射文件

### 烧录与调试

项目预配置了 4 种 VSCode 调试方案（Cortex-Debug 插件）：

| 配置 | 调试器 | 目标 |
|------|--------|------|
| J-Link H750 | SEGGER J-Link GDB Server | STM32H750VB |
| J-Link F103 | SEGGER J-Link GDB Server | STM32F103RC |
| OpenOCD ST-Link H750 | OpenOCD | STM32H750VB |
| OpenOCD ST-Link F103 | OpenOCD | STM32F103RC |

命令行烧录：

```bash
# OpenOCD
openocd -f interface/jlink.cfg -f target/stm32h7x.cfg \
  -c "program build/output/Template.elf verify reset exit"

# J-Link Commander
JLinkExe -device STM32H750VB -if SWD -speed 4000 \
  -CommanderScript flash.jlink
```

## 架构设计

```
┌─────────────────────────────────────────────┐
│  App Layer        (app/)                    │
│  任务逻辑、数据采集、Modbus 寄存器映射       │
├─────────────────────────────────────────────┤
│  Device Layer     (device/)                 │
│  LED、Relay、Modbus 设备抽象                │
├─────────────────────────────────────────────┤
│  Driver Layer     (drivers/)                │
│  GPIO、UART、ADC 统一接口 + 平台实现        │
├─────────────────────────────────────────────┤
│  HAL Layer        (mcu/)                    │
│  STM32 HAL 库、CMSIS、启动文件              │
└─────────────────────────────────────────────┘
```

约束规则：
- App 层只能调用 Device 层、Common 组件和 CMSIS-RTOS2 接口
- Device 层只能通过 Driver 层抽象接口访问硬件
- 禁止 App/Device 层直接包含 `stm32*.h` 或调用 `HAL_*` 函数

## SEGGER SystemView 实时调试

项目深度集成 SystemView，通过 J-Link RTT 实现零开销任务跟踪。

使用方法：
1. J-Link 连接目标板
2. 启动 SystemView 桌面软件
3. Target → Start Recording
4. 实时查看任务甘特图、CPU 占用率、中断时序

配置入口：`main.c` 中 `SEGGER_SYSVIEW_Conf()` + `SEGGER_SYSVIEW_Start()`

## 应用示例（H750 平台）

### LED 闪烁任务
- 500ms 周期闪烁，演示 GPIO 驱动 + FreeRTOS 任务调度

### Modbus RTU 从机
- 双串口从机（UART1 RS232 / UART2 RS485）
- 从机地址：145
- 保持寄存器：地址 100-199（共 100 个）
- 支持功能码 0x03（读保持寄存器）

### ADC 采样与滤波
- 双通道 ADC（ADC1/ADC2），DMA 连续采样
- 两级数字滤波：MAF（移动平均）→ WMAF（加权移动平均）
- 互斥锁保护采样数据与 Modbus 寄存器的一致性

## 驱动层 API

### GPIO
```c
void gpio_init(gpio_id_t id);
void gpio_set(gpio_id_t id);
void gpio_reset(gpio_id_t id);
void gpio_toggle(gpio_id_t id);
uint8_t gpio_read(gpio_id_t id);
```

### UART
```c
int uart_init(uart_id_t id, uint8_t *buffer, size_t size);
int uart_send(uart_id_t id, const uint8_t *data, size_t len);
int uart_receive(uart_id_t id, uint8_t *data, size_t len, uint32_t timeout);
size_t uart_available(uart_id_t id);
```

### ADC
```c
int adc_init(adc_id_t id);
int adc_start_dma(adc_id_t id);
uint16_t *adc_get_dma_buffer(adc_id_t id);
size_t adc_get_dma_length(adc_id_t id);
```

## 设备层 API

### LED
```c
void led_init(led_id_t id);
void led_on(led_id_t id);
void led_off(led_id_t id);
void led_toggle(led_id_t id);
```

### 继电器
```c
void relay_init(relay_id_t id);
void relay_on(relay_id_t id);
void relay_off(relay_id_t id);
```

### Modbus
```c
void modbus_init(modbus_dev_t *dev, uart_id_t uart, uint8_t addr,
                 uint16_t *regs, uint16_t start, uint16_t count);
void modbus_poll(modbus_dev_t *dev);
void modbus_set_byte_timeout(modbus_dev_t *dev, uint32_t timeout_ms);
void modbus_set_read_timeout(modbus_dev_t *dev, uint32_t timeout_ms);
```

## 移植指南

### 添加新 MCU 平台

1. `mcu/` 下添加 HAL 库、启动文件、链接脚本
2. `project/usr/drivers/<platform>/` 实现驱动接口（board.c, drv_gpio.c, drv_uart.c, drv_adc.c）
3. `project/usr/core/<platform>/` 添加中断处理与 HAL MSP
4. `project/usr/inc/<platform>/` 添加 FreeRTOSConfig.h 等配置
5. `project/usr/CMakeLists.txt` 中添加 PLATFORM 选项与对应配置

### 添加新外设驱动

1. `project/usr/drivers/` 创建驱动头文件（如 `drv_spi.h`）定义抽象接口
2. `project/usr/drivers/<platform>/` 实现平台相关代码
3. `board.c` / `board.h` 中定义硬件资源描述符
4. 更新 CMakeLists.txt 添加源文件

## 代码规范

本项目遵循 BARR-C 嵌入式编码标准，详见 `agents.md`。

核心规则：
- 2 空格缩进
- `if(condition)` 关键字与括号无空格
- 左大括号换行
- 全局变量 `g_` 前缀，静态变量 `s_` 前缀，类型 `_t` 后缀
- 驱动/设备层：小写下划线命名（`uart_init`, `led_toggle`）
- 系统层：`DRV_` 前缀（`DRV_System_Init`）
- Doxygen `@` 风格注释

## 常见问题

**Q: 如何切换编译目标平台？**

CMake：`cmake -DPLATFORM=stm32f103rct6 ..`
Keil：在工程中切换 Target

**Q: 如何修改串口波特率？**

编辑 `project/usr/drivers/<platform>/board.c` 中的 `uart_desc` 结构体 `baudrate` 字段。

**Q: 如何增加 FreeRTOS 任务栈？**

修改任务属性中的 `stack_size`（单位：字节）：
```c
const osThreadAttr_t task_attributes = {
  .stack_size = 512 * 4,  // 2KB
};
```

**Q: 如何调整 ADC 采样率？**

修改 `project/usr/drivers/<platform>/drv_adc.c` 中的 ADC 时钟分频和采样时间。

## 许可证

MIT License

## 联系方式

- 作者：Dylan
- 项目地址：https://github.com/jingwanbadiandalaohu/QP-Frame-STM32
