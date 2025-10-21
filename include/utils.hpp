#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <print>

inline std::string getFileContents(const std::string& filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Error: Could not open file '" + filename + "'");
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    file.close();
    return content;
}

constexpr auto TAB4 = "    ";

inline void print_indent(int indent) {
    for (int i = 0; i < indent; i++) {
        std::print("  ");
    }
}

// LLVM-style...
template <typename T, typename U> inline bool isa(const U* u) {
    return u != nullptr && T::classof(u);
}

template <typename T, typename U> inline T* dyn_cast(U* u) {
    return isa<T>(u) ? static_cast<T*>(u) : nullptr;
}
