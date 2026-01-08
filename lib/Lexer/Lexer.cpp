#include <print>

#include "CAPI/lexer.h"
#include "nanocc/Lexer/Lexer.hpp"

namespace nanocc {
std::deque<Token> lexer(std::string& s, bool debug) {
    CTokenVec c_tokens = clexer(const_cast<char*>(s.c_str()), s.size(), debug);

    std::deque<Token> tokens;
    tokens.resize(c_tokens.count);

    for (size_t i = 0; i < c_tokens.count; i++) {
        Token token;
        token.type = static_cast<TokenType>(c_tokens.items[i].type);
        token.lexeme = std::string(c_tokens.items[i].start, c_tokens.items[i].length);
        tokens[i] = std::move(token);
    }
    freeCTokens(c_tokens);
    if (debug) {
        std::println("----- Lexical Analysis -----");
        std::size_t i = 0;
        for (const auto& [token_type, lexeme] : tokens) {
            std::println("| {} | {} | {} |", i++, tokenTypeToString(token_type), lexeme);
        }
        std::println("----------------------------");
    }
    return tokens;
}
} // namespace nanocc