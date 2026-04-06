#include <fstream>
#include <print>
#include <sstream>
#include <string>

#include "nanocc/Utils/Utils.hpp"

#define RESET "\033[0m"
#define BOLD "\033[1m"
#define RED "\033[31m"
#define YELLOW "\033[33m"
#define CYAN "\033[36m"
#define GREY "\033[90m"

void printIndent(int indent) {
  for (int i = 0; i < indent; i++) {
    std::printf("  ");
  }
}

std::string getUniqueName(const std::string& prefix) {
  static std::size_t counter = 0;
  // is the dot (.) okay? keeps it unique...
  return prefix + "." + std::to_string(counter++);
};

std::string getLabelName(const std::string& prefix) {
  assert(!prefix.empty() && "Label prefix cannot be empty");
  static std::size_t label_counter = 0;
  return prefix + "." + std::to_string(label_counter++);
};

namespace {
std::string getLine(const std::string& filename, size_t line) {
  std::ifstream file(filename);
  std::string curr;
  for (size_t i = 1; i <= line; ++i) {
    if (!std::getline(file, curr))
      return "";
  }
  return curr;
}
} // namespace

namespace nanocc {
std::string getFileContents(const std::string& filename) {
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

void raiseError(const std::string& filename, size_t line, size_t column,
                const char* errorStage, const std::string& errorMessage) {
  if (column > 0) {
    std::println(stderr, "{}{}:{}:{}:{} {}{}{} Error:{} {}", BOLD, filename,
                 line, column, RESET, BOLD, RED, errorStage, RESET,
                 errorMessage);
  } else {
    std::println(stderr, "{}{}:{}:{} {}{}{} Error:{} {}", BOLD, filename, line,
                 RESET, BOLD, RED, errorStage, RESET, errorMessage);
  }

  std::string src = getLine(filename, line);
  if (!src.empty()) {
    std::println(stderr, "{}{} |{} {}", GREY, line, RESET, src);

    if (column > 0) {
      std::print(stderr, "{}  |{} ", GREY, RESET);

      for (size_t i = 1; i < column; ++i) {
        if (i - 1 < src.size() && src[i - 1] == '\t')
          std::print(stderr, "\t");
        else
          std::print(stderr, " ");
      }

      std::println(stderr, "{}{}^{}", BOLD, RED, RESET);
    }
  }

  std::exit(1);
}
} // namespace nanocc