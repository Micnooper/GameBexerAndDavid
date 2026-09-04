cmake_minimum_required(VERSION 3.15)

project(PongGame)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(/mnt/c/Users/david/MIREA/game/raylib-src raylib_build)

add_executable(pong game.cpp)

target_link_libraries(pong raylib)
