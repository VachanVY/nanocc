#pragma once

#include <deque>
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "asmgen.hpp"
#include "irgen.hpp"
#include "lexer.hpp"
#include "checker.hpp"

class ASTNode;
class ProgramNode;
class FunctionNode;

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

class DeclarationNode;

class ExprNode;
class ExprFactorNode; // derived from ExprNode // helper class for ExprNode
class VarNode;
class ConstantNode;
class UnaryNode;
class BinaryNode;
class AssignmentNode;
class ConditionalNode;

class IdentifierNode; // Do we need this? Just a string would do?

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
    void resolveTypes(SymbolTable& sym_table);
    // std::unique_ptr<IRProgramNode> emit_ir();
};

class FunctionNode : public ASTNode {
  public:
    std::unique_ptr<IdentifierNode> var_identifier; // function name
    std::unique_ptr<BlockNode> body;                // list of statements/declarations

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
    void resolveTypes(SymbolTable& sym_table);
    // std::unique_ptr<IRFunctionNode> emit_ir();
};

class BlockNode : public ASTNode {
  public:
    std::vector<std::unique_ptr<BlockItemNode>> block_items;

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
    void resolveTypes(SymbolTable& sym_table);
    // std::vector<std::unique_ptr<IRInstructionNode>> emit_ir();
};

class BlockItemNode : public ASTNode {
  public:
    std::unique_ptr<StatementNode> statement;
    std::unique_ptr<DeclarationNode> declaration;

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
    void resolveTypes(SymbolTable& sym_table);
    // std::vector<std::unique_ptr<IRInstructionNode>> emit_ir();
};

class DeclarationNode : public ASTNode {
  public:
    std::unique_ptr<IdentifierNode> var_identifier;
    std::unique_ptr<ExprNode> init_expr; // OPTIONAL

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
    void resolveTypes(SymbolTable& sym_table);
    // std::vector<std::unique_ptr<IRInstructionNode>> emit_ir();
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
    void resolveTypes(SymbolTable& sym_table);
    // std::vector<std::unique_ptr<IRInstructionNode>> emit_ir();
};

class ReturnNode : public StatementNode {
  public:
    std::unique_ptr<ExprNode> ret_expr;

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
    void resolveTypes(SymbolTable& sym_table);
    // std::vector<std::unique_ptr<IRInstructionNode>> emit_ir();
};

// for non-return statements
class ExpressionNode : public StatementNode {
  public:
    std::unique_ptr<ExprNode> expr;

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
    void resolveTypes(SymbolTable& sym_table);
    // std::vector<std::unique_ptr<IRInstructionNode>> emit_ir();
};

class IfElseNode : public StatementNode {
  public:
    std::unique_ptr<ExprNode> condition;
    std::unique_ptr<StatementNode> if_block;
    std::unique_ptr<StatementNode> else_block; // OPTIONAL

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
    void resolveTypes(SymbolTable& sym_table);
    // std::vector<std::unique_ptr<IRInstructionNode>> emit_ir();
};

class CompoundNode : public StatementNode {
  public:
    std::unique_ptr<BlockNode> block;

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
    void resolveTypes(SymbolTable& sym_table);
    // std::vector<std::unique_ptr<IRInstructionNode>> emit_ir();
};

// ✶✶✶ for Loop Related Nodes
// dummy labels during parsing; replace with actual labels
// in loop labelling phase in semantic analysis (checker.cpp)

class BreakNode : public StatementNode {
  public:
    std::unique_ptr<IdentifierNode> label; // ✶✶✶

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
    void resolveTypes(SymbolTable& sym_table);
    // std::vector<std::unique_ptr<IRInstructionNode>> emit_ir();
};

class ContinueNode : public StatementNode {
  public:
    std::unique_ptr<IdentifierNode> label; // ✶✶✶

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
    void resolveTypes(SymbolTable& sym_table);
    // std::vector<std::unique_ptr<IRInstructionNode>> emit_ir();
};

class WhileNode : public StatementNode {
  public:
    std::unique_ptr<ExprNode> condition;
    std::unique_ptr<StatementNode> body;

    std::unique_ptr<IdentifierNode> label; // ✶✶✶

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
    void resolveTypes(SymbolTable& sym_table);
    // std::vector<std::unique_ptr<IRInstructionNode>> emit_ir();
};

class DoWhileNode : public StatementNode {
  public:
    std::unique_ptr<StatementNode> body;
    std::unique_ptr<ExprNode> condition;

    std::unique_ptr<IdentifierNode> label; // ✶✶✶

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
    void resolveTypes(SymbolTable& sym_table);
    // std::vector<std::unique_ptr<IRInstructionNode>> emit_ir();
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
    void resolveTypes(SymbolTable& sym_table);
    // std::vector<std::unique_ptr<IRInstructionNode>> emit_ir();
};

class ForInitNode : public ASTNode {
  public:
    std::unique_ptr<DeclarationNode> declaration; // OPTIONAL
    std::unique_ptr<ExprNode> init_expr;          // OPTIONAL

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
    /* void resolveTypes(SymbolTable& sym_table);
    std::vector<std::unique_ptr<IRInstructionNode>> emit_ir() */
    ;
};

// for null statements (i.e., just a semicolon)
class NullNode : public StatementNode {
  public:
    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
    void resolveTypes(SymbolTable& sym_table);
    // std::vector<std::unique_ptr<IRInstructionNode>> emit_ir();
};

class ExprNode : public ASTNode {
  public:
    // <exp> = <factor> | <expr> <binary> <expr>
    // <exp> is of the form <factor> ( <binary> <expr> )*
    std::unique_ptr<ExprFactorNode> left_exprf;

    void parse(std::deque<Token>& tokens, size_t& pos, int min_precedence = 0);
    void dump(int indent = 0) const override;
    void resolveTypes(SymbolTable& sym_table);
    // virtual std::shared_ptr<IRValNode> // runtime polymorphism for ExprFactorNode
    // emit_ir(std::vector<std::unique_ptr<IRInstructionNode>>& instructions);
};

class ExprFactorNode : public ExprNode {
  public:
    std::unique_ptr<ConstantNode> constant;  // <int>: a constant integer
    std::unique_ptr<VarNode> var_identifier; // <identifier>
    std::unique_ptr<UnaryNode> unary;        // <unary> <factor>
    std::unique_ptr<ExprNode> expr;          // "(" <expr> ")"

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
    /// @brief runtime polymorphism for BinaryNode, AssignmentNode, ConditionalNode
    virtual void resolveTypes(SymbolTable& sym_table);
    // runtime polymorphism for BinaryNode, AssignmentNode,
    // virtual std::shared_ptr<IRValNode>
    // emit_ir(std::vector<std::unique_ptr<IRInstructionNode>>& instructions);
};

class ConstantNode : public ExprFactorNode {
  public:
    std::string val;

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
    void resolveTypes(SymbolTable& sym_table) override {};
};

class VarNode : public ExprFactorNode {
  public:
    std::unique_ptr<IdentifierNode> var_name;

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
    void resolveTypes(SymbolTable& sym_table) override;
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
    void resolveTypes(SymbolTable& sym_table) override;
};

class BinaryNode : public ExprFactorNode {
  public:
    std::string op_type;
    std::unique_ptr<ExprNode> left_expr;  // can also be ExprFactorNode
    std::unique_ptr<ExprNode> right_expr; // can also be ExprFactorNode... ig...

    BinaryNode() = default;
    BinaryNode(std::string op, std::unique_ptr<ExprNode> left, std::unique_ptr<ExprNode> right)
        : op_type(std::move(op)), left_expr(std::move(left)), right_expr(std::move(right)) {}

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override;
    void resolveTypes(SymbolTable& sym_table) override;
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
    void resolveTypes(SymbolTable& sym_table) override;
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
    void resolveTypes(SymbolTable& sym_table) override;
    static bool classof(const ExprFactorNode* u) {
        return dynamic_cast<const ConditionalNode*>(u) != nullptr;
    }
};

class IdentifierNode : public ASTNode {
  public:
    std::string name;

    void parse(std::deque<Token>& tokens, size_t& pos);
    void dump(int indent = 0) const override { dump(indent, true); };
    void dump(int indent, bool new_line) const;
};

std::unique_ptr<ProgramNode> parse(std::deque<Token>& tokens);
