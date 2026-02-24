#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class AsmASTNode {
public:
  virtual ~AsmASTNode() = default;
};

class AsmProgramNode;

class AsmTopLevelNode;
class AsmFunctionNode;
class AsmStaticVariableNode;

class AsmInstructionNode; // base class
class AsmMovNode;
class AsmUnaryNode;
class AsmBinaryNode; // all bin ops except "/" and "%"
class AsmCmpNode;
class AsmIdivNode; // for "/" and "%"
class AsmCdqNode;  // for sign extension before idiv
class AsmJmpNode;
class AsmJmpCCNode;
class AsmSetCCNode;
class AsmLabelNode;
class AsmAllocateStackNode;
class AsmDeallocateStackNode;
class AsmPushNode;
class AsmCallNode; // `call func` // doesn't have parameters vector inside it.
class AsmRetNode;

class AsmOperandNode; // base class // always use via `shared_ptr`
class AsmImmediateNode;
class AsmRegisterNode;
class AsmPseudoNode;
class AsmStackNode;
class AsmDataNode;

// TODO(VachanVY): Should I detach Asm*X*Node class methods into functions in
// different files doing same type of work? visitor pattern...

class AsmProgramNode : public AsmASTNode {
public:
  std::vector<std::unique_ptr<AsmTopLevelNode>> top_level;
  static bool classof(const AsmASTNode *node) {
    return dynamic_cast<const AsmProgramNode *>(node) != nullptr;
  }
  void
  resolvePseudoRegisters(std::unordered_map<std::string, int> &pseudo_reg_map,
                         std::vector<int> &stack_offsets);
  void fixUpInstructions(const std::vector<int> &stack_sizes);
  void generateAsm(std::ostream &os);
};

class AsmTopLevelNode : public AsmASTNode {
public:
  virtual ~AsmTopLevelNode() = default;

  static bool classof(const AsmASTNode *node) {
    return dynamic_cast<const AsmTopLevelNode *>(node) != nullptr;
  }
  virtual void
  resolvePseudoRegisters(std::unordered_map<std::string, int> &pseudo_reg_map,
                         int &stack_offset) = 0;
  virtual void fixUpInstructions(const int &stack_size) = 0;
  virtual void generateAsm(std::ostream &os) = 0;
};

class AsmFunctionNode : public AsmTopLevelNode {
public:
  std::string name;
  bool global;
  std::vector<std::unique_ptr<AsmInstructionNode>> instructions;

  AsmFunctionNode() = default;
  explicit AsmFunctionNode(std::string name, bool global)
      : name(std::move(name)), global(global) {}

  static bool classof(const AsmTopLevelNode *node) {
    return dynamic_cast<const AsmFunctionNode *>(node) != nullptr;
  }

  void
  resolvePseudoRegisters(std::unordered_map<std::string, int> &pseudo_reg_map,
                         int &nxt_offset);
  void fixUpInstructions(const int &stack_size);
  void generateAsm(std::ostream &os);
};

class AsmStaticVariableNode : public AsmTopLevelNode {
public:
  std::string name;
  bool global;
  std::string init;

  static bool classof(const AsmTopLevelNode *node) {
    return dynamic_cast<const AsmStaticVariableNode *>(node) != nullptr;
  }

  AsmStaticVariableNode() = default;
  explicit AsmStaticVariableNode(std::string name, bool global,
                                 std::string init)
      : name(std::move(name)), global(global), init(std::move(init)) {}

  void
  resolvePseudoRegisters(std::unordered_map<std::string, int> &pseudo_reg_map,
                         int &nxt_offset) override{};       // no-op
  void fixUpInstructions(const int &stack_size) override{}; // no-op
  void generateAsm(std::ostream &os) override;
};

// Instruction Nodes -- base
class AsmInstructionNode : public AsmASTNode {
public:
  static bool classof(const AsmASTNode *node) {
    return dynamic_cast<const AsmInstructionNode *>(node) != nullptr;
  }
  virtual void
  resolvePseudoRegisters(std::unordered_map<std::string, int> &pseudo_reg_map,
                         int &nxt_offset){}; // default no-op
  virtual void
  fixUpInstructions(std::vector<std::unique_ptr<AsmInstructionNode>>
                        &instructions){}; // default no-op

  virtual void generateAsm(std::ostream &os) = 0;
};

class AsmMovNode : public AsmInstructionNode {
public:
  std::shared_ptr<AsmOperandNode> src;
  std::shared_ptr<AsmOperandNode> dest;

  AsmMovNode() = default;
  AsmMovNode(std::shared_ptr<AsmOperandNode> src,
             std::shared_ptr<AsmOperandNode> dest)
      : src(std::move(src)), dest(std::move(dest)) {}

  static bool classof(const AsmInstructionNode *node) {
    return dynamic_cast<const AsmMovNode *>(node) != nullptr;
  }

  void
  resolvePseudoRegisters(std::unordered_map<std::string, int> &pseudo_reg_map,
                         int &nxt_offset) override;
  void fixUpInstructions(
      std::vector<std::unique_ptr<AsmInstructionNode>> &instructions) override;

  void generateAsm(std::ostream &os) override;
};

class AsmUnaryNode : public AsmInstructionNode {
public:
  std::string op_type;
  std::shared_ptr<AsmOperandNode> operand;

  AsmUnaryNode() = default;
  AsmUnaryNode(std::string op_type, std::shared_ptr<AsmOperandNode> operand)
      : op_type(std::move(op_type)), operand(std::move(operand)) {}

  static bool classof(const AsmInstructionNode *node) {
    return dynamic_cast<const AsmUnaryNode *>(node) != nullptr;
  }

  void
  resolvePseudoRegisters(std::unordered_map<std::string, int> &pseudo_reg_map,
                         int &nxt_offset) override;
  void generateAsm(std::ostream &os) override;
};

class AsmBinaryNode : public AsmInstructionNode {
public:
  std::string op_type;
  std::shared_ptr<AsmOperandNode> src;
  std::shared_ptr<AsmOperandNode> dest;

  AsmBinaryNode() = default;
  AsmBinaryNode(std::string op_type, std::shared_ptr<AsmOperandNode> src,
                std::shared_ptr<AsmOperandNode> dest)
      : op_type(std::move(op_type)), src(std::move(src)),
        dest(std::move(dest)) {}

  static bool classof(const AsmInstructionNode *node) {
    return dynamic_cast<const AsmBinaryNode *>(node) != nullptr;
  }
  void
  resolvePseudoRegisters(std::unordered_map<std::string, int> &pseudo_reg_map,
                         int &nxt_offset) override;
  void fixUpInstructions(
      std::vector<std::unique_ptr<AsmInstructionNode>> &instructions) override;
  void generateAsm(std::ostream &os) override;
};

class AsmCmpNode : public AsmInstructionNode {
public:
  // compare src2 - src1
  std::shared_ptr<AsmOperandNode> src1;
  std::shared_ptr<AsmOperandNode> src2;

  AsmCmpNode() = default;
  AsmCmpNode(std::shared_ptr<AsmOperandNode> src,
             std::shared_ptr<AsmOperandNode> dest)
      : src1(std::move(src)), src2(std::move(dest)) {}

  static bool classof(const AsmInstructionNode *node) {
    return dynamic_cast<const AsmCmpNode *>(node) != nullptr;
  }

  void
  resolvePseudoRegisters(std::unordered_map<std::string, int> &pseudo_reg_map,
                         int &nxt_offset) override;
  void fixUpInstructions(
      std::vector<std::unique_ptr<AsmInstructionNode>> &instructions) override;
  void generateAsm(std::ostream &os) override;
};

class AsmIdivNode : public AsmInstructionNode {
public:
  std::shared_ptr<AsmOperandNode> divisor; // a/b (divisor = b; dividend = a)

  AsmIdivNode() = default;
  explicit AsmIdivNode(std::shared_ptr<AsmOperandNode> divisor)
      : divisor(std::move(divisor)) {}

  static bool classof(const AsmInstructionNode *node) {
    return dynamic_cast<const AsmIdivNode *>(node) != nullptr;
  }
  void
  resolvePseudoRegisters(std::unordered_map<std::string, int> &pseudo_reg_map,
                         int &nxt_offset) override;
  void fixUpInstructions(
      std::vector<std::unique_ptr<AsmInstructionNode>> &instructions) override;
  void generateAsm(std::ostream &os) override;
};

class AsmCdqNode : public AsmInstructionNode {
public:
  static bool classof(const AsmInstructionNode *node) {
    return dynamic_cast<const AsmCdqNode *>(node) != nullptr;
  }
  void
  resolvePseudoRegisters(std::unordered_map<std::string, int> &pseudo_reg_map,
                         int &nxt_offset) override{}; // no-op
  void generateAsm(std::ostream &os) override;
};

class AsmJmpNode : public AsmInstructionNode {
public:
  std::string label;

  AsmJmpNode() = default;
  explicit AsmJmpNode(std::string label) : label(std::move(label)) {}

  static bool classof(const AsmInstructionNode *node) {
    return dynamic_cast<const AsmJmpNode *>(node) != nullptr;
  }
  void generateAsm(std::ostream &os) override;
};

class AsmJmpCCNode : public AsmInstructionNode {
public:
  std::string cond_code;
  std::string label;

  AsmJmpCCNode() = default;
  AsmJmpCCNode(std::string cond_code, std::string label)
      : cond_code(std::move(cond_code)), label(std::move(label)) {}

  static bool classof(const AsmInstructionNode *node) {
    return dynamic_cast<const AsmJmpCCNode *>(node) != nullptr;
  }
  void generateAsm(std::ostream &os) override;
};

class AsmSetCCNode : public AsmInstructionNode {
public:
  std::string cond_code;
  std::shared_ptr<AsmOperandNode> dest;

  AsmSetCCNode() = default;
  AsmSetCCNode(std::string cond_code, std::shared_ptr<AsmOperandNode> dest)
      : cond_code(std::move(cond_code)), dest(std::move(dest)) {}

  static bool classof(const AsmInstructionNode *node) {
    return dynamic_cast<const AsmSetCCNode *>(node) != nullptr;
  }

  void
  resolvePseudoRegisters(std::unordered_map<std::string, int> &pseudo_reg_map,
                         int &nxt_offset) override;
  void generateAsm(std::ostream &os) override;
};

class AsmLabelNode : public AsmInstructionNode {
public:
  std::string label;

  AsmLabelNode() = default;
  explicit AsmLabelNode(std::string label) : label(std::move(label)) {}

  static bool classof(const AsmInstructionNode *node) {
    return dynamic_cast<const AsmLabelNode *>(node) != nullptr;
  }

  void generateAsm(std::ostream &os) override;
};

class AsmAllocateStackNode : public AsmInstructionNode {
public:
  int stack_size = 0;

  AsmAllocateStackNode() = default;
  explicit AsmAllocateStackNode(int stack_size) : stack_size(stack_size) {}

  static bool classof(const AsmInstructionNode *node) {
    return dynamic_cast<const AsmAllocateStackNode *>(node) != nullptr;
  }
  void
  resolvePseudoRegisters(std::unordered_map<std::string, int> &pseudo_reg_map,
                         int &nxt_offset) override{}; // no-op
  void fixUpInstructions(std::vector<std::unique_ptr<AsmInstructionNode>>
                             &instructions) override; //{}; // no-op

  void generateAsm(std::ostream &os) override;
};

class AsmDeallocateStackNode : public AsmInstructionNode {
public:
  int stack_size = 0;

  AsmDeallocateStackNode() = default;
  explicit AsmDeallocateStackNode(int stack_size) : stack_size(stack_size) {}

  void generateAsm(std::ostream &os) override;
};

class AsmPushNode : public AsmInstructionNode {
public:
  std::shared_ptr<AsmOperandNode> operand;

  AsmPushNode() = default;
  explicit AsmPushNode(std::shared_ptr<AsmOperandNode> operand)
      : operand(std::move(operand)) {}

  void
  resolvePseudoRegisters(std::unordered_map<std::string, int> &pseudo_reg_map,
                         int &nxt_offset) override;
  void generateAsm(std::ostream &os) override;
};

class AsmCallNode : public AsmInstructionNode {
public:
  std::string func_name;

  AsmCallNode() = default;
  explicit AsmCallNode(std::string func_name)
      : func_name(std::move(func_name)) {}
  void generateAsm(std::ostream &os) override;
};

class AsmRetNode : public AsmInstructionNode {
public:
  static bool classof(const AsmInstructionNode *node) {
    return dynamic_cast<const AsmRetNode *>(node) != nullptr;
  }
  void
  resolvePseudoRegisters(std::unordered_map<std::string, int> &pseudo_reg_map,
                         int &nxt_offset) override{}; // no-op
  void fixUpInstructions(std::vector<std::unique_ptr<AsmInstructionNode>>
                             &instructions) override{}; // no-op

  void generateAsm(std::ostream &os) override;
};
// Instruction Nodes -- end

// Operand Nodes -- base
class AsmOperandNode : public AsmASTNode {
public:
  virtual ~AsmOperandNode() = default;
  static bool classof(const AsmOperandNode *node) { return node != nullptr; }

  virtual void generateAsm(std::ostream &os) = 0;
};

class AsmImmediateNode : public AsmOperandNode {
public:
  std::string value;

  AsmImmediateNode() = default;
  explicit AsmImmediateNode(std::string value) : value(std::move(value)) {}

  static bool classof(const AsmOperandNode *node) {
    return dynamic_cast<const AsmImmediateNode *>(node) != nullptr;
  }

  void generateAsm(std::ostream &os) override;
};

class AsmRegisterNode : public AsmOperandNode {
public:
  std::string name;

  AsmRegisterNode() = default;
  explicit AsmRegisterNode(std::string name) : name(std::move(name)) {}

  static bool classof(const AsmOperandNode *node) {
    return dynamic_cast<const AsmRegisterNode *>(node) != nullptr;
  }

  void generateAsm(std::ostream &os) override;
};

/// @brief The name psuedo register refered to operands which can potentially be
/// stored in registers But after adding the `static` keyword, the term pseudo
/// register is a bit of a misnomer since we we convert every IR variable to
/// `AsmPseudoNode`, even if they have static storage.
class AsmPseudoNode : public AsmOperandNode {
public:
  std::string identifier;

  AsmPseudoNode() = default;
  explicit AsmPseudoNode(std::string identifier)
      : identifier(std::move(identifier)) {}

  static bool classof(const AsmOperandNode *node) {
    return dynamic_cast<const AsmPseudoNode *>(node) != nullptr;
  }

  void generateAsm(std::ostream &os) override {
    throw std::runtime_error(
        "AsmPseudoNode should have been resolved before emission");
  };
};

/// Ultimately we need to replace every usage of AsmPseudoNode with AsmStackNode
class AsmStackNode : public AsmOperandNode {
public:
  // positive (for func args) or negative (for local vars)
  int offset = 0;

  AsmStackNode() = default;
  explicit AsmStackNode(int offset) : offset(offset) {}

  static bool classof(const AsmOperandNode *node) {
    return dynamic_cast<const AsmStackNode *>(node) != nullptr;
  }

  void generateAsm(std::ostream &os) override;
};

class AsmDataNode : public AsmOperandNode {
public:
  std::string identifier;

  AsmDataNode() = default;
  explicit AsmDataNode(std::string identifier)
      : identifier(std::move(identifier)) {}

  static bool classof(const AsmOperandNode *node) {
    return dynamic_cast<const AsmDataNode *>(node) != nullptr;
  }

  void generateAsm(std::ostream &os) override;
};
// Operand Nodes -- end
