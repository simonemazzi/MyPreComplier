cmake_minimum_required(VERSION 3.30)
project(Homework1 C)

set(CMAKE_C_STANDARD 23)
add_executable(Homework1 src/main.c src/MyPreCompilerFunctions.c src/MyPreCompiler.h)

# Forza modalità Debug
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")



set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
include_directories(${CMAKE_SOURCE_DIR}/src)

