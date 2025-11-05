#include <deque>
#include <memory>
#include <print>
#include <vector>

#include "lexer.hpp"
#include "parser.hpp"
#include "utils.hpp"

namespace { // some helper vars/functions
static const std::unordered_map<std::string, int> BINOP_PRECEDENCE = {
    {"*", 50},  {"/", 50},  {"%", 50},  {"+", 45},  {"-", 45}, {"<", 35}, {"<=", 35}, {">", 35},
    {">=", 35}, {"==", 30}, {"!=", 30}, {"&&", 10}, {"||", 5}, {"?", 3},  {"=", 1}};

constexpr bool isUnary(const std::string& op) {
    return op == "~" || op == "-" || op == "!";
    /* || op == "--"; */ // decrement only for lexing now
}

constexpr int getPrecedence(const std::string& op) {
    auto it = BINOP_PRECEDENCE.find(op);
    if (it == BINOP_PRECEDENCE.end())
        throw std::runtime_error("Invalid operator: " + op);
    return it->second;
}

constexpr bool isBinop(const std::string& op) { return BINOP_PRECEDENCE.contains(op); }
} // namespace

/* Dev Docs
=> what's the `parse` method for "X" doing?
    * checks for syntax correctness and creates node for each non-terminal
variable in "X"

=> when ever we find a new a non-terminal variable in "X" create a node pointer
for it and parse
*/

void expect(const std::deque<Token>& tokens, TokenType expected, size_t& pos) {
    if (pos >= tokens.size()) {
        throw std::runtime_error(std::format(
            "Syntax Error: Expected '{}', but reached end of input", tokenTypeToString(expected)));
    }
    const auto& [token_type, lexeme] = tokens[pos];

    if (expected != token_type) {
        throw std::runtime_error(
            std::format("Syntax Error: Expected '{}', but found '{}': '{}' at pos:{}",
                        tokenTypeToString(expected), tokenTypeToString(token_type), lexeme, pos));
    }
    pos++;
}

// method definitions of parser.hpp
void ProgramNode::parse(std::deque<Token>& tokens, size_t& pos) {
    this->func = std::make_unique<FunctionNode>();
    this->func->parse(tokens, pos);
}

void ProgramNode::dump(int indent) const {
    printIndent(indent);
    std::println("Program(");
    this->func->dump(indent + 1);
    std::println(")");
}

void FunctionNode::parse(std::deque<Token>& tokens, size_t& pos) {
    expect(tokens, TokenType::INT, pos);
    this->var_identifier = std::make_unique<IdentifierNode>();
    this->var_identifier->parse(tokens, pos); // parse function/variable name
    expect(tokens, TokenType::LPAREN, pos);
    expect(tokens, TokenType::VOID, pos);
    expect(tokens, TokenType::RPAREN, pos);
    this->body = std::make_unique<BlockNode>();
    this->body->parse(tokens, pos);
}

void FunctionNode::dump(int indent) const {
    printIndent(indent);
    std::println("Function(");
    this->var_identifier->dump(indent + 1);
    printIndent(indent + 1);
    std::println("body=(");
    this->body->dump(indent + 2);
    printIndent(indent + 1);
    std::println(")"); // end of body
    printIndent(indent);
    std::println(")"); // end of Function
}

void BlockNode::parse(std::deque<Token>& tokens, size_t& pos) {
    expect(tokens, TokenType::LBRACE, pos);
    while (tokens[pos].type != TokenType::RBRACE) { // "}"
        auto block_item = std::make_unique<BlockItemNode>();
        block_item->parse(tokens, pos);
        this->block_items.push_back(std::move(block_item));
    }
    expect(tokens, TokenType::RBRACE, pos);
}

void BlockNode::dump(int indent) const {
    printIndent(indent);
    std::println("Block(");
    for (const auto& block_item : this->block_items) {
        block_item->dump(indent + 1);
    }
    printIndent(indent);
    std::println(")");
}

void BlockItemNode::parse(std::deque<Token>& tokens, size_t& pos) {
    if (tokens[pos].type == TokenType::INT) { // int <var_name>; | int <var_name> = <expr>;
        this->declaration = std::make_unique<DeclarationNode>();
        this->declaration->parse(tokens, pos);
    } else {
        this->statement = std::make_unique<StatementNode>();
        this->statement->parse(tokens, pos);
    }
}

void BlockItemNode::dump(int indent) const {
    if (this->declaration) {
        this->declaration->dump(indent);
    } else if (this->statement) {
        this->statement->dump(indent);
    }
}

void StatementNode::parse(std::deque<Token>& tokens, size_t& pos) {
    if (tokens[pos].type == TokenType::RETURN) {
        this->return_stmt = std::make_unique<ReturnNode>();
        this->return_stmt->parse(tokens, pos);
    } else if (tokens[pos].type == TokenType::SEMICOLON) {
        this->null_stmt = std::make_unique<NullNode>();
        this->null_stmt->parse(tokens, pos);
    } else if (tokens[pos].type == TokenType::LBRACE) {
        this->compound_stmt = std::make_unique<CompoundNode>();
        this->compound_stmt->parse(tokens, pos);
    } else if (tokens[pos].type == TokenType::IF) {
        this->ifelse_stmt = std::make_unique<IfElseNode>();
        this->ifelse_stmt->parse(tokens, pos);
    } else {
        this->expression_stmt = std::make_unique<ExpressionNode>();
        this->expression_stmt->parse(tokens, pos);
    }
}

void StatementNode::dump(int indent) const {
    if (this->return_stmt) {
        this->return_stmt->dump(indent);
    } else if (this->null_stmt) {
        this->null_stmt->dump(indent);
    } else if (this->ifelse_stmt) {
        this->ifelse_stmt->dump(indent);
    } else if (this->compound_stmt) {
        this->compound_stmt->dump(indent);
    } else {
        this->expression_stmt->dump(indent);
    }
}

void ReturnNode::parse(std::deque<Token>& tokens, size_t& pos) {
    expect(tokens, TokenType::RETURN, pos);
    this->ret_expr = std::make_unique<ExprNode>();
    this->ret_expr->parse(tokens, pos);
    expect(tokens, TokenType::SEMICOLON, pos);
}

void ReturnNode::dump(int indent) const {
    printIndent(indent);
    std::println("Return(");
    if (this->ret_expr) {
        this->ret_expr->dump(indent + 1);
    }
    printIndent(indent);
    std::println(")");
}

void ExpressionNode::parse(std::deque<Token>& tokens, size_t& pos) {
    this->expr = std::make_unique<ExprNode>();
    this->expr->parse(tokens, pos);
    expect(tokens, TokenType::SEMICOLON, pos);
}

void ExpressionNode::dump(int indent) const {
    printIndent(indent);
    std::println("Expression(");
    this->expr->dump(indent + 1);
    printIndent(indent);
    std::println(")");
}

void IfElseNode::parse(std::deque<Token>& tokens, size_t& pos) {
    expect(tokens, TokenType::IF, pos);
    expect(tokens, TokenType::LPAREN, pos);
    this->condition = std::make_unique<ExprNode>();
    this->condition->parse(tokens, pos);
    expect(tokens, TokenType::RPAREN, pos);

    this->if_block = std::make_unique<StatementNode>();
    this->if_block->parse(tokens, pos);

    if (tokens[pos].type == TokenType::ELSE) {
        expect(tokens, TokenType::ELSE, pos);
        this->else_block = std::make_unique<StatementNode>();
        this->else_block->parse(tokens, pos);
    }
}

void IfElseNode::dump(int indent) const {
    printIndent(indent);
    std::println("IfElse(");
    this->condition->dump(indent + 1);
    this->if_block->dump(indent + 1);
    if (this->else_block) {
        this->else_block->dump(indent + 1);
    }
    printIndent(indent);
    std::println(")");
}

void CompoundNode::parse(std::deque<Token>& tokens, size_t& pos) {
    this->block = std::make_unique<BlockNode>();
    this->block->parse(tokens, pos);
}

void CompoundNode::dump(int indent) const { this->block->dump(indent); }

void NullNode::parse(std::deque<Token>& tokens, size_t& pos) {
    expect(tokens, TokenType::SEMICOLON, pos);
}

void NullNode::dump(int indent) const {
    printIndent(indent);
    std::println(";"); // represent null statement
}

void DeclarationNode::parse(std::deque<Token>& tokens, size_t& pos) {
    expect(tokens, TokenType::INT, pos);
    this->var_identifier = std::make_unique<IdentifierNode>();
    this->var_identifier->parse(tokens, pos);
    if (tokens[pos].type == TokenType::ASSIGN) {
        expect(tokens, TokenType::ASSIGN, pos);
        this->init_expr = std::make_unique<ExprNode>();
        this->init_expr->parse(tokens, pos);
    }
    expect(tokens, TokenType::SEMICOLON, pos);
}

void DeclarationNode::dump(int indent) const {
    printIndent(indent);
    std::println("Declaration(");
    this->var_identifier->dump(indent + 1, this->init_expr != nullptr);
    if (this->init_expr) { // OPTIONAL
        this->init_expr->dump(indent + 1);
        printIndent(indent);
    }
    std::println(")");
}

/*````
parse_exp(1 + 2 * 3, min_prec=0)
    parse 1
    "+" ≥ 0 → enter loop
    parse "+"
    parse_exp(2 * 3, min_prec=46): "*" ≥ 46 → parse 2*3 → Bin(*, 2, 3) |◍◍◍◍|
    Result: Bin(+, 1, Bin(*, 2, 3))

parse_exp(1 * 2 + 3, min_prec=0)
    parse 1
    "*" ≥ 0 → enter loop
    parse "*"
    parse_exp(2 + 3, min_prec=51): "+" < 51 → return 2 |◍◍◍◍|
    Result so far: Bin(*, 1, 2)
    "+" ≥ 0 → continue loop, parse +3
    Result: Bin(+, Bin(*, 1, 2), 3)
```*/
void ExprNode::parse(std::deque<Token>& tokens, size_t& pos, int min_precedence) {
    this->left_exprf = std::make_unique<ExprFactorNode>();
    this->left_exprf->parse(tokens, pos);

    while (pos < tokens.size() && isBinop(tokens[pos].lexeme) &&
           getPrecedence(tokens[pos].lexeme) >= min_precedence) {
        const auto& [token_type, op] = tokens[pos];
        int op_prec = getPrecedence(op);

        auto right_expr = std::make_unique<ExprNode>();
        auto left_expr = std::make_unique<ExprNode>();
        // RIGHT ASSOCIATIVE OPERATORS
        if (token_type == TokenType::ASSIGN) {
            expect(tokens, TokenType::ASSIGN, pos); // consume '='

            right_expr->parse(tokens, pos, op_prec);

            // wrap `left_exprf` in an `ExprNode`
            left_expr->left_exprf = std::move(this->left_exprf);

            this->left_exprf =
                std::make_unique<AssignmentNode>(std::move(left_expr), std::move(right_expr));
        } else if (token_type == TokenType::QUESTION) {
            expect(tokens, TokenType::QUESTION, pos); // consume '?'

            auto middle_expr = std::make_unique<ExprNode>();
            middle_expr->parse(tokens, pos, 0); // reset precedence for middle expr

            expect(tokens, TokenType::COLON, pos); // consume ':'

            right_expr->parse(tokens, pos, op_prec);

            // wrap `left_exprf` in an `ExprNode`
            left_expr->left_exprf = std::move(this->left_exprf);

            this->left_exprf = std::make_unique<ConditionalNode>(
                std::move(left_expr), std::move(middle_expr), std::move(right_expr));
        }
        // LEFT ASSOCIATIVE OPERATORS
        else {
            expect(tokens, token_type, pos); // consume operator

            // the + 1 makes this left associative
            right_expr->parse(tokens, pos, op_prec + 1); // |◍◍◍◍|

            // wrap `left_exprf` in an `ExprNode`
            left_expr->left_exprf = std::move(this->left_exprf);

            this->left_exprf =
                std::make_unique<BinaryNode>(op, std::move(left_expr), std::move(right_expr));
        }
    }
    // the final result is we need is `left_exprf`
}

/// @brief <exp> is of the form <factor> ( <binary> <expr> )*
void ExprNode::dump(int indent) const { this->left_exprf->dump(indent); }

void ExprFactorNode::parse(std::deque<Token>& tokens, size_t& pos) {
    const auto& [token_type, lexeme] = tokens[pos];
    if (token_type == TokenType::CONSTANT) { // <int>: a constant integer
        this->constant = std::make_unique<ConstantNode>();
        this->constant->parse(tokens, pos);
    } else if (token_type == TokenType::IDENTIFIER) { // <identifier>
        this->var_identifier = std::make_unique<VarNode>();
        this->var_identifier->parse(tokens, pos);
    } else if (isUnary(lexeme)) { // <unary> <factor>
        this->unary = std::make_unique<UnaryNode>();
        this->unary->parse(tokens, pos);
        /// should I keep the below lines in `UnaryNode::parse`?
        /* this->unary->operand = std::make_unique<ExprFactorNode>();
        this->unary->operand->parse(tokens, pos); */
    } else if (token_type == TokenType::LPAREN) { // "(" <expr> ")"
        expect(tokens, TokenType::LPAREN, pos);
        this->expr = std::make_unique<ExprNode>();
        // 0 init precedence when parsing a factor
        this->expr->parse(tokens, pos, 0);
        expect(tokens, TokenType::RPAREN, pos);
    } else {
        throw std::runtime_error("Syntax Error: Malformed Expression Factor");
    }
}

void ExprFactorNode::dump(int indent) const {
    if (this->constant) {
        this->constant->dump(indent);
    } else if (this->var_identifier) {
        this->var_identifier->dump(indent);
    } else if (this->unary) {
        this->unary->dump(indent);
    } else if (this->expr) {
        this->expr->dump(indent);
    } else {
        throw std::runtime_error("Malformed Expression Factor during dump");
    }
}

void VarNode::parse(std::deque<Token>& tokens, size_t& pos) {
    this->var_name = std::make_unique<IdentifierNode>();
    this->var_name->parse(tokens, pos);
}

void VarNode::dump(int indent) const {
    printIndent(indent);
    std::print("Var(");
    this->var_name->dump(0, false);
    std::println(")");
}

void IdentifierNode::parse(std::deque<Token>& tokens, size_t& pos) {
    const auto& [token_type, actual] = tokens[pos++];
    if (token_type != TokenType::IDENTIFIER) {
        throw std::runtime_error(
            std::format("Syntax Error: Expected identifier but got '{}'", actual));
    }
    this->name = actual;
}

void IdentifierNode::dump(int indent, bool new_line) const {
    printIndent(indent);
    std::print("name='{}'", this->name);
    if (new_line) {
        std::println();
    }
}

void ConstantNode::parse(std::deque<Token>& tokens, size_t& pos) {
    const auto& [token_type, actual] = tokens[pos++];
    if (token_type != TokenType::CONSTANT) {
        throw std::runtime_error(
            std::format("Syntax Error: Expected constant integer but got '{}'", actual));
    }
    this->val = actual;
}

void ConstantNode::dump(int indent) const {
    printIndent(indent);
    std::println("Constant({})", this->val);
}

void UnaryNode::parse(std::deque<Token>& tokens, size_t& pos) {
    const auto& [token_type, actual] = tokens[pos++];
    if (!isUnary(actual)) {
        throw std::runtime_error(
            std::format("Syntax Error: Expected a unary operator but got '{}':'{}' at pos:{}",
                        tokenTypeToString(token_type), actual, pos));
    }
    this->op_type = actual;
    this->operand = std::make_unique<ExprFactorNode>();
    this->operand->parse(tokens, pos);
}

void UnaryNode::dump(int indent) const {
    printIndent(indent);
    std::println("Unary({}", this->op_type);
    this->operand->dump(indent + 1);
    printIndent(indent);
    std::println(")");
}

void BinaryNode::parse(std::deque<Token>& tokens, size_t& pos) {
    throw std::runtime_error("Shouldn't reach `BinaryNode::parse`, handled in `ExprNode::parse`");
}

void BinaryNode::dump(int indent) const {
    printIndent(indent);
    std::println("Binary({},", this->op_type);
    this->left_expr->dump(indent + 1);
    this->right_expr->dump(indent + 1);
    printIndent(indent);
    std::println(")");
}

void AssignmentNode::parse(std::deque<Token>& tokens, size_t& pos) {
    throw std::runtime_error(
        "Shouldn't reach `AssignmentNode::parse`, handled in `ExprNode::parse`");
}

void AssignmentNode::dump(int indent) const {
    printIndent(indent);
    std::println("Assignment(");
    this->left_expr->dump(indent + 1);
    this->right_expr->dump(indent + 1);
    printIndent(indent);
    std::println(")");
}

void ConditionalNode::parse(std::deque<Token>& tokens, size_t& pos) {
    throw std::runtime_error(
        "Shouldn't reach here: `ConditionalNode::parse`, handled in `ExprNode::parse`");
}

void ConditionalNode::dump(int indent) const {
    printIndent(indent);
    std::println("Conditional(");
    this->condition->dump(indent + 1);
    this->true_expr->dump(indent + 1);
    this->false_expr->dump(indent + 1);
    printIndent(indent);
    std::println(")");
}

std::unique_ptr<ProgramNode> parse(std::deque<Token>& tokens) {
    size_t pos = 0;
    auto ast = std::make_unique<ProgramNode>();
    ast->parse(tokens, pos);
    if (pos != tokens.size()) {
        const auto& [token_type, actual] = tokens[pos];
        throw std::runtime_error(
            std::format("Syntax Error: Unexpected token '{}' of class '{}' at top level", actual,
                        tokenTypeToString(token_type)));
    }
    return ast;
}

/*
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::println(stderr,
                     "Usage: {} [--lex|--parse|--validate|--tacky|--codegen]"
                     "<source_file>",
                     argv[0]);
        return 1;
    }

    // Find the source file (non-flag argument)
    std::string filename;
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (!arg.starts_with("--")) {
            filename = arg;
            break;
        }
    }

    if (filename.empty()) {
        std::println(stderr, "Error: No source file specified");
        return 1;
    }
    auto contents = getFileContents(filename);
    auto tokens = lexer(contents);
    size_t i = 0;
    for (const auto& [token_type, lexeme] : tokens) {
        std::println("{}, {}, {}", i++, tokenTypeToString(token_type), lexeme);
    }
    auto ast = parse(tokens);
    ast->dump();
    return 0;
}
// */