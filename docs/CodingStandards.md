# nanocc Coding Standards

This document describes the coding standards for the nanocc project.

## General Principles

The nanocc project follows LLVM coding standards with some modifications.

## Header Files

### Include Guards

Use `#ifndef`/`#define`/`#endif` style include guards instead of `#pragma once`:

```cpp
#ifndef NANOCC_COMPONENT_HEADER_HPP
#define NANOCC_COMPONENT_HEADER_HPP

// ... header content ...

#endif // NANOCC_COMPONENT_HEADER_HPP
```

The include guard macro should follow the pattern:
- Start with `NANOCC_`
- Follow the directory structure: `COMPONENT_SUBCOMPONENT_`
- End with the filename in uppercase: `FILENAME_HPP`

### Header Organization

1. Header guard
2. System includes
3. Project includes
4. Forward declarations
5. Class/function declarations
6. Inline implementations (if necessary)
7. Closing header guard

## File Organization

- **Public headers**: `include/nanocc/Component/Header.hpp`
- **Implementation files**: `lib/Component/Source.cpp`
- **Tool sources**: `tools/toolname.cpp`

## Formatting

The project uses `.clang-format` with LLVM style as the base:
- 4 space indentation
- 100 character line limit
- K&R brace style
- Left pointer alignment

## Naming Conventions

- **Classes**: PascalCase (e.g., `ASTNode`, `IRGenerator`)
- **Functions**: camelCase (e.g., `generateCode`, `parseExpression`)
- **Variables**: camelCase (e.g., `tokenList`, `currentNode`)
- **Constants**: PascalCase (e.g., `MaxIterations`)
- **Namespaces**: lowercase (e.g., `nanocc`)

## Documentation

Use Doxygen-style comments for public APIs:

```cpp
/// @brief Brief description of the function
/// @param paramName Description of parameter
/// @return Description of return value
ReturnType functionName(ParamType paramName);
```
