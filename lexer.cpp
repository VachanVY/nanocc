#include <cctype>
#include <print>
#include <regex>
#include <stdexcept>
#include <string>

#include "lexer.hpp"

std::pair<TokenType, std::regex> token_specs[] = {
    // "keywords" should get more priority than "identifiers", so lex them first
    {TokenType::INT, std::regex("int\\b")},
    {TokenType::VOID, std::regex("void\\b")},
    {TokenType::RETURN, std::regex("return\\b")},
    {TokenType::IF, std::regex("if\\b")},
    {TokenType::ELSE, std::regex("else\\b")},
    {TokenType::QUESTION, std::regex("\\?")},
    {TokenType::COLON, std::regex(":")},
    {TokenType::DO, std::regex("do\\b")},
    {TokenType::WHILE, std::regex("while\\b")},
    {TokenType::FOR, std::regex("for\\b")},
    {TokenType::BREAK, std::regex("break\\b")},
    {TokenType::CONTINUE, std::regex("continue\\b")},

    // "unary operators"
    {TokenType::TILDE, std::regex("~")},
    {TokenType::DECREMENT, std::regex("--")},
    {TokenType::NOT, std::regex("!")},
    // both unary and binary
    {TokenType::MINUS, std::regex("-")},
    // "binary operators"
    {TokenType::PLUS, std::regex("\\+")},
    {TokenType::STAR, std::regex("\\*")},
    {TokenType::SLASH, std::regex("/")},
    {TokenType::PERCENT, std::regex("%")},
    {TokenType::AND, std::regex("&&")},
    {TokenType::OR, std::regex("\\|\\|")},

    // assignment operators // comes under binary operators but has less precedence
    {TokenType::ASSIGN, std::regex("=")},

    {TokenType::EQUAL, std::regex("==")},
    {TokenType::NOT_EQUAL, std::regex("!=")},
    {TokenType::LESSTHAN, std::regex("<")},
    {TokenType::GREATERTHAN, std::regex(">")},
    {TokenType::LESS_EQUAL, std::regex("<=")},
    {TokenType::GREATER_EQUAL, std::regex(">=")},

    {TokenType::IDENTIFIER, std::regex("[a-zA-Z_]\\w*\\b")}, // variable/function names
    {TokenType::CONSTANT, std::regex("[0-9]+\\b")},
    {TokenType::LPAREN, std::regex("\\(")},
    {TokenType::RPAREN, std::regex("\\)")},
    {TokenType::LBRACE, std::regex("\\{")},
    {TokenType::RBRACE, std::regex("\\}")},
    {TokenType::SEMICOLON, std::regex(";")},
};

std::deque<Token> lexer(const std::string& s) {
    std::deque<Token> tokens;
    size_t pos = 0;
    while (pos < s.length()) {
        if (std::isspace(s[pos])) {
            pos++;
            continue;
        }

        // handle comments //
        if (pos < s.length() - 1 && s[pos] == '/' && s[pos + 1] == '/') {
            // Skip until end of line
            while (pos < s.length() && s[pos] != '\n') {
                pos++;
            }
            continue;
        }

        // handle comments /* */
        if (pos < s.length() - 1 && s[pos] == '/' && s[pos + 1] == '*') {
            pos += 2; // skip /*
            while (pos < s.length() - 1) {
                if (s[pos] == '*' && s[pos + 1] == '/') {
                    pos += 2; // skip */
                    break;
                }
                pos++;
            }
            continue;
        }

        size_t match_length = 0;
        std::string match;
        TokenType class_type;
        bool matched = false;
        for (const auto& [tokenClass, regex] : token_specs) {
            std::smatch tmp_match;
            std::string remainder = s.substr(pos); // s[pos:]
            if (std::regex_search(remainder, tmp_match, regex,
                                  std::regex_constants::match_continuous)) {
                if (tmp_match.length() > match_length) { // what if they are equal? how to decide
                                                         // which one? // keep the first one
                    class_type = tokenClass;
                    match = tmp_match.str();
                    match_length = tmp_match.length();
                    matched = true;
                }
            }
        }

        // throw error if no regex matched
        if (!matched) {
            throw std::runtime_error("Unexpected Syntax error at position " + std::to_string(pos));
        }

        // remove substr of the string now that it is tokenized
        pos += match_length;

        // add to `tokens`
        tokens.emplace_back(class_type, match);
    }
    return tokens;
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

    for(auto& [token_type, lexeme]: tokens){
        std::println("{}, {}", tokenTypeToString(token_type), lexeme);
    }

    return 0;
}
*/