#pragma once

#include <stdexcept>
#include <string>

enum class TokenType {
#define X(name, str) name,
#include "tokens.def"
#undef X
};

constexpr std::string_view tokenTypeToString(TokenType type) {
  switch (type) {
#define X(name, str)                                                           \
  case TokenType::name:                                                        \
    return str;
#include "tokens.def"
#undef X
  }
  throw std::runtime_error("Unknown TokenType");
}