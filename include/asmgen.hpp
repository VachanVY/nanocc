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
class AsmAllocateStackNode;
class AsmRetNode;

class AsmOperandNode; // base class
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

    void emit_asm(std::ostream& os);
};

class AsmFunctionNode : public AsmASTNode {
  public:
    std::string name;
    std::vector<std::unique_ptr<AsmInstructionNode>> instructions;
    static bool classof(const AsmASTNode* node) {
        return dynamic_cast<const AsmFunctionNode*>(node) != nullptr;
    }

    void resolvePseudoRegisters(std::unordered_map<std::string, int>& pseudo_reg_map,
                                int& nxt_offset);
    void fixUpInstructions(const int& stack_size);

    void emit_asm(std::ostream& os);
};

// Instruction Nodes -- base
class AsmInstructionNode : public AsmASTNode {
  public:
    static bool classof(const AsmASTNode* node) {
        return dynamic_cast<const AsmInstructionNode*>(node) != nullptr;
    }
    virtual void resolvePseudoRegisters(std::unordered_map<std::string, int>& pseudo_reg_map,
                                        int& nxt_offset) = 0;
    virtual void
    fixUpInstructions(std::vector<std::unique_ptr<AsmInstructionNode>>& instructions) = 0;

    virtual void emit_asm(std::ostream& os) = 0;
};

class AsmMovNode : public AsmInstructionNode {
  public:
    std::shared_ptr<AsmOperandNode> src;
    std::shared_ptr<AsmOperandNode> dest;
    static bool classof(const AsmInstructionNode* node) {
        return dynamic_cast<const AsmMovNode*>(node) != nullptr;
    }

    void resolvePseudoRegisters(std::unordered_map<std::string, int>& pseudo_reg_map,
                                int& nxt_offset) override;
    void fixUpInstructions(std::vector<std::unique_ptr<AsmInstructionNode>>& instructions) override;

    void emit_asm(std::ostream& os) override;
};

class AsmUnaryNode : public AsmInstructionNode {
  public:
    std::string op_type;
    std::shared_ptr<AsmOperandNode> operand;
    static bool classof(const AsmInstructionNode* node) {
        return dynamic_cast<const AsmUnaryNode*>(node) != nullptr;
    }

    void resolvePseudoRegisters(std::unordered_map<std::string, int>& pseudo_reg_map,
                                int& nxt_offset) override;
    void fixUpInstructions(std::vector<std::unique_ptr<AsmInstructionNode>>& instructions)
        override{}; // no-op // NOTE(VachanVY): Can I use default behavior as no-op?

    void emit_asm(std::ostream& os) override;
};

class AsmAllocateStackNode : public AsmInstructionNode {
  public:
    int stack_size = 0;
    static bool classof(const AsmInstructionNode* node) {
        return dynamic_cast<const AsmAllocateStackNode*>(node) != nullptr;
    }
    void resolvePseudoRegisters(std::unordered_map<std::string, int>& pseudo_reg_map,
                                int& nxt_offset) override{}; // no-op
    void fixUpInstructions(
        std::vector<std::unique_ptr<AsmInstructionNode>>& instructions) override{}; // no-op

    void emit_asm(std::ostream& os) override;
};

class AsmRetNode : public AsmInstructionNode {
  public:
    static bool classof(const AsmInstructionNode* node) {
        return dynamic_cast<const AsmRetNode*>(node) != nullptr;
    }
    void resolvePseudoRegisters(std::unordered_map<std::string, int>& pseudo_reg_map,
                                int& nxt_offset) override{}; // no-op
    void fixUpInstructions(
        std::vector<std::unique_ptr<AsmInstructionNode>>& instructions) override{}; // no-op

    void emit_asm(std::ostream& os) override;
};
// Instruction Nodes -- end

// Operand Nodes -- base
class AsmOperandNode : public AsmASTNode {
  public:
    virtual ~AsmOperandNode() = default;
    static bool classof(const AsmOperandNode* node) { return node != nullptr; }

    virtual void emit_asm(std::ostream& os) = 0;
};

class AsmImmediateNode : public AsmOperandNode {
  public:
    int value = 0;
    static bool classof(const AsmOperandNode* node) {
        return dynamic_cast<const AsmImmediateNode*>(node) != nullptr;
    }

    void emit_asm(std::ostream& os) override;
};

class AsmRegisterNode : public AsmOperandNode {
  public:
    std::string name;
    static bool classof(const AsmOperandNode* node) {
        return dynamic_cast<const AsmRegisterNode*>(node) != nullptr;
    }

    void emit_asm(std::ostream& os) override;
};

class AsmPseudoNode : public AsmOperandNode {
  public:
    std::string identifier;
    static bool classof(const AsmOperandNode* node) {
        return dynamic_cast<const AsmPseudoNode*>(node) != nullptr;
    }

    void emit_asm(std::ostream& os) override {
        throw std::runtime_error("AsmPseudoNode should have been resolved before emission");
    };
};

class AsmStackNode : public AsmOperandNode {
    /// Ultimately we need to replace every usage of AsmPseudoNode with
    /// AsmStackNode
  public:
    int offset = 0; // negate while emitting asm
    static bool classof(const AsmOperandNode* node) {
        return dynamic_cast<const AsmStackNode*>(node) != nullptr;
    }

    void emit_asm(std::ostream& os) override;
};
// Operand Nodes -- end
