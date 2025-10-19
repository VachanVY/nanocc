#pragma once

#include <fstream>
#include <sstream>
#include <string>

inline std::string getFileContents(const std::string &filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Error: Could not open file '" + filename +
                                 "'");
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    file.close();
    return content;
}

inline void print_indent(int indent) {
    for (int i = 0; i < indent; i++) {
        std::print("  ");
    }
}