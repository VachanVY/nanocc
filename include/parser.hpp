#pragma once

#include <deque>
#include <memory>
#include <vector>

#include "asmgen.hpp"
#include "lexer.hpp"

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
    std::unique_ptr<AsmProgramNode> parse_asm_ast();
};

class FunctionNode : public ASTNode {
  public:
    std::unique_ptr<IdentifierNode> var_identifier; // function name
    std::unique_ptr<StatementNode> statement;       // statement inside function

    void parse(std::deque<Token> &tokens, size_t &pos);
    void dump(int indent = 0) const override;
    std::unique_ptr<AsmFunctionNode> parse_asm_ast();
};

class StatementNode : public ASTNode {
  public:
    std::unique_ptr<ExprNode> expr;

    void parse(std::deque<Token> &tokens, size_t &pos);
    void dump(int indent = 0) const override;
    std::vector<std::unique_ptr<AsmIntructionNode>> parse_asm_ast();
};

class ExprNode : public ASTNode {
  public:
    std::unique_ptr<IntNode> integer;
    void parse(std::deque<Token> &tokens, size_t &pos);
    void dump(int indent = 0) const override;
    std::unique_ptr<AsmIntructionNode> parse_asm_ast();
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

std::unique_ptr<ProgramNode> parse(std::deque<Token> &tokens);
