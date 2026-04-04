#pragma once

#include <list>
#include <memory>
#include <string>
#include <vector>

#include "nanocc/AST/AST.hpp"

class IRNode {
public:
  virtual ~IRNode() = default;
};

class IRProgramNode;

class IRTopLevelNode;
class IRFunctionNode;
class IRStaticVarNode;

// base class
class IRInstructionNode;
class IRRetNode;
class IRUnaryNode;
class IRBinaryNode;
class IRCopyNode;
class IRJumpNode;
class IRJumpIfZeroNode;
class IRJumpIfNotZeroNode;
class IRLabelNode;
class IRFunctionCallNode;

// base class; use `shared_ptr` for this and its derived classes
class IRValNode;
class IRConstNode;
class IRVariableNode;

class IRProgramNode : public IRNode {
public:
  std::vector<std::unique_ptr<IRTopLevelNode>> topLevel;

  IRProgramNode() = default;
  explicit IRProgramNode(std::vector<std::unique_ptr<IRTopLevelNode>> topLvl)
      : topLevel(std::move(topLvl)) {}
};

class IRTopLevelNode : public IRNode {
public:
  virtual ~IRTopLevelNode() = default;
};

class IRFunctionNode : public IRTopLevelNode {
public:
  std::string funcName;
  bool global;
  std::vector<std::string> parameters;
  std::list<std::unique_ptr<IRInstructionNode>> IRInstructions;

  IRFunctionNode() = default;
  virtual ~IRFunctionNode() = default;
  IRFunctionNode(std::string name, bool global,
                 std::list<std::unique_ptr<IRInstructionNode>> instruction_list)
      : funcName(std::move(name)), global(global),
        IRInstructions(std::move(instruction_list)) {}

  static bool classof(const IRTopLevelNode* node) {
    return dynamic_cast<const IRFunctionNode*>(node) != nullptr;
  }
};

class IRStaticVarNode : public IRTopLevelNode {
public:
  std::unique_ptr<IdentifierNode> varName;
  bool global;
  std::string init;

  IRStaticVarNode() = default;
  virtual ~IRStaticVarNode() = default;
  IRStaticVarNode(std::unique_ptr<IdentifierNode> name, bool global,
                  std::string init)
      : varName(std::move(name)), global(global), init(std::move(init)) {}

  static bool classof(const IRTopLevelNode* node) {
    return dynamic_cast<const IRStaticVarNode*>(node) != nullptr;
  }
};

class IRInstructionNode : public IRNode {
public:
  virtual ~IRInstructionNode() = default;
  /// @brief Used in basic block construction for IR Optimization
  /// @return boolean true/false
  virtual bool isTerminator() const { return false; }
};

class IRRetNode : public IRInstructionNode {
public:
  std::shared_ptr<IRValNode> retValue;

  IRRetNode() = default;
  explicit IRRetNode(std::shared_ptr<IRValNode> retVal)
      : retValue(std::move(retVal)) {}

  static bool classof(const IRInstructionNode* u) {
    return dynamic_cast<const IRRetNode*>(u) != nullptr;
  }
  bool isTerminator() const override { return true; }
};

class IRUnaryNode : public IRInstructionNode {
public:
  TokenType opType;
  std::shared_ptr<IRValNode> valSrc;
  std::shared_ptr<IRValNode> valDest;

  IRUnaryNode() = default;
  IRUnaryNode(TokenType op, std::shared_ptr<IRValNode> src,
              std::shared_ptr<IRValNode> dest)
      : opType(std::move(op)), valSrc(std::move(src)),
        valDest(std::move(dest)) {}

  static bool classof(const IRInstructionNode* u) {
    return dynamic_cast<const IRUnaryNode*>(u) != nullptr;
  }
};

class IRBinaryNode : public IRInstructionNode {
public:
  TokenType opType;
  std::shared_ptr<IRValNode> valSrcL;
  std::shared_ptr<IRValNode> valSrcR;
  std::shared_ptr<IRValNode> valDest;

  IRBinaryNode() = default;
  IRBinaryNode(TokenType op, std::shared_ptr<IRValNode> srcL,
               std::shared_ptr<IRValNode> srcR, std::shared_ptr<IRValNode> dest)
      : opType(std::move(op)), valSrcL(std::move(srcL)),
        valSrcR(std::move(srcR)), valDest(std::move(dest)) {}

  static bool classof(const IRInstructionNode* u) {
    return dynamic_cast<const IRBinaryNode*>(u) != nullptr;
  }
};

/// @brief val_src => val_dest
class IRCopyNode : public IRInstructionNode {
public:
  std::shared_ptr<IRValNode> ValSrc;
  std::shared_ptr<IRValNode> ValDest;

  IRCopyNode() = default;
  IRCopyNode(std::shared_ptr<IRValNode> src, std::shared_ptr<IRValNode> dest)
      : ValSrc(std::move(src)), ValDest(std::move(dest)) {}

  static bool classof(const IRInstructionNode* u) {
    return dynamic_cast<const IRCopyNode*>(u) != nullptr;
  }
};

class IRJumpNode : public IRInstructionNode {
public:
  std::string labelName;

  IRJumpNode() = default;
  explicit IRJumpNode(std::string label) : labelName(std::move(label)) {}

  static bool classof(const IRInstructionNode* u) {
    return dynamic_cast<const IRJumpNode*>(u) != nullptr;
  }
  bool isTerminator() const override { return true; }
};

class IRJumpIfZeroNode : public IRInstructionNode {
public:
  std::shared_ptr<IRValNode> condition;
  std::string labelName;

  IRJumpIfZeroNode() = default;
  IRJumpIfZeroNode(std::shared_ptr<IRValNode> cond, std::string label)
      : condition(std::move(cond)), labelName(std::move(label)) {}

  static bool classof(const IRInstructionNode* u) {
    return dynamic_cast<const IRJumpIfZeroNode*>(u) != nullptr;
  }
  bool isTerminator() const override { return true; }
};

class IRJumpIfNotZeroNode : public IRInstructionNode {
public:
  std::shared_ptr<IRValNode> condition;
  std::string labelName;

  IRJumpIfNotZeroNode() = default;
  IRJumpIfNotZeroNode(std::shared_ptr<IRValNode> cond, std::string label)
      : condition(std::move(cond)), labelName(std::move(label)) {}

  static bool classof(const IRInstructionNode* u) {
    return dynamic_cast<const IRJumpIfNotZeroNode*>(u) != nullptr;
  }
  bool isTerminator() const override { return true; }
};

class IRLabelNode : public IRInstructionNode {
public:
  std::string labelName;

  IRLabelNode() = default;
  explicit IRLabelNode(std::string name) : labelName(std::move(name)) {}

  static bool classof(const IRInstructionNode* u) {
    return dynamic_cast<const IRLabelNode*>(u) != nullptr;
  }
};

class IRFunctionCallNode : public IRInstructionNode {
public:
  std::string funcName;
  std::vector<std::shared_ptr<IRValNode>> arguments;
  std::shared_ptr<IRValNode> returnDest;

  IRFunctionCallNode() = default;
  explicit IRFunctionCallNode(std::string name,
                              std::vector<std::shared_ptr<IRValNode>> args,
                              std::shared_ptr<IRValNode> ret_dest)
      : funcName(std::move(name)), arguments(std::move(args)),
        returnDest(std::move(ret_dest)) {}

  static bool classof(const IRInstructionNode* u) {
    return dynamic_cast<const IRFunctionCallNode*>(u) != nullptr;
  }
};

class IRValNode : public IRNode {
public:
  virtual ~IRValNode() = default;
};

class IRConstNode : public IRValNode {
public:
  int IntVal;

  IRConstNode() = default;
  explicit IRConstNode(int value) : IntVal(value) {}

  static bool classof(const IRValNode* v) {
    return dynamic_cast<const IRConstNode*>(v) != nullptr;
  }
};

class IRVariableNode : public IRValNode {
public:
  std::string varName;

  IRVariableNode() = default;
  explicit IRVariableNode(std::string name) : varName(std::move(name)) {}

  static bool classof(const IRValNode* v) {
    return dynamic_cast<const IRVariableNode*>(v) != nullptr;
  }
};

namespace nanocc {
std::unique_ptr<IRProgramNode> generateIntermRepr(const ProgramNode& ast,
                                                  bool debug = false);
} // namespace nanocc
