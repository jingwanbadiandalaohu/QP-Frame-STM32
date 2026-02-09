# Codex 规则

## 代码注释

- 生成新代码时，为关键逻辑和非显而易见步骤添加简短中文注释。
- 不为简单赋值或自解释语句添加无意义注释。

## 交互称呼

- 每次和我交流都要叫我名字 dylan

## 固定流程

- 永远只读取 agents.md，并默认使用 UTF-8。

---

# Kiro 工作规则

## 文件创建规则

**禁止随意创建 Markdown 文档文件！**

- ❌ 不要为了解释概念而创建 MD 文件
- ❌ 不要为了写文档而创建 MD 文件
- ❌ 不要为了总结而创建 MD 文件

**只在以下情况创建文件：**
- ✅ 用户明确要求创建文件
- ✅ 项目必需的配置文件（如 CMakeLists.txt）
- ✅ 项目必需的代码文件（.c, .h）

**正确做法：**
- 直接在对话中回答问题
- 用代码注释解释概念
- 在现有文件中添加注释

## 回答方式

- 简洁明了，不要过度冗长
- 直接给出解决方案
- 用代码和注释说明，而不是文档

---

# C代码风格规范 (基于BARR-C嵌入式标准)

## 关键字与括号格式
- if/else/while/for 关键字与左括号之间不能有空格

正确示例：
```c
if(condition)
while(condition)
for(int i = 0; i < n; i++)
```

错误示例：
```c
if (condition)
while (condition)
```

## 大括号格式
- if/else/while/for 语句的左大括号必须换行

正确示例：
```c
if(condition)
{
  // code
}
```

错误示例：
```c
if(condition) {
  // code
}
```

## 函数定义
- 函数左大括号必须换行

正确示例：
```c
void MyFunction(void)
{
  // code
}
```

## 命名规范
- 使用2空格缩进
- 函数命名按模块职责使用统一前缀（如 Driver_/Device_/App_），不强制 BSP_/MX_。
- 全局变量前缀：g_
- 静态变量前缀：s_ 或无前缀
- 宏定义全大写，用下划线分隔：`#define MAX_BUFFER_SIZE 256`
- 类型定义后缀 `_t`：`typedef uint8_t byte_t;`

## 安全编码
- 使用 `snprintf` 代替 `sprintf` (防止缓冲区溢出)
- 指针初始化为 `NULL`
- 局部变量在声明时初始化
- 硬件寄存器和中断共享变量必须使用 `volatile`
- 所有可能失败的函数必须检查返回值

## 头文件规范
- 必须有 include guard
```c
#ifndef MODULE_NAME_H
#define MODULE_NAME_H
// ...
#endif /* MODULE_NAME_H */
```
- 包含顺序：系统头文件 → HAL库 → 项目头文件

## 代码行规范
- 每行不超过100字符
- 一行只做一件事，避免复合语句

## CMakeLists.txt 注释规范
- 文件路径注释必须与 `#` 号对齐，使用空格填充
- 注释内容简洁明了，说明文件用途

正确示例：
```cmake
../Middlewares/Third_Party/FreeRTOS/tasks.c                                         #任务代码
../Middlewares/Third_Party/FreeRTOS/queue.c                                         #队列代码
```

错误示例：
```cmake
../Middlewares/Third_Party/FreeRTOS/tasks.c         #任务代码
../Middlewares/Third_Party/FreeRTOS/queue.c       #队列代码
```

## 注释规范 (基于BARR-C与Doxygen标准)

### 基本原则
- 注释使用完整句子，语法正确，标点恰当
- 注释应解释"为什么"而非"怎么做"，假设读者了解C语言
- 注释数量和长度应与代码复杂度成正比
- 避免冗余注释，如 `numero <<= 2; // 左移2位` 这种显而易见的说明

### 注释格式
- 单行注释使用 `//`，多行注释使用 `/* ... */`
- Doxygen文档注释使用 `/** ... */` 格式
- Doxygen命令使用 `@` 前缀（如 `@brief`），而非 `\`

### 文件头注释 (必须)
每个源文件和头文件必须包含文件头注释：
```c
/**
 * @file    filename.c
 * @author  作者名(Dylan)
 * @date    2026-01-15(创建时间)
 * @brief   文件功能简要说明
 *
 * @details 详细说明（可选），描述文件的主要功能、
 *          使用方法或注意事项
 */
```

### 函数注释 (必须)
所有函数必须有Doxygen格式的注释：
```c
/**
 * @brief   函数功能简要说明
 *
 * @details 详细说明（可选）
 *
 * @param[in]   param1  输入参数说明
 * @param[out]  param2  输出参数说明
 * @param[in,out] param3 输入输出参数说明
 *
 * @return  返回值说明
 * @retval  0   成功
 * @retval  -1  失败
 *
 * @note    注意事项（可选）
 * @warning 警告信息（可选）
 */
int BSP_Function(int param1, int *param2, int *param3);
```

无参数或无返回值时：
```c
/**
 * @brief   初始化系统
 *
 * @param   None
 * @return  None
 */
void BSP_Init(void);
```

### 变量与宏注释
```c
/**
 * @brief 变量简要说明
 */
uint32_t g_systemTick = 0;

int g_errorCode = 0;  /**< 行尾简短说明 */

/**
 * @brief 宏定义说明
 */
#define MAX_RETRY_COUNT  3
```

### 枚举与结构体注释
```c
/**
 * @brief 状态枚举
 */
typedef enum
{
  STATE_IDLE = 0,   /**< 空闲状态 */
  STATE_RUNNING,    /**< 运行状态 */
  STATE_ERROR       /**< 错误状态 */
} SystemState_t;

/**
 * @brief 配置结构体
 */
typedef struct
{
  uint32_t baudrate;  /**< 波特率 */
  uint8_t  dataBits;  /**< 数据位 */
  uint8_t  stopBits;  /**< 停止位 */
} UartConfig_t;
```

### 特殊标记注释
使用大写标记突出重要信息：

| 标记 | 用途 | 示例 |
|------|------|------|
| `TODO:` | 待完成的功能 | `// TODO: 实现超时处理` |
| `FIXME:` | 需要修复的问题 | `// FIXME: 边界条件未处理` |
| `WARNING:` | 修改代码的风险提示 | `// WARNING: 延时值经验确定，移植时需调整` |
| `NOTE:` | 重要说明或设计原因 | `// NOTE: 芯片勘误表要求此处延时` |
| `HACK:` | 临时解决方案 | `// HACK: 绕过硬件bug` |

示例：
```c
// WARNING: 此延时值针对480MHz主频，修改时钟配置需重新调整
HAL_Delay(10);

// NOTE: 根据STM32H750勘误表ES0396，ADC校准后需等待
for(volatile int i = 0; i < 100; i++);

// TODO(张三): 添加DMA传输支持
// FIXME: 高波特率下偶发丢包
```

### 代码块注释
复杂算法或多步骤操作应分块注释：
```c
// Step 1: 配置GPIO时钟
__HAL_RCC_GPIOA_CLK_ENABLE();

// Step 2: 配置引脚模式
GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

// Step 3: 设置初始电平
HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
```

### 外部引用注释
引用外部文档（数据手册、设计规格等）时必须注明来源：
```c
// 参考: STM32H750参考手册 RM0433 Rev7, 第25.4.3节
// 参考: 项目设计文档 doc/uart_protocol.md
```

### 禁止事项
- 禁止注释掉大段代码提交，应使用版本控制
- 禁止使用无意义注释如 `i++; // i加1`
- 禁止注释与代码不一致（代码修改后必须同步更新注释）
- 禁止在注释中包含敏感信息（密码、密钥等）

---

# 项目偏好（自动提取）

以下规则由当前项目代码与配置归纳得到，后续默认遵循。

## 架构与依赖约束

- 默认采用四层架构：HAL_Layer → Driver_Layer → Device_Layer → App_Layer。
- App_Layer 允许调用：Device_Layer、Common Modules、CMSIS-RTOS2。
- Device_Layer 仅通过 Driver_Layer 抽象接口访问硬件。
- 禁止 App_Layer 与 Device_Layer 直接包含或调用 HAL 头文件/接口（如 `stm32*.h`、`HAL_*`）。

## C工程组织偏好

- `main.c` 保持“按层分组 include + 中文分组注释”的结构顺序。
- RTOS 任务创建采用：静态任务函数声明 → `osThreadAttr_t` 局部常量 → `osThreadNew()`。
- 外设初始化优先顺序：系统初始化 → 设备初始化 → 驱动启动（如DMA）→ RTOS启动。
- 对可能失败的初始化与关键调用默认检查返回值，失败路径进入统一错误处理。

## 命名与实现习惯

- 模块级全局变量使用 `g_` 前缀；文件内静态对象使用 `s_` 前缀。
- 描述符类型与实例沿用 `_t` 后缀和 `xxx_desc_t` 风格。
- 新增代码默认保持2空格缩进与不超过100字符单行长度。

## 提交流程偏好

- 当用户请求生成提交信息时，默认输出中文 Conventional Commits。
- 提交信息格式：`<type>: <中文描述>`。
- 类型集合：`feat`、`fix`、`docs`、`style`、`refactor`、`test`、`chore`。

## 回答偏好

- 优先给可执行改动与落地结果，不写额外文档。
- 说明尽量短句中文，必要时用代码注释解释设计原因。

## 偏好沉淀工作流

- 先执行用户当前任务，不因偏好整理中断交付。
- 每次任务结束时，给出“可沉淀偏好候选”清单（仅列新增或变更项）。
- 当用户回复“沉淀”时，自动将候选项追加写入 `agents.md`。
- 若用户未回复“沉淀”，则不写入文件，只在当次会话临时使用。

## 任务执行最小流程（迁移自 everything-claude-code 思路）

- 先给出1~2句执行计划，再开始改代码。
- 改动前先定位影响范围（调用点、头文件依赖、任务/中断上下文）。
- 每次只做一个逻辑闭环改动，避免大范围混改。
- 改完必须做最小验证：编译检查 + 关键路径自测。
- 输出结果时必须包含：改了什么、为什么、怎么验证。
- 若存在风险或未知项，明确列出假设与回退点。

## 代码评审清单（嵌入式C）

- 是否破坏分层：App/Device 是否直接触达 HAL。
- 失败路径是否完整：返回值、超时、空指针、边界值。
- 共享资源是否安全：中断/任务并发、volatile、临界区。
- 注释是否解释“为什么”，且与代码保持一致。
- 是否符合现有风格：2空格缩进、if(condition) 与大括号换行。

## 高风险操作防护

- 未经用户明确允许，不执行破坏性命令（如 reset --hard、批量删除）。
- 涉及脚本批量替换或跨目录修改时，先给变更范围摘要再执行。
- 对关键文件（main.c、启动流程、RTOS任务创建）修改时，必须给出影响说明。
