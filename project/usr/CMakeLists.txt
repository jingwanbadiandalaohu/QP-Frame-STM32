# Project/chip/toolchain config
set(__PROJ_NAME__ "Template" CACHE STRING "Project name")
set(__CHIP_TYPE__ "STM32H750vbt6" CACHE STRING "Chip type")
set(__TOOLCHAIN__ "ARMGCC" CACHE STRING "Toolchain type")

set(PLATFORM "stm32h750vbt6" CACHE STRING "Target platform")
set_property(CACHE PLATFORM PROPERTY STRINGS "stm32h750vbt6")

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/output)
set(HEX_FILE ${EXECUTABLE_OUTPUT_PATH}/${__PROJ_NAME__}.hex)
set(BIN_FILE ${EXECUTABLE_OUTPUT_PATH}/${__PROJ_NAME__}.bin)
set(MAP_FILE ${EXECUTABLE_OUTPUT_PATH}/${__PROJ_NAME__}.map)

cmake_minimum_required(VERSION 3.20)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_LIST_DIR}/../ide/gcc/scripts/cmake/armgcc_cm7.cmake")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(${__PROJ_NAME__} C CXX ASM)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../mcu/stm32h750vbt6 ${CMAKE_CURRENT_BINARY_DIR}/stm32h7xx_library)

set(PRO_SRC
    app/main.c

    core/${PLATFORM}/stm32h7xx_it.c
    core/${PLATFORM}/stm32h7xx_hal_msp.c
    core/${PLATFORM}/stm32h7xx_hal_timebase_tim.c
    core/${PLATFORM}/syscalls.c

    drivers/${PLATFORM}/drv_gpio.c
    drivers/${PLATFORM}/drv_uart.c
    drivers/${PLATFORM}/drv_adc.c
    drivers/${PLATFORM}/drv_system.c
    drivers/${PLATFORM}/board.c

    common/filter/filter.c
)

set(MIDDLEWARE_SRC
    ../Middlewares/Third_Party/Printf/printf.c

    ../Middlewares/Third_Party/FreeRTOS/tasks.c
    ../Middlewares/Third_Party/FreeRTOS/queue.c
    ../Middlewares/Third_Party/FreeRTOS/list.c
    ../Middlewares/Third_Party/FreeRTOS/timers.c
    ../Middlewares/Third_Party/FreeRTOS/event_groups.c
    ../Middlewares/Third_Party/FreeRTOS/stream_buffer.c
    ../Middlewares/Third_Party/FreeRTOS/portable/GCC/ARM_CM7/r0p1/port.c
    ../Middlewares/Third_Party/FreeRTOS/portable/MemMang/heap_4.c
    ../Middlewares/Third_Party/CMSIS-FreeRTOS/CMSIS/RTOS2/FreeRTOS/Source/cmsis_os2.c
)

add_executable(${__PROJ_NAME__})
target_sources(${__PROJ_NAME__} PRIVATE ${PRO_SRC} ${MIDDLEWARE_SRC})

target_include_directories(${__PROJ_NAME__} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/drivers
    ${CMAKE_CURRENT_LIST_DIR}/drivers/${PLATFORM}
    ${CMAKE_CURRENT_LIST_DIR}/device
    ${CMAKE_CURRENT_LIST_DIR}/common/filter
    ${CMAKE_CURRENT_LIST_DIR}/app
    ${CMAKE_CURRENT_LIST_DIR}/core
    ${CMAKE_CURRENT_LIST_DIR}/core/${PLATFORM}

    ${CMAKE_CURRENT_LIST_DIR}/../Middlewares/Third_Party/Printf

    ${CMAKE_CURRENT_LIST_DIR}/inc/${PLATFORM}
    ${CMAKE_CURRENT_LIST_DIR}/../Middlewares/Third_Party/FreeRTOS/include
    ${CMAKE_CURRENT_LIST_DIR}/../Middlewares/Third_Party/FreeRTOS/portable/GCC/ARM_CM7/r0p1
    ${CMAKE_CURRENT_LIST_DIR}/../Middlewares/Third_Party/CMSIS-FreeRTOS/CMSIS/RTOS2/FreeRTOS/Include
    ${CMAKE_CURRENT_LIST_DIR}/../Middlewares/Third_Party/CMSIS_5/CMSIS/RTOS2/Include
)

target_link_libraries(${__PROJ_NAME__} PRIVATE "stm32h7xx_library")

target_link_options(${__PROJ_NAME__} PRIVATE
    -Wl,-Map=${MAP_FILE}
)

set_target_properties(
    ${__PROJ_NAME__} PROPERTIES
    OUTPUT_NAME "${__PROJ_NAME__}.elf"
)

add_custom_command(TARGET ${__PROJ_NAME__} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${__PROJ_NAME__}> ${HEX_FILE}
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${__PROJ_NAME__}> ${BIN_FILE}
    COMMENT "Convert ELF to HEX and BIN files"
)
