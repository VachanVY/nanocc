#pragma once

#include <memory>
#include "nanocc/AST/AST.hpp"
#include "nanocc/Sema/Sema.hpp"

namespace Sema {
// indetifier resolution -- start
void programNodeResolveTypes(std::unique_ptr<ProgramNode>&, IdentifierMap&);
void declarationNodeResolveTypes(std::unique_ptr<DeclarationNode>&, IdentifierMap&);
void variableDeclNodeFileScopeResolveType(std::unique_ptr<VariableDeclNode>&, IdentifierMap&);
void variableDeclNodeBlockScopeResolveTypes(std::unique_ptr<VariableDeclNode>&, IdentifierMap&);
void functionDeclNodeResolveTypes(std::unique_ptr<FunctionDeclNode>&, IdentifierMap&);
void blockNodeResolveTypes(std::unique_ptr<BlockNode>&, IdentifierMap&);
void blockItemNodeResolveTypes(const std::unique_ptr<BlockItemNode>&, IdentifierMap&);
void statementNodeResolveTypes(std::unique_ptr<StatementNode>&, IdentifierMap&);
void returnNodeResolveTypes(std::unique_ptr<ReturnNode>&, IdentifierMap&);
void expressionNodeResolveTypes(std::unique_ptr<ExpressionNode>&, IdentifierMap&);
void ifElseNodeResolveTypes(std::unique_ptr<IfElseNode>&, IdentifierMap&);
void compoundNodeResolveTypes(std::unique_ptr<CompoundNode>&, IdentifierMap&);
void breakNodeResolveTypes(std::unique_ptr<BreakNode>&, IdentifierMap&);
void continueNodeResolveTypes(std::unique_ptr<ContinueNode>&, IdentifierMap&);
void whileNodeResolveTypes(std::unique_ptr<WhileNode>&, IdentifierMap&);
void doWhileNodeResolveTypes(std::unique_ptr<DoWhileNode>&, IdentifierMap&);
void forNodeResolveTypes(std::unique_ptr<ForNode>&, IdentifierMap&);
void forInitNodeResolveTypes(std::unique_ptr<ForInitNode>&, IdentifierMap&);
void nullNodeResolveTypes(std::unique_ptr<NullNode>&, IdentifierMap&);
void exprNodeResolveTypes(std::unique_ptr<ExprNode>&, IdentifierMap&);
void exprFactorNodeResolveTypes(std::unique_ptr<ExprFactorNode>&, IdentifierMap&);
void varNodeResolveTypes(std::unique_ptr<VarNode>&, IdentifierMap&);
void constantNodeResolveTypes(std::unique_ptr<ConstantNode>&, IdentifierMap&);
void binaryNodeResolveTypes(std::unique_ptr<BinaryNode>&, IdentifierMap&);
void assignmentNodeResolveTypes(std::unique_ptr<AssignmentNode>&, IdentifierMap&);
void conditionalNodeResolveTypes(std::unique_ptr<ConditionalNode>&, IdentifierMap&);
void functionCallNodeResolveTypes(std::unique_ptr<FunctionCallNode>&, IdentifierMap&);
void unaryNodeResolveTypes(std::unique_ptr<UnaryNode>&, IdentifierMap&);
// identifier resolution -- end

// type checking -- start
void programNodeCheckTypes(std::unique_ptr<ProgramNode>& program_node,
                           TypeCheckerSymbolTable& type_checker_map);
void declarationNodeCheckTypes(std::unique_ptr<DeclarationNode>& declaration_node,
                               TypeCheckerSymbolTable& type_checker_map);
void variableDeclNodeFileScopeCheckTypes(std::unique_ptr<VariableDeclNode>& variable_decl_node,
                                         TypeCheckerSymbolTable& type_checker_map);
void variableDeclNodeBlockScopeCheckTypes(std::unique_ptr<VariableDeclNode>& variable_decl_node,
                                          TypeCheckerSymbolTable& type_checker_map);
void functionDeclNodeCheckTypes(std::unique_ptr<FunctionDeclNode>& function_decl_node,
                                TypeCheckerSymbolTable& type_checker_map);
void blockNodeCheckTypes(std::unique_ptr<BlockNode>& block_node,
                         TypeCheckerSymbolTable& type_checker_map);
void blockItemNodeCheckTypes(const std::unique_ptr<BlockItemNode>& block_item_node,
                             TypeCheckerSymbolTable& type_checker_map);
void statementNodeCheckTypes(std::unique_ptr<StatementNode>& statement_node,
                             TypeCheckerSymbolTable& type_checker_map);
void returnNodeCheckTypes(std::unique_ptr<ReturnNode>& return_node,
                          TypeCheckerSymbolTable& type_checker_map);
void expressionNodeCheckTypes(std::unique_ptr<ExpressionNode>& expression_node,
                              TypeCheckerSymbolTable& type_checker_map);
void ifElseNodeCheckTypes(std::unique_ptr<IfElseNode>& ifelse_node,
                          TypeCheckerSymbolTable& type_checker_map);
void compoundNodeCheckTypes(std::unique_ptr<CompoundNode>& compound_node,
                            TypeCheckerSymbolTable& type_checker_map);
void breakNodeCheckTypes(std::unique_ptr<BreakNode>& break_node,
                         TypeCheckerSymbolTable& type_checker_map);
void continueNodeCheckTypes(std::unique_ptr<ContinueNode>& continue_node,
                            TypeCheckerSymbolTable& type_checker_map);
void whileNodeCheckTypes(std::unique_ptr<WhileNode>& while_node,
                         TypeCheckerSymbolTable& type_checker_map);
void doWhileNodeCheckTypes(std::unique_ptr<DoWhileNode>& dowhile_node,
                           TypeCheckerSymbolTable& type_checker_map);
void forNodeCheckTypes(std::unique_ptr<ForNode>& for_node,
                       TypeCheckerSymbolTable& type_checker_map);
void forInitNodeCheckTypes(std::unique_ptr<ForInitNode>& for_init_node,
                           TypeCheckerSymbolTable& type_checker_map);
void nullNodeCheckTypes(std::unique_ptr<NullNode>& null_node,
                        TypeCheckerSymbolTable& type_checker_map);
void exprNodeCheckTypes(std::unique_ptr<ExprNode>& expr_node,
                        TypeCheckerSymbolTable& type_checker_map);
void exprFactorNodeCheckTypes(std::unique_ptr<ExprFactorNode>& expr_factor_node,
                              TypeCheckerSymbolTable& type_checker_map);
void varNodeCheckTypes(std::unique_ptr<VarNode>& var_node,
                       TypeCheckerSymbolTable& type_checker_map);
void constantNodeCheckTypes(std::unique_ptr<ConstantNode>& constant_node,
                            TypeCheckerSymbolTable& type_checker_map);
void unaryNodeCheckTypes(std::unique_ptr<UnaryNode>& unary_node,
                         TypeCheckerSymbolTable& type_checker_map);
void binaryNodeCheckTypes(std::unique_ptr<BinaryNode>& binary_node,
                          TypeCheckerSymbolTable& type_checker_map);
void assignmentNodeCheckTypes(std::unique_ptr<AssignmentNode>& assignment_node,
                              TypeCheckerSymbolTable& type_checker_map);
void functionCallNodeCheckTypes(std::unique_ptr<FunctionCallNode>& function_call_node,
                                TypeCheckerSymbolTable& type_checker_map);
void conditionalNodeCheckTypes(std::unique_ptr<ConditionalNode>& conditional_node,
                               TypeCheckerSymbolTable& type_checker_map);
// type checking -- end

// loop labelling -- start
void programNodeLoopLabelling(std::unique_ptr<ProgramNode>& program_node, std::string& loop_label);
void declarationNodeLoopLabelling(std::unique_ptr<DeclarationNode>& declaration_node,
                                  std::string& loop_label);
void variableDeclNodeLoopLabelling(std::unique_ptr<VariableDeclNode>& variable_decl_node,
                                   std::string& loop_label);
void functionDeclNodeLoopLabelling(std::unique_ptr<FunctionDeclNode>& function_decl_node,
                                   std::string& loop_label);
void blockNodeLoopLabelling(std::unique_ptr<BlockNode>& block_node, std::string& loop_label);
void blockItemNodeLoopLabelling(const std::unique_ptr<BlockItemNode>& block_item_node,
                                std::string& loop_label);
void statementNodeLoopLabelling(std::unique_ptr<StatementNode>& statement_node,
                                std::string& loop_label);
void returnNodeLoopLabelling(std::unique_ptr<ReturnNode>& return_node, std::string& loop_label);
void expressionNodeLoopLabelling(std::unique_ptr<ExpressionNode>& expression_node,
                                 std::string& loop_label);
void ifElseNodeLoopLabelling(std::unique_ptr<IfElseNode>& ifelse_node, std::string& loop_label);
void compoundNodeLoopLabelling(std::unique_ptr<CompoundNode>& compound_node,
                               std::string& loop_label);
void breakNodeLoopLabelling(std::unique_ptr<BreakNode>& break_node, std::string& loop_label);
void continueNodeLoopLabelling(std::unique_ptr<ContinueNode>& continue_node,
                               std::string& loop_label);
void whileNodeLoopLabelling(std::unique_ptr<WhileNode>& while_node, std::string& loop_label);
void doWhileNodeLoopLabelling(std::unique_ptr<DoWhileNode>& dowhile_node, std::string& loop_label);
void forNodeLoopLabelling(std::unique_ptr<ForNode>& for_node, std::string& loop_label);
void nullNodeLoopLabelling(std::unique_ptr<NullNode>& null_node, std::string& loop_label);
// loop labelling -- end
} // namespace Sema