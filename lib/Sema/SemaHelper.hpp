#pragma once

#include "nanocc/AST/AST.hpp"
#include "nanocc/Sema/Sema.hpp"
#include <memory>

#define STAGE "Semantic Analysis"

namespace Sema {
// indetifier resolution -- start
void programNodeResolveTypes(ProgramNode&, IdentifierMap&);
void declarationNodeResolveTypes(DeclarationNode&, IdentifierMap&);
void variableDeclNodeFileScopeResolveType(VariableDeclNode&, IdentifierMap&);
void variableDeclNodeBlockScopeResolveTypes(VariableDeclNode&, IdentifierMap&);
void functionDeclNodeResolveTypes(FunctionDeclNode&, IdentifierMap&);
void blockNodeResolveTypes(BlockNode&, IdentifierMap&);
void blockItemNodeResolveTypes(const BlockItemNode&, IdentifierMap&);
void statementNodeResolveTypes(StatementNode&, IdentifierMap&);
void returnNodeResolveTypes(ReturnNode&, IdentifierMap&);
void expressionNodeResolveTypes(ExpressionNode&, IdentifierMap&);
void ifElseNodeResolveTypes(IfElseNode&, IdentifierMap&);
void compoundNodeResolveTypes(CompoundNode&, IdentifierMap&);
void breakNodeResolveTypes(BreakNode&, IdentifierMap&);
void continueNodeResolveTypes(ContinueNode&, IdentifierMap&);
void whileNodeResolveTypes(WhileNode&, IdentifierMap&);
void doWhileNodeResolveTypes(DoWhileNode&, IdentifierMap&);
void forNodeResolveTypes(ForNode&, IdentifierMap&);
void forInitNodeResolveTypes(ForInitNode&, IdentifierMap&);
void nullNodeResolveTypes(NullNode&, IdentifierMap&);
void exprNodeResolveTypes(ExprNode&, IdentifierMap&);
void exprFactorNodeResolveTypes(ExprFactorNode&, IdentifierMap&);
void varNodeResolveTypes(VarNode&, IdentifierMap&);
void constantNodeResolveTypes(ConstantNode&, IdentifierMap&);
void binaryNodeResolveTypes(BinaryNode&, IdentifierMap&);
void assignmentNodeResolveTypes(AssignmentNode&, IdentifierMap&);
void conditionalNodeResolveTypes(ConditionalNode&, IdentifierMap&);
void functionCallNodeResolveTypes(FunctionCallNode&, IdentifierMap&);
void unaryNodeResolveTypes(UnaryNode&, IdentifierMap&);
// identifier resolution -- end

// type checking -- start
void programNodeCheckTypes(ProgramNode& program_node,
                           TypeCheckerSymbolTable& type_checker_map);
void declarationNodeCheckTypes(DeclarationNode& declaration_node,
                               TypeCheckerSymbolTable& type_checker_map);
void variableDeclNodeFileScopeCheckTypes(
    VariableDeclNode& variable_decl_node,
    TypeCheckerSymbolTable& type_checker_map);
void variableDeclNodeBlockScopeCheckTypes(
    VariableDeclNode& variable_decl_node,
    TypeCheckerSymbolTable& type_checker_map);
void functionDeclNodeCheckTypes(FunctionDeclNode& function_decl_node,
                                TypeCheckerSymbolTable& type_checker_map);
void blockNodeCheckTypes(BlockNode& block_node,
                         TypeCheckerSymbolTable& type_checker_map);
void blockItemNodeCheckTypes(const BlockItemNode& block_item_node,
                             TypeCheckerSymbolTable& type_checker_map);
void statementNodeCheckTypes(StatementNode& statement_node,
                             TypeCheckerSymbolTable& type_checker_map);
void returnNodeCheckTypes(ReturnNode& return_node,
                          TypeCheckerSymbolTable& type_checker_map);
void expressionNodeCheckTypes(ExpressionNode& expression_node,
                              TypeCheckerSymbolTable& type_checker_map);
void ifElseNodeCheckTypes(IfElseNode& ifelse_node,
                          TypeCheckerSymbolTable& type_checker_map);
void compoundNodeCheckTypes(CompoundNode& compound_node,
                            TypeCheckerSymbolTable& type_checker_map);
void breakNodeCheckTypes(BreakNode& break_node,
                         TypeCheckerSymbolTable& type_checker_map);
void continueNodeCheckTypes(ContinueNode& continue_node,
                            TypeCheckerSymbolTable& type_checker_map);
void whileNodeCheckTypes(WhileNode& while_node,
                         TypeCheckerSymbolTable& type_checker_map);
void doWhileNodeCheckTypes(DoWhileNode& dowhile_node,
                           TypeCheckerSymbolTable& type_checker_map);
void forNodeCheckTypes(ForNode& for_node,
                       TypeCheckerSymbolTable& type_checker_map);
void forInitNodeCheckTypes(ForInitNode& for_init_node,
                           TypeCheckerSymbolTable& type_checker_map);
void nullNodeCheckTypes(NullNode& null_node,
                        TypeCheckerSymbolTable& type_checker_map);
void exprNodeCheckTypes(ExprNode& expr_node,
                        TypeCheckerSymbolTable& type_checker_map);
void exprFactorNodeCheckTypes(ExprFactorNode& expr_factor_node,
                              TypeCheckerSymbolTable& type_checker_map);
void varNodeCheckTypes(VarNode& var_node,
                       TypeCheckerSymbolTable& type_checker_map);
void constantNodeCheckTypes(ConstantNode& constant_node,
                            TypeCheckerSymbolTable& type_checker_map);
void unaryNodeCheckTypes(UnaryNode& unary_node,
                         TypeCheckerSymbolTable& type_checker_map);
void binaryNodeCheckTypes(BinaryNode& binary_node,
                          TypeCheckerSymbolTable& type_checker_map);
void assignmentNodeCheckTypes(AssignmentNode& assignment_node,
                              TypeCheckerSymbolTable& type_checker_map);
void functionCallNodeCheckTypes(FunctionCallNode& function_call_node,
                                TypeCheckerSymbolTable& type_checker_map);
void conditionalNodeCheckTypes(ConditionalNode& conditional_node,
                               TypeCheckerSymbolTable& type_checker_map);
// type checking -- end

// loop labelling -- start
void programNodeLoopLabelling(ProgramNode& program_node,
                              std::string& loop_label);
void declarationNodeLoopLabelling(DeclarationNode& declaration_node,
                                  std::string& loop_label);
void variableDeclNodeLoopLabelling(VariableDeclNode& variable_decl_node,
                                   std::string& loop_label);
void functionDeclNodeLoopLabelling(FunctionDeclNode& function_decl_node,
                                   std::string& loop_label);
void blockNodeLoopLabelling(BlockNode& block_node, std::string& loop_label);
void blockItemNodeLoopLabelling(const BlockItemNode& block_item_node,
                                std::string& loop_label);
void statementNodeLoopLabelling(StatementNode& statement_node,
                                std::string& loop_label);
void returnNodeLoopLabelling(ReturnNode& return_node, std::string& loop_label);
void expressionNodeLoopLabelling(ExpressionNode& expression_node,
                                 std::string& loop_label);
void ifElseNodeLoopLabelling(IfElseNode& ifelse_node, std::string& loop_label);
void compoundNodeLoopLabelling(CompoundNode& compound_node,
                               std::string& loop_label);
void breakNodeLoopLabelling(BreakNode& break_node, std::string& loop_label);
void continueNodeLoopLabelling(ContinueNode& continue_node,
                               std::string& loop_label);
void whileNodeLoopLabelling(WhileNode& while_node, std::string& loop_label);
void doWhileNodeLoopLabelling(DoWhileNode& dowhile_node,
                              std::string& loop_label);
void forNodeLoopLabelling(ForNode& for_node, std::string& loop_label);
void nullNodeLoopLabelling(NullNode& null_node, std::string& loop_label);
// loop labelling -- end
} // namespace Sema