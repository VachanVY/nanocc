cmake_minimum_required(VERSION 3.22.1)
project(nanocc
        DESCRIPTION "The Nano C Compiler"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(lib)
add_subdirectory(tools)

# PUBLIC — “I need this, and users of me need it too”
# INTERFACE: "I don’t need it, but users of me do"
# PRIVATE: "I need it, but users of me don’t"
