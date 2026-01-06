#pragma once

#include <memory>
#include <vector>

#include "nanocc/IR/IR.hpp"

class IRGen {
public:
    static std::unique_ptr<IRProgramNode> programNodeIRGen(std::unique_ptr<ProgramNode>& program_node);
    static std::unique_ptr<IRFunctionNode> functionDeclNodeIRGen(std::unique_ptr<FunctionDeclNode>& function);

    // statement and block nodes
    static std::vector<std::unique_ptr<IRInstructionNode>> blockNodeIRGen(std::unique_ptr<BlockNode>& block);
    static std::vector<std::unique_ptr<IRInstructionNode>> blockItemNodeIRGen(std::unique_ptr<BlockItemNode>& block_item);
    static std::vector<std::unique_ptr<IRInstructionNode>> declarationNodeIRGen(std::unique_ptr<DeclarationNode>& declaration);
    static std::vector<std::unique_ptr<IRInstructionNode>> variableDeclNodeIRGen(std::unique_ptr<VariableDeclNode>& var);
    static std::vector<std::unique_ptr<IRInstructionNode>> statementNodeIRGen(std::unique_ptr<StatementNode>& statement);
    static std::vector<std::unique_ptr<IRInstructionNode>> returnNodeIRGen(std::unique_ptr<ReturnNode>& return_stmt);
    static std::vector<std::unique_ptr<IRInstructionNode>> expressionNodeIRGen(std::unique_ptr<ExpressionNode>& expression_stmt);
    static std::vector<std::unique_ptr<IRInstructionNode>> ifElseNodeIRGen(std::unique_ptr<IfElseNode>& ifelse_stmt);
    static std::vector<std::unique_ptr<IRInstructionNode>> compoundNodeIRGen(std::unique_ptr<CompoundNode>& compound_stmt);
    static std::vector<std::unique_ptr<IRInstructionNode>> breakNodeIRGen(std::unique_ptr<BreakNode>& break_stmt);
    static std::vector<std::unique_ptr<IRInstructionNode>> continueNodeIRGen(std::unique_ptr<ContinueNode>& continue_stmt);
    static std::vector<std::unique_ptr<IRInstructionNode>> whileNodeIRGen(std::unique_ptr<WhileNode>& while_stmt);
    static std::vector<std::unique_ptr<IRInstructionNode>> doWhileNodeIRGen(std::unique_ptr<DoWhileNode>& dowhile_stmt);
    static std::vector<std::unique_ptr<IRInstructionNode>> forNodeIRGen(std::unique_ptr<ForNode>& for_stmt);
    static std::vector<std::unique_ptr<IRInstructionNode>> nullNodeIRGen(std::unique_ptr<NullNode>& null_stmt);
    static std::vector<std::unique_ptr<IRInstructionNode>> forInitNodeIRGen(std::unique_ptr<ForInitNode>& init);

    // expr nodes
    static std::shared_ptr<IRValNode> exprNodeIRGen(std::unique_ptr<ExprNode>& expr, std::vector<std::unique_ptr<IRInstructionNode>>& instructions);
    static std::shared_ptr<IRValNode> exprFactorNodeIRGen(std::unique_ptr<ExprFactorNode>& exprf, std::vector<std::unique_ptr<IRInstructionNode>>& instructions);
    static std::shared_ptr<IRValNode> constantNodeIRGen(std::unique_ptr<ConstantNode>& constant, std::vector<std::unique_ptr<IRInstructionNode>>& instructions);
    static std::shared_ptr<IRValNode> varNodeIRGen(std::unique_ptr<VarNode>& var, std::vector<std::unique_ptr<IRInstructionNode>>& instructions);
    static std::shared_ptr<IRValNode> unaryNodeIRGen(std::unique_ptr<UnaryNode>& unary, std::vector<std::unique_ptr<IRInstructionNode>>& instructions);
    static std::shared_ptr<IRValNode> binaryNodeIRGen(BinaryNode* binop, std::vector<std::unique_ptr<IRInstructionNode>>& instructions);
    static std::shared_ptr<IRValNode> assignmentNodeIRGen(AssignmentNode* assignop, std::vector<std::unique_ptr<IRInstructionNode>>& instructions);
    static std::shared_ptr<IRValNode> conditionalNodeIRGen(ConditionalNode* condop, std::vector<std::unique_ptr<IRInstructionNode>>& instructions);
    static std::shared_ptr<IRValNode> functionCallNodeIRGen(std::unique_ptr<FunctionCallNode>& func_call, std::vector<std::unique_ptr<IRInstructionNode>>& instructions);

    // dump functions
    static void irProgramNodeDump(std::unique_ptr<IRProgramNode>& ir_program, int indent);
    static void irFunctionNodeDump(std::unique_ptr<IRFunctionNode>& ir_function, int indent);
    static void irRetNodeDump(IRRetNode* ret_node, int indent);
    static void irUnaryNodeDump(IRUnaryNode* unary_node, int indent);
    static void irBinaryNodeDump(IRBinaryNode* binary_node, int indent);
    static void irCopyNodeDump(IRCopyNode* copy_node, int indent);
    static void irJumpNodeDump(IRJumpNode* jump_node, int indent);
    static void irJumpIfZeroNodeDump(IRJumpIfZeroNode* jump_node, int indent);
    static void irJumpIfNotZeroNodeDump(IRJumpIfNotZeroNode* jump_node, int indent);
    static void irLabelNodeDump(IRLabelNode* label_node, int indent);
    static void irFunctionCallNodeDump(IRFunctionCallNode* func_call_node, int indent);
    static std::string irValNodeDump(std::shared_ptr<IRValNode>& val_node);
    static void irInstructionNodeDump(std::unique_ptr<IRInstructionNode>& instr_node, int indent);
};