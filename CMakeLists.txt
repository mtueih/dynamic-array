cmake_minimum_required(VERSION 4.1)
project(dynamic_array C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(darr STATIC src/dynamic_array.c)
target_include_directories(darr PUBLIC include)


add_executable(dynamic_array src/main.c)
target_include_directories(dynamic_array PUBLIC include)
target_link_libraries(dynamic_array PRIVATE darr)
