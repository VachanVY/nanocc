#pragma once

#include <memory>
#include <string>
#include <vector>

#include "asmgen.hpp"

class IRNode {
  public:
    virtual ~IRNode() = default;
};

class IRProgramNode;

class IRFunctionNode;

class IRInstructionNode; // base class
class IRRetNode;
class IRUnaryNode;
class IRBinaryNode;

class IRValNode; // base class
class IRConstNode;
class IRVariableNode;

// skeleton of the defined class above
class IRProgramNode : public IRNode {
  public:
    std::unique_ptr<IRFunctionNode> func;

    void dump_ir(int indent = 0) const;
    std::unique_ptr<AsmProgramNode> emit_asm();
};

class IRFunctionNode : public IRNode {
  public:
    std::string var_name;
    std::vector<std::unique_ptr<IRInstructionNode>> instructions;

    void dump_ir(int indent) const;
    std::unique_ptr<AsmFunctionNode> emit_asm();
};

class IRInstructionNode : public IRNode {
  public:
    virtual ~IRInstructionNode() = default;
    virtual void dump_ir(int indent) const = 0;
    virtual std::vector<std::unique_ptr<AsmInstructionNode>> emit_asm() = 0;
};

class IRRetNode : public IRInstructionNode {
  public:
    std::shared_ptr<IRValNode> val;

    void dump_ir(int indent) const override;
    std::vector<std::unique_ptr<AsmInstructionNode>> emit_asm() override;
};

class IRUnaryNode : public IRInstructionNode {
  public:
    std::string op_type;
    std::shared_ptr<IRValNode> val_src;
    std::shared_ptr<IRValNode> val_dest;

    void dump_ir(int indent) const override;
    std::vector<std::unique_ptr<AsmInstructionNode>> emit_asm() override;
};

class IRBinaryNode : public IRInstructionNode {
  public:
    std::string op_type;
    std::shared_ptr<IRValNode> val_src1;
    std::shared_ptr<IRValNode> val_src2;
    std::shared_ptr<IRValNode> val_dest;

    void dump_ir(int indent) const override;
    std::vector<std::unique_ptr<AsmInstructionNode>> emit_asm() override;
};

class IRValNode : public IRNode {
  public:
    virtual ~IRValNode() = default;
    virtual std::string dump_ir() const = 0;
    virtual std::shared_ptr<AsmOperandNode> emit_asm() = 0;
};

class IRConstNode : public IRValNode {
  public:
    std::string val;
    std::string dump_ir() const override;
    std::shared_ptr<AsmOperandNode> emit_asm() override; // return AsmImmediateNode
};

class IRVariableNode : public IRValNode {
  public:
    std::string var_name;
    std::string dump_ir() const override;
    std::shared_ptr<AsmOperandNode> emit_asm() override; // return AsmPseudoNode
};
