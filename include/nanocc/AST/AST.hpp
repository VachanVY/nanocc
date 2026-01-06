#pragma once

#include <vector>
#include <memory>
#include <string>
#include <deque>

#include "nanocc/Lexer/Lexer.hpp"

class ASTNode;
class ProgramNode;

class DeclarationNode;
class VariableDeclNode;
class FunctionDeclNode;

// a BlockNode will contain multiple BlockItemNodes which are either statements or declarations
class BlockNode;
class BlockItemNode;

class StatementNode;
class ReturnNode;
class ExpressionNode;
class IfElseNode;
class CompoundNode; // Do we need this? Just holds a BlockNode... just use BlockNode directly?
class BreakNode;
class ContinueNode;
class WhileNode;
class DoWhileNode;
class ForNode;
class NullNode;

// Derived from ASTNode
class ForInitNode; // Helper for init in ForNode | Can be declaration, expression or nothing

class ExprNode;
class ExprFactorNode; // derived from ExprNode // helper class for ExprNode
class VarNode;
class ConstantNode;
class UnaryNode;
class BinaryNode;
class AssignmentNode;
class ConditionalNode;
class FunctionCallNode;

class IdentifierNode; // Do we need this? Just a string would do?

/// @brief parse function for every Node type derived from this class
class ASTNode {
  public:
    virtual ~ASTNode() = default;
    virtual void dump(int indent = 0) const = 0;
};

class ProgramNode : public ASTNode {
  public:
    std::vector<std::unique_ptr<FunctionDeclNode>> functions;

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
    
};

class DeclarationNode : public ASTNode {
  public:
    std::unique_ptr<FunctionDeclNode> func;
    std::unique_ptr<VariableDeclNode> var;

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;

};

// int "name"; | int "name" = <expr>;
class VariableDeclNode : public ASTNode {
  public:
    std::unique_ptr<IdentifierNode> var_identifier;
    std::unique_ptr<ExprNode> init_expr; // OPTIONAL

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
};

class FunctionDeclNode : public ASTNode {
  public:
    std::unique_ptr<IdentifierNode> func_name; // function name
    std::vector<std::unique_ptr<IdentifierNode>> parameters;
    // OPTIONAL(body): present for function definitions; absent for function declarations;
    std::unique_ptr<BlockNode> body;

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
};

class BlockNode : public ASTNode {
  public:
    std::vector<std::unique_ptr<BlockItemNode>> block_items;

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
};

class BlockItemNode : public ASTNode {
  public:
    std::unique_ptr<StatementNode> statement;
    std::unique_ptr<DeclarationNode> declaration;

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
};

class StatementNode : public ASTNode {
  public:
    std::unique_ptr<ReturnNode> return_stmt;
    std::unique_ptr<ExpressionNode> expression_stmt;
    std::unique_ptr<IfElseNode> ifelse_stmt;
    std::unique_ptr<CompoundNode> compound_stmt;
    std::unique_ptr<BreakNode> break_stmt;
    std::unique_ptr<ContinueNode> continue_stmt;
    std::unique_ptr<WhileNode> while_stmt;
    std::unique_ptr<DoWhileNode> dowhile_stmt;
    std::unique_ptr<ForNode> for_stmt;
    std::unique_ptr<NullNode> null_stmt;

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
};

class ReturnNode : public StatementNode {
  public:
    std::unique_ptr<ExprNode> ret_expr;

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
};

// for non-return statements
class ExpressionNode : public StatementNode {
  public:
    std::unique_ptr<ExprNode> expr;

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
};

class IfElseNode : public StatementNode {
  public:
    std::unique_ptr<ExprNode> condition;
    std::unique_ptr<StatementNode> if_block;
    std::unique_ptr<StatementNode> else_block; // OPTIONAL

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
};

class CompoundNode : public StatementNode {
  public:
    std::unique_ptr<BlockNode> block;

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
};

// ✶✶✶ for Loop Related Nodes
// no labels during parsing; replace with actual labels
// in loop labelling phase in semantic analysis (checker.cpp)

class BreakNode : public StatementNode {
  public:
    std::unique_ptr<IdentifierNode> label; // ✶✶✶

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
};

class ContinueNode : public StatementNode {
  public:
    std::unique_ptr<IdentifierNode> label; // ✶✶✶

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
};

class WhileNode : public StatementNode {
  public:
    std::unique_ptr<ExprNode> condition;
    std::unique_ptr<StatementNode> body;

    std::unique_ptr<IdentifierNode> label; // ✶✶✶

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
};

class DoWhileNode : public StatementNode {
  public:
    std::unique_ptr<StatementNode> body;
    std::unique_ptr<ExprNode> condition;

    std::unique_ptr<IdentifierNode> label; // ✶✶✶

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
};

class ForNode : public StatementNode {
  public:
    std::unique_ptr<ForInitNode> init;   // <declaration> || <expr> ; || ;
    std::unique_ptr<ExprNode> condition; // OPTIONAL(expr)
    std::unique_ptr<ExprNode> post;      // OPTIONAL(expr)
    std::unique_ptr<StatementNode> body; // loop body

    std::unique_ptr<IdentifierNode> label; // ✶✶✶

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
};

class ForInitNode : public ASTNode {
  public:
    std::unique_ptr<VariableDeclNode> declaration; // OPTIONAL
    std::unique_ptr<ExprNode> init_expr;           // OPTIONAL

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
};

// for null statements (i.e., just a semicolon)
class NullNode : public StatementNode {
  public:
    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
};

class ExprNode : public ASTNode {
  public:
    // <exp> = <factor> | <expr> <binary> <expr>
    // <exp> is of the form <factor> ( <binary> <expr> )*
    std::unique_ptr<ExprFactorNode> left_exprf;

    void parse(std::deque<Token>& tokens, size_t& pos, int min_precedence = 0);
    void dump(int indent = 0) const override;
};

class ExprFactorNode : public ExprNode {
    /*everything which <factor> can parse has higher precedence than what <expr> can parse;
      eg. !a + b is parsed as [!a] + b not ![a + b]
      we keep function calls here so that the below doesn't happen:
      eg. b(x) + c is parsed as [[b(x)] + c] not [b][(x) + c]
    */
  public:
    std::unique_ptr<ConstantNode> constant;      // <int>: a constant integer
    std::unique_ptr<VarNode> var_identifier;     // <identifier>
    std::unique_ptr<UnaryNode> unary;            // <unary> <factor>
    std::unique_ptr<ExprNode> expr;              // "(" <expr> ")"
    std::unique_ptr<FunctionCallNode> func_call; // <identifier> "(" [ <arg_list> ] ")"

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
};

class ConstantNode : public ExprFactorNode {
  public:
    std::string val;

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
};

class VarNode : public ExprFactorNode {
  public:
    std::unique_ptr<IdentifierNode> var_name;

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
    static bool classof(const ExprFactorNode* u) {
        return dynamic_cast<const VarNode*>(u) != nullptr;
    }
};

class UnaryNode : public ExprFactorNode {
  public:
    std::string op_type;
    std::unique_ptr<ExprFactorNode> operand;

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
};

class BinaryNode : public ExprFactorNode {
  public:
    std::string op_type;
    std::unique_ptr<ExprNode> left_expr;
    std::unique_ptr<ExprNode> right_expr;

    BinaryNode() = default;
    BinaryNode(std::string op, std::unique_ptr<ExprNode> left, std::unique_ptr<ExprNode> right)
        : op_type(std::move(op)), left_expr(std::move(left)), right_expr(std::move(right)) {}

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
    static bool classof(const ExprFactorNode* u) {
        return dynamic_cast<const BinaryNode*>(u) != nullptr;
    }
};

class AssignmentNode : public ExprFactorNode {
  public:
    std::unique_ptr<ExprNode> left_expr;
    std::unique_ptr<ExprNode> right_expr;

    AssignmentNode() = default;
    AssignmentNode(std::unique_ptr<ExprNode> left, std::unique_ptr<ExprNode> right)
        : left_expr(std::move(left)), right_expr(std::move(right)) {}

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
    static bool classof(const ExprFactorNode* u) {
        return dynamic_cast<const AssignmentNode*>(u) != nullptr;
    }
};

class ConditionalNode : public ExprFactorNode {
  public:
    std::unique_ptr<ExprNode> condition;
    std::unique_ptr<ExprNode> true_expr;
    std::unique_ptr<ExprNode> false_expr;

    ConditionalNode() = default;
    ConditionalNode(std::unique_ptr<ExprNode> cond, std::unique_ptr<ExprNode> t_expr,
                    std::unique_ptr<ExprNode> f_expr)
        : condition(std::move(cond)), true_expr(std::move(t_expr)), false_expr(std::move(f_expr)) {}

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
    static bool classof(const ExprFactorNode* u) {
        return dynamic_cast<const ConditionalNode*>(u) != nullptr;
    }
};

class FunctionCallNode : public ExprFactorNode {
  public:
    std::unique_ptr<IdentifierNode> func_identifier;
    std::vector<std::unique_ptr<ExprNode>> arguments;

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
    static bool classof(const ExprFactorNode* u) {
        return dynamic_cast<const FunctionCallNode*>(u) != nullptr;
    }
};

class IdentifierNode : public ASTNode {
  public:
    std::string name;

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override { dump(indent, true); };
    void dump(int indent, bool new_line) const;
};

