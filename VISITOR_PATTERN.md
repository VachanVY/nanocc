# Visitor Pattern Refactoring - Proof of Concept

## Overview

This proof of concept demonstrates the Visitor Pattern applied to a subset of the compiler's AST nodes. The implementation shows how the pattern can be used to separate compiler phases (parsing, semantic analysis, IR generation) from the AST node structure itself.

## What Has Been Implemented

### 1. Visitor Interface (`include/parser.hpp`)

A base `ASTVisitor` interface has been added with visit methods for a subset of nodes:

**Expression Nodes:**
- `BinaryNode` - Binary operations (+, -, *, /, etc.)
- `UnaryNode` - Unary operations (-, !, ~)
- `ConstantNode` - Integer constants
- `VarNode` - Variable references

**Statement Nodes:**
- `ReturnNode` - Return statements
- `ExpressionNode` - Expression statements

### 2. Accept Methods

Each of the above node classes now has an `accept(ASTVisitor& visitor)` method that implements the double-dispatch pattern:

```cpp
void BinaryNode::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}
```

### 3. Example Visitor Implementations (`include/example_visitors.hpp`)

Two concrete visitor classes demonstrate different use cases:

**ExamplePrintVisitor:**
- Shows how to traverse and inspect AST nodes
- Prints information about each visited node
- Demonstrates basic visitor functionality

**SemanticAnalysisVisitor:**
- Wraps existing `resolveTypes()` and `checkTypes()` methods
- Shows how visitors can delegate to existing functionality
- Demonstrates backwards compatibility during transition

### 4. Demo Program (`visitor_demo.cpp`)

A demonstration program that:
- Parses a C source file
- Uses visitors to process the AST
- Shows both the visitor pattern and existing methods working side-by-side
- Proves the pattern doesn't break existing functionality

## Building and Running

### Build the Demo

```bash
cd build
cmake .. -DCMAKE_CXX_COMPILER=g++-14
make visitor_demo
```

### Run the Demo

```bash
./build/visitor_demo test_simple.c
```

## Architecture Benefits

The visitor pattern provides several key benefits:

### ✅ Separation of Concerns
- AST nodes focus purely on structure and data
- Operations (semantic analysis, IR generation, etc.) are in separate visitor classes
- Each concern can be developed and tested independently

### ✅ Extensibility
- New operations can be added by creating new visitor classes
- No need to modify existing AST node classes
- Follows the Open/Closed Principle

### ✅ Testability
- Each visitor can be unit tested independently
- Mock visitors can be created for testing
- Easier to test individual compiler phases in isolation

### ✅ Maintainability
- Clear boundaries between phases
- Changes to one phase don't affect others
- Easier to understand and reason about code

## Current Status

### ✅ Completed
- [x] Visitor interface defined
- [x] Accept methods added to subset of nodes
- [x] Example visitors implemented
- [x] Demo program working
- [x] Builds successfully
- [x] Existing functionality preserved

### 📝 Future Work

To fully apply the visitor pattern across the entire compiler:

1. **Expand to All AST Nodes**
   - Add accept methods to all remaining AST node classes
   - Extend ASTVisitor interface with visit methods for all nodes

2. **Create Production Visitors**
   - `SemanticAnalyzer` - Consolidate resolveTypes(), checkTypes(), loopLabelling()
   - `IRGenerator` - Consolidate all generateIR() methods
   - Move logic from node methods into visitor methods

3. **Extend to IR Nodes**
   - Create `IRVisitor` interface
   - Add accept methods to IR nodes
   - `AsmGenerator` - Consolidate all lowerToAsm() methods

4. **Update Main Pipeline**
   - Modify `helper.hpp` to use visitors instead of calling node methods directly
   - Ensure backwards compatibility during transition

5. **Deprecate Old Methods**
   - Once visitors are fully working, remove old methods from nodes
   - Keep only `parse()` and `dump()` in nodes

## Example Usage

```cpp
// Create a visitor
ExamplePrintVisitor visitor;

// Parse a file
auto ast = parse(tokens);

// Visit nodes in the AST
for (auto& function : ast->functions) {
    if (function->body) {
        for (auto& item : function->body->block_items) {
            if (item->statement && item->statement->return_stmt) {
                // Use the visitor pattern
                item->statement->return_stmt->accept(visitor);
            }
        }
    }
}
```

## Design Decisions

### Why Keep Existing Methods?

The existing methods (`resolveTypes()`, `checkTypes()`, `generateIR()`, etc.) are kept during the transition to:
- Ensure the compiler continues to work
- Allow gradual migration to the visitor pattern
- Enable both patterns to coexist during development
- Reduce risk of breaking changes

### Why Start with a Subset?

Starting with expression and statement nodes:
- Proves the concept with manageable scope
- Validates the approach before full commitment
- Allows for feedback and iteration
- Demonstrates the pattern to other developers

### Future Visitor Hierarchy

The visitor pattern can support multiple types of visitors:

```
ASTVisitor (interface)
├── SemanticAnalyzer
│   ├── IdentifierResolver
│   ├── TypeChecker
│   └── LoopLabeller
├── IRGenerator
└── ASTDebugPrinter

IRVisitor (interface)
├── AsmGenerator
└── IROptimizer
```

## Testing

To test the visitor pattern:

```bash
# Run the demo with different C files
./build/visitor_demo test_simple.c

# Verify existing functionality still works
./build/nanocc -S test_simple.c -o test.s
gcc test.s -o test
./test
echo $?  # Should print 42
```

## Conclusion

This proof of concept successfully demonstrates that the Visitor Pattern can be applied to the nanocc compiler to achieve better separation of concerns. The pattern works alongside existing code, proving it can be adopted incrementally without breaking functionality.

The next steps would be to systematically expand the visitor interfaces to cover all node types and gradually migrate logic from node methods into visitor classes.
