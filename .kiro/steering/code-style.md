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
- 函数前缀：BSP_（板级支持）、MX_（外设初始化）
- 全局变量前缀：g_
- 静态变量前缀：s_ 或无前缀
- 宏定义全大写，用下划线分隔：`#define MAX_BUFFER_SIZE 256`
- 类型定义后缀 `_t`：`typedef uint8_t byte_t;`

## 安全编码
- 使用 `snprintf` 代替 `sprintf` (防止缓冲区溢出)
<!-- - 避免魔数，用 `const` 或 `#define` 定义常量 -->
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
../Middlewares/Third_Party/FreeRTOS/list.c                                          #列表代码
../Middlewares/Third_Party/FreeRTOS/timers.c                                        #定时器代码
../Middlewares/Third_Party/FreeRTOS/event_groups.c                                  #事件组代码
../Middlewares/Third_Party/FreeRTOS/stream_buffer.c                                 #流缓冲区代码
../Middlewares/Third_Party/FreeRTOS/portable/GCC/ARM_CM7/r0p1/port.c                #FreeRTOS针对Cortex-M7的移植文件
../Middlewares/Third_Party/FreeRTOS/portable/MemMang/heap_4.c                       #FreeRTOS内存管理方案
../Middlewares/Third_Party/CMSIS-FreeRTOS/CMSIS/RTOS2/FreeRTOS/Source/cmsis_os2.c   #CMSIS-RTOS2接口文件
```

错误示例：
```cmake
../Middlewares/Third_Party/FreeRTOS/tasks.c         #任务代码
../Middlewares/Third_Party/FreeRTOS/queue.c       #队列代码
../Middlewares/Third_Party/FreeRTOS/list.c        #列表代码
```
