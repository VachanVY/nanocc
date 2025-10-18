#include <deque>
#include <memory>
#include <print>
#include <vector>

#include "lexer.hpp"
#include "parser.hpp"

/*
=> what's the `parse` method for "X" doing?
    * checks for syntax correctness and creates node for each non-terminal
variable in "X"

=> when ever we find a new a non-terminal variable in "X" create a node pointer
for it and parse
*/

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
    const auto &[token_class, actual] = tokens[pos];

    if (expected != actual) {
        throw std::runtime_error(std::format(
            "Syntax Error: Expected '{}', but found '{}': '{}' at pos:{}",
            expected, token_class, actual, pos));
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

void StatementNode::parse(std::deque<Token> &tokens, size_t &pos) {
    expect(tokens, "return", pos); // 6
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

void ExprNode::parse(std::deque<Token> &tokens, size_t &pos) {
    const auto &[token_class, lexeme] = tokens[pos];
    if (token_class == "constant") {
        constant = std::make_unique<ConstantNode>();
        constant->parse(tokens, pos);
    } else if (token_class == "tilde" || token_class == "negate") {
        unary = std::make_unique<UnaryNode>();
        unary->parse(tokens, pos);
        expr = std::make_unique<ExprNode>();
        expr->parse(tokens, pos);
    } else if (token_class == "(") {
        expect(tokens, "(", pos);
        expr = std::make_unique<ExprNode>();
        expr->parse(tokens, pos);
        expect(tokens, ")", pos);
    } else {
        throw std::runtime_error("Syntax Error: Malformed Expression");
    }
}

void ExprNode::dump(int indent) const {
    if (constant) {
        constant->dump(indent);
        return;
    }
    if (unary && expr) {
        print_indent(indent);
        std::println("Unary({},", unary->op_type);
        expr->dump(indent + 1);
        print_indent(indent);
        std::println(")");
        return;
    }
    if (expr) {
        expr->dump(indent);
        return;
    }
}

void IdentifierNode::parse(std::deque<Token> &tokens, size_t &pos) {
    const auto &[token_class, actual] = tokens[pos++];
    if (token_class != "identifier") {
        throw std::runtime_error(std::format(
            "Syntax Error: Expected identifier but got '{}'", actual));
    }
    name = actual;
}

void IdentifierNode::dump(int indent) const {
    print_indent(indent);
    std::println("name='{}'", name);
}

void ConstantNode::parse(std::deque<Token> &tokens, size_t &pos) {
    const auto &[token_class, actual] = tokens[pos++];
    if (token_class != "constant") {
        throw std::runtime_error(std::format(
            "Syntax Error: Expected constant integer but got '{}'", actual));
    }
    val = std::stoi(actual);
}

void ConstantNode::dump(int indent) const {
    print_indent(indent);
    std::println("Constant({})", val);
}

void UnaryNode::parse(std::deque<Token> &tokens, size_t &pos) {
    const auto &[token_class, actual] = tokens[pos++];
    if (!(token_class == "tilde" || token_class == "negate")) {
        throw std::runtime_error(std::format(
            "Syntax Error: Expected '~' or '-' but got '{}':'{}' at pos:{}",
            token_class, actual, pos));
    }
    op_type = actual;
}

void UnaryNode::dump(int indent) const {
    print_indent(indent);
    std::println("Unary({})", op_type);
}

std::unique_ptr<ProgramNode> parse(std::deque<Token> &tokens) {
    size_t pos = 0;
    auto ast = std::make_unique<ProgramNode>();
    ast->parse(tokens, pos);
    if (pos != tokens.size()) {
        const auto &[token_class, actual] = tokens[pos];
        throw std::runtime_error(std::format(
            "Syntax Error: Unexpected token '{}' of class '{}' at top level",
            actual, token_class));
    }
    return ast;
}

/*
#include "utils.hpp"
int main(int argc, char *argv[]) {
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
    for (const auto &[token_class, lexemes] : tokens) {
        std::println("{}, {}, {}", i++, token_class, lexemes);
    }
    auto ast = parse(tokens);
    ast->dump();

    return 0;
}
*/