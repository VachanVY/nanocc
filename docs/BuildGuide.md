# Build Guide

This guide describes how to build nanocc from source.

## Prerequisites

- CMake 3.22.1 or later
- C++23 compatible compiler (GCC 11+, Clang 14+)
- GCC (for preprocessing C source files)

## Quick Build

```bash
./buildcc.sh
```

This script will:
1. Configure the project with CMake
2. Build all targets

## Manual Build

```bash
# Create build directory
mkdir -p build
cd build

# Configure
cmake ..

# Build
cmake --build .

# The compiler will be at: build/tools/nanocc
```

## Build Targets

- `nanocc` - The main compiler executable
- `nanocc_lib` - The compiler library

## Using the Compiler

```bash
# Compile a C source file
./build/tools/nanocc -S source.c -o output.s

# Assemble and link
gcc output.s -o executable
```

## Testing

```bash
# Clone test suite
git clone https://github.com/VachanVY/writing-a-c-compiler-tests.git

# Run tests
./writing-a-c-compiler-tests/test_compiler ./build/tools/nanocc --chapter 9 -v
```

## CMake Options

Currently no special CMake options are available.

## Troubleshooting

### CMake version too old
Ensure you have CMake 3.22.1 or later:
```bash
cmake --version
```

### Compiler not C++23 compatible
Check your compiler version:
```bash
g++ --version  # GCC 11 or later
clang++ --version  # Clang 14 or later
```
