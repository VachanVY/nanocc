#include <deque>
#include <memory>
#include <print>
#include <vector>

#include "lexer.hpp"
#include "parser.hpp"
#include "utils.hpp"

namespace { // some helper vars/functions
static const std::unordered_map<std::string, int> BINOP_PRECEDENCE = {
    {"*", 50}, {"/", 50},  {"%", 50},  {"+", 45},  {"-", 45},  {"<", 35}, {"<=", 35},
    {">", 35}, {">=", 35}, {"==", 30}, {"!=", 30}, {"&&", 10}, {"||", 5},
};

inline bool isUnary(const std::string& op) {
    return op == "~" || op == "-" || op == "!";
    /* || op == "--"; */ // decrement only for lexing now
}

inline int getPrecedence(const std::string& op) {
    auto it = BINOP_PRECEDENCE.find(op);
    if (it == BINOP_PRECEDENCE.end())
        throw std::runtime_error("Invalid operator: " + op);
    return it->second;
}

inline bool isBinop(const std::string& op) { return BINOP_PRECEDENCE.contains(op); }
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
    func = std::make_unique<FunctionNode>();
    func->parse(tokens, pos);
}

void ProgramNode::dump(int indent) const {
    printIndent(indent);
    std::println("Program(");
    if (func) {
        func->dump(indent + 1);
    }
    std::println(")");
}

void FunctionNode::parse(std::deque<Token>& tokens, size_t& pos) {
    expect(tokens, TokenType::INT, pos);
    var_identifier = std::make_unique<IdentifierNode>();
    var_identifier->parse(tokens, pos); // parse function/variable name
    expect(tokens, TokenType::LPAREN, pos);
    expect(tokens, TokenType::VOID, pos);
    expect(tokens, TokenType::RPAREN, pos);
    expect(tokens, TokenType::LBRACE, pos);
    statement = std::make_unique<StatementNode>();
    statement->parse(tokens, pos);
    expect(tokens, TokenType::RBRACE, pos);
}

void FunctionNode::dump(int indent) const {
    printIndent(indent);
    std::println("Function(");
    if (var_identifier) {
        var_identifier->dump(indent + 1);
    }
    if (statement) {
        statement->dump(indent + 1);
    }
    printIndent(indent);
    std::println(")");
}

void StatementNode::parse(std::deque<Token>& tokens, size_t& pos) {
    expect(tokens, TokenType::RETURN, pos);
    expr = std::make_unique<ExprNode>();
    expr->parse(tokens, pos);
    expect(tokens, TokenType::SEMICOLON, pos);
}

void StatementNode::dump(int indent) const {
    printIndent(indent);
    std::println("body=Return(");
    if (expr) {
        expr->dump(indent + 1);
    }
    printIndent(indent);
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
    left_exprf = std::make_unique<ExprFactorNode>();
    left_exprf->parse(tokens, pos);

    while (pos < tokens.size() && isBinop(tokens[pos].lexeme) &&
           getPrecedence(tokens[pos].lexeme) >= min_precedence) {
        std::string op = tokens[pos].lexeme;
        int op_prec = getPrecedence(op);

        binop = std::make_unique<BinaryNode>();
        binop->parse(tokens, pos);

        right_expr = std::make_unique<ExprNode>();
        right_expr->parse(tokens, pos, op_prec + 1); // |◍◍◍◍|

        auto factor = std::make_unique<BinaryNode>();
        factor->left = std::move(left_exprf);
        factor->right = std::move(right_expr);
        factor->op_type = op;

        left_exprf = std::move(factor);
    }
    // the final result is we need is `left_exprf`
}

/// @brief <exp> is of the form <factor> ( <binary> <expr> )*
void ExprNode::dump(int indent) const {
    if (left_exprf) {
        left_exprf->dump(indent);
    }
}

void ExprFactorNode::parse(std::deque<Token>& tokens, size_t& pos) {
    const auto& [token_type, lexeme] = tokens[pos];
    if (token_type == TokenType::CONSTANT) { // <constant>
        constant = std::make_unique<ConstantNode>();
        constant->parse(tokens, pos);
    } else if (isUnary(lexeme)) { // <unary> <factor>
        unary = std::make_unique<UnaryNode>();
        unary->parse(tokens, pos);
        factor = std::make_unique<ExprFactorNode>();
        factor->parse(tokens, pos);
    } else if (token_type == TokenType::LPAREN) { // "(<expr>)"
        expect(tokens, TokenType::LPAREN, pos);
        expr = std::make_unique<ExprNode>();
        // 0 init precedence when parsing a factor
        expr->parse(tokens, pos, 0);
        expect(tokens, TokenType::RPAREN, pos);
    } else {
        throw std::runtime_error("Syntax Error: Malformed Expression Factor");
    }
}

void ExprFactorNode::dump(int indent) const {
    if (constant) {
        constant->dump(indent);
        return;
    }
    if (unary && factor) {
        printIndent(indent);
        std::println("Unary({},", unary->op_type);
        factor->dump(indent + 1);
        printIndent(indent);
        std::println(")");
        return;
    }
    if (expr) {
        expr->dump(indent);
        return;
    }
}

void IdentifierNode::parse(std::deque<Token>& tokens, size_t& pos) {
    const auto& [token_type, actual] = tokens[pos++];
    if (token_type != TokenType::IDENTIFIER) {
        throw std::runtime_error(
            std::format("Syntax Error: Expected identifier but got '{}'", actual));
    }
    name = actual;
}

void IdentifierNode::dump(int indent) const {
    printIndent(indent);
    std::println("name='{}'", name);
}

void ConstantNode::parse(std::deque<Token>& tokens, size_t& pos) {
    const auto& [token_type, actual] = tokens[pos++];
    if (token_type != TokenType::CONSTANT) {
        throw std::runtime_error(
            std::format("Syntax Error: Expected constant integer but got '{}'", actual));
    }
    val = actual;
}

void ConstantNode::dump(int indent) const {
    printIndent(indent);
    std::println("Constant({})", val);
}

void UnaryNode::parse(std::deque<Token>& tokens, size_t& pos) {
    const auto& [token_type, actual] = tokens[pos++];
    if (!isUnary(actual)) {
        throw std::runtime_error(
            std::format("Syntax Error: Expected a unary operator but got '{}':'{}' at pos:{}",
                        tokenTypeToString(token_type), actual, pos));
    }
    op_type = actual;
}

void UnaryNode::dump(int indent) const {
    printIndent(indent);
    std::println("Unary({})", op_type);
}

void BinaryNode::parse(std::deque<Token>& tokens, size_t& pos) {
    const auto& [token_type, actual] = tokens[pos++];
    if (!isBinop(actual)) {
        throw std::runtime_error(
            std::format("Syntax Error: Expected binary operator but got '{}' : '{}' at pos:{}",
                        tokenTypeToString(token_type), actual, pos));
    }
    op_type = actual;
}

void BinaryNode::dump(int indent) const {
    printIndent(indent);
    std::println("Binary({},", op_type);
    if (left) {
        left->dump(indent + 1);
    }
    if (right) {
        right->dump(indent + 1);
    }
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
*/