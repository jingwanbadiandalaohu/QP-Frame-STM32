# Codex-STM32

基于 STM32H7/F1 的嵌入式应用开发框架，采用分层架构设计，集成 FreeRTOS 实时操作系统。

## 项目特性

- **多 MCU 支持**：STM32H750VBT6、STM32F103RCT6
- **分层架构**：驱动层 → 设备层 → 应用层
- **RTOS 支持**：集成 FreeRTOS + CMSIS-RTOS2 接口
- **多 IDE 支持**：Keil MDK、GCC/CMake、EIDE
- **通信协议**：Modbus RTU（基于 nanoMODBUS）
- **外设驱动**：GPIO、UART、ADC（支持 DMA）
- **工具组件**：环形缓冲区、数字滤波器（MAF/WMAF）

## 目录结构

```
.
├── mcu/                          # MCU 相关文件
│   └── stm32h750vbt6/            # STM32H750 CMSIS 和 HAL 库
├── project/
│   ├── ide/                      # IDE 工程文件
│   │   ├── keil/                 # Keil MDK 工程
│   │   ├── gcc/                  # GCC 工具链脚本
│   │   └── eide/                 # EIDE 工程
│   ├── Middlewares/              # 第三方中间件
│   │   └── Third_Party/
│   │       ├── FreeRTOS/         # FreeRTOS 内核
│   │       ├── CMSIS-FreeRTOS/   # CMSIS-RTOS2 接口
│   │       ├── nanoMODBUS/       # Modbus 协议栈
│   │       └── Printf/           # 轻量级 printf 库
│   └── usr/                      # 用户代码
│       ├── app/                  # 应用层
│       ├── device/               # 设备层（LED、继电器、Modbus）
│       ├── drivers/              # 驱动层（GPIO、UART、ADC）
│       ├── common/               # 通用组件（滤波器、环形缓冲区）
│       ├── core/                 # 核心文件（启动、中断、系统调用）
│       └── inc/                  # 配置头文件
└── .codex/                       # Codex 技能库
    └── skills/
        ├── freertos-task-design/ # FreeRTOS 任务设计技能
        └── stm32h7-driver-dev/   # STM32H7 驱动开发技能
```

## 快速开始

### 环境要求

**Keil MDK 开发**
- Keil MDK 5.36+
- ARM Compiler 6
- J-Link 或 ST-Link 调试器

**GCC/CMake 开发**
- CMake 3.20+
- ARM GCC 工具链（arm-none-eabi-gcc）
- OpenOCD 或 J-Link GDB Server

### 编译方法

#### 使用 Keil MDK

1. 打开 `project/ide/keil/Template.uvprojx`
2. 选择目标平台（STM32H750 或 STM32F103）
3. 编译并下载到目标板

#### 使用 CMake + GCC

```bash
cd project/usr
mkdir build && cd build
cmake ..
make -j4
```

生成的文件位于 `build/output/` 目录：
- `Template.elf` - ELF 可执行文件
- `Template.hex` - HEX 烧录文件
- `Template.bin` - BIN 烧录文件
- `Template.map` - 链接映射文件

### 烧录程序

**使用 OpenOCD**
```bash
openocd -f interface/jlink.cfg -f target/stm32h7x.cfg \
  -c "program build/output/Template.elf verify reset exit"
```

**使用 J-Link**
```bash
JLinkExe -device STM32H750VB -if SWD -speed 4000 \
  -CommanderScript flash.jlink
```

## 应用示例

当前示例实现了以下功能：

### 1. LED 闪烁任务
- 500ms 周期闪烁
- 演示基本 GPIO 驱动和 FreeRTOS 任务

### 2. Modbus RTU 从机
- 双串口 Modbus 从机（UART1/UART2）
- 从机地址：145
- 保持寄存器：100-199（共 100 个）
- 支持功能码 0x03（读保持寄存器）

**测试命令（Modbus Poll）**
```
从机地址: 145
功能码: 03 (Read Holding Registers)
起始地址: 100
数量: 10
```

### 3. ADC 采样与滤波
- 双通道 ADC（ADC1/ADC2）
- DMA 连续采样
- 两级数字滤波：
  - 一级：移动平均滤波（MAF）
  - 二级：加权移动平均滤波（WMAF）
- 串口输出原始值和滤波值

## 驱动层 API

### GPIO 驱动
```c
void gpio_init(gpio_id_t id);
void gpio_set(gpio_id_t id);
void gpio_reset(gpio_id_t id);
void gpio_toggle(gpio_id_t id);
uint8_t gpio_read(gpio_id_t id);
```

### UART 驱动
```c
int uart_init(uart_id_t id, uint8_t *buffer, size_t size);
int uart_send(uart_id_t id, const uint8_t *data, size_t len);
int uart_receive(uart_id_t id, uint8_t *data, size_t len, uint32_t timeout);
size_t uart_available(uart_id_t id);
```

### ADC 驱动
```c
int adc_init(adc_id_t id);
int adc_start_dma(adc_id_t id);
uint16_t *adc_get_dma_buffer(adc_id_t id);
size_t adc_get_dma_length(adc_id_t id);
```

## 设备层 API

### LED 设备
```c
void led_init(led_id_t id);
void led_on(led_id_t id);
void led_off(led_id_t id);
void led_toggle(led_id_t id);
```

### 继电器设备
```c
void relay_init(relay_id_t id);
void relay_on(relay_id_t id);
void relay_off(relay_id_t id);
```

### Modbus 设备
```c
void modbus_init(modbus_dev_t *dev, uart_id_t uart, uint8_t addr, 
                 uint16_t *regs, uint16_t start, uint16_t count);
void modbus_poll(modbus_dev_t *dev);
void modbus_set_byte_timeout(modbus_dev_t *dev, uint32_t timeout_ms);
void modbus_set_read_timeout(modbus_dev_t *dev, uint32_t timeout_ms);
```

## 代码规范

本项目遵循 **BARR-C 嵌入式编码标准**，详见 `.kiro/steering/code-style.md`

### 关键规范
- 关键字与括号无空格：`if(condition)`
- 左大括号换行
- 2 空格缩进
- 函数命名：
  - 驱动层/设备层：小写+下划线（`uart_init`, `led_toggle`）
  - 系统层：`DRV_` 前缀（`DRV_System_Init`）
- 全局变量前缀：`g_`
- 静态变量前缀：`s_`
- 类型定义后缀：`_t`（`uart_desc_t`, `adc_desc_t`）
- Doxygen 注释格式

## 移植指南

### 添加新 MCU 支持

1. 在 `mcu/` 下添加新 MCU 的 HAL 库
2. 在 `project/usr/drivers/` 下创建平台驱动实现
3. 在 `project/usr/core/` 下添加启动文件和中断向量表
4. 在 `project/usr/inc/` 下添加配置头文件
5. 更新 `CMakeLists.txt` 中的 `PLATFORM` 选项

### 添加新外设驱动

1. 在 `project/usr/drivers/` 下创建驱动头文件（如 `drv_spi.h`）
2. 在 `project/usr/drivers/<platform>/` 下实现平台相关代码
3. 在 `board.c` 中定义硬件资源
4. 更新 `CMakeLists.txt` 添加源文件

## 常见问题

### Q: 如何修改串口波特率？
A: 编辑 `project/usr/drivers/<platform>/board.c` 中的 `uart_desc` 结构体：
```c
static struct uart_desc s_uart2_rs485 = {
  .instance = USART2,
  .baudrate = 115200  // 修改此处
};
```

### Q: 如何增加 FreeRTOS 任务栈大小？
A: 修改任务创建时的 `stack_size` 参数（单位：字，1 字 = 4 字节）：
```c
const osThreadAttr_t task_attributes = {
  .stack_size = 512 * 4,  // 2KB 栈空间
};
```

### Q: 如何调整 ADC 采样率？
A: 修改 `project/usr/drivers/<platform>/drv_adc.c` 中的 ADC 时钟分频和采样时间配置。

## 许可证

本项目采用 MIT 许可证，详见 LICENSE 文件。

## 贡献指南

欢迎提交 Issue 和 Pull Request！

提交代码前请确保：
1. 遵循项目代码规范
2. 添加必要的 Doxygen 注释
3. 测试通过所有目标平台

## 联系方式

- 作者：Dylan
- 项目地址：https://github.com/jingwanbadiandalaohu/QP-Frame-STM32

---

**最后更新：2026-02-03**
