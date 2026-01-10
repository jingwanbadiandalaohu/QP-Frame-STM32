# -----------------------------------------------------------------------------
# 设置交叉编译工具链路径
# 如果外部未显式指定 ARMGCC_ROOT_DIR，则在 Windows 下优先尝试使用环境变量 QTOOLS
# 找到 GNU Arm Embedded 工具链目录，并统一为 CMake 需要的正斜杠路径格式
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
# 使用静态库进行编译器探测
# 说明：跳过对可执行文件的动态检查，否则目标平台为嵌入式设备时可能无法通过探测
#
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)


# -----------------------------------------------------------------------------
# 设置交叉编译工具
# 根据主机系统决定可执行文件后缀，并拼接到 arm-none-eabi-* 工具名上
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
# 统一开启 Thumb、警告、数据/函数段分离等，并分别设置 C/C++/ASM 编译参数和链接参数
#
set(COMMON_FLAGS "-mthumb -Wall -Wextra -fno-common -ffunction-sections -fdata-sections -fmessage-length=0")

set(CMAKE_C_FLAGS          "${COMMON_FLAGS} -std=gnu99")
set(CMAKE_CXX_FLAGS        "${COMMON_FLAGS} -std=gnu++17")
set(CMAKE_ASM_FLAGS        "${COMMON_FLAGS} -x assembler-with-cpp")
set(CMAKE_EXE_LINKER_FLAGS "-mthumb -static -Wl,--gc-sections,--cref -Wl,--print-memory-usage --specs=nano.specs --specs=nosys.specs" CACHE INTERNAL "Linker options")


# -----------------------------------------------------------------------------
# 设置 Debug 构建模式选项
# 以 -Og 为主、保留调试信息，便于单步与变量查看
#
set(CMAKE_C_FLAGS_DEBUG          "-Og -g3" CACHE INTERNAL "C Compiler options for debug build type")
set(CMAKE_CXX_FLAGS_DEBUG        "-Og -g3" CACHE INTERNAL "C++ Compiler options for debug build type")
set(CMAKE_ASM_FLAGS_DEBUG        "-g3"     CACHE INTERNAL "ASM Compiler options for debug build type")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "-g3"     CACHE INTERNAL "Linker options for debug build type")


# -----------------------------------------------------------------------------
# 设置 Release 构建模式选项
# 以 -Os 为主进行尺寸优化，并关闭调试信息输出
#
set(CMAKE_C_FLAGS_RELEASE          "-Os -g0" CACHE INTERNAL "C Compiler options for release build type")
set(CMAKE_CXX_FLAGS_RELEASE        "-Os -g0" CACHE INTERNAL "C++ Compiler options for release build type")
set(CMAKE_ASM_FLAGS_RELEASE        ""        CACHE INTERNAL "ASM Compiler options for release build type")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-g0"     CACHE INTERNAL "Linker options for release build type")


# -----------------------------------------------------------------------------
# 设置 program/include/package/library 的查找路径和查找模式
# 指定根路径为工具链目录，限制 include/package/library 只在根路径内查找
#
if(NOT ARMGCC_ROOT_DIR STREQUAL "")
    set(CMAKE_FIND_ROOT_PATH "${ARMGCC_ROOT_DIR}")
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
endif()
