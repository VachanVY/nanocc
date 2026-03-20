#pragma once

#include "nanocc/IR/IR.hpp"


namespace IRGen {
// dump functions
void programNodeIRDump(const IRProgramNode &ir_program, int indent);
void functionNodeIRDump(const IRFunctionNode &ir_function, int indent);
void staticVarNodeIRDump(const IRStaticVarNode &ir_static_var, int indent);
void retNodeIRDump(const IRRetNode &ret_node, int indent);
void unaryNodeIRDump(const IRUnaryNode &unary_node, int indent);
void binaryNodeIRDump(const IRBinaryNode &binary_node, int indent);
void copyNodeIRDump(const IRCopyNode &copy_node, int indent);
void jumpNodeIRDump(const IRJumpNode &jump_node, int indent);
void jumpIfZeroNodeIRDump(const IRJumpIfZeroNode &jump_node, int indent);
void jumpIfNotZeroNodeIRDump(const IRJumpIfNotZeroNode &jump_node, int indent);
void labelNodeIRDump(const IRLabelNode &label_node, int indent);
void functionCallNodeIRDump(const IRFunctionCallNode &func_call_node, int indent);
std::string valNodeIRDump(const IRValNode &val_node);
void instructionNodeIRDump(const IRInstructionNode &instr_node, int indent);
} // namespace IRGen
