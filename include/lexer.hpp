#pragma once

#include <fstream>
#include <print>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

typedef struct {
    std::string token_class; // token class of each lexeme
    std::string lexemes;     // matched string for the regex of that token_class
} Token;

extern std::vector<std::pair<std::string, std::regex>> token_specs;

std::deque<Token> lexer(const std::string &s);