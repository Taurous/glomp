cmake_minimum_required(VERSION 3.16)

project(glomp)

set(CMAKE_CXX_STANDARD 20)

add_executable(glomp src/lexer.cpp src/glomp.cpp)

target_compile_options(glomp PRIVATE -g -Wall -Werror)
target_include_directories(glomp PRIVATE include)
