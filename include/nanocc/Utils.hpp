#ifndef NANOCC_UTILS_HPP
#define NANOCC_UTILS_HPP

#include <cassert>
#include <print>
#include <fstream>
#include <sstream>
#include <string>

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
    static std::size_t label_counter = 0;
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

namespace nanocc {
/// @brief Reads the contents of a file into a string after preprocessing it with GCC.
/// @param filename The name of the file to read.
/// @return The preprocessed contents of the file as a string.
inline std::string getFileContents(const std::string& filename) {
    // generate unique temp file name
    auto hash = std::hash<std::string>{}(filename);
    std::string preprocessed_file = "/tmp/preprocessed_" + std::to_string(hash) + ".i";

    // gcc preprocessor
    std::string preprocess_cmd = "gcc -E -P " + filename + " -o " + preprocessed_file;
    if (std::system(preprocess_cmd.c_str()) != 0) {
        throw std::runtime_error("Error: GCC preprocessor failed for file '" + filename + "'");
    }

    std::ifstream file(preprocessed_file);
    if (!file.is_open()) {
        throw std::runtime_error("Error: Could not open preprocessed file '" + preprocessed_file +
                                 "'");
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    std::remove(preprocessed_file.c_str());

    return buffer.str();
}
} // namespace nanocc

#endif // NANOCC_UTILS_HPP