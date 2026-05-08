set(LIB_NAME "stm32f1xx_library")

set(STM32_HAL_SRC
    STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c                   
    STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c            
    STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.c              
    STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c       
    STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim.c
    STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim_ex.c
    STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_uart.c           
    STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c        
    STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c          
    STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pwr.c          
    STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash.c            
    STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash_ex.c         
)

set(STM_SYS_CORE_SRC
    CMSIS/Device/ST/STM32F1xx/Source/Templates/system_stm32f1xx.c
    CMSIS/Device/ST/STM32F1xx/Source/Templates/gcc/startup_stm32f103xe.s
)

add_library(${LIB_NAME} STATIC "")

set_target_properties(${LIB_NAME} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

target_sources(${LIB_NAME} PUBLIC
    ${STM32_HAL_SRC}
    ${STM_SYS_CORE_SRC}
)

target_include_directories(${LIB_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/STM32F1xx_HAL_Driver/Inc
    ${CMAKE_CURRENT_LIST_DIR}/CMSIS/Device/ST/STM32F1xx/Include
    ${CMAKE_CURRENT_LIST_DIR}/CMSIS/Include
    ${CMAKE_CURRENT_LIST_DIR}/../../project/usr/inc/stm32f103rct6
)

target_compile_definitions(${LIB_NAME} PUBLIC
    USE_HAL_DRIVER
    STM32F103xE
)

target_link_options(${LIB_NAME} PUBLIC
    -T "${CMAKE_CURRENT_LIST_DIR}/CMSIS/Device/ST/STM32F1xx/Source/Templates/gcc/linker/STM32F103RC_FLASH.ld"
)
