# -----------------------------------------------------------------------------
# 设置交叉编译工具链路径
#
if(NOT DEFINED ARMGCC_ROOT_DIR)
    set(ARMGCC_ROOT_DIR "")
    if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
        if(DEFINED ENV{QTOOLS})
            set(ARMGCC_ROOT_DIR "$ENV{QTOOLS}/gnu_arm-none-eabi")
            string(REPLACE "\\" "/" ARMGCC_ROOT_DIR "${ARMGCC_ROOT_DIR}")
        endif()
    endif()
endif()

if(NOT ARMGCC_ROOT_DIR STREQUAL "")
    set(ARMGCC_BIN_DIR "${ARMGCC_ROOT_DIR}/bin")
else()
    set(ARMGCC_BIN_DIR "")
endif()


# -----------------------------------------------------------------------------
# 使用静态库进行编译器测试 (跳过可执行文件的动态检查) 否则如果目标平台是嵌入式设备可能会过不了编译器测试
#
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)


# -----------------------------------------------------------------------------
# 设置交叉编译工具
#
if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
    set(EXEC_EXT ".exe" )
else()
    set(EXEC_EXT "" )
endif()

if(NOT ARMGCC_BIN_DIR STREQUAL "")
    set(CMAKE_C_COMPILER   "${ARMGCC_BIN_DIR}/arm-none-eabi-gcc${EXEC_EXT}")
    set(CMAKE_CXX_COMPILER "${ARMGCC_BIN_DIR}/arm-none-eabi-gcc${EXEC_EXT}")
    set(CMAKE_ASM_COMPILER "${ARMGCC_BIN_DIR}/arm-none-eabi-gcc${EXEC_EXT}")
    set(OBJCOPY            "${ARMGCC_BIN_DIR}/arm-none-eabi-objcopy${EXEC_EXT}")
    set(SIZE               "${ARMGCC_BIN_DIR}/arm-none-eabi-size${EXEC_EXT}")
else()
    set(CMAKE_C_COMPILER   "arm-none-eabi-gcc${EXEC_EXT}")
    set(CMAKE_CXX_COMPILER "arm-none-eabi-gcc${EXEC_EXT}")
    set(CMAKE_ASM_COMPILER "arm-none-eabi-gcc${EXEC_EXT}")
    set(OBJCOPY            "arm-none-eabi-objcopy${EXEC_EXT}")
    set(SIZE               "arm-none-eabi-size${EXEC_EXT}")
endif()


# -----------------------------------------------------------------------------
# 设置交叉编译工具链通用选项
#
set(COMMON_FLAGS "-mthumb -Wall -Wextra -Werror -fno-common -ffunction-sections -fdata-sections -fmessage-length=0")

set(CMAKE_C_FLAGS          "${COMMON_FLAGS} -std=gnu99")
set(CMAKE_CXX_FLAGS        "${COMMON_FLAGS} -std=gnu++17")
set(CMAKE_ASM_FLAGS        "${COMMON_FLAGS} -x assembler-with-cpp")
set(CMAKE_EXE_LINKER_FLAGS "-mthumb -static -Wl,--gc-sections,--cref -Wl,--print-memory-usage --specs=nano.specs --specs=nosys.specs" CACHE INTERNAL "Linker options")


# -----------------------------------------------------------------------------
# 设置 Debug 构建模式选项
#
set(CMAKE_C_FLAGS_DEBUG          "-Og -g3" CACHE INTERNAL "C Compiler options for debug build type")
set(CMAKE_CXX_FLAGS_DEBUG        "-Og -g3" CACHE INTERNAL "C++ Compiler options for debug build type")
set(CMAKE_ASM_FLAGS_DEBUG        "-g3"     CACHE INTERNAL "ASM Compiler options for debug build type")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "-g3"     CACHE INTERNAL "Linker options for debug build type")


# -----------------------------------------------------------------------------
# 设置 Release 构建模式选项
#
set(CMAKE_C_FLAGS_RELEASE          "-Os -g0" CACHE INTERNAL "C Compiler options for release build type")
set(CMAKE_CXX_FLAGS_RELEASE        "-Os -g0" CACHE INTERNAL "C++ Compiler options for release build type")
set(CMAKE_ASM_FLAGS_RELEASE        ""        CACHE INTERNAL "ASM Compiler options for release build type")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-g0"     CACHE INTERNAL "Linker options for release build type")


# -----------------------------------------------------------------------------
# 设置 program/include/package/library 的查找路径和查找模式
#
if(NOT ARMGCC_ROOT_DIR STREQUAL "")
    set(CMAKE_FIND_ROOT_PATH "${ARMGCC_ROOT_DIR}")
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
endif()
