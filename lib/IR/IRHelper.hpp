#pragma once

#include <memory>
#include <vector>

#include "nanocc/IR/IR.hpp"

template <typename T> using VecPtr = std::vector<std::unique_ptr<T>>;
template <typename T> using UPtr = std::unique_ptr<T>;
template <typename T> using ShPtr = std::shared_ptr<T>;

namespace irgen {
UPtr<IRProgramNode> programNodeIRGen(UPtr<ProgramNode>& program_node);
UPtr<IRFunctionNode> functionDeclNodeIRGen(UPtr<FunctionDeclNode>& function);

// statement and block nodes
VecPtr<IRInstructionNode> blockNodeIRGen(UPtr<BlockNode>& block);
VecPtr<IRInstructionNode> blockItemNodeIRGen(UPtr<BlockItemNode>& block_item);
VecPtr<IRInstructionNode> declarationNodeIRGen(UPtr<DeclarationNode>& declaration);
VecPtr<IRInstructionNode> variableDeclNodeIRGen(UPtr<VariableDeclNode>& var);
VecPtr<IRInstructionNode> statementNodeIRGen(UPtr<StatementNode>& statement);
VecPtr<IRInstructionNode> returnNodeIRGen(UPtr<ReturnNode>& return_stmt);
VecPtr<IRInstructionNode> expressionNodeIRGen(UPtr<ExpressionNode>& expression_stmt);
VecPtr<IRInstructionNode> ifElseNodeIRGen(UPtr<IfElseNode>& ifelse_stmt);
VecPtr<IRInstructionNode> compoundNodeIRGen(UPtr<CompoundNode>& compound_stmt);
VecPtr<IRInstructionNode> breakNodeIRGen(UPtr<BreakNode>& break_stmt);
VecPtr<IRInstructionNode> continueNodeIRGen(UPtr<ContinueNode>& continue_stmt);
VecPtr<IRInstructionNode> whileNodeIRGen(UPtr<WhileNode>& while_stmt);
VecPtr<IRInstructionNode> doWhileNodeIRGen(UPtr<DoWhileNode>& dowhile_stmt);
VecPtr<IRInstructionNode> forNodeIRGen(UPtr<ForNode>& for_stmt);
VecPtr<IRInstructionNode> nullNodeIRGen(UPtr<NullNode>& null_stmt);
VecPtr<IRInstructionNode> forInitNodeIRGen(UPtr<ForInitNode>& init);

// expr nodes
ShPtr<IRValNode> exprNodeIRGen(UPtr<ExprNode>& expr, VecPtr<IRInstructionNode>& instructions);
ShPtr<IRValNode> exprFactorNodeIRGen(UPtr<ExprFactorNode>& exprf,
                                     VecPtr<IRInstructionNode>& instructions);
ShPtr<IRValNode> constantNodeIRGen(UPtr<ConstantNode>& constant,
                                   VecPtr<IRInstructionNode>& instructions);
ShPtr<IRValNode> varNodeIRGen(UPtr<VarNode>& var, VecPtr<IRInstructionNode>& instructions);
ShPtr<IRValNode> unaryNodeIRGen(UPtr<UnaryNode>& unary, VecPtr<IRInstructionNode>& instructions);
ShPtr<IRValNode> binaryNodeIRGen(BinaryNode* binop, VecPtr<IRInstructionNode>& instructions);
ShPtr<IRValNode> assignmentNodeIRGen(AssignmentNode* assignop,
                                     VecPtr<IRInstructionNode>& instructions);
ShPtr<IRValNode> conditionalNodeIRGen(ConditionalNode* condop,
                                      VecPtr<IRInstructionNode>& instructions);
ShPtr<IRValNode> functionCallNodeIRGen(UPtr<FunctionCallNode>& func_call,
                                       VecPtr<IRInstructionNode>& instructions);

// dump functions
void programNodeIRDump(UPtr<IRProgramNode>& ir_program, int indent);
void functionNodeIRDump(UPtr<IRFunctionNode>& ir_function, int indent);
void retNodeIRDump(IRRetNode* ret_node, int indent);
void unaryNodeIRDump(IRUnaryNode* unary_node, int indent);
void binaryNodeIRDump(IRBinaryNode* binary_node, int indent);
void copyNodeIRDump(IRCopyNode* copy_node, int indent);
void jumpNodeIRDump(IRJumpNode* jump_node, int indent);
void jumpIfZeroNodeIRDump(IRJumpIfZeroNode* jump_node, int indent);
void jumpIfNotZeroNodeIRDump(IRJumpIfNotZeroNode* jump_node, int indent);
void labelNodeIRDump(IRLabelNode* label_node, int indent);
void functionCallNodeIRDump(IRFunctionCallNode* func_call_node, int indent);
std::string valNodeIRDump(ShPtr<IRValNode>& val_node);
void instructionNodeIRDump(UPtr<IRInstructionNode>& instr_node, int indent);
} // namespace irgen