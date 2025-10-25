#include <print>

#include "asmgen.hpp"
#include "codegen.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "utils.hpp"

// g++ -std=c++23 -I./include lexer.cpp parser.cpp asmgen.cpp codegen.cpp
// main.cpp -o main.out
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::println(stderr,
                     "Usage: {} [--lex|--parse|--validate|--tacky|--codegen]"
                     "<source_file>",
                     argv[0]);
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

    const std::string source = getFileContents(filename);
    auto tokens = lexer(source);
    auto ast = parse(tokens);
    ast->dump();
    auto ir_ast = ast->emit_ir();
    ir_ast->dump_ir();
    auto asm_ast = ir_ast->emit_asm();

    // maps from `AsmPseudoNode::identifier` to assigned `AsmStackNode::offset`
    std::unordered_map<std::string, int> pseudo_reg_map;
    int stack_offset = 0; // offsets grow in 4-byte (int) increments // increment by 4 then use
    asm_ast->resolvePseudoRegisters(pseudo_reg_map, stack_offset);
    std::println("After resolving pseudo registers: {}", stack_offset);

    asm_ast->fixUpInstructions(stack_offset);

    // Emit assembly to string stream
    std::ostringstream output;
    asm_ast->emit_asm(output);

    // Get the base filename without extension
    std::string base_filename = filename;
    size_t dot_pos = base_filename.rfind('.');
    if (dot_pos != std::string::npos) {
        base_filename = base_filename.substr(0, dot_pos);
    }

    // Write assembly to .s file
    std::string asm_filename = base_filename + ".s";
    std::ofstream asm_file(asm_filename);
    if (!asm_file) {
        std::println(stderr, "Error: Failed to write to {}", asm_filename);
        return 1;
    }
    asm_file << output.str();
    asm_file.close();

    std::string gcc_command = "gcc " + asm_filename + " -o " + base_filename;
    int result = std::system(gcc_command.c_str());

    if (result != 0) {
        std::println(stderr, "Error: gcc failed with exit code {}", result);
        return 1;
    }
    std::println("Successfully compiled to executable with result: {}", result);

    return 0;
}