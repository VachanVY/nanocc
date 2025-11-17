/*
AI GENERATED CODE - I was too lazy to see how the 
test framework (writing-a-c-compiler-tests) worked...
*/

#include <print>

#include "asmgen.hpp"
#include "codegen.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "checker.hpp"
#include "irgen.hpp"
#include "utils.hpp"
#include "helper.hpp"

// g++ -std=c++23 -I./include lexer.cpp parser.cpp checker.cpp irgen.cpp
// asmgen.cpp codegen.cpp main.cpp -o main.out
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::println(stderr,
                     "Usage: {} [--lex|--parse|--validate|--tacky|--codegen|-c]"
                     "<source_file>",
                     argv[0]);
        return 1;
    }

    bool compile_only = false; // -c flag: compile to object file only
    std::string filename;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-c") {
            compile_only = true;
        } else if (!arg.starts_with("-")) {
            filename = arg;
            break;
        }
    }

    if (filename.empty()) {
        std::println(stderr, "Error: No source file specified");
        return 1;
    }

    // Keep original filename for output
    std::string original_filename = filename;

    auto contents = getFileContents(filename);
    auto tokens = lexer(contents, true);
    auto ast = parse(tokens, true);
    semanticAnalysis(ast, global_type_checker_map, true);
    auto ir = generateIR(ast, true);
    auto asm_ast = ir->lowerToAsm();
    correctAsmInstructions(asm_ast, true);
    auto output = generateAsm(asm_ast);

    // base filename without extension
    std::string base_filename = original_filename;
    size_t dot_pos = base_filename.rfind('.');
    if (dot_pos != std::string::npos) {
        base_filename = base_filename.substr(0, dot_pos);
    }

    // write asm to .s file
    std::string asm_filename = base_filename + ".s";
    std::ofstream asm_file(asm_filename);
    if (!asm_file) {
        std::println(stderr, "Error: Failed to write to {}", asm_filename);
        return 1;
    }
    asm_file << output.str();
    asm_file.close();

    if (compile_only) {
        // Compile to object file only (for -c flag)
        std::string obj_filename = base_filename + ".o";
        std::string gcc_command = "gcc -c " + asm_filename + " -o " + obj_filename;
        int result = std::system(gcc_command.c_str());

        if (result != 0) {
            std::println(stderr, "Error: gcc failed with exit code {}", result);
            return 1;
        }
        return 0;
    } else {
        // Link to create executable
        std::string gcc_command = "gcc " + asm_filename + " -o " + base_filename;
        int result = std::system(gcc_command.c_str());

        if (result != 0) {
            std::println(stderr, "Error: gcc failed with exit code {}", result);
            return 1;
        }
        std::println("Successfully compiled to executable with result: {}", result);
        return 0;
    }
}
