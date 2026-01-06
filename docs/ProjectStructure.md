# Project Structure

This document describes the organization of the nanocc codebase.

## Directory Layout

```
nanocc/
├── include/nanocc/      # Public header files
│   ├── AST/            # Abstract Syntax Tree definitions
│   ├── Codegen/        # Code generation interfaces
│   ├── IR/             # Intermediate representation
│   ├── Lexer/          # Lexical analysis
│   ├── Parser/         # Parsing
│   ├── Sema/           # Semantic analysis
│   ├── Target/         # Target-specific code
│   │   └── X86/        # X86 target support
│   └── Utils.hpp       # Utility functions
├── lib/                # Library implementation files
│   ├── Codegen/        # Code generation implementation
│   ├── IR/             # IR generation and manipulation
│   ├── Lexer/          # Lexer implementation
│   ├── Parser/         # Parser implementation
│   ├── Sema/           # Semantic analysis implementation
│   └── Target/         # Target-specific implementations
│       └── X86/        # X86 code generation
├── tools/              # Executable programs
│   └── nanocc.cpp      # Main compiler driver
├── docs/               # Documentation
├── unittests/          # Unit tests
├── legacy/             # Legacy code (preserved for reference)
└── CMakeLists.txt      # Build configuration
```

## Design Philosophy

The project follows LLVM-style organization:

1. **Separation of Interface and Implementation**
   - Public headers in `include/nanocc/`
   - Implementation in `lib/`

2. **Modular Architecture**
   - Each compiler phase is a separate module
   - Clear dependencies between modules

3. **Target Independence**
   - Core compiler logic is target-independent
   - Target-specific code isolated in `Target/`

## Module Dependencies

```
Lexer → Parser → Sema → IR → Codegen → Target
```

- **Lexer**: No dependencies (except utilities)
- **Parser**: Depends on Lexer, AST
- **Sema**: Depends on AST
- **IR**: Depends on AST
- **Codegen**: Depends on IR
- **Target**: Depends on Codegen

## Adding New Components

1. Create header in `include/nanocc/Component/`
2. Add implementation in `lib/Component/`
3. Update `lib/CMakeLists.txt` to include new source files
4. Add documentation in `docs/`
5. Add unit tests in `unittests/Component/`

## Legacy Code

The `legacy/` directory contains the original flat structure. It's preserved for:
- Historical reference
- Testing framework compatibility
- Gradual migration

New development should use the modular structure.
