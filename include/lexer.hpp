#pragma once

#include <deque>
#include <fstream>
#include <print>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

enum class TokenType {
    INT,       // int
    VOID,      // void
    RETURN,    // return

    TILDE,     // ~
    DECREMENT, // --

    MINUS,     // -

    PLUS,     // +
    STAR,     // *
    SLASH,    // /
    PERCENT,  // %

    IDENTIFIER, // identifier
    CONSTANT,   // constant
    LPAREN,     // (
    RPAREN,     // )
    LBRACE,     // {
    RBRACE,     // }
    SEMICOLON,  // ;
};

struct Token {
    TokenType type;      // token category produced by the lexer
    std::string lexeme;  // source text matched for that token
};

extern std::vector<std::pair<TokenType, std::regex>> token_specs;

inline std::string_view tokenTypeToString(TokenType type) {
    static constexpr std::string names[] = {
        "int",        // TokenType::INT
        "void",       // TokenType::VOID
        "return",     // TokenType::RETURN

        "~",          // TokenType::TILDE
        "--",         // TokenType::DECREMENT

        "-",          // TokenType::MINUS

        "+",          // TokenType::PLUS
        "*",          // TokenType::STAR
        "/",          // TokenType::SLASH
        "%",          // TokenType::PERCENT

        "identifier", // TokenType::IDENTIFIER
        "constant",   // TokenType::CONSTANT
        "(",          // TokenType::LPAREN
        ")",          // TokenType::RPAREN
        "{",          // TokenType::LBRACE
        "}",          // TokenType::RBRACE
        ";"};         // TokenType::SEMICOLON

    const auto index = static_cast<std::size_t>(type);
    return names[index];
}

std::deque<Token> lexer(const std::string& s);