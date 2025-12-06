#pragma once

#include "parser.hpp"
#include <print>

/// @brief Example visitor that demonstrates the visitor pattern
/// This visitor prints information about nodes it visits
class ExamplePrintVisitor : public ASTVisitor {
public:
    void visit(BinaryNode& node) override {
        std::println("Visiting BinaryNode with operator: {}", node.op_type);
        // Could recursively visit children here if needed
    }
    
    void visit(UnaryNode& node) override {
        std::println("Visiting UnaryNode with operator: {}", node.op_type);
    }
    
    void visit(ConstantNode& node) override {
        std::println("Visiting ConstantNode with value: {}", node.val);
    }
    
    void visit(VarNode& node) override {
        std::println("Visiting VarNode with name: {}", node.var_name->name);
    }
    
    void visit(ReturnNode& node) override {
        std::println("Visiting ReturnNode");
    }
    
    void visit(ExpressionNode& node) override {
        std::println("Visiting ExpressionNode");
    }
};

/// @brief Example visitor that wraps existing semantic analysis methods
/// This demonstrates how visitors can delegate to existing functionality
class SemanticAnalysisVisitor : public ASTVisitor {
private:
    IdentifierMap& sym_table_;
    TypeCheckerSymbolTable& type_map_;
    
public:
    SemanticAnalysisVisitor(IdentifierMap& sym_table, TypeCheckerSymbolTable& type_map)
        : sym_table_(sym_table), type_map_(type_map) {}
    
    void visit(BinaryNode& node) override {
        // Delegate to existing methods for now
        node.resolveTypes(sym_table_);
        node.checkTypes(type_map_);
    }
    
    void visit(UnaryNode& node) override {
        node.resolveTypes(sym_table_);
        node.checkTypes(type_map_);
    }
    
    void visit(ConstantNode& node) override {
        node.resolveTypes(sym_table_);
        node.checkTypes(type_map_);
    }
    
    void visit(VarNode& node) override {
        node.resolveTypes(sym_table_);
        node.checkTypes(type_map_);
    }
    
    void visit(ReturnNode& node) override {
        node.resolveTypes(sym_table_);
        node.checkTypes(type_map_);
    }
    
    void visit(ExpressionNode& node) override {
        node.resolveTypes(sym_table_);
        node.checkTypes(type_map_);
    }
};
