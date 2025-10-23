#include <deque>
#include <memory>
#include <print>
#include <vector>

#include "lexer.hpp"
#include "parser.hpp"
#include "utils.hpp"

namespace { // some helper vars/functions
std::unordered_map<std::string, int> op_precedence = {
    {"*", 50}, {"/", 50}, {"%", 50}, {"+", 45}, {"-", 45},
};

inline bool is_unary(const std::string& op) {
    return op == "~" || op == "-";
    /* || op == "--"; */ // decrement only for lexing now
}

inline bool is_binop(const std::string& op) {
    return op == "+" || op == "-" || op == "*" || op == "/" || op == "%";
}

inline int getPrecedence(const std::string& op) {
    try {
        return op_precedence.at(op);
    } catch (const std::out_of_range& e) {
        throw std::runtime_error("Invalid operator for precedence lookup: " + op);
    }
}
} // namespace

/* Dev Docs
=> what's the `parse` method for "X" doing?
    * checks for syntax correctness and creates node for each non-terminal
variable in "X"

=> when ever we find a new a non-terminal variable in "X" create a node pointer
for it and parse
*/

void expect(const std::deque<Token>& tokens, const std::string& expected, size_t& pos) {
    if (pos >= tokens.size()) {
        throw std::runtime_error(
            std::format("Syntax Error: Expected '{}', but reached end of input", expected));
    }
    const auto& [token_class, actual] = tokens[pos];

    if (expected != actual) {
        throw std::runtime_error(
            std::format("Syntax Error: Expected '{}', but found '{}': '{}' at pos:{}", expected,
                        token_class, actual, pos));
    }
    pos++;
}

// method definitions of parser.hpp
void ProgramNode::parse(std::deque<Token>& tokens, size_t& pos) {
    func = std::make_unique<FunctionNode>();
    func->parse(tokens, pos);
}

void ProgramNode::dump(int indent) const {
    print_indent(indent);
    std::println("Program(");
    if (func) {
        func->dump(indent + 1);
    }
    std::println(")");
}

void FunctionNode::parse(std::deque<Token>& tokens, size_t& pos) {
    expect(tokens, "int", pos);
    var_identifier = std::make_unique<IdentifierNode>();
    var_identifier->parse(tokens, pos); // parse function/variable name
    expect(tokens, "(", pos);
    expect(tokens, "void", pos);
    expect(tokens, ")", pos);
    expect(tokens, "{", pos);
    statement = std::make_unique<StatementNode>();
    statement->parse(tokens, pos);
    expect(tokens, "}", pos);
}

void FunctionNode::dump(int indent) const {
    print_indent(indent);
    std::println("Function(");
    if (var_identifier) {
        var_identifier->dump(indent + 1);
    }
    if (statement) {
        statement->dump(indent + 1);
    }
    print_indent(indent);
    std::println(")");
}

void StatementNode::parse(std::deque<Token>& tokens, size_t& pos) {
    expect(tokens, "return", pos);
    expr = std::make_unique<ExprNode>();
    expr->parse(tokens, pos);
    expect(tokens, ";", pos);
}

void StatementNode::dump(int indent) const {
    print_indent(indent);
    std::println("body=Return(");
    if (expr) {
        expr->dump(indent + 1);
    }
    print_indent(indent);
    std::println(")");
}

// precedence climbing parser for expressions
// idk how i would come up with this algo on my own lol...
void ExprNode::parse(std::deque<Token>& tokens, size_t& pos, int min_precedence) {
    left_exprf = std::make_unique<ExprFactorNode>();
    left_exprf->parse(tokens, pos);

    while (pos < tokens.size() && is_binop(tokens[pos].lexemes) &&
           getPrecedence(tokens[pos].lexemes) >= min_precedence) {
        std::string op = tokens[pos].lexemes;
        int op_prec = getPrecedence(op);

        binop = std::make_unique<BinaryNode>();
        binop->parse(tokens, pos);

        right_expr = std::make_unique<ExprNode>();
        right_expr->parse(tokens, pos, op_prec + 1);

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
    const auto& [token_class, lexeme] = tokens[pos];
    if (token_class == "constant") {
        constant = std::make_unique<ConstantNode>();
        constant->parse(tokens, pos);
    } else if (is_unary(lexeme)) {
        unary = std::make_unique<UnaryNode>();
        unary->parse(tokens, pos);
        factor = std::make_unique<ExprFactorNode>();
        factor->parse(tokens, pos);
    } else if (token_class == "(") {
        expect(tokens, "(", pos);
        expr = std::make_unique<ExprNode>();
        // 0 init precedence when parsing a factor
        expr->parse(tokens, pos, 0);
        expect(tokens, ")", pos);
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
        print_indent(indent);
        std::println("Unary({},", unary->op_type);
        factor->dump(indent + 1);
        print_indent(indent);
        std::println(")");
        return;
    }
    if (expr) {
        expr->dump(indent);
        return;
    }
}

void IdentifierNode::parse(std::deque<Token>& tokens, size_t& pos) {
    const auto& [token_class, actual] = tokens[pos++];
    if (token_class != "identifier") {
        throw std::runtime_error(
            std::format("Syntax Error: Expected identifier but got '{}'", actual));
    }
    name = actual;
}

void IdentifierNode::dump(int indent) const {
    print_indent(indent);
    std::println("name='{}'", name);
}

void ConstantNode::parse(std::deque<Token>& tokens, size_t& pos) {
    const auto& [token_class, actual] = tokens[pos++];
    if (token_class != "constant") {
        throw std::runtime_error(
            std::format("Syntax Error: Expected constant integer but got '{}'", actual));
    }
    val = std::stoi(actual);
}

void ConstantNode::dump(int indent) const {
    print_indent(indent);
    std::println("Constant({})", val);
}

void UnaryNode::parse(std::deque<Token>& tokens, size_t& pos) {
    const auto& [token_class, actual] = tokens[pos++];
    if (!is_unary(actual)) {
        throw std::runtime_error(
            std::format("Syntax Error: Expected '~' or '-' but got '{}':'{}' at pos:{}",
                        token_class, actual, pos));
    }
    op_type = actual;
}

void UnaryNode::dump(int indent) const {
    print_indent(indent);
    std::println("Unary({})", op_type);
}

void BinaryNode::parse(std::deque<Token>& tokens, size_t& pos) {
    const auto& [token_class, actual] = tokens[pos++];
    if (!is_binop(actual)) {
        throw std::runtime_error(
            std::format("Syntax Error: Expected binary operator but got '{}' : '{}' at pos:{}",
                        token_class, actual, pos));
    }
    op_type = actual;
}

void BinaryNode::dump(int indent) const {
    print_indent(indent);
    std::println("Binary({},", op_type);
    if (left) {
        left->dump(indent + 1);
    }
    if (right) {
        right->dump(indent + 1);
    }
    print_indent(indent);
    std::println(")");
}

std::unique_ptr<ProgramNode> parse(std::deque<Token>& tokens) {
    size_t pos = 0;
    auto ast = std::make_unique<ProgramNode>();
    ast->parse(tokens, pos);
    if (pos != tokens.size()) {
        const auto& [token_class, actual] = tokens[pos];
        throw std::runtime_error(std::format(
            "Syntax Error: Unexpected token '{}' of class '{}' at top level", actual, token_class));
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
    for (const auto& [token_class, lexemes] : tokens) {
        std::println("{}, {}, {}", i++, token_class, lexemes);
    }
    auto ast = parse(tokens);
    ast->dump();

    return 0;
}
*/