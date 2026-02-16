
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
    // IF NOT `no_renaming` then variable will be given a unique name in identifier resolution pass
    // in the identifier resolution pass in semantic analysis,
    // Eg: static int x; at file scope, `no_renaming` will be true, so won't be renamed
    //     int x       ; at block scope, `no_renaming` will be false, so will be renamed
    bool no_renaming;
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

// declare attricutes for `TypeCheckerSymbolTable`
struct FuncAttr;
struct StaticAttr;
struct LocalAttr;
using IdentifierAttrs = std::variant<FuncAttr, StaticAttr, LocalAttr>;

struct Tentative;
struct Initial;
struct NoIntializer;
using InitValue = std::variant<Tentative, Initial, NoIntializer>;

struct Tentative {};
struct Initial {
    std::string value;
};
struct NoIntializer {};

struct FuncAttr {
    bool defined;
    /// @brief false if has internal linkage (`static`), true otherwise
    bool global;
};
struct StaticAttr {
    InitValue init;
    /// @brief || false for block scope `static` variables ||
    /// true for block scope `extern` variables ||
    /// true for all NON `static` variables in file scope ||
    bool global;
};
struct LocalAttr {};

struct SymbolTableEntry {
    Type type;
    IdentifierAttrs attrs;
};

/// @brief type checker symbol table
using TypeCheckerSymbolTable = std::unordered_map<std::string, SymbolTableEntry>;

namespace nanocc {
/// @brief global type checker symbol table, helpful in codegen phase
/// add suffix @PLT for non defined function
extern TypeCheckerSymbolTable global_type_checker_map;

void semanticAnalysis(std::unique_ptr<ProgramNode>& ast, bool debug = false);

void semaIdentifierResolution(std::unique_ptr<ProgramNode>& program_node);
void semaCheckTypes(std::unique_ptr<ProgramNode>& ast,
                    TypeCheckerSymbolTable& global_type_checker_map);
void semaLoopLabelling(std::unique_ptr<ProgramNode>& ast);
} // namespace nanocc
