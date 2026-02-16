cmake_minimum_required(VERSION 3.22.1)
project(nanocc
        DESCRIPTION "The Nano C Compiler"
        LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

option(BUILD_NANOCC_TEST "Build nanocc test executable" OFF)

add_subdirectory(lib)
add_subdirectory(tools)
