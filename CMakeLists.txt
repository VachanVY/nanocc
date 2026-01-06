cmake_minimum_required(VERSION 3.22.1)

project(nanocc
        VERSION 0.1.0
        DESCRIPTION "The Nano C Compiler"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for tooling (clangd, etc.)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build options
option(NANOCC_BUILD_TESTS "Build unit tests" OFF)
option(NANOCC_BUILD_DOCS "Build documentation" OFF)

# Add subdirectories
add_subdirectory(lib)
add_subdirectory(tools)

if(NANOCC_BUILD_TESTS)
    enable_testing()
    add_subdirectory(unittests)
endif()

# CMake Visibility Notes:
# PUBLIC - "I need this, and users of me need it too"
# INTERFACE - "I don't need it, but users of me do"
# PRIVATE - "I need it, but users of me don't"
