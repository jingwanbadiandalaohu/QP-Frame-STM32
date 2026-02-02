# ============================================================================
# 项目配置
# ============================================================================
# 项目名称
set(__PROJ_NAME__ "Template" CACHE STRING "Project name")
# 芯片型号
set(__CHIP_TYPE__ "STM32H750vbt6" CACHE STRING "Chip type")
# 工具链类型
set(__TOOLCHAIN__ "ARMGCC" CACHE STRING "Toolchain type")

# 目标平台
set(PLATFORM "stm32h750vbt6" CACHE STRING "Target platform")
set_property(CACHE PLATFORM PROPERTY STRINGS "stm32h750vbt6")

# 输出路径配置
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/output)
set(HEX_FILE ${EXECUTABLE_OUTPUT_PATH}/${__PROJ_NAME__}.hex)
set(BIN_FILE ${EXECUTABLE_OUTPUT_PATH}/${__PROJ_NAME__}.bin)
set(MAP_FILE ${EXECUTABLE_OUTPUT_PATH}/${__PROJ_NAME__}.map)

# ============================================================================
# CMake基础配置
# ============================================================================
cmake_minimum_required(VERSION 3.20)

# 工具链文件路径
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_LIST_DIR}/../ide/gcc/scripts/cmake/armgcc_cm7.cmake")
# 导出编译命令（用于IDE代码补全）
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 定义项目
project(${__PROJ_NAME__} C CXX ASM)

# 构建类型配置（Debug/Release）
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

# ============================================================================
# 添加STM32 HAL库子目录
# ============================================================================
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../mcu/stm32h750vbt6 ${CMAKE_CURRENT_BINARY_DIR}/stm32h7xx_library)

# ============================================================================
# 项目源文件
# ============================================================================
set(PRO_SRC
    app/main.c                                                                      #应用入口

    core/${PLATFORM}/stm32h7xx_it.c                                                 #中断服务函数
    core/${PLATFORM}/stm32h7xx_hal_msp.c                                            #HAL MSP初始化
    core/${PLATFORM}/stm32h7xx_hal_timebase_tim.c                                   #HAL时基定时器
    core/${PLATFORM}/syscalls.c                                                     #系统调用

    drivers/${PLATFORM}/drv_gpio.c                                                  #GPIO驱动
    drivers/${PLATFORM}/drv_uart.c                                                  #UART驱动
    drivers/${PLATFORM}/drv_adc.c                                                   #ADC驱动
    drivers/${PLATFORM}/drv_system.c                                                #系统驱动
    drivers/${PLATFORM}/board.c                                                     #板级资源定义

    device/led.c                                                                    #LED设备
    device/relay.c                                                                  #继电器设备
    device/modbus.c                                                                 #Modbus设备

    common/filter/filter.c                                                          #滤波器
    common/ringbuffer/ringbuffer.c                                                  #环形缓冲区
)

# ============================================================================
# 中间件源文件
# ============================================================================
set(MIDDLEWARE_SRC
    ../Middlewares/Third_Party/FreeRTOS/tasks.c                                     #任务代码
    ../Middlewares/Third_Party/FreeRTOS/queue.c                                     #队列代码
    ../Middlewares/Third_Party/FreeRTOS/list.c                                      #列表代码
    ../Middlewares/Third_Party/FreeRTOS/timers.c                                    #定时器代码
    ../Middlewares/Third_Party/FreeRTOS/event_groups.c                              #事件组代码
    ../Middlewares/Third_Party/FreeRTOS/stream_buffer.c                             #流缓冲区代码
    ../Middlewares/Third_Party/FreeRTOS/portable/GCC/ARM_CM7/r0p1/port.c            #FreeRTOS针对Cortex-M7的移植文件
    ../Middlewares/Third_Party/FreeRTOS/portable/MemMang/heap_4.c                   #FreeRTOS内存管理方案
    ../Middlewares/Third_Party/CMSIS-FreeRTOS/CMSIS/RTOS2/FreeRTOS/Source/cmsis_os2.c #CMSIS-RTOS2接口文件

    ../Middlewares/Third_Party/Printf/printf.c                                      #Printf库
    ../Middlewares/Third_Party/nanoMODBUS/nanomodbus.c                              #nanoMODBUS库
)

# ============================================================================
# 创建可执行文件目标
# ============================================================================
add_executable(${__PROJ_NAME__})
target_sources(${__PROJ_NAME__} PRIVATE ${PRO_SRC} ${MIDDLEWARE_SRC})

# ============================================================================
# 头文件包含路径
# ============================================================================
target_include_directories(${__PROJ_NAME__} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/drivers                                               #驱动层头文件
    ${CMAKE_CURRENT_LIST_DIR}/drivers/${PLATFORM}                                   #平台驱动头文件
    ${CMAKE_CURRENT_LIST_DIR}/device                                                #设备层头文件
    ${CMAKE_CURRENT_LIST_DIR}/common/filter                                         #滤波器头文件
    ${CMAKE_CURRENT_LIST_DIR}/common/ringbuffer                                     #环形缓冲区头文件
    ${CMAKE_CURRENT_LIST_DIR}/app                                                   #应用层头文件
    ${CMAKE_CURRENT_LIST_DIR}/core                                                  #核心层头文件
    ${CMAKE_CURRENT_LIST_DIR}/core/${PLATFORM}                                      #平台核心头文件


    ${CMAKE_CURRENT_LIST_DIR}/inc/${PLATFORM}                                       #平台配置头文件
    ${CMAKE_CURRENT_LIST_DIR}/../Middlewares/Third_Party/FreeRTOS/include          #FreeRTOS头文件
    ${CMAKE_CURRENT_LIST_DIR}/../Middlewares/Third_Party/FreeRTOS/portable/GCC/ARM_CM7/r0p1 #FreeRTOS移植头文件
    ${CMAKE_CURRENT_LIST_DIR}/../Middlewares/Third_Party/CMSIS-FreeRTOS/CMSIS/RTOS2/FreeRTOS/Include #CMSIS-RTOS2头文件
    ${CMAKE_CURRENT_LIST_DIR}/../Middlewares/Third_Party/CMSIS_5/CMSIS/RTOS2/Include #CMSIS RTOS2头文件

    ${CMAKE_CURRENT_LIST_DIR}/../Middlewares/Third_Party/Printf                     #Printf库头文件
    ${CMAKE_CURRENT_LIST_DIR}/../Middlewares/Third_Party/nanoMODBUS                 #nanoMODBUS头文件
)

# ============================================================================
# 链接库
# ============================================================================
target_link_libraries(${__PROJ_NAME__} PRIVATE "stm32h7xx_library")

# ============================================================================
# 链接选项
# ============================================================================
target_link_options(${__PROJ_NAME__} PRIVATE
    -Wl,-Map=${MAP_FILE}                                                            #生成MAP文件
)

# ============================================================================
# 目标属性设置
# ============================================================================
set_target_properties(
    ${__PROJ_NAME__} PROPERTIES
    OUTPUT_NAME "${__PROJ_NAME__}.elf"                                              #输出ELF文件名
)

# ============================================================================
# 构建后命令：生成HEX和BIN文件
# ============================================================================
add_custom_command(TARGET ${__PROJ_NAME__} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${__PROJ_NAME__}> ${HEX_FILE}   #生成HEX文件
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${__PROJ_NAME__}> ${BIN_FILE} #生成BIN文件
    COMMENT "Convert ELF to HEX and BIN files"
)
