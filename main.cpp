#include <print>

#include "asmgen.hpp"
#include "codegen.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "checker.hpp"
#include "irgen.hpp"
#include "utils.hpp"

void semanticAnalysis(std::unique_ptr<ProgramNode>& ast, bool debug);
std::unique_ptr<IRProgramNode> generateIR(std::unique_ptr<ProgramNode>& ast, bool debug);
void correctAsmInstructions(std::unique_ptr<AsmProgramNode>& asm_ast, bool debug);
std::ostringstream generateAsm(std::unique_ptr<AsmProgramNode>& asm_ast);

// g++ -std=c++23 -I./include lexer.cpp parser.cpp checker.cpp irgen.cpp 
// asmgen.cpp codegen.cpp main.cpp -o main.out
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::println(stderr,
                     "Usage: {} [--lex|--parse|--validate|--tacky|--codegen]"
                     "<source_file>",
                     argv[0]);
        return 1;
    }

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

    auto contents = getFileContents(filename);
    auto tokens = lexer(contents, true);
    auto ast = parse(tokens, true);
    semanticAnalysis(ast, true);
    auto ir = generateIR(ast, true);
    auto asm_ast = ir->lowerToAsm();
    correctAsmInstructions(asm_ast, true);
    auto output = generateAsm(asm_ast);

    // base filename without extension
    std::string base_filename = filename;
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

    std::string gcc_command = "gcc " + asm_filename + " -o " + base_filename;
    int result = std::system(gcc_command.c_str());

    if (result != 0) {
        std::println(stderr, "Error: gcc failed with exit code {}", result);
        return 1;
    }
    std::println("Successfully compiled to executable with result: {}", result);

    return 0;
}

/// @brief Performs semantic analysis on the AST.
/// @param ast The AST to analyze.
/// @param debug Whether to print debug information during analysis.
void semanticAnalysis(std::unique_ptr<ProgramNode>& ast, bool debug) {
    SymbolTable global_sym_table;
    ast->resolveTypes(global_sym_table);
    if (debug) {
        std::println("----- Identifier Resolution -----");
        ast->dump();
        std::println("---------------------------------");
    }

    std::string loop_label = "";
    ast->loopLabelling(loop_label);
    if (debug) {
        std::println("----- Loop Labelling -----");
        ast->dump();
        std::println("--------------------------");
    }
}


/// @brief Generates the intermediate representation (IR) from the given AST
/// @param ast The AST to convert
/// @param debug Whether to print debug information during conversion
/// @return The generated IR
std::unique_ptr<IRProgramNode> generateIR(std::unique_ptr<ProgramNode>& ast, bool debug) {
    auto ir_program = ast->generateIR();
    if (debug) {
        std::println("----------- IR Generation -----------");
        ir_program->dump();
        std::println("-------------------------------------");
    }
    return ir_program;
}

/// @brief Corrects the assembly instructions in the given ASM AST
/// @param asm_ast The ASM AST to correct
/// @param debug Whether to print debug information during correction
void correctAsmInstructions(std::unique_ptr<AsmProgramNode>& asm_ast, bool debug) {
    // maps from `AsmPseudoNode::identifier` to assigned `AsmStackNode::offset`
    std::unordered_map<std::string, int> pseudo_reg_map;
    int stack_offset = 0; // offsets grow in 4-byte (int) increments // increment by 4 then use
    asm_ast->resolvePseudoRegisters(pseudo_reg_map, stack_offset);
    if (debug) {
        std::println("After resolving pseudo registers: {}", stack_offset);
    }

    asm_ast->fixUpInstructions(stack_offset);
}

/// @brief Generates the assembly code from the given ASM AST
/// @param asm_ast The ASM AST to generate code from
/// @return The generated assembly code as a string stream
std::ostringstream generateAsm(std::unique_ptr<AsmProgramNode>& asm_ast) {
    std::ostringstream os;
    asm_ast->generateAsm(os);
    return os;
}