#pragma once

#include <deque>
#include <fstream>
#include <print>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#define TOKEN_LIST                                                                                 \
    X(INT, "int")                                                                                  \
    X(VOID, "void")                                                                                \
    X(RETURN, "return")                                                                            \
    X(IF, "if")                                                                                    \
    X(ELSE, "else")                                                                                \
    X(QUESTION, "?")                                                                               \
    X(COLON, ":")                                                                                  \
                                                                                                   \
    X(TILDE, "~")                                                                                  \
    X(DECREMENT, "--")                                                                             \
    X(NOT, "!")                                                                                    \
                                                                                                   \
    X(MINUS, "-")                                                                                  \
                                                                                                   \
    X(PLUS, "+")                                                                                   \
    X(STAR, "*")                                                                                   \
    X(SLASH, "/")                                                                                  \
    X(PERCENT, "%")                                                                                \
    X(AND, "&&")                                                                                   \
    X(OR, "||")                                                                                    \
                                                                                                   \
    X(ASSIGN, "=")                                                                                 \
                                                                                                   \
    X(EQUAL, "==")                                                                                 \
    X(NOT_EQUAL, "!=")                                                                             \
    X(LESSTHAN, "<")                                                                               \
    X(GREATERTHAN, ">")                                                                            \
    X(LESS_EQUAL, "<=")                                                                            \
    X(GREATER_EQUAL, ">=")                                                                         \
                                                                                                   \
    X(IDENTIFIER, "identifier")                                                                    \
    X(CONSTANT, "constant")                                                                        \
    X(LPAREN, "(")                                                                                 \
    X(RPAREN, ")")                                                                                 \
    X(LBRACE, "{")                                                                                 \
    X(RBRACE, "}")                                                                                 \
    X(SEMICOLON, ";")

enum class TokenType {
#define X(name, str) name,
    TOKEN_LIST
#undef X
};

struct Token {
    TokenType type;     // token category produced by the lexer
    std::string lexeme; // source text matched for that token
};

extern std::pair<TokenType, std::regex> token_specs[];

constexpr std::string_view tokenTypeToString(TokenType type) {
    switch (type) {
#define X(name, str)                                                                               \
    case TokenType::name:                                                                          \
        return str;
        TOKEN_LIST
#undef X
    }
    throw std::runtime_error("Unknown TokenType");
}

std::deque<Token> lexer(const std::string& s);