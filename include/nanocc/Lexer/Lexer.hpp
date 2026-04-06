#pragma once

#include <deque>
#include <stdexcept>
#include <string>

#include "nanocc/Utils/Tokens.hpp"

struct TokenLocation {
  std::string filename;
  size_t line;
  size_t column;
};

struct Token {
  TokenType type;         // token category produced by the lexer
  std::string lexeme;     // source text matched for that token
  TokenLocation location; // location of the token in the source
};

namespace nanocc {
/// @brief Lexical analyzer that converts source code string into tokens.
/// @param s The source code string to analyze.
/// @param debug Whether to print debug information during lexing.
/// @return A deque of tokens generated from the source code.
std::deque<Token> lexer(const std::string& s, bool debug = false);
} // namespace nanocc