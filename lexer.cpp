#include <string>
#include <regex>
#include <print>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdexcept>

std::string getFileContents(const std::string &filename) {
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

typedef struct {
    std::string token_class; // token class of each lexeme
    std::string lexemes;     // matched string for the regex of that token_class
} Token;

std::vector<std::pair<std::string, std::regex>> token_specs = {
    {"int_keyword", std::regex("int\\b")},
    {"void_keyword", std::regex("void\\b")},
    {"return_keyword", std::regex("return\\b")},

    {"identifier", std::regex("[a-zA-Z_]\\w*\\b")}, // variable/function names
    {"constant", std::regex("[0-9]+\\b")},
    {"open_brackets", std::regex("\\(")},
    {"close_brackets", std::regex("\\)")},
    {"open_brace", std::regex("\\{")},
    {"close_brace", std::regex("\\}")},
    {"semicolon", std::regex(";")}};

std::vector<Token> lexer(std::string &s) {
    std::vector<Token> tokens;
    size_t pos = 0;
    while (pos < s.length()) {
        if (std::isspace(s[pos])) {
            pos++;
            continue;
        }

        // handle comments //
        if (pos < s.length() - 1 && s[pos] == '/' && s[pos + 1] == '/') {
            // Skip until end of line
            while (pos < s.length() && s[pos] != '\n') {
                pos++;
            }
            continue;
        }

        // handle comments /* */
        if (pos < s.length() - 1 && s[pos] == '/' && s[pos + 1] == '*') {
            pos += 2; // skip /*
            while (pos < s.length() - 1) {
                if (s[pos] == '*' && s[pos + 1] == '/') {
                    pos += 2; // skip */
                    break;
                }
                pos++;
            }
            continue;
        }

        size_t match_length = 0;
        std::string match;
        std::string class_type;
        for (auto &[tokenClass, regex] : token_specs) {
            std::smatch tmp_match;
            std::string remainder = s.substr(pos); // s[pos:]
            if (std::regex_search(remainder, tmp_match, regex, std::regex_constants::match_continuous)) {
                if (tmp_match.length() > match_length) { // what if they are equal? how to decide which one?
                    class_type = tokenClass;
                    match = tmp_match.str();
                    match_length = tmp_match.length();
                }
            }
        }

        // throw error if no regex matched
        if (match_length == 0) {
            throw std::runtime_error("Unexpected Syntax error at position " + std::to_string(pos));
        }

        // remove substr of the string now that it
        pos += match_length;

        // add to `tokens`
        tokens.push_back((Token){class_type, match});
    }
    return tokens;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::println(stderr, "Usage: {} [--lex|--parse|--validate|--tacky|--codegen] <source_file>", argv[0]);
        return 1;
    }

    // Find the source file (non-flag argument)
    std::string filename;
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (!arg.starts_with("--")) {
            filename = arg;
            break;
        }
    }

    if (filename.empty()) {
        std::println(stderr, "Error: No source file specified");
        return 1;
    }

    try {
        std::string content = getFileContents(filename);
        auto tokens = lexer(content);

        for (const auto &[token_class, string] : tokens) {
            std::println("{}\t {}", token_class, string);
        }
    } catch (const std::exception &e) {
        std::println(stderr, "{}", e.what());
        return 1;
    }

    return 0;
}
