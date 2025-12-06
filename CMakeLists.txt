cmake_minimum_required(VERSION 3.22.1)
project(nanocc
        DESCRIPTION "The Nano C Compiler"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(
    BUILD_TEST
    "build test.cpp (minimal file for test framework) instead of nanocc.cpp (the actual C compiler)"
)

set(
    COMMON_SOURCES
    lexer.cpp
    parser.cpp
    checker.cpp
    irgen.cpp
    asmgen.cpp
    codegen.cpp
)

if(BUILD_TEST)
    message(STATUS "Building test_nanocc executable")
    add_executable(test_nanocc test.cpp ${COMMON_SOURCES})
else()
    message(STATUS "Building nanocc executable")
    add_executable(nanocc nanocc.cpp ${COMMON_SOURCES})
endif()

# Add visitor pattern demo executable
add_executable(visitor_demo visitor_demo.cpp ${COMMON_SOURCES})

include_directories(${PROJECT_SOURCE_DIR}/include)