# -----------------------------------------------------------------------------
# 设置交叉编译的目标平台
#
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)


# -----------------------------------------------------------------------------
# 引入 arm-gcc 工具链基础设置
#
include("${CMAKE_CURRENT_LIST_DIR}/internal/armgcc.cmake")


# -----------------------------------------------------------------------------
# 引入 ARM Cortex-M3 架构相关的编译选项
#

set(MCPU_FLAGS "-mcpu=cortex-m3")#cortex-m3架构
set(VFP_FLAGS  "")#m3架构不支持VFP浮点计算单元

set(CMAKE_C_FLAGS   "${MCPU_FLAGS} ${VFP_FLAGS} ${CMAKE_C_FLAGS}"   CACHE INTERNAL "C Compiler options")
set(CMAKE_CXX_FLAGS "${MCPU_FLAGS} ${VFP_FLAGS} ${CMAKE_CXX_FLAGS}" CACHE INTERNAL "C++ Compiler options")
set(CMAKE_ASM_FLAGS "${MCPU_FLAGS} ${VFP_FLAGS} ${CMAKE_ASM_FLAGS}" CACHE INTERNAL "ASM Compiler options")
