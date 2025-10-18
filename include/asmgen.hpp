#pragma once

#include <memory>
#include <string>
#include <vector>

class AsmASTNode {
  public:
    virtual ~AsmASTNode() = default;
};

class AsmProgramNode;
class AsmFunctionNode;
class AsmIntructionNode;
class AsmMovNode;
class AsmRetNode;
class AsmOperandNode;

class AsmProgramNode : public AsmASTNode {
  public:
    std::unique_ptr<AsmFunctionNode> func;
};

class AsmFunctionNode : public AsmASTNode {
  public:
    std::string name;
    std::vector<std::unique_ptr<AsmIntructionNode>> instructions;
};

// base class for `ret` and `mov`
class AsmIntructionNode : public AsmASTNode {};

// `mov`
class AsmMovNode : public AsmIntructionNode {
  public:
    std::unique_ptr<AsmOperandNode> src;  // $2
    std::unique_ptr<AsmOperandNode> dest; // %eax
};

// `ret`
class AsmRetNode : public AsmIntructionNode {};

class AsmOperandNode : public AsmASTNode {
  public:
    virtual ~AsmOperandNode() = default;
};

class AsmImmediateNode : public AsmOperandNode {
  public:
    int value = 0;
};

class AsmRegisterNode : public AsmOperandNode {
  public:
    std::string name;
};
