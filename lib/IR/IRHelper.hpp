#pragma once

#include <memory>
#include <vector>

#include "nanocc/IR/IR.hpp"

namespace IRGen {
std::unique_ptr<IRProgramNode>
programNodeIRGen(const ProgramNode& program_node);
std::unique_ptr<IRFunctionNode>
functionDeclNodeIRGen(const FunctionDeclNode& function);

// statement and block nodes
std::vector<std::unique_ptr<IRInstructionNode>>
blockNodeIRGen(const BlockNode& block);
std::vector<std::unique_ptr<IRInstructionNode>>
blockItemNodeIRGen(const BlockItemNode& block_item);
std::vector<std::unique_ptr<IRInstructionNode>>
declarationNodeIRGen(const DeclarationNode& declaration);
std::vector<std::unique_ptr<IRInstructionNode>>
variableDeclNodeIRGen(const VariableDeclNode& var);
std::vector<std::unique_ptr<IRInstructionNode>>
statementNodeIRGen(const StatementNode& statement);
std::vector<std::unique_ptr<IRInstructionNode>>
returnNodeIRGen(const ReturnNode& return_stmt);
std::vector<std::unique_ptr<IRInstructionNode>>
expressionNodeIRGen(const ExpressionNode& expression_stmt);
std::vector<std::unique_ptr<IRInstructionNode>>
ifElseNodeIRGen(const IfElseNode& ifelse_stmt);
std::vector<std::unique_ptr<IRInstructionNode>>
compoundNodeIRGen(const CompoundNode& compound_stmt);
std::vector<std::unique_ptr<IRInstructionNode>>
breakNodeIRGen(const BreakNode& break_stmt);
std::vector<std::unique_ptr<IRInstructionNode>>
continueNodeIRGen(const ContinueNode& continue_stmt);
std::vector<std::unique_ptr<IRInstructionNode>>
whileNodeIRGen(const WhileNode& while_stmt);
std::vector<std::unique_ptr<IRInstructionNode>>
doWhileNodeIRGen(const DoWhileNode& dowhile_stmt);
std::vector<std::unique_ptr<IRInstructionNode>>
forNodeIRGen(const ForNode& for_stmt);
std::vector<std::unique_ptr<IRInstructionNode>>
nullNodeIRGen(const NullNode& null_stmt);
std::vector<std::unique_ptr<IRInstructionNode>>
forInitNodeIRGen(const ForInitNode& init);

// expr nodes
std::shared_ptr<IRValNode>
exprNodeIRGen(const ExprNode& expr,
              std::vector<std::unique_ptr<IRInstructionNode>>& instructions);
std::shared_ptr<IRValNode> exprFactorNodeIRGen(
    const ExprFactorNode& exprf,
    std::vector<std::unique_ptr<IRInstructionNode>>& instructions);
std::shared_ptr<IRValNode> constantNodeIRGen(
    const ConstantNode& constant,
    std::vector<std::unique_ptr<IRInstructionNode>>& instructions);
std::shared_ptr<IRValNode>
varNodeIRGen(const VarNode& var,
             std::vector<std::unique_ptr<IRInstructionNode>>& instructions);
std::shared_ptr<IRValNode>
unaryNodeIRGen(const UnaryNode& unary,
               std::vector<std::unique_ptr<IRInstructionNode>>& instructions);
std::shared_ptr<IRValNode>
binaryNodeIRGen(const BinaryNode* binop,
                std::vector<std::unique_ptr<IRInstructionNode>>& instructions);
std::shared_ptr<IRValNode> assignmentNodeIRGen(
    const AssignmentNode* assignop,
    std::vector<std::unique_ptr<IRInstructionNode>>& instructions);
std::shared_ptr<IRValNode> conditionalNodeIRGen(
    const ConditionalNode* condop,
    std::vector<std::unique_ptr<IRInstructionNode>>& instructions);
std::shared_ptr<IRValNode> functionCallNodeIRGen(
    const FunctionCallNode& func_call,
    std::vector<std::unique_ptr<IRInstructionNode>>& instructions);
} // namespace IRGen