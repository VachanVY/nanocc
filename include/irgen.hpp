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
class IRCopyNode;
class IRJumpNode;
class IRJumpIfZeroNode;
class IRJumpIfNotZeroNode;
class IRLabelNode;

class IRValNode; // base class || use `shared_ptr` for this and its derived classes
class IRConstNode;
class IRVariableNode;

// skeleton of the defined class above
class IRProgramNode : public IRNode {
  public:
    std::unique_ptr<IRFunctionNode> func;

    IRProgramNode() = default;
    explicit IRProgramNode(std::unique_ptr<IRFunctionNode> func_ptr) : func(std::move(func_ptr)) {}

    void dump_ir(int indent = 0) const;
    std::unique_ptr<AsmProgramNode> emit_asm();
};

class IRFunctionNode : public IRNode {
  public:
    std::string var_name;
    std::vector<std::unique_ptr<IRInstructionNode>> instructions;

    IRFunctionNode() = default;
    IRFunctionNode(std::string name,
                   std::vector<std::unique_ptr<IRInstructionNode>> instruction_list)
        : var_name(std::move(name)), instructions(std::move(instruction_list)) {}

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

    IRRetNode() = default;
    explicit IRRetNode(std::shared_ptr<IRValNode> val_ptr) : val(std::move(val_ptr)) {}

    void dump_ir(int indent) const override;
    std::vector<std::unique_ptr<AsmInstructionNode>> emit_asm() override;
};

class IRUnaryNode : public IRInstructionNode {
  public:
    std::string op_type;
    std::shared_ptr<IRValNode> val_src;
    std::shared_ptr<IRValNode> val_dest;

    IRUnaryNode() = default;
    IRUnaryNode(std::string op, std::shared_ptr<IRValNode> src, std::shared_ptr<IRValNode> dest)
        : op_type(std::move(op)), val_src(std::move(src)), val_dest(std::move(dest)) {}

    void dump_ir(int indent) const override;
    std::vector<std::unique_ptr<AsmInstructionNode>> emit_asm() override;
};

class IRBinaryNode : public IRInstructionNode {
  public:
    std::string op_type;
    std::shared_ptr<IRValNode> val_src1;
    std::shared_ptr<IRValNode> val_src2;
    std::shared_ptr<IRValNode> val_dest;

    IRBinaryNode() = default;
    IRBinaryNode(std::string op, std::shared_ptr<IRValNode> src1, std::shared_ptr<IRValNode> src2,
                 std::shared_ptr<IRValNode> dest)
        : op_type(std::move(op)), val_src1(std::move(src1)), val_src2(std::move(src2)),
          val_dest(std::move(dest)) {}

    void dump_ir(int indent) const override;
    std::vector<std::unique_ptr<AsmInstructionNode>> emit_asm() override;
};

class IRCopyNode : public IRInstructionNode {
  public:
    std::shared_ptr<IRValNode> val_src;
    std::shared_ptr<IRValNode> val_dest;

    IRCopyNode() = default;
    IRCopyNode(std::shared_ptr<IRValNode> src, std::shared_ptr<IRValNode> dest)
        : val_src(std::move(src)), val_dest(std::move(dest)) {}

    void dump_ir(int indent) const override;
    std::vector<std::unique_ptr<AsmInstructionNode>> emit_asm() override;
};

class IRJumpNode : public IRInstructionNode {
  public:
    std::string target_label;

    IRJumpNode() = default;
    explicit IRJumpNode(std::string label) : target_label(std::move(label)) {}

    void dump_ir(int indent) const override;
    std::vector<std::unique_ptr<AsmInstructionNode>> emit_asm() override;
};

class IRJumpIfZeroNode : public IRInstructionNode {
  public:
    std::shared_ptr<IRValNode> condition;
    std::string target_label;

    IRJumpIfZeroNode() = default;
    IRJumpIfZeroNode(std::shared_ptr<IRValNode> cond, std::string label)
        : condition(std::move(cond)), target_label(std::move(label)) {}

    void dump_ir(int indent) const override;
    std::vector<std::unique_ptr<AsmInstructionNode>> emit_asm() override;
};

class IRJumpIfNotZeroNode : public IRInstructionNode {
  public:
    std::shared_ptr<IRValNode> condition;
    std::string target_label;

    IRJumpIfNotZeroNode() = default;
    IRJumpIfNotZeroNode(std::shared_ptr<IRValNode> cond, std::string label)
        : condition(std::move(cond)), target_label(std::move(label)) {}

    void dump_ir(int indent) const override;
    std::vector<std::unique_ptr<AsmInstructionNode>> emit_asm() override;
};

class IRLabelNode : public IRInstructionNode {
  public:
    std::string label_name;

    IRLabelNode() = default;
    explicit IRLabelNode(std::string name) : label_name(std::move(name)) {}

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

    IRConstNode() = default;
    explicit IRConstNode(std::string value) : val(std::move(value)) {}

    std::string dump_ir() const override;
    std::shared_ptr<AsmOperandNode> emit_asm() override; // return AsmImmediateNode
};

class IRVariableNode : public IRValNode {
  public:
    std::string var_name;

    IRVariableNode() = default;
    explicit IRVariableNode(std::string name) : var_name(std::move(name)) {}

    std::string dump_ir() const override;
    std::shared_ptr<AsmOperandNode> emit_asm() override; // return AsmPseudoNode
};
