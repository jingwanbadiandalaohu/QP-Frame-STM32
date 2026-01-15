# 工程/芯片/工具链基础配置
set(__PROJ_NAME__ "Template" CACHE STRING "Project name")
set(__CHIP_TYPE__ "STM32H750vbt6" CACHE STRING "Chip type")
set(__TOOLCHAIN__ "ARMGCC" CACHE STRING "Toolchain type")

# 平台选择
set(PLATFORM "stm32h7" CACHE STRING "Target platform")
set_property(CACHE PLATFORM PROPERTY STRINGS "stm32h7" "stm32f4" "stm32f1")

# 产物输出路径与文件名
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/output)
set(HEX_FILE ${EXECUTABLE_OUTPUT_PATH}/${__PROJ_NAME__}.hex)
set(BIN_FILE ${EXECUTABLE_OUTPUT_PATH}/${__PROJ_NAME__}.bin)
set(MAP_FILE ${EXECUTABLE_OUTPUT_PATH}/${__PROJ_NAME__}.map)

# CMake最低版本要求
cmake_minimum_required(VERSION 3.20)

# 交叉编译工具链配置
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_LIST_DIR}/../ide/gcc/scripts/cmake/armgcc_cm7.cmake")

# 生成compile_commands.json供IDE使用
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 定义工程与支持的语言
project(${__PROJ_NAME__} C CXX ASM)

# 构建类型默认使用 Debug，并限制可选项
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()


# -----------------------------------------------------------------------------

# 引入芯片库并指定构建目录
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../mcu/stm32h750vbt6 ${CMAKE_CURRENT_BINARY_DIR}/stm32h7xx_library)

# 工程源码列表
set(PRO_SRC
    # 应用层
    app/main.c

    # 核心启动文件
    core/stm32h7xx_it.c
    core/stm32h7xx_hal_msp.c
    core/stm32h7xx_hal_timebase_tim.c
    core/syscalls.c

    # BSP层
    bsp/bsp.c
    bsp/bsp_adc.c
    bsp/bsp_uart.c
    bsp/bsp_gpio.c

    # 平台无关模块
    common/filter/filter.c
)

# 根据平台选择添加平台实现文件
if(PLATFORM STREQUAL "stm32h7")
    list(APPEND PRO_SRC
        drivers/stm32h7/drv_gpio_impl.c
        drivers/stm32h7/drv_uart_impl.c
        drivers/stm32h7/drv_adc_impl.c
        drivers/stm32h7/drv_system_impl.c
    )
endif()

# 中间件源文件
set(MIDDLEWARE_SRC
    # Printf
    ../Middlewares/Third_Party/printf/printf.c
    
    # FreeRTOS
    ../Middlewares/Third_Party/FreeRTOS/tasks.c                                         #任务代码
    ../Middlewares/Third_Party/FreeRTOS/queue.c                                         #队列代码
    ../Middlewares/Third_Party/FreeRTOS/list.c                                          #列表代码
    ../Middlewares/Third_Party/FreeRTOS/timers.c                                        #定时器代码
    ../Middlewares/Third_Party/FreeRTOS/event_groups.c                                  #事件组代码
    ../Middlewares/Third_Party/FreeRTOS/stream_buffer.c                                 #流缓冲区代码
    ../Middlewares/Third_Party/FreeRTOS/portable/GCC/ARM_CM7/r0p1/port.c                #FreeRTOS针对Cortex-M7的移植文件
    ../Middlewares/Third_Party/FreeRTOS/portable/MemMang/heap_4.c                       #FreeRTOS内存管理方案
    ../Middlewares/Third_Party/CMSIS-FreeRTOS/CMSIS/RTOS2/FreeRTOS/Source/cmsis_os2.c   #CMSIS-RTOS2接口文件
)

# 定义可执行目标并添加源码
add_executable(${__PROJ_NAME__})
target_sources(${__PROJ_NAME__} PRIVATE ${PRO_SRC} ${MIDDLEWARE_SRC})

# 头文件搜索路径
target_include_directories(${__PROJ_NAME__} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/drivers                # 驱动层接口
    ${CMAKE_CURRENT_LIST_DIR}/drivers/${PLATFORM}    # 平台实现
    ${CMAKE_CURRENT_LIST_DIR}/common/filter          # 平台无关模块
    ${CMAKE_CURRENT_LIST_DIR}/app                    # 应用层
    ${CMAKE_CURRENT_LIST_DIR}/core                   # 核心文件
    ${CMAKE_CURRENT_LIST_DIR}/bsp                    # BSP层
    
    # Printf Includes
    ${CMAKE_CURRENT_LIST_DIR}/../Middlewares/Third_Party/Printf

    # FreeRTOS Includes
    ${CMAKE_CURRENT_LIST_DIR}/inc
    ${CMAKE_CURRENT_LIST_DIR}/../Middlewares/Third_Party/FreeRTOS/include
    ${CMAKE_CURRENT_LIST_DIR}/../Middlewares/Third_Party/FreeRTOS/portable/GCC/ARM_CM7/r0p1
    ${CMAKE_CURRENT_LIST_DIR}/../Middlewares/Third_Party/CMSIS-FreeRTOS/CMSIS/RTOS2/FreeRTOS/Include
    ${CMAKE_CURRENT_LIST_DIR}/../Middlewares/Third_Party/CMSIS_5/CMSIS/RTOS2/Include
)


# 链接芯片库目标
target_link_libraries(${__PROJ_NAME__} PRIVATE "stm32h7xx_library")

# 生成map文件
target_link_options(${__PROJ_NAME__} PRIVATE
    -Wl,-Map=${MAP_FILE}
)

# 生成elf文件
set_target_properties(
    ${__PROJ_NAME__} PROPERTIES
    OUTPUT_NAME "${__PROJ_NAME__}.elf"
)

# elf转换为hex和bin文件
add_custom_command(TARGET ${__PROJ_NAME__} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${__PROJ_NAME__}> ${HEX_FILE}
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${__PROJ_NAME__}> ${BIN_FILE}
    COMMENT "Convert ELF to HEX and BIN files"
)
