set(__PROJ_NAME__ "Template" CACHE STRING "Proj name")
set(__CHIP_TYPE__ "STM32H750vbt6" CACHE STRING "Chip type")
set(__TOOLCHAIN__ "ARMGCC" CACHE STRING "Toolchain type")

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/output)
set(HEX_FILE ${EXECUTABLE_OUTPUT_PATH}/${__PROJ_NAME__}.hex)
set(BIN_FILE ${EXECUTABLE_OUTPUT_PATH}/${__PROJ_NAME__}.bin)
set(MAP_FILE ${EXECUTABLE_OUTPUT_PATH}/${__PROJ_NAME__}.map)

cmake_minimum_required(VERSION 3.20)

#设置交叉编译工具配置文件
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_LIST_DIR}/../ide/gcc/scripts/cmake/armgcc_cm7.cmake")


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(${__PROJ_NAME__} C CXX ASM)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()


# -----------------------------------------------------------------------------

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../../mcu/stm32h750xx ${CMAKE_CURRENT_BINARY_DIR}/stm32h7xx_library)

set(PRO_SRC
    main.c
    core/stm32h7xx_it.c
    core/stm32h7xx_hal_msp.c
    core/stm32h7xx_hal_timebase_tim.c
    core/syscalls.c
    bsp/bsp.c
    bsp/adc.c
    bsp/debug_uart.c
    bsp/printf.c
    
    # FreeRTOS
    ../Middlewares/Third_Party/FreeRTOS/tasks.c         #任务代码
    ../Middlewares/Third_Party/FreeRTOS/queue.c         #队列代码
    ../Middlewares/Third_Party/FreeRTOS/list.c          #列表代码
    ../Middlewares/Third_Party/FreeRTOS/timers.c        #定时器代码
    ../Middlewares/Third_Party/FreeRTOS/event_groups.c  #事件组代码
    ../Middlewares/Third_Party/FreeRTOS/stream_buffer.c #流缓冲区代码
    ../Middlewares/Third_Party/FreeRTOS/portable/GCC/ARM_CM7/r0p1/port.c   #FreeRTOS针对Cortex-M7的移植文件
    ../Middlewares/Third_Party/FreeRTOS/portable/MemMang/heap_4.c          #FreeRTOS内存管理方案
    ../Middlewares/Third_Party/CMSIS-FreeRTOS/CMSIS/RTOS2/FreeRTOS/Source/cmsis_os2.c   #CMSIS-RTOS2接口文件
)
add_executable(${__PROJ_NAME__})
target_sources(${__PROJ_NAME__} PRIVATE ${PRO_SRC})

target_include_directories(${__PROJ_NAME__} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/core
    ${CMAKE_CURRENT_LIST_DIR}/bsp
    ${CMAKE_CURRENT_LIST_DIR}/bsp
    
    # FreeRTOS Includes
    ${CMAKE_CURRENT_LIST_DIR}/inc
    ${CMAKE_CURRENT_LIST_DIR}/../Middlewares/Third_Party/FreeRTOS/include
    ${CMAKE_CURRENT_LIST_DIR}/../Middlewares/Third_Party/FreeRTOS/portable/GCC/ARM_CM7/r0p1
    ${CMAKE_CURRENT_LIST_DIR}/../Middlewares/Third_Party/CMSIS-FreeRTOS/CMSIS/RTOS2/FreeRTOS/Include
    ${CMAKE_CURRENT_LIST_DIR}/../Middlewares/Third_Party/CMSIS_5/CMSIS/RTOS2/Include
)


target_link_libraries(${__PROJ_NAME__} PRIVATE "stm32h7xx_library")

#生成map文件
target_link_options(${__PROJ_NAME__} PRIVATE
    -Wl,-Map=${MAP_FILE}
)

#生成elf文件
set_target_properties(
    ${__PROJ_NAME__} PROPERTIES
    OUTPUT_NAME "${__PROJ_NAME__}.elf"
)

#elf转化成hex,bin文件
add_custom_command(TARGET ${__PROJ_NAME__} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${__PROJ_NAME__}> ${HEX_FILE}
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${__PROJ_NAME__}> ${BIN_FILE}
    COMMENT "Convert ELF to HEX and BIN files"
)
