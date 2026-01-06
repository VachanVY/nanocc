
#pragma once

#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include <memory>

#include "nanocc/AST/AST.hpp"

struct VariableScope {
    std::string unique_name;
    bool from_curr_scope;
    bool external_linkage;
};

/// @brief maps variables names given in source code to unique identifier names
/// along with scope and linkage information
using IdentifierMap = std::unordered_map<std::string, VariableScope>;

// Type Checking Stuff
struct IntType; // int type
struct FuncType;
using Type = std::variant<IntType, FuncType>;

struct IntType {};
// FuncType:- attributes: param_types, return_type, defined,
struct FuncType {
    std::vector<std::unique_ptr<Type>> param_types;
    std::unique_ptr<Type> return_type;
    bool defined; // tracks whether we have already type-checked a defination of that function
};

/// @brief type checker symbol table
using TypeCheckerSymbolTable = std::unordered_map<std::string, Type>;

namespace nanocc {
/// @brief global type checker symbol table, helpful in codegen phase
// add suffix @PLT for non defined function
extern TypeCheckerSymbolTable global_type_checker_map;

void semanticAnalysis(std::unique_ptr<ProgramNode>& ast, bool debug = false);

void semaIdentifierResolution(std::unique_ptr<ProgramNode>& program_node);
void semaCheckTypes(std::unique_ptr<ProgramNode>& ast,
                    TypeCheckerSymbolTable& global_type_checker_map);
void semaLoopLabelling(std::unique_ptr<ProgramNode>& ast);
} // namespace nanocc
