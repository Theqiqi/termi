cmake_minimum_required(VERSION 3.21)
project(LinuxConsoleGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_executable(ConsoleGame main.cpp)