#pragma once

#include <string>
#include <deque>
#include <stdexcept>

enum class TokenType {
#define X(name, str) name,
    #include "../../../lib/Lexer/CAPI/tokens.def"
#undef X
};

struct Token {
    TokenType type;     // token category produced by the lexer
    std::string lexeme; // source text matched for that token
};

constexpr std::string_view tokenTypeToString(TokenType type) {
    switch (type) {
#define X(name, str)                                                                               \
    case TokenType::name:                                                                          \
        return str;
    #include "../../../lib/Lexer/CAPI/tokens.def"
#undef X
    }
    throw std::runtime_error("Unknown TokenType");
}

namespace nanocc {
/// @brief Lexical analyzer that converts source code string into tokens.
/// @param s The source code string to analyze.
/// @param debug Whether to print debug information during lexing.
/// @return A deque of tokens generated from the source code.
std::deque<Token> lexer(std::string& s, bool debug = false);
} // namespace nanocc