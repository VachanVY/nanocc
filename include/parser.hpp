#pragma once

#include <deque>
#include <memory>

#include "lexer.hpp"

// Forward declarations for AST node classes
class ASTNode;
class ProgramNode;
class FunctionNode;
class StatementNode;
class ExprNode;
class IdentifierNode;
class IntNode;

class Parser {
  private:
    /* data */
  public:
    Parser(/* args */);
    size_t pos = 0;
};

/// @brief parse function for every Node type derived from this class
class ASTNode {
  public:
    virtual ~ASTNode() = default;
    virtual void dump(int indent = 0) const = 0;
};

class ProgramNode : public ASTNode {
  public:
    std::unique_ptr<FunctionNode> func;

    void parse(std::deque<Token> &tokens, size_t &pos);
    void dump(int indent = 0) const override;
};

class FunctionNode : public ASTNode {
  public:
    std::unique_ptr<IdentifierNode> var;
    std::unique_ptr<StatementNode> statement;

    void parse(std::deque<Token> &tokens, size_t &pos);
    void dump(int indent = 0) const override;
};

class StatementNode : public ASTNode {
  public:
    std::unique_ptr<ExprNode> expr;

    void parse(std::deque<Token> &tokens, size_t &pos);
    void dump(int indent = 0) const override;
};

class ExprNode : public ASTNode {
  public:
    std::unique_ptr<IntNode> integer;
    void parse(std::deque<Token> &tokens, size_t &pos);
    void dump(int indent = 0) const override;
};

class IdentifierNode : public ASTNode {
  public:
    std::string name;
    void parse(std::deque<Token> &tokens, size_t &pos);
    void dump(int indent = 0) const override;
};

class IntNode : public ASTNode {
  public:
    int val;
    void parse(std::deque<Token> &tokens, size_t &pos);
    void dump(int indent = 0) const override;
};
