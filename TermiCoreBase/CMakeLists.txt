cmake_minimum_required(VERSION 3.21)
project(TermiCoreBase)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# -------------------------
# 1. 定义库（内部 C++ 实现）
# -------------------------
# 现在是 header-only 库
add_library(TermiCoreBase INTERFACE)

# 暴露 include 目录给外部
target_include_directories(TermiCoreBase
        PUBLIC INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include
        #PUBLIC include     # 对外暴露的 C API
        #PRIVATE src        # 内部实现
)

# -------------------------
# 2. 定义测试程序
# -------------------------
add_executable(console
        tests/main.cpp
)

target_link_libraries(console PRIVATE TermiCoreBase)