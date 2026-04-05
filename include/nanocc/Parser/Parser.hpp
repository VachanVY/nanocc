#pragma once

#include <deque>
#include <memory>

#include "nanocc/AST/AST.hpp"
#include "nanocc/Lexer/Lexer.hpp"

namespace nanocc {
/// @brief Parses the tokens into an AST.
/// @param tokens The deque of tokens to parse.
/// @param debug Whether to print debug information during parsing.
/// @return The root of the generated AST.
std::unique_ptr<ProgramNode> parse(std::deque<Token>& tokens,
                                   bool debug = false);
} // namespace nanocc
