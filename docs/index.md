# nanocc Documentation

Welcome to the nanocc compiler documentation.

## Overview

nanocc is a C compiler implementation following modern compiler design principles.

## Structure

The project is organized following LLVM-style conventions:

- `include/nanocc/` - Public header files
- `lib/` - Library implementation files
- `tools/` - Executable programs
- `docs/` - Documentation
- `unittests/` - Unit tests

## Components

- **Lexer** - Tokenization of source code
- **Parser** - Abstract Syntax Tree generation
- **Sema** - Semantic analysis
- **IR** - Intermediate representation generation
- **Codegen** - Code generation
- **Target** - Target-specific code emission

## Building

```bash
./buildcc.sh
```

See [Build Guide](BuildGuide.md) for detailed build instructions.
