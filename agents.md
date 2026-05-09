## 交互与流程

### 交互称呼

- 每次和我交流都要叫我名字 Dylan。

### 固定流程

- 优先读取 `agents.md`，并默认使用 UTF-8；仅在任务确有必要时读取其他文件。
- 先执行用户当前任务，不因偏好整理中断交付。

### 回答方式

- 简洁明了，不要过度冗长。
- 直接给出解决方案。
- 优先给可执行改动与落地结果，不写额外文档。
- 说明尽量短句中文，必要时用代码注释解释设计原因。

---

## MDK (Keil) 工程操作

### 工程结构理解

当需要理解任何 MDK 工程结构时，按以下通用步骤分析：

#### 1. 定位工程文件

MDK 工程文件格式：`*.uvprojx`（XML 格式）

常见位置：
- 项目根目录
- `project/` 或 `ide/keil/` 子目录
- 使用 `fileSearch` 查找：`fileSearch(query=".uvprojx")`

#### 2. 提取关键配置信息

**目标芯片配置：**
- `<TargetName>`：编译目标名称
- `<Device>`：具体芯片型号（如 STM32F103RC、STM32H750VBTx）
- `<Cpu>`：内核类型与内存配置（如 Cortex-M3、Cortex-M7）

**编译器配置：**
- `<Define>`：预定义宏（如 `USE_HAL_DRIVER`、`STM32H750xx`）
- `<IncludePath>`：头文件包含路径（分号 `;` 分隔）
- `<Optim>`：优化等级（0=无优化，1=Level 1，2=Level 2，3=Level 3）
- `<uC99>`：是否启用 C99 标准

**链接器配置：**
- `<ScatterFile>`：分散加载文件（`.sct`）
- `<TextAddressRange>`：代码段起始地址
- `<DataAddressRange>`：数据段起始地址

#### 3. 分析项目分组结构

**查找 `<Groups>` 节点：**
- 每个 `<Group>` 代表一个分组
- `<GroupName>`：分组名称
- `<Files>`：包含的文件列表
- `<FilePath>`：文件相对路径

**常见分组模式：**
- **应用层**：`App`、`Application`、`User`
- **驱动层**：`Drivers`、`BSP`、`HAL_Driver`
- **中间件**：`Middlewares`、`RTOS`、`Third_Party`
- **启动文件**：`Startup`、`Core`、`CMSIS`
- **外设库**：`STM32_HAL`、`StdPeriph_Driver`

#### 4. 提取头文件包含路径

**使用 PowerShell 快速提取：**
```powershell
Select-String -Path "path/to/project.uvprojx" -Pattern "IncludePath" -Context 0,1 | Select-Object -First 5
```

**使用 grep 搜索：**
```bash
grepSearch(query="IncludePath", includePattern="**/*.uvprojx")
```

**路径特点：**
- 相对于工程文件所在目录
- 使用反斜杠 `\` 或 `..` 表示相对路径
- 多个路径用分号 `;` 分隔

#### 5. 输出结构摘要

分析完成后，输出以下信息：

**基本信息：**
- 工程文件路径
- 目标芯片型号与内核
- 编译器版本与优化等级

**项目结构：**
- 分组列表（树形展示）
- 每个分组包含的文件数量
- 文件类型分布（`.c`、`.h`、`.s`）

**配置信息：**
- 预定义宏列表
- 头文件包含路径列表（按层次分类）
- 链接器配置（Flash/RAM 地址）

**架构分析（可选）：**
- 识别项目采用的架构模式（分层、模块化等）
- 检查是否存在架构约束违反（如跨层调用）

#### 6. 实用技巧

**快速定位关键文件：**
- `main.c`：通常在 `App` 或 `User` 分组
- 启动文件（`.s`）：通常在 `Startup` 或 `Core` 分组
- 中断处理（`*_it.c`）：通常在 `Core` 或 `User` 分组
- HAL 配置（`*_hal_conf.h`）：通常在 `Inc` 或 `Core` 分组

**识别第三方库：**
- FreeRTOS：`tasks.c`、`queue.c`、`portable/`
- CMSIS：`core_cm*.h`、`system_*.c`
- HAL 库：`stm32*xx_hal_*.c`
- 中间件：`Middlewares/` 或 `Third_Party/` 目录

**检查编译配置：**
- 多目标配置：一个工程可能包含多个 `<Target>`（如 Debug/Release）
- 条件编译：检查 `<Define>` 中的宏定义
- 优化等级：影响代码大小和执行速度

#### 7. 注意事项

- 工程文件是 XML 格式，可能很大（几千行），优先使用搜索而非全文读取
- 路径分隔符可能是 `\` 或 `/`，需要统一处理
- 同一工程可能有多个编译目标（Target），注意区分
- 头文件路径是相对路径，需要结合工程文件位置计算绝对路径

---

### 工程编译流程

当用户要求编译 MDK 工程时，按以下流程执行：

#### 1. 定位工程文件和 Keil 路径

- 查找 `*.uvprojx` 工程文件
- 确认 Keil 安装路径（通常为 `C:\Keil_v5\UV4\UV4.exe` 或 `D:\DevelopTool\Keil\UV4\UV4.exe`）

#### 2. 执行编译命令

**增量编译（Build）：**
```bash
& "Keil路径\UV4.exe" -b "工程文件路径.uvprojx" -j0
```

**全量编译（Rebuild）：**
```bash
& "Keil路径\UV4.exe" -r "工程文件路径.uvprojx" -j0
```

**参数说明：**
- `-b`：Build（增量编译，只编译修改过的文件）
- `-r`：Rebuild（全量编译，重新编译所有文件）
- `-j0`：使用所有 CPU 核心并行编译
- `-o`：输出日志到文件（可选）
- `timeout: 120000`：设置 2 分钟超时

#### 3. 读取编译日志

**Keil 自动生成的日志文件：**
- 路径：`工程目录/Objects/工程名.build_log.htm`
- 格式：HTML 格式，包含完整编译输出

**读取方式：**
```powershell
# 等待文件写入完成后读取尾部
Start-Sleep -Seconds 1; Get-Content "path/to/project.build_log.htm" -Tail 50
```

或直接读取完整文件：
```bash
readFile(path="path/to/project.build_log.htm")
```

#### 4. 分析并输出结果

从日志中提取并展示：

**编译状态：**
- 错误数（Error(s)）
- 警告数（Warning(s)）
- 编译时间（Build Time Elapsed）

**程序大小（仅 Rebuild 后可用）：**
- `Code`：代码段大小（字节）
- `RO-data`：只读数据大小（常量、字符串）
- `RW-data`：已初始化可读写数据大小
- `ZI-data`：未初始化数据大小（BSS段、堆栈）

**资源占用计算：**
- **Flash 占用** = Code + RO-data + RW-data
- **RAM 占用** = RW-data + ZI-data
- **占用百分比**：根据目标芯片规格计算

#### 5. 用户触发关键词

当用户说以下任一关键词时，自动执行编译流程：
- "编译"
- "重新编译"
- "rebuild"
- "build"

**默认行为：**
- 首次编译或需要完整信息时使用 Rebuild
- 快速验证时使用 Build

#### 6. 编译日志分析示例

**成功示例：**
```
".\Objects\Template.axf" - 0 Error(s), 0 Warning(s).
Program Size: Code=30564 RO-data=940 RW-data=1040 ZI-data=72512
Build Time Elapsed:  00:00:03
```

**输出格式：**
- ✅ 编译成功：0 Error(s), 0 Warning(s)
- ⏱️ 编译时间：3 秒
- 💾 Flash 占用：32,544 字节 (≈31.8 KB)
- 💾 RAM 占用：73,552 字节 (≈71.8 KB)

---

## 文件创建规则

**禁止随意创建 Markdown 文档文件。**

- ❌ 不要为了解释概念而创建 MD 文件。
- ❌ 不要为了写文档而创建 MD 文件。
- ❌ 不要为了总结而创建 MD 文件。

**只在以下情况创建文件：**

- ✅ 用户明确要求创建文件。
- ✅ 项目必需的配置文件（如 `CMakeLists.txt`）。
- ✅ 项目必需的代码文件（`.c`、`.h`）。

**正确做法：**

- 直接在对话中回答问题。
- 用代码注释解释概念。
- 在现有文件中添加注释。

---

## C 代码风格规范（基于 BARR-C）

### 关键字与括号格式

- `if/else/while/for` 关键字与左括号之间不能有空格。

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

### 大括号格式

- `if/else/while/for` 语句的左大括号必须换行。
- 函数定义的左大括号必须换行。

正确示例：

```c
if(condition)
{
  // code
}

void MyFunction(void)
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

### 命名与格式

- 使用 2 空格缩进。
- 一行只做一件事，避免复合语句。
- 函数命名按模块职责使用统一前缀（如 `Driver_`、`Device_`、`App_`），不强制 `BSP_`/`MX_`。
- 全局变量前缀：`g_`。
- 文件内静态变量前缀：`s_`（或保持模块既有习惯）。
- 宏定义全大写，用下划线分隔：`#define MAX_BUFFER_SIZE 256`。
- 类型定义后缀 `_t`：`typedef uint8_t byte_t;`。
- 描述符类型与实例沿用 `_t` 后缀和 `xxx_desc_t` 风格。
- 相关代码段（如寄存器映射、状态表、配置表）内，行尾注释 `//` 默认按列对齐。

### 安全编码

- 使用 `snprintf` 代替 `sprintf`（防止缓冲区溢出）。
- 指针初始化为 `NULL`。
- 局部变量在声明时初始化。
- 硬件寄存器和中断共享变量必须使用 `volatile`。
- 所有可能失败的函数必须检查返回值，并提供明确失败路径（必要时进入统一错误处理）。
- 失败路径默认覆盖：返回值、超时、空指针、边界值。

### 头文件规范

- 必须有 include guard。

```c
#ifndef MODULE_NAME_H
#define MODULE_NAME_H
// ...
#endif /* MODULE_NAME_H */
```

- 包含顺序：系统头文件 → HAL 库 → 项目头文件。

### CMakeLists.txt 注释规范

- 文件路径注释必须与 `#` 号对齐，使用空格填充。
- 注释内容简洁明了，说明文件用途。

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

---

## 注释规范（基于 BARR-C 与 Doxygen）

### 基本原则

- 注释使用完整句子，语法正确，标点恰当。
- 注释应解释"为什么"而非"怎么做"，假设读者了解 C 语言。
- 注释数量和长度应与代码复杂度成正比。
- 避免冗余注释（如 `numero <<= 2; // 左移2位`）。
- 生成新代码时，为关键逻辑和非显而易见步骤添加简短中文注释。
- 不为简单赋值或自解释语句添加无意义注释。
- 生成代码时，禁止删除用户已注释掉的代码内容；除非用户明确要求清理。

### 注释格式

- 单行注释使用 `//`，多行注释使用 `/* ... */`。
- Doxygen 文档注释使用 `/** ... */` 格式。
- Doxygen 命令使用 `@` 前缀（如 `@brief`），而非 `\`。

### 文件头注释（必须）

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

### 函数注释（必须）

所有函数必须有 Doxygen 格式注释：

```c
/**
 * @brief   函数功能简要说明
 *
 * @details 详细说明（可选）
 *
 * @param[in]     param1   输入参数说明
 * @param[out]    param2   输出参数说明
 * @param[in,out] param3   输入输出参数说明
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

| 标记         | 用途               | 示例                                         |
| ------------ | ------------------ | -------------------------------------------- |
| `TODO:`    | 待完成的功能       | `// TODO: 实现超时处理`                    |
| `FIXME:`   | 需要修复的问题     | `// FIXME: 边界条件未处理`                 |
| `WARNING:` | 修改代码的风险提示 | `// WARNING: 延时值经验确定，移植时需调整` |
| `NOTE:`    | 重要说明或设计原因 | `// NOTE: 芯片勘误表要求此处延时`          |
| `HACK:`    | 临时解决方案       | `// HACK: 绕过硬件 bug`                    |

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

引用外部文档时必须注明来源：

```c
// 参考: STM32H750参考手册 RM0433 Rev7, 第25.4.3节
// 参考: 项目设计文档 doc/uart_protocol.md
```

### 禁止事项

- 禁止注释掉大段代码提交，应使用版本控制。
- 禁止使用无意义注释（如 `i++; // i加1`）。
- 禁止注释与代码不一致（代码修改后必须同步更新注释）。
- 禁止在注释中包含敏感信息（密码、密钥等）。

---

## 项目架构与依赖约束

- 默认采用四层架构：`HAL_Layer → Driver_Layer → Device_Layer → App_Layer`。
- `App_Layer` 允许调用：`Device_Layer`、Common Modules、CMSIS-RTOS2。
- `Device_Layer` 仅通过 `Driver_Layer` 抽象接口访问硬件。
- 禁止 `App_Layer` 与 `Device_Layer` 直接包含或调用 HAL 头文件/接口（如 `stm32*.h`、`HAL_*`）。

### C 工程组织偏好

- `main.c` 保持"按层分组 include + 中文分组注释"的结构顺序。
- RTOS 任务创建采用：静态任务函数声明 → `osThreadAttr_t` 局部常量 → `osThreadNew()`。
- 外设初始化优先顺序：系统初始化 → 设备初始化 → 驱动启动（如 DMA）→ RTOS 启动。

---

## 任务执行与验证流程

- 默认采用最小闭环：研究 → 计划 → 实施 → 验证。
- 编码前至少完成 3 个相似实现阅读，并记录可复用点与约束。
- 改动前先定位影响范围：调用点、头文件依赖、输入输出、配置来源、调用链路、任务/中断上下文。
- 每次只做一个逻辑闭环改动，避免大范围混改。
- 先给出 1~2 句执行计划，再开始改代码。
- 改完必须做最小验证：编译检查 + 关键路径自测。
- 验证说明必须可复现：写清命令、预期结果、实际结果。
- 若验证失败，停止交付并说明原因；连续 3 次验证失败则回到需求与设计复盘。
- 若仓库缺少测试或工具，明确记录缺口与补偿计划。
- 输出结果时必须包含：改了什么、为什么、怎么验证。

### 代码评审清单（嵌入式 C）

- 是否破坏分层：`App/Device` 是否直接触达 HAL。
- 失败路径是否完整：返回值、超时、空指针、边界值。
- 共享资源是否安全：中断/任务并发、`volatile`、临界区。
- 注释是否解释"为什么"，且与代码保持一致。
- 是否符合现有风格：2 空格缩进、`if(condition)` 与大括号换行。

### 复杂任务质量审查（可选）

- 技术维度：代码质量、测试覆盖、规范遵循。
- 战略维度：需求匹配、架构一致、风险评估。
- 综合评分：0~100，并给出"通过/退回/需讨论"。

---

## 风险控制与回退

- 未经用户明确允许，不执行破坏性命令（如 `reset --hard`、批量删除）。
- 涉及脚本批量替换或跨目录修改时，先给变更范围摘要再执行。
- 涉及关键文件（如 `main.c`、启动流程、RTOS 任务创建）时，必须给出影响面说明。
- 对破坏性改动必须提供迁移步骤或回退点。
- 若存在风险或未知项，明确列出假设与回退点。

---

## 提交与偏好沉淀

### 提交流程偏好

- 当用户请求生成提交信息时，默认输出中文 Conventional Commits。
- 提交信息格式：`<type>: <中文描述>`。
- 类型集合：`feat`、`fix`、`docs`、`style`、`refactor`、`test`、`chore`。

### 偏好沉淀工作流

- 每次任务结束时，给出"可沉淀偏好候选"清单（仅列新增或变更项）。
- 当用户回复"沉淀"时，自动将候选项追加写入 `agents.md`。
- 若用户未回复"沉淀"，则不写入文件，只在当次会话临时使用。

---

## 落地原则

- 复用优先于重写；一致性优先于个人偏好。
- 先满足真实需求，避免过度设计；新增抽象前确认复用收益。
- 在保证功能与行为不变前提下，优先简化实现，提升代码可读性与可理解性。
- 所有结论应基于代码或文档证据，不做无依据假设。
- 若规则冲突，以本文件前文更具体约束优先。

---

## 已沉淀偏好（2026-02-28）

- DMA 专用缓冲统一放在 `board.c`/`board.h` 管理，便于集中维护。
- 辅助函数若只服务单点逻辑，优先内联到目标函数，减少不必要跳转。
- 新开对话且缺少上下文时，先分析当前项目代码现状（至少定位相关实现与调用链）再改代码，避免沿用过期上下文导致错误修改。
- 全项目评审时默认先排除 `mcu/` 与 `Middlewares/` 第三方目录，优先聚焦 `project/usr` 自研代码。
