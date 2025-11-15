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
class IRFunctionCallNode;

class IRValNode; // base class || use `shared_ptr` for this and its derived classes
class IRConstNode;
class IRVariableNode;

// skeleton of the defined class above
class IRProgramNode : public IRNode {
  public:
    std::vector<std::unique_ptr<IRFunctionNode>> functions;

    IRProgramNode() = default;
    explicit IRProgramNode(std::vector<std::unique_ptr<IRFunctionNode>> func_list)
        : functions(std::move(func_list)) {}

    void dump(int indent = 0) const;
    std::unique_ptr<AsmProgramNode> lowerToAsm();
};

class IRFunctionNode : public IRNode {
  public:
    std::string func_name;
    std::vector<std::string> parameters;
    std::vector<std::unique_ptr<IRInstructionNode>> instructions;

    IRFunctionNode() = default;
    IRFunctionNode(std::string name,
                   std::vector<std::unique_ptr<IRInstructionNode>> instruction_list)
        : func_name(std::move(name)), instructions(std::move(instruction_list)) {}

    void dump(int indent) const;
    std::unique_ptr<AsmFunctionNode> lowerToAsm();
};

class IRInstructionNode : public IRNode {
  public:
    virtual ~IRInstructionNode() = default;
    virtual void dump(int indent) const = 0;
    virtual std::vector<std::unique_ptr<AsmInstructionNode>> lowerToAsm() = 0;
};

class IRRetNode : public IRInstructionNode {
  public:
    std::shared_ptr<IRValNode> ret_val;

    IRRetNode() = default;
    explicit IRRetNode(std::shared_ptr<IRValNode> val_ptr) : ret_val(std::move(val_ptr)) {}

    void dump(int indent) const override;
    std::vector<std::unique_ptr<AsmInstructionNode>> lowerToAsm() override;
};

class IRUnaryNode : public IRInstructionNode {
  public:
    std::string op_type;
    std::shared_ptr<IRValNode> val_src;
    std::shared_ptr<IRValNode> val_dest;

    IRUnaryNode() = default;
    IRUnaryNode(std::string op, std::shared_ptr<IRValNode> src, std::shared_ptr<IRValNode> dest)
        : op_type(std::move(op)), val_src(std::move(src)), val_dest(std::move(dest)) {}

    void dump(int indent) const override;
    std::vector<std::unique_ptr<AsmInstructionNode>> lowerToAsm() override;
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

    void dump(int indent) const override;
    std::vector<std::unique_ptr<AsmInstructionNode>> lowerToAsm() override;
};

/// @brief val_src => val_dest
class IRCopyNode : public IRInstructionNode {
  public:
    std::shared_ptr<IRValNode> val_src;
    std::shared_ptr<IRValNode> val_dest;

    IRCopyNode() = default;
    IRCopyNode(std::shared_ptr<IRValNode> src, std::shared_ptr<IRValNode> dest)
        : val_src(std::move(src)), val_dest(std::move(dest)) {}

    void dump(int indent) const override;
    std::vector<std::unique_ptr<AsmInstructionNode>> lowerToAsm() override;
};

class IRJumpNode : public IRInstructionNode {
  public:
    std::string target_label;

    IRJumpNode() = default;
    explicit IRJumpNode(std::string label) : target_label(std::move(label)) {}

    void dump(int indent) const override;
    std::vector<std::unique_ptr<AsmInstructionNode>> lowerToAsm() override;
};

class IRJumpIfZeroNode : public IRInstructionNode {
  public:
    std::shared_ptr<IRValNode> condition;
    std::string target_label;

    IRJumpIfZeroNode() = default;
    IRJumpIfZeroNode(std::shared_ptr<IRValNode> cond, std::string label)
        : condition(std::move(cond)), target_label(std::move(label)) {}

    void dump(int indent) const override;
    std::vector<std::unique_ptr<AsmInstructionNode>> lowerToAsm() override;
};

class IRJumpIfNotZeroNode : public IRInstructionNode {
  public:
    std::shared_ptr<IRValNode> condition;
    std::string target_label;

    IRJumpIfNotZeroNode() = default;
    IRJumpIfNotZeroNode(std::shared_ptr<IRValNode> cond, std::string label)
        : condition(std::move(cond)), target_label(std::move(label)) {}

    void dump(int indent) const override;
    std::vector<std::unique_ptr<AsmInstructionNode>> lowerToAsm() override;
};

class IRLabelNode : public IRInstructionNode {
  public:
    std::string label_name;

    IRLabelNode() = default;
    explicit IRLabelNode(std::string name) : label_name(std::move(name)) {}

    void dump(int indent) const override;
    std::vector<std::unique_ptr<AsmInstructionNode>> lowerToAsm() override;
};

class IRFunctionCallNode : public IRInstructionNode {
  public:
    std::string func_name;
    std::vector<std::shared_ptr<IRValNode>> arguments;
    std::shared_ptr<IRValNode> return_dest;

    IRFunctionCallNode() = default;
    explicit IRFunctionCallNode(std::string name, std::vector<std::shared_ptr<IRValNode>> args,
                                std::shared_ptr<IRValNode> ret_dest)
        : func_name(std::move(name)), arguments(std::move(args)), return_dest(std::move(ret_dest)) {
    }

    void dump(int indent) const override;
    std::vector<std::unique_ptr<AsmInstructionNode>> lowerToAsm() override;
};

class IRValNode : public IRNode {
  public:
    virtual ~IRValNode() = default;
    virtual std::string dump() const = 0;
    virtual std::shared_ptr<AsmOperandNode> lowerToAsm() = 0;
};

class IRConstNode : public IRValNode {
  public:
    std::string val;

    IRConstNode() = default;
    explicit IRConstNode(std::string value) : val(std::move(value)) {}

    std::string dump() const override;
    std::shared_ptr<AsmOperandNode> lowerToAsm() override; // return AsmImmediateNode
};

class IRVariableNode : public IRValNode {
  public:
    std::string var_name;

    IRVariableNode() = default;
    explicit IRVariableNode(std::string name) : var_name(std::move(name)) {}

    std::string dump() const override;
    std::shared_ptr<AsmOperandNode> lowerToAsm() override; // return AsmPseudoNode
};