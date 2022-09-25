cmake_minimum_required(VERSION 3.21)
project(c_snake_game C)

set(CMAKE_C_STANDARD 99)

add_executable(c_snake_game main.c snake.h snake.c)
