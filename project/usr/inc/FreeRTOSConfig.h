/*
 * FreeRTOS Kernel V10.3.1
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html
 *----------------------------------------------------------*/

/* Ensure stdint is only used by the compiler, and not the assembler. */
#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__)
#include <stdint.h>
extern uint32_t SystemCoreClock;
#endif

#define configUSE_PREEMPTION 1                      // 使用抢占式调度
#define configUSE_TIME_SLICING 1                    // 使用时间片轮转调度(同优先级任务启用时间片轮转)
#define configUSE_IDLE_HOOK 0                       // 空闲任务钩子
#define configUSE_TICK_HOOK 0                       // 滴答定时器钩子
#define configCPU_CLOCK_HZ (SystemCoreClock)        // 系统时钟频率
#define configTICK_RATE_HZ ((TickType_t)1000)       // 滴答定时器频率
#define configMAX_PRIORITIES (56)                   // 最大优先级数
#define configMINIMAL_STACK_SIZE ((uint16_t)256)    // 最小堆栈大小
#define configTOTAL_HEAP_SIZE ((size_t)(64 * 1024)) // 总堆栈大小
#define configMAX_TASK_NAME_LEN (16)                // 任务名称长度
#define configUSE_TRACE_FACILITY 1                  // 使用跟踪功能
#define configUSE_16_BIT_TICKS 0                    // 使用16位滴答定时器
#define configIDLE_SHOULD_YIELD 1                   // 空闲任务应该让出CPU时间
#define configUSE_MUTEXES 1                         // 使用互斥量
#define configQUEUE_REGISTRY_SIZE 8                 // 队列注册大小
#define configCHECK_FOR_STACK_OVERFLOW 0            // 检查堆栈溢出
#define configUSE_RECURSIVE_MUTEXES 1               // 使用递归互斥量
#define configUSE_MALLOC_FAILED_HOOK 0              // 使用内存分配失败钩子
#define configUSE_APPLICATION_TASK_TAG 0            // 使用应用程序任务标签
#define configUSE_COUNTING_SEMAPHORES 1             // 使用计数信号量
#define configGENERATE_RUN_TIME_STATS 0             // 生成运行时统计信息
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0   // 使用端口优化任务选择

/* Software timer definitions. */
#define configUSE_TIMERS 1
#define configTIMER_TASK_PRIORITY (2)
#define configTIMER_QUEUE_LENGTH 10
#define configTIMER_TASK_STACK_DEPTH (256)

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */
#define INCLUDE_vTaskPrioritySet 1
#define INCLUDE_uxTaskPriorityGet 1
#define INCLUDE_vTaskDelete 1
#define INCLUDE_vTaskCleanUpResources 1
#define INCLUDE_vTaskSuspend 1
#define INCLUDE_vTaskDelayUntil 1
#define INCLUDE_vTaskDelay 1
#define INCLUDE_xTaskGetSchedulerState 1
#define INCLUDE_xTaskAbortDelay 1
#define INCLUDE_xTaskGetHandle 1
#define INCLUDE_xEventGroupSetBitFromISR 1
#define INCLUDE_xTimerPendFunctionCall 1
#define INCLUDE_xQueueGetMutexHolder 1
#define INCLUDE_xSemaphoreGetMutexHolder 1
#define INCLUDE_pcTaskGetTaskName 1
#define INCLUDE_uxTaskGetStackHighWaterMark 1
#define INCLUDE_xTaskGetCurrentTaskHandle 1
#define INCLUDE_eTaskGetState 1

/*
 * The CMSIS-RTOS V2 defines 56 priorities (0-55).
 * FreeRTOS priorities are 0-(configMAX_PRIORITIES-1).
 * So configMAX_PRIORITIES should be 56.
 */

/* Cortex-M specific definitions. */
#ifdef __NVIC_PRIO_BITS
/* __BVIC_PRIO_BITS will be specified when CMSIS is being used. */
#define configPRIO_BITS __NVIC_PRIO_BITS
#else
#define configPRIO_BITS 4 /* 15 priority levels */
#endif

/* The lowest interrupt priority that can be used in a call to a "set priority"
function. */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY 15

/* The highest interrupt priority that can be used by any interrupt service
routine that makes calls to interrupt safe FreeRTOS API functions.  DO NOT CALL
INTERRUPT SAFE FREERTOS API FUNCTIONS FROM ANY INTERRUPT THAT HAS A HIGHER
PRIORITY THAN THIS! (higher priorities are lower numeric values. */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 5

/* Interrupt priorities used by the kernel port layer itself.  These are generic
to all Cortex-M ports, and do not rely on any particular library functions. */
#define configKERNEL_INTERRUPT_PRIORITY                                        \
  (configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY                                   \
  (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))

/* Definitions that map the FreeRTOS port interrupt handlers to their CMSIS
standard names. */
#define vPortSVCHandler SVC_Handler
#define xPortPendSVHandler PendSV_Handler

/* IMPORTANT: SysTick_Handler is NOT mapped here because we might want to attach
 * it in stm32h7xx_it.c to CALL xPortSysTickHandler(), or map it here if we
 * remove it from stm32h7xx_it.c. For now, I will NOT map it, but I will assume
 * we need to call xPortSysTickHandler from existing SysTick_Handler, OR we
 * comment out SysTick_Handler in stm32h7xx_it.c and map it here. The standard
 * way is to map it here: #define xPortSysTickHandler SysTick_Handler But
 * checked stm32h7xx_it.c content first.
 */
/* #define xPortSysTickHandler SysTick_Handler */

#endif /* FREERTOS_CONFIG_H */
