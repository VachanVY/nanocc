#pragma once

#include <memory>
#include <string>
#include <vector>

class IRNode {
  public:
    virtual ~IRNode() = default;
};

class IRProgramNode;

class IRFunctionNode;

class IRInstructionNode;
class IRRetNode;
class IRUnaryNode;

class IRValNode;
class IRConstNode;
class IRVariableNode;

class IRUnaryNode;
class IRComplementNode;
class IRNegateNode;

// skeleton of the defined class above
class IRProgramNode : public IRNode {
  public:
    std::unique_ptr<IRFunctionNode> func;
    void dump_ir(int indent = 0) const;
};

class IRFunctionNode : public IRNode {
  public:
    std::string var_name;
    std::vector<std::unique_ptr<IRInstructionNode>> instructions;
    void dump_ir(int indent) const;
};

class IRInstructionNode : public IRNode {
  public:
    virtual ~IRInstructionNode() = default;
    virtual void dump_ir(int indent) const = 0;
};

class IRRetNode : public IRInstructionNode {
  public:
    std::unique_ptr<IRValNode> val;
    void dump_ir(int indent) const override;
};

class IRUnaryNode : public IRInstructionNode {
  public:
    std::string op_type;
    std::unique_ptr<IRValNode> val_src;
    std::unique_ptr<IRValNode> val_dest;
    void dump_ir(int indent) const override;
};

class IRValNode : public IRNode {
  public:
    virtual ~IRValNode() = default;
    virtual std::string repr() const = 0;
};

class IRConstNode : public IRValNode {
  public:
    int val;
    std::string repr() const override;
};

class IRVariableNode : public IRValNode {
  public:
    std::string var_name;
    std::string repr() const override;
};
