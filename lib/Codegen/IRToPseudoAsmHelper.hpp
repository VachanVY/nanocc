#pragma once

#include <memory>
#include <vector>

#include "nanocc/Codegen/ASM.hpp"
#include "nanocc/IR/IR.hpp"

namespace AsmGen {
std::unique_ptr<AsmProgramNode> programLowerIRToAsm(IRProgramNode& program);
std::unique_ptr<AsmFunctionNode> functionLowerIRToAsm(IRFunctionNode& func);
std::unique_ptr<AsmStaticVariableNode>
staticVarLowerIRToAsm(IRStaticVarNode& static_var);
std::vector<std::unique_ptr<AsmInstructionNode>>
instructionLowerIRToAsm(const std::unique_ptr<IRInstructionNode>& instr);
std::shared_ptr<AsmOperandNode>
operandLowerIRToAsm(const std::shared_ptr<IRValNode>& val);

std::vector<std::unique_ptr<AsmInstructionNode>>
retLowerIRToAsm(IRRetNode& node);
std::vector<std::unique_ptr<AsmInstructionNode>>
unaryLowerIRToAsm(IRUnaryNode& node);
std::vector<std::unique_ptr<AsmInstructionNode>>
binaryLowerIRToAsm(IRBinaryNode& node);
std::vector<std::unique_ptr<AsmInstructionNode>>
copyLowerIRToAsm(IRCopyNode& node);
std::vector<std::unique_ptr<AsmInstructionNode>>
jumpLowerIRToAsm(IRJumpNode& node);
std::vector<std::unique_ptr<AsmInstructionNode>>
jumpIfZeroLowerIRToAsm(IRJumpIfZeroNode& node);
std::vector<std::unique_ptr<AsmInstructionNode>>
jumpIfNotZeroLowerIRToAsm(IRJumpIfNotZeroNode& node);
std::vector<std::unique_ptr<AsmInstructionNode>>
labelLowerIRToAsm(IRLabelNode& node);
std::vector<std::unique_ptr<AsmInstructionNode>>
functionCallLowerIRToAsm(IRFunctionCallNode& node);
} // namespace AsmGen
