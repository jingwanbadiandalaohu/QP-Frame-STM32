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

# -g 参数：控制调试信息的密度
# g 代表 Debug Information。它不影响代码运行速度，只影响生成的 .elf 或 .exe 文件里包含多少“调试元数据”。
# -g0：完全不生成调试信息。发布正式产品时使用，防止代码被反编译，且减小文件体积。
# -g1：只生成最基本的调试信息（能看回溯栈）。
# -g2：标准等级。
# -g3：最高等级。不仅包含变量信息，还包含宏定义等额外信息。你在 Debug 配置里看到它，说明它是为了让你在调试时能看清每一行代码和宏。

# -O 参数：控制优化等级
# O 代表 Optimization。它直接影响代码的机器码执行逻辑。
# -Og：专为调试设计的优化。它会尝试进行不干扰调试的性能优化。它保证了你在单步执行时，代码运行的顺序和你在编辑器里看到的一模一样。
# -Os：尺寸优化（Optimize for Size）。这是嵌入式开发最常用的。它会尽可能减小生成的代码体积，以适应内存较小的芯片（如 Flash 空间不足时）。
# -O0：完全不优化。
# -O2 / -O3：追求极限速度优化。

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
