#pragma once

#include <memory>
#include <string>
#include <utility>
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
    IRProgramNode() = default;
    explicit IRProgramNode(std::unique_ptr<IRFunctionNode> func_ptr) : func(std::move(func_ptr)) {}
    std::unique_ptr<IRFunctionNode> func;

    void dump_ir(int indent = 0) const;
    std::unique_ptr<AsmProgramNode> emit_asm();
};

class IRFunctionNode : public IRNode {
  public:
    IRFunctionNode() = default;
    IRFunctionNode(std::string name,
                   std::vector<std::unique_ptr<IRInstructionNode>> instruction_list)
        : var_name(std::move(name)), instructions(std::move(instruction_list)) {}
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
    IRRetNode() = default;
    explicit IRRetNode(std::shared_ptr<IRValNode> val_ptr) : val(std::move(val_ptr)) {}
    std::shared_ptr<IRValNode> val;

    void dump_ir(int indent) const override;
    std::vector<std::unique_ptr<AsmInstructionNode>> emit_asm() override;
};

class IRUnaryNode : public IRInstructionNode {
  public:
    IRUnaryNode() = default;
    IRUnaryNode(std::string op, std::shared_ptr<IRValNode> src, std::shared_ptr<IRValNode> dest)
        : op_type(std::move(op)), val_src(std::move(src)), val_dest(std::move(dest)) {}
    std::string op_type;
    std::shared_ptr<IRValNode> val_src;
    std::shared_ptr<IRValNode> val_dest;

    void dump_ir(int indent) const override;
    std::vector<std::unique_ptr<AsmInstructionNode>> emit_asm() override;
};

class IRBinaryNode : public IRInstructionNode {
  public:
    IRBinaryNode() = default;
    IRBinaryNode(std::string op, std::shared_ptr<IRValNode> src1, std::shared_ptr<IRValNode> src2,
                 std::shared_ptr<IRValNode> dest)
        : op_type(std::move(op)), val_src1(std::move(src1)), val_src2(std::move(src2)),
          val_dest(std::move(dest)) {}
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
    IRConstNode() = default;
    explicit IRConstNode(std::string value) : val(std::move(value)) {}
    std::string val;
    std::string dump_ir() const override;
    std::shared_ptr<AsmOperandNode> emit_asm() override; // return AsmImmediateNode
};

class IRVariableNode : public IRValNode {
  public:
    IRVariableNode() = default;
    explicit IRVariableNode(std::string name) : var_name(std::move(name)) {}
    std::string var_name;
    std::string dump_ir() const override;
    std::shared_ptr<AsmOperandNode> emit_asm() override; // return AsmPseudoNode
};
