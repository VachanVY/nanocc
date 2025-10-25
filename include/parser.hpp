#pragma once

#include <deque>
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "asmgen.hpp"
#include "irgen.hpp"
#include "lexer.hpp"

class ASTNode;
class ProgramNode;
class FunctionNode;

class StatementNode;

class ExprNode;
class ConstantNode;
class UnaryNode;
class BinaryNode;
class IdentifierNode;

class ExprFactorNode;

/// @brief parse function for every Node type derived from this class
class ASTNode {
  public:
    virtual ~ASTNode() = default;
    virtual void dump(int indent = 0) const = 0;
};

class ProgramNode : public ASTNode {
  public:
    std::unique_ptr<FunctionNode> func;

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
    std::unique_ptr<IRProgramNode> emit_ir();
};

class FunctionNode : public ASTNode {
  public:
    std::unique_ptr<IdentifierNode> var_identifier; // function name
    std::unique_ptr<StatementNode> statement;       // statement inside function

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
    std::unique_ptr<IRFunctionNode> emit_ir();
};

class StatementNode : public ASTNode {
  public:
    std::unique_ptr<ExprNode> expr;

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
    std::vector<std::unique_ptr<IRInstructionNode>> emit_ir();
};

class ExprNode : public ASTNode {
  public:
    // delete these later
    // std::unique_ptr<ConstantNode> constant;
    // std::unique_ptr<UnaryNode> unary;
    // std::unique_ptr<ExprNode> expr;

    // <exp> = <factor> | <expr> <binary> <expr>
    // <exp> is of the form <factor> ( <binary> <expr> )*
    std::unique_ptr<ExprFactorNode> left_exprf;
    std::unique_ptr<BinaryNode> binop;
    std::unique_ptr<ExprNode> right_expr; // will be null since we move it to left_exprf, ig...

    void parse(std::deque<Token>& tokens, size_t& pos, int min_precedence = 0);
    void dump(int indent = 0) const override;
    virtual std::shared_ptr<IRValNode> // runtime polymorphism for ExprFactorNode
    emit_ir(std::vector<std::unique_ptr<IRInstructionNode>>& instructions);
};

class ExprFactorNode : public ExprNode {
  public:
    std::unique_ptr<ConstantNode> constant;
    std::unique_ptr<UnaryNode> unary;
    std::unique_ptr<ExprFactorNode> factor;
    // for parenthesized expressions. // will be null after it's parsed
    std::unique_ptr<ExprNode> expr;

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
    // runtime polymorphism for BinaryNode, (ConstantNode, UnaryNode, not sure about these...)
    virtual std::shared_ptr<IRValNode>
    emit_ir(std::vector<std::unique_ptr<IRInstructionNode>>& instructions);
};

class IdentifierNode : public ASTNode {
  public:
    std::string name;

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
};

class ConstantNode : public ExprFactorNode {
  public:
    std::string val;

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
};

class UnaryNode : public ExprFactorNode {
  public:
    std::string op_type;

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
};

class BinaryNode : public ExprFactorNode {
  public:
    std::string op_type;
    std::unique_ptr<ExprNode> left;  // can also be ExprFactorNode
    std::unique_ptr<ExprNode> right; // can also be ExprFactorNode... ig...

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
    static bool classof(const ExprFactorNode* u) {
        // BinaryNode is derived from ExprNode
        return dynamic_cast<const BinaryNode*>(u) != nullptr;
    }
};

std::unique_ptr<ProgramNode> parse(std::deque<Token>& tokens);
