#pragma once

#include <memory>
#include <list>

#include "nanocc/IR/IR.hpp"

using IRInstructionList = std::list<std::unique_ptr<IRInstructionNode>>;
namespace IRGen {
std::unique_ptr<IRProgramNode> programNodeIRGen(const ProgramNode& program_node);
std::unique_ptr<IRFunctionNode>
functionDeclNodeIRGen(const FunctionDeclNode& function);

// statement and block nodes
IRInstructionList blockNodeIRGen(const BlockNode& block);
IRInstructionList
blockItemNodeIRGen(const BlockItemNode& block_item);
IRInstructionList
declarationNodeIRGen(const DeclarationNode& declaration);
IRInstructionList
variableDeclNodeIRGen(const VariableDeclNode& var);
IRInstructionList
statementNodeIRGen(const StatementNode& statement);
IRInstructionList
returnNodeIRGen(const ReturnNode& return_stmt);
IRInstructionList
expressionNodeIRGen(const ExpressionNode& expression_stmt);
IRInstructionList
ifElseNodeIRGen(const IfElseNode& ifelse_stmt);
IRInstructionList
compoundNodeIRGen(const CompoundNode& compound_stmt);
IRInstructionList
breakNodeIRGen(const BreakNode& break_stmt);
IRInstructionList
continueNodeIRGen(const ContinueNode& continue_stmt);
IRInstructionList
whileNodeIRGen(const WhileNode& while_stmt);
IRInstructionList
doWhileNodeIRGen(const DoWhileNode& dowhile_stmt);
IRInstructionList
forNodeIRGen(const ForNode& for_stmt);
IRInstructionList
nullNodeIRGen(const NullNode& null_stmt);
IRInstructionList
forInitNodeIRGen(const ForInitNode& init);

// expr nodes
std::shared_ptr<IRValNode>
exprNodeIRGen(const ExprNode& expr,
              IRInstructionList& instructions);
std::shared_ptr<IRValNode> exprFactorNodeIRGen(
    const ExprFactorNode& exprf,
    IRInstructionList& instructions);
std::shared_ptr<IRValNode> constantNodeIRGen(
    const ConstantNode& constant,
    IRInstructionList& instructions);
std::shared_ptr<IRValNode>
varNodeIRGen(const VarNode& var,
             IRInstructionList& instructions);
std::shared_ptr<IRValNode>
unaryNodeIRGen(const UnaryNode& unary,
               IRInstructionList& instructions);
std::shared_ptr<IRValNode>
binaryNodeIRGen(const BinaryNode* binop,
                IRInstructionList& instructions);
std::shared_ptr<IRValNode> assignmentNodeIRGen(
    const AssignmentNode* assignop,
    IRInstructionList& instructions);
std::shared_ptr<IRValNode> conditionalNodeIRGen(
    const ConditionalNode* condop,
    IRInstructionList& instructions);
std::shared_ptr<IRValNode> functionCallNodeIRGen(
    const FunctionCallNode& func_call,
    IRInstructionList& instructions);
} // namespace IRGen