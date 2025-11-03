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

class AsmFunctionNode;

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
class AsmRetNode;

class AsmOperandNode; // base class // always use via `shared_ptr`
class AsmImmediateNode;
class AsmRegisterNode;
class AsmPseudoNode;
class AsmStackNode;

class AsmProgramNode : public AsmASTNode {
  public:
    std::unique_ptr<AsmFunctionNode> func;
    static bool classof(const AsmASTNode* node) {
        return dynamic_cast<const AsmProgramNode*>(node) != nullptr;
    }
    void resolvePseudoRegisters(std::unordered_map<std::string, int>& pseudo_reg_map,
                                int& nxt_offset);
    void fixUpInstructions(const int& stack_size);

    void generateAsm(std::ostream& os);
};

class AsmFunctionNode : public AsmASTNode {
  public:
    std::string name;
    std::vector<std::unique_ptr<AsmInstructionNode>> instructions;

    AsmFunctionNode() = default;
    explicit AsmFunctionNode(std::string name) : name(std::move(name)) {}

    static bool classof(const AsmASTNode* node) {
        return dynamic_cast<const AsmFunctionNode*>(node) != nullptr;
    }

    void resolvePseudoRegisters(std::unordered_map<std::string, int>& pseudo_reg_map,
                                int& nxt_offset);
    void fixUpInstructions(const int& stack_size);

    void generateAsm(std::ostream& os);
};

// Instruction Nodes -- base
class AsmInstructionNode : public AsmASTNode {
  public:
    static bool classof(const AsmASTNode* node) {
        return dynamic_cast<const AsmInstructionNode*>(node) != nullptr;
    }
    virtual void resolvePseudoRegisters(std::unordered_map<std::string, int>& pseudo_reg_map,
                                        int& nxt_offset) {}; // default no-op
    virtual void fixUpInstructions(std::vector<std::unique_ptr<AsmInstructionNode>>& instructions) {
    }; // default no-op

    virtual void generateAsm(std::ostream& os) = 0;
};

class AsmMovNode : public AsmInstructionNode {
  public:
    std::shared_ptr<AsmOperandNode> src;
    std::shared_ptr<AsmOperandNode> dest;

    AsmMovNode() = default;
    AsmMovNode(std::shared_ptr<AsmOperandNode> src, std::shared_ptr<AsmOperandNode> dest)
        : src(std::move(src)), dest(std::move(dest)) {}

    static bool classof(const AsmInstructionNode* node) {
        return dynamic_cast<const AsmMovNode*>(node) != nullptr;
    }

    void resolvePseudoRegisters(std::unordered_map<std::string, int>& pseudo_reg_map,
                                int& nxt_offset) override;
    void fixUpInstructions(std::vector<std::unique_ptr<AsmInstructionNode>>& instructions) override;

    void generateAsm(std::ostream& os) override;
};

class AsmUnaryNode : public AsmInstructionNode {
  public:
    std::string op_type;
    std::shared_ptr<AsmOperandNode> operand;

    AsmUnaryNode() = default;
    AsmUnaryNode(std::string op_type, std::shared_ptr<AsmOperandNode> operand)
        : op_type(std::move(op_type)), operand(std::move(operand)) {}

    static bool classof(const AsmInstructionNode* node) {
        return dynamic_cast<const AsmUnaryNode*>(node) != nullptr;
    }

    void resolvePseudoRegisters(std::unordered_map<std::string, int>& pseudo_reg_map,
                                int& nxt_offset) override;
    void generateAsm(std::ostream& os) override;
};

class AsmBinaryNode : public AsmInstructionNode {
  public:
    std::string op_type;
    std::shared_ptr<AsmOperandNode> src;
    std::shared_ptr<AsmOperandNode> dest;

    AsmBinaryNode() = default;
    AsmBinaryNode(std::string op_type, std::shared_ptr<AsmOperandNode> src,
                  std::shared_ptr<AsmOperandNode> dest)
        : op_type(std::move(op_type)), src(std::move(src)), dest(std::move(dest)) {}

    static bool classof(const AsmInstructionNode* node) {
        return dynamic_cast<const AsmBinaryNode*>(node) != nullptr;
    }
    void resolvePseudoRegisters(std::unordered_map<std::string, int>& pseudo_reg_map,
                                int& nxt_offset) override;
    void fixUpInstructions(std::vector<std::unique_ptr<AsmInstructionNode>>& instructions) override;
    void generateAsm(std::ostream& os) override;
};

class AsmCmpNode : public AsmInstructionNode {
  public:
    // compare src2 - src1
    std::shared_ptr<AsmOperandNode> src1;
    std::shared_ptr<AsmOperandNode> src2;

    AsmCmpNode() = default;
    AsmCmpNode(std::shared_ptr<AsmOperandNode> src, std::shared_ptr<AsmOperandNode> dest)
        : src1(std::move(src)), src2(std::move(dest)) {}

    static bool classof(const AsmInstructionNode* node) {
        return dynamic_cast<const AsmCmpNode*>(node) != nullptr;
    }

    void resolvePseudoRegisters(std::unordered_map<std::string, int>& pseudo_reg_map,
                                int& nxt_offset) override;
    void fixUpInstructions(std::vector<std::unique_ptr<AsmInstructionNode>>& instructions) override;
    void generateAsm(std::ostream& os) override;
};

class AsmIdivNode : public AsmInstructionNode {
  public:
    std::shared_ptr<AsmOperandNode> divisor; // a/b (divisor = b; dividend = a)

    AsmIdivNode() = default;
    explicit AsmIdivNode(std::shared_ptr<AsmOperandNode> divisor) : divisor(std::move(divisor)) {}

    static bool classof(const AsmInstructionNode* node) {
        return dynamic_cast<const AsmIdivNode*>(node) != nullptr;
    }
    void resolvePseudoRegisters(std::unordered_map<std::string, int>& pseudo_reg_map,
                                int& nxt_offset) override;
    void fixUpInstructions(std::vector<std::unique_ptr<AsmInstructionNode>>& instructions) override;
    void generateAsm(std::ostream& os) override;
};

class AsmCdqNode : public AsmInstructionNode {
  public:
    static bool classof(const AsmInstructionNode* node) {
        return dynamic_cast<const AsmCdqNode*>(node) != nullptr;
    }
    void resolvePseudoRegisters(std::unordered_map<std::string, int>& pseudo_reg_map,
                                int& nxt_offset) override {}; // no-op
    void generateAsm(std::ostream& os) override;
};

class AsmJmpNode : public AsmInstructionNode {
  public:
    std::string label;

    AsmJmpNode() = default;
    explicit AsmJmpNode(std::string label) : label(std::move(label)) {}

    static bool classof(const AsmInstructionNode* node) {
        return dynamic_cast<const AsmJmpNode*>(node) != nullptr;
    }
    void generateAsm(std::ostream& os) override;
};

class AsmJmpCCNode : public AsmInstructionNode {
  public:
    std::string cond_code;
    std::string label;

    AsmJmpCCNode() = default;
    AsmJmpCCNode(std::string cond_code, std::string label)
        : cond_code(std::move(cond_code)), label(std::move(label)) {}

    static bool classof(const AsmInstructionNode* node) {
        return dynamic_cast<const AsmJmpCCNode*>(node) != nullptr;
    }
    void generateAsm(std::ostream& os) override;
};

class AsmSetCCNode : public AsmInstructionNode {
  public:
    std::string cond_code;
    std::shared_ptr<AsmOperandNode> dest;

    AsmSetCCNode() = default;
    AsmSetCCNode(std::string cond_code, std::shared_ptr<AsmOperandNode> dest)
        : cond_code(std::move(cond_code)), dest(std::move(dest)) {}

    static bool classof(const AsmInstructionNode* node) {
        return dynamic_cast<const AsmSetCCNode*>(node) != nullptr;
    }

    void resolvePseudoRegisters(std::unordered_map<std::string, int>& pseudo_reg_map,
                                int& nxt_offset) override;
    void generateAsm(std::ostream& os) override;
};

class AsmLabelNode : public AsmInstructionNode {
  public:
    std::string label;

    AsmLabelNode() = default;
    explicit AsmLabelNode(std::string label) : label(std::move(label)) {}

    static bool classof(const AsmInstructionNode* node) {
        return dynamic_cast<const AsmLabelNode*>(node) != nullptr;
    }

    void generateAsm(std::ostream& os) override;
};

class AsmAllocateStackNode : public AsmInstructionNode {
  public:
    int stack_size = 0;

    AsmAllocateStackNode() = default;
    explicit AsmAllocateStackNode(int stack_size) : stack_size(stack_size) {}

    static bool classof(const AsmInstructionNode* node) {
        return dynamic_cast<const AsmAllocateStackNode*>(node) != nullptr;
    }
    void resolvePseudoRegisters(std::unordered_map<std::string, int>& pseudo_reg_map,
                                int& nxt_offset) override {}; // no-op
    void
    fixUpInstructions(std::vector<std::unique_ptr<AsmInstructionNode>>& instructions) override {
    }; // no-op

    void generateAsm(std::ostream& os) override;
};

class AsmRetNode : public AsmInstructionNode {
  public:
    static bool classof(const AsmInstructionNode* node) {
        return dynamic_cast<const AsmRetNode*>(node) != nullptr;
    }
    void resolvePseudoRegisters(std::unordered_map<std::string, int>& pseudo_reg_map,
                                int& nxt_offset) override {}; // no-op
    void
    fixUpInstructions(std::vector<std::unique_ptr<AsmInstructionNode>>& instructions) override {
    }; // no-op

    void generateAsm(std::ostream& os) override;
};
// Instruction Nodes -- end

// Operand Nodes -- base
class AsmOperandNode : public AsmASTNode {
  public:
    virtual ~AsmOperandNode() = default;
    static bool classof(const AsmOperandNode* node) { return node != nullptr; }

    virtual void generateAsm(std::ostream& os) = 0;
};

class AsmImmediateNode : public AsmOperandNode {
  public:
    std::string value;

    AsmImmediateNode() = default;
    explicit AsmImmediateNode(std::string value) : value(std::move(value)) {}

    static bool classof(const AsmOperandNode* node) {
        return dynamic_cast<const AsmImmediateNode*>(node) != nullptr;
    }

    void generateAsm(std::ostream& os) override;
};

class AsmRegisterNode : public AsmOperandNode {
  public:
    std::string name;

    AsmRegisterNode() = default;
    explicit AsmRegisterNode(std::string name) : name(std::move(name)) {}

    static bool classof(const AsmOperandNode* node) {
        return dynamic_cast<const AsmRegisterNode*>(node) != nullptr;
    }

    void generateAsm(std::ostream& os) override;
};

class AsmPseudoNode : public AsmOperandNode {
  public:
    std::string identifier;

    AsmPseudoNode() = default;
    explicit AsmPseudoNode(std::string identifier) : identifier(std::move(identifier)) {}

    static bool classof(const AsmOperandNode* node) {
        return dynamic_cast<const AsmPseudoNode*>(node) != nullptr;
    }

    void generateAsm(std::ostream& os) override {
        throw std::runtime_error("AsmPseudoNode should have been resolved before emission");
    };
};

class AsmStackNode : public AsmOperandNode {
    /// Ultimately we need to replace every usage of AsmPseudoNode with
    /// AsmStackNode
  public:
    int offset = 0; // negate while emitting asm

    AsmStackNode() = default;
    explicit AsmStackNode(int offset) : offset(offset) {}

    static bool classof(const AsmOperandNode* node) {
        return dynamic_cast<const AsmStackNode*>(node) != nullptr;
    }

    void generateAsm(std::ostream& os) override;
};
// Operand Nodes -- end
