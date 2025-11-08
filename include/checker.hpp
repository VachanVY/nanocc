#pragma once

#include <unordered_map>
#include <string>

struct VariableScope {
    std::string unique_name;
    bool from_curr_scope; /* only used by DeclarationNode to prevent redeclaration */
};

/// @brief maps variables names given in source code to unique identifier names
typedef std::unordered_map<std::string, VariableScope> SymbolTable;

// UnaryNode
// AssignmentNode
// VarNode
// DeclarationNode
// CompoundNode