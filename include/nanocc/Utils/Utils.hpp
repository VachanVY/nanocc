#pragma once

#include <cassert>
#include <fstream>
#include <print>
#include <sstream>
#include <string>

constexpr const char* TAB4 = "    ";

inline void printIndent(int indent) {
  for (int i = 0; i < indent; i++) {
    std::printf("  ");
  }
}

inline std::string getUniqueName(const std::string& prefix) {
  static std::size_t counter = 0;
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
template <typename To, typename From> bool isa(const From* u) {
  return u != nullptr && To::classof(u);
}

template <typename To, typename From> To* cast(From* Val) {
  assert(isa<To>(Val) && "cast<T>() argument of wrong type!");
  return static_cast<To*>(Val);
}

template <typename To, typename From> const To* cast(const From* Val) {
  assert(isa<To>(Val) && "cast<T>() argument of wrong type!");
  return static_cast<const To*>(Val);
}

template <typename To, typename From> To* dyn_cast(From* u) {
  return isa<To>(u) ? cast<To>(u) : nullptr;
}

template <typename To, typename From> const To* dyn_cast(const From* u) {
  return isa<To>(u) ? cast<To>(u) : nullptr;
}

namespace nanocc {
/// @brief Reads the contents of a file into a string
/// after preprocessing it with GCC.
/// @param filename The name of the file to read.
/// @return The preprocessed contents of the file as a string.
inline std::string getFileContents(const std::string& filename) {
  std::string command = "gcc -E " + filename;

  FILE* pipe = popen(command.c_str(), "r");
  if (!pipe) {
    throw std::runtime_error("Error: Failed to run GCC preprocessor");
  }

  std::string result;
  char buffer[4096];

  while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
    result += buffer;
  }

  int status = pclose(pipe);
  if (status != 0) {
    throw std::runtime_error(
        std::format("Error: GCC preprocessor failed for file '{}'", filename));
  }

  return result;
}

/// @brief Custom error reporting function that prints the error
/// message along with the source location and stage of the compiler
/// where the error occurred, then exits the program.
/// @param filename The name of the file where the error occurred.
/// @param line The line number where the error occurred.
/// @param column The column number where the error occurred.
/// @param errorStage Lexing, Parsing, Semantic Analysis, etc
/// @param errorMessage The error message to display.
inline void raiseError(std::string filename, size_t line, size_t column,
                       const char* errorStage, std::string&& errorMessage) {
  std::print(stderr, "{}:{}:", filename, line);
  if (column > 0)
    std::print(stderr, "{}:", column);
  std::println(stderr, " {} Error:\n {}", errorStage, errorMessage);
  std::exit(1);
}
} // namespace nanocc