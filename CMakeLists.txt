cmake_minimum_required(VERSION 4.0)
project(untitled11)

set(CMAKE_CXX_STANDARD 20)

add_executable(untitled11 snakeGamee.cpp snake.cpp
        snake.cpp)

find_package(raylib REQUIRED)

target_link_libraries(untitled11 PRIVATE raylib)
