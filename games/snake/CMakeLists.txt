cmake_minimum_required(VERSION 3.10)
project(SnakeGameProject)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
# 使用 CURRENT 关键字，防止 bin 目录跑到项目根目录去
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# 1. 搜集逻辑层的源文件（必须包含 .cpp！）
# 假设你的文件都在 src 目录下
set(LIB_SOURCES
        src/SnakeGame.cpp
        src/ParticleSystem.hpp
        src/GameUI.cpp
        src/SnakeAI.cpp
        src/SnakeAI.h
)

# 2. 创建逻辑静态库
add_library(SnakeLogic STATIC ${LIB_SOURCES})
target_link_libraries(SnakeLogic PUBLIC TermiCoreBase)
# 3. 告诉库它的头文件在哪，这样链接它的目标都能自动找到头文件
target_include_directories(SnakeLogic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_definitions(SnakeLogic PUBLIC GAME_DEBUG)
# 4. 游戏主程序
# 包含 main.cpp 和负责渲染的 GameUI.cpp
add_executable(snake src/main.cpp)
target_link_libraries(snake PRIVATE SnakeLogic)


# --- 自动化测试部分 ---
enable_testing()

# 5. 创建测试可执行文件
# 它不需要 GameUI.cpp，因为它只测逻辑
add_executable(logic_tests tests/test_main.cpp)
target_link_libraries(logic_tests PRIVATE SnakeLogic)

# 6. 将测试添加到 CTest
add_test(NAME SnakeLogicTest COMMAND logic_tests)

#在终端进入 build 目录，输入
#ctest --output-on-failure
