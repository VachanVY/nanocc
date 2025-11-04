#pragma once

#include <cassert>
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

inline void printIndent(int indent) {
    for (int i = 0; i < indent; i++) {
        std::printf("  ");
    }
}

inline std::string getUniqueName(const std::string& prefix) {
    static int counter = 0;
    // is the dot (.) okay? keeps it unique...
    return prefix + "." + std::to_string(counter++);
};

inline std::string getLabelName(const std::string& prefix) {
    assert(!prefix.empty() && "Label prefix cannot be empty");
    static size_t label_counter = 0;
    return prefix + "." + std::to_string(label_counter++);
};

// LLVM-style...
// TODO(VachanVY): As of now the `classof` methods use dynamic_cast internally.
// This is a temporary solution and may be replaced with a more efficient
template <typename To, typename From> inline bool isa(const From* u) {
    return u != nullptr && To::classof(u);
}

template <typename To, typename From> inline To* cast(From* Val) {
    assert(isa<To>(Val) && "cast<T>() argument of wrong type!");
    return static_cast<To*>(Val);
}

template <typename To, typename From> inline To* dyn_cast(From* u) {
    return isa<To>(u) ? cast<To>(u) : nullptr;
}
