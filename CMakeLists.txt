cmake_minimum_required(VERSION 3.16)

project(glomp)

add_executable(glomp include/tokens.hpp include/lexer.hpp include/compiler.hpp include/interpreter.hpp src/lexer.cpp src/compiler.cpp src/interpreter.cpp src/glomp.cpp)

target_compile_features(glomp PRIVATE cxx_std_17)
target_compile_options(glomp PRIVATE -g -Wall -Werror)
target_include_directories(glomp PRIVATE include)
