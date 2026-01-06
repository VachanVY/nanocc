#pragma once

#include <memory>
#include "nanocc/AST/AST.hpp"
#include "nanocc/Sema/Sema.hpp"

// indetifier resolution -- start
void ProgramNodeResolveTypes(std::unique_ptr<ProgramNode>&, IdentifierMap&);
void DeclarationNodeResolveTypes(std::unique_ptr<DeclarationNode>&, IdentifierMap&);
void VariableDeclNodeResolveTypes(std::unique_ptr<VariableDeclNode>&, IdentifierMap&);
void FunctionDeclNodeResolveTypes(std::unique_ptr<FunctionDeclNode>&, IdentifierMap&);
void BlockNodeResolveTypes(std::unique_ptr<BlockNode>&, IdentifierMap&);
void BlockItemNodeResolveTypes(const std::unique_ptr<BlockItemNode>&, IdentifierMap&);
void StatementNodeResolveTypes(std::unique_ptr<StatementNode>&, IdentifierMap&);
void ReturnNodeResolveTypes(std::unique_ptr<ReturnNode>&, IdentifierMap&);
void ExpressionNodeResolveTypes(std::unique_ptr<ExpressionNode>&, IdentifierMap&);
void IfElseNodeResolveTypes(std::unique_ptr<IfElseNode>&, IdentifierMap&);
void CompoundNodeResolveTypes(std::unique_ptr<CompoundNode>&, IdentifierMap&);
void BreakNodeResolveTypes(std::unique_ptr<BreakNode>&, IdentifierMap&);
void ContinueNodeResolveTypes(std::unique_ptr<ContinueNode>&, IdentifierMap&);
void WhileNodeResolveTypes(std::unique_ptr<WhileNode>&, IdentifierMap&);
void DoWhileNodeResolveTypes(std::unique_ptr<DoWhileNode>&, IdentifierMap&);
void ForNodeResolveTypes(std::unique_ptr<ForNode>&, IdentifierMap&);
void ForInitNodeResolveTypes(std::unique_ptr<ForInitNode>&, IdentifierMap&);
void NullNodeResolveTypes(std::unique_ptr<NullNode>&, IdentifierMap&);
void ExprNodeResolveTypes(std::unique_ptr<ExprNode>&, IdentifierMap&);
void ExprFactorNodeResolveTypes(std::unique_ptr<ExprFactorNode>&, IdentifierMap&);
void VarNodeResolveTypes(std::unique_ptr<VarNode>&, IdentifierMap&);
void ConstantNodeResolveTypes(std::unique_ptr<ConstantNode>&, IdentifierMap&);
void BinaryNodeResolveTypes(std::unique_ptr<BinaryNode>&, IdentifierMap&);
void AssignmentNodeResolveTypes(std::unique_ptr<AssignmentNode>&, IdentifierMap&);
void ConditionalNodeResolveTypes(std::unique_ptr<ConditionalNode>&, IdentifierMap&);
void FunctionCallNodeResolveTypes(std::unique_ptr<FunctionCallNode>&, IdentifierMap&);
void UnaryNodeResolveTypes(std::unique_ptr<UnaryNode>&, IdentifierMap&);
// identifier resolution -- end

// type checking -- start
void ProgramNodeCheckTypes(std::unique_ptr<ProgramNode>& program_node, TypeCheckerSymbolTable& type_checker_map);
void DeclarationNodeCheckTypes(std::unique_ptr<DeclarationNode>& declaration_node, TypeCheckerSymbolTable& type_checker_map);
void VariableDeclNodeCheckTypes(std::unique_ptr<VariableDeclNode>& variable_decl_node, TypeCheckerSymbolTable& type_checker_map);
void FunctionDeclNodeCheckTypes(std::unique_ptr<FunctionDeclNode>& function_decl_node, TypeCheckerSymbolTable& type_checker_map);
void BlockNodeCheckTypes(std::unique_ptr<BlockNode>& block_node, TypeCheckerSymbolTable& type_checker_map);
void BlockItemNodeCheckTypes(const std::unique_ptr<BlockItemNode>& block_item_node, TypeCheckerSymbolTable& type_checker_map);
void StatementNodeCheckTypes(std::unique_ptr<StatementNode>& statement_node, TypeCheckerSymbolTable& type_checker_map);
void ReturnNodeCheckTypes(std::unique_ptr<ReturnNode>& return_node, TypeCheckerSymbolTable& type_checker_map);
void ExpressionNodeCheckTypes(std::unique_ptr<ExpressionNode>& expression_node, TypeCheckerSymbolTable& type_checker_map);
void IfElseNodeCheckTypes(std::unique_ptr<IfElseNode>& ifelse_node, TypeCheckerSymbolTable& type_checker_map);
void CompoundNodeCheckTypes(std::unique_ptr<CompoundNode>& compound_node, TypeCheckerSymbolTable& type_checker_map);
void BreakNodeCheckTypes(std::unique_ptr<BreakNode>& break_node, TypeCheckerSymbolTable& type_checker_map);
void ContinueNodeCheckTypes(std::unique_ptr<ContinueNode>& continue_node, TypeCheckerSymbolTable& type_checker_map);
void WhileNodeCheckTypes(std::unique_ptr<WhileNode>& while_node, TypeCheckerSymbolTable& type_checker_map);
void DoWhileNodeCheckTypes(std::unique_ptr<DoWhileNode>& dowhile_node, TypeCheckerSymbolTable& type_checker_map);
void ForNodeCheckTypes(std::unique_ptr<ForNode>& for_node, TypeCheckerSymbolTable& type_checker_map);
void ForInitNodeCheckTypes(std::unique_ptr<ForInitNode>& for_init_node, TypeCheckerSymbolTable& type_checker_map);
void NullNodeCheckTypes(std::unique_ptr<NullNode>& null_node, TypeCheckerSymbolTable& type_checker_map);
void ExprNodeCheckTypes(std::unique_ptr<ExprNode>& expr_node, TypeCheckerSymbolTable& type_checker_map);
void ExprFactorNodeCheckTypes(std::unique_ptr<ExprFactorNode>& expr_factor_node, TypeCheckerSymbolTable& type_checker_map);
void VarNodeCheckTypes(std::unique_ptr<VarNode>& var_node, TypeCheckerSymbolTable& type_checker_map);
void ConstantNodeCheckTypes(std::unique_ptr<ConstantNode>& constant_node, TypeCheckerSymbolTable& type_checker_map);
void UnaryNodeCheckTypes(std::unique_ptr<UnaryNode>& unary_node, TypeCheckerSymbolTable& type_checker_map);
void BinaryNodeCheckTypes(std::unique_ptr<BinaryNode>& binary_node, TypeCheckerSymbolTable& type_checker_map);
void AssignmentNodeCheckTypes(std::unique_ptr<AssignmentNode>& assignment_node, TypeCheckerSymbolTable& type_checker_map);
void FunctionCallNodeCheckTypes(std::unique_ptr<FunctionCallNode>& function_call_node, TypeCheckerSymbolTable& type_checker_map);
void ConditionalNodeCheckTypes(std::unique_ptr<ConditionalNode>& conditional_node, TypeCheckerSymbolTable& type_checker_map);
// type checking -- end

// loop labelling -- start
void ProgramNodeLoopLabelling(std::unique_ptr<ProgramNode>& program_node, std::string& loop_label);
void DeclarationNodeLoopLabelling(std::unique_ptr<DeclarationNode>& declaration_node, std::string& loop_label);
void VariableDeclNodeLoopLabelling(std::unique_ptr<VariableDeclNode>& variable_decl_node, std::string& loop_label);
void FunctionDeclNodeLoopLabelling(std::unique_ptr<FunctionDeclNode>& function_decl_node, std::string& loop_label);
void BlockNodeLoopLabelling(std::unique_ptr<BlockNode>& block_node, std::string& loop_label);
void BlockItemNodeLoopLabelling(const std::unique_ptr<BlockItemNode>& block_item_node, std::string& loop_label);
void StatementNodeLoopLabelling(std::unique_ptr<StatementNode>& statement_node, std::string& loop_label);
void ReturnNodeLoopLabelling(std::unique_ptr<ReturnNode>& return_node, std::string& loop_label);
void ExpressionNodeLoopLabelling(std::unique_ptr<ExpressionNode>& expression_node, std::string& loop_label);
void IfElseNodeLoopLabelling(std::unique_ptr<IfElseNode>& ifelse_node, std::string& loop_label);
void CompoundNodeLoopLabelling(std::unique_ptr<CompoundNode>& compound_node, std::string& loop_label);
void BreakNodeLoopLabelling(std::unique_ptr<BreakNode>& break_node, std::string& loop_label);
void ContinueNodeLoopLabelling(std::unique_ptr<ContinueNode>& continue_node, std::string& loop_label);
void WhileNodeLoopLabelling(std::unique_ptr<WhileNode>& while_node, std::string& loop_label);
void DoWhileNodeLoopLabelling(std::unique_ptr<DoWhileNode>& dowhile_node, std::string& loop_label);
void ForNodeLoopLabelling(std::unique_ptr<ForNode>& for_node, std::string& loop_label);
void NullNodeLoopLabelling(std::unique_ptr<NullNode>& null_node, std::string& loop_label);
// loop labelling -- end