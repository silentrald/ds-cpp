cmake_minimum_required(VERSION 3.22.1)
project(ds)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

add_executable(${PROJECT_NAME} src/main.cpp)

include_directories(src)

