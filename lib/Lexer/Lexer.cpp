#include <print>

#include "CAPI/lexer.h"
#include "nanocc/Lexer/Lexer.hpp"

namespace nanocc {
std::deque<Token> lexer(const std::string& s, bool debug) {
  CTokenVec c_tokens = clexer(const_cast<char*>(s.c_str()), s.size(), debug);

  std::deque<Token> tokens;
  tokens.resize(c_tokens.count);

  for (size_t i = 0; i < c_tokens.count; i++) {
    Token token;
    token.type = static_cast<TokenType>(c_tokens.items[i].type);
    // TODO(VachanVY): can we avoid this allocation? use string_view?
    token.lexeme =
        std::string(c_tokens.items[i].start, c_tokens.items[i].length);
    // why std::move used here? enable move assignment operator instead of copy
    // assignment operator
    token.location = {.filename = c_tokens.items[i].location.filename,
                      .line = c_tokens.items[i].location.line,
                      .column = c_tokens.items[i].location.column};
    tokens[i] = std::move(token);
  }
  freeCTokens(c_tokens);
  return tokens;
}
} // namespace nanocc