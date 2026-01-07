set(LIB_NAME "stm32h7xx_library")

set(STM32_HAL_SRC
    STM32H7xx_HAL_Driver/Src/stm32h7xx_hal.c                   
    STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_cortex.c            
    STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_dma.c              
    STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_dma_ex.c         
    STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_gpio.c       
    STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_tim.c
    STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_tim_ex.c
    STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_uart.c           
    STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_uart_ex.c        
    STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_rcc.c        
    STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_rcc_ex.c          
    STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_pwr.c          
    STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_pwr_ex.c          
    STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_flash.c            
    STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_flash_ex.c         
    STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_exti.c
)

set(STM_SYS_CORE_SRC
    CMSIS/Device/ST/STM32H7xx/Source/Templates/system_stm32h7xx.c
    CMSIS/Device/ST/STM32H7xx/Source/Templates/gcc/startup_stm32h750xx.s
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
    ${CMAKE_CURRENT_LIST_DIR}/STM32H7xx_HAL_Driver/Inc
    ${CMAKE_CURRENT_LIST_DIR}/CMSIS/Device/ST/STM32H7xx/Include
    ${CMAKE_CURRENT_LIST_DIR}/CMSIS/Include
)

target_compile_definitions(${LIB_NAME} PUBLIC
    USE_HAL_DRIVER
    STM32H750xx
)

target_link_options(${LIB_NAME} PUBLIC
    -T "${CMAKE_CURRENT_LIST_DIR}/CMSIS/Device/ST/STM32H7xx/Source/Templates/gcc/linker/stm32h750xx_flash.ld"
)
