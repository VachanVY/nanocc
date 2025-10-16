#include <deque>
#include <memory>
#include <print>
#include <vector>

#include "lexer.hpp"
#include "parser.hpp"
#include "utils.hpp"

// Helper to print with indentation
void print_indent(int indent) {
    for (int i = 0; i < indent; ++i) {
        std::print("  ");
    }
}

void expect(const std::deque<Token> &tokens, const std::string &expected,
            size_t &pos) {
    if (pos >= tokens.size()) {
        throw std::runtime_error(std::format(
            "Syntax Error: Expected '{}', but reached end of input", expected));
    }
    auto [token_class, actual] = tokens[pos];

    if (expected != actual) {
        throw std::runtime_error(
            std::format("Syntax Error: Expected '{}', but found '{}': '{}'",
                        expected, token_class, actual));
    }
    pos++;
}

// method definitions of parser.hpp
void ProgramNode::parse(std::deque<Token> &tokens, size_t &pos) {
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

void FunctionNode::parse(std::deque<Token> &tokens, size_t &pos) {
    expect(tokens, "int", pos);
    var = std::make_unique<IdentifierNode>();
    var->parse(tokens, pos); // parse function/variable name
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
    std::println("FunctionNode(");
    if (var) {
        var->dump(indent + 1);
    }
    if (statement) {
        statement->dump(indent + 1);
    }
}

void StatementNode::parse(std::deque<Token> &tokens, size_t &pos) {
    expect(tokens, "return", pos);
    expr = std::make_unique<ExprNode>();
    expr->parse(tokens, pos);
    expect(tokens, ";", pos);
}

void StatementNode::dump(int indent) const {
    print_indent(indent);
    std::print("body=");
    if (expr) {
        expr->dump(indent + 1);
    }
}

void ExprNode::parse(std::deque<Token> &tokens, size_t &pos) {
    integer = std::make_unique<IntNode>();
    integer->parse(tokens, pos);
}

void ExprNode::dump(int indent) const {
    // print_indent(indent);
    std::println("Expr(");
    if (integer) {
        integer->dump(indent + 1);
    }
    print_indent(indent);
    std::println(")");
}

void IdentifierNode::parse(std::deque<Token> &tokens, size_t &pos) {
    auto [token_class, actual] = tokens[pos++];
    if (token_class != "identifier") {
        throw std::runtime_error(
            std::format("Expected identifier but got {}", actual));
    }
    name = actual;
}

void IdentifierNode::dump(int indent) const {
    print_indent(indent);
    std::println("name='{}'", name);
}

void IntNode::parse(std::deque<Token> &tokens, size_t &pos) {
    auto [token_class, actual] = tokens[pos++];
    if (token_class != "constant") {
        throw std::runtime_error(
            std::format("Expected constant integer but got {}", actual));
    }
    val = std::stoi(actual);
}

void IntNode::dump(int indent) const {
    print_indent(indent);
    std::println("Int({})", val);
}

std::unique_ptr<ProgramNode> parse(std::deque<Token> &tokens) {
    size_t pos = 0;
    auto ast = std::make_unique<ProgramNode>();
    ast->parse(tokens, pos);
    return ast;
}

int main() {
    std::string code = "int main(void) { return 42; }";
    auto tokens = lexer(code);
    auto ast = parse(tokens);

    std::println("AST Dump:");
    ast->dump();

    return 0;
}