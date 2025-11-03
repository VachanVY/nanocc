#pragma once

#include <unordered_map>
#include <string>

/// @brief maps variables names given in source code to unique identifier names
typedef std::unordered_map<std::string, std::string> SymbolTable;