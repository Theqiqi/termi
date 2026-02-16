cmake_minimum_required(VERSION 3.21)
project(TermiCoreBase)

set(CMAKE_CXX_STANDARD 17)

# 统一设置输出路径 (相对于最顶层目录)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 定义库
add_library(TermiCoreBase STATIC  # 明确指定为静态库
        src/cg_gfx.cpp
        src/ray_console.cpp
)

# 暴露接口
target_include_directories(TermiCoreBase
        PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# 测试程序
add_executable(console tests/main.cpp)
target_link_libraries(console PRIVATE TermiCoreBase)