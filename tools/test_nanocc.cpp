#include <print>
#include <fstream>

#include "CompilerPipeline.hpp"

// garbage test file for testing the complete nanocc pipeline
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

    std::string original_filename = filename;

    std::string asm_output = getAsmOutputFromCFile(filename, true);

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
    asm_file << asm_output;
    asm_file.close();

    if (compile_only) {
        // to object file only (for -c flag)
        std::string obj_filename = base_filename + ".o";
        std::string gcc_command = "gcc -c " + asm_filename + " -o " + obj_filename;
        int result = std::system(gcc_command.c_str());

        if (result != 0) {
            std::println(stderr, "Error: gcc failed with exit code {}", result);
            return 1;
        }
    } else {
        // link => executable
        std::string gcc_command = "gcc " + asm_filename + " -o " + base_filename;
        int result = std::system(gcc_command.c_str());

        if (result != 0) {
            std::println(stderr, "Error: gcc failed with exit code {}", result);
            return 1;
        }
        std::println("Successfully compiled to executable with result: {}", result);
    }
    return 0;
}
