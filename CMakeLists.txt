# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.10)

project ("MyConsoleGames")

# Include sub-projects.
enable_testing()
add_subdirectory (TermiCoreBase)
add_subdirectory(games/snake)
add_subdirectory(games/tetris)