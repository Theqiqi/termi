# 使用 CURRENT 关键字，防止 bin 目录跑到项目根目录去
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# 建议把 src 下的所有 cpp 都包含进来，防止以后增加逻辑类时报错
file(GLOB_RECURSE SNAKE_SOURCES "src/*.cpp" "src/*.h")

add_executable(snake ${SNAKE_SOURCES})

# 核心：这行代码会完成三件事：
# 1. 链接 libTermiCoreBase.a
# 2. 自动把 TermiCoreBase/include 加入到搜索路径
# 3. 继承 TermiCoreBase 的 C++17 标准
target_link_libraries(snake PRIVATE TermiCoreBase)