cmake_minimum_required(VERSION 4.1)
project(DynamicArray C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(darr STATIC
	include/dynamic_array.h
	src/dynamic_array.c
)
target_include_directories(darr PUBLIC include)

add_executable(${PROJECT_NAME}
	src/main.c
)
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PRIVATE darr)

