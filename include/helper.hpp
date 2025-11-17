#pragma once
#include "asmgen.hpp"
#include "codegen.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "checker.hpp"
#include "irgen.hpp"

/// @brief Performs semantic analysis on the AST.
/// @param ast The AST to analyze.
/// @param debug Whether to print debug information during analysis.
inline void semanticAnalysis(std::unique_ptr<ProgramNode>& ast,
                             TypeCheckerSymbolTable& global_type_checker_map, bool debug) {
    IdentifierMap global_sym_table;
    ast->resolveTypes(global_sym_table);
    if (debug) {
        std::println("----- Identifier Resolution -----");
        ast->dump();
        std::println("---------------------------------");
    }

    ast->checkTypes(global_type_checker_map);
    if (debug) {
        std::println("----- Type Checking -----");
        ast->dump();
        std::println("-------------------------");
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
inline std::unique_ptr<IRProgramNode> generateIR(std::unique_ptr<ProgramNode>& ast, bool debug) {
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
inline void correctAsmInstructions(std::unique_ptr<AsmProgramNode>& asm_ast, bool debug) {
    // maps from `AsmPseudoNode::identifier` to assigned `AsmStackNode::offset`
    std::unordered_map<std::string, int> pseudo_reg_map;
    // for every function, stack offset starts at 0
    int num_functions = asm_ast->functions.size();
    // offsets grow in 4-byte (int) increments // increment by 4 then use
    std::vector<int> stack_offsets(num_functions, 0);
    asm_ast->resolvePseudoRegisters(pseudo_reg_map, stack_offsets);
    if (debug) {
        for (std::size_t i = 0; i < stack_offsets.size(); i++) {
            std::println("Function {}: Stack Size = {}", asm_ast->functions[i]->name,
                         stack_offsets[i]);
        }
    }
    asm_ast->fixUpInstructions(stack_offsets);
}

/// @brief Generates the assembly code from the given ASM AST
/// @param asm_ast The ASM AST to generate code from
/// @return The generated assembly code as a string stream
inline std::ostringstream generateAsm(std::unique_ptr<AsmProgramNode>& asm_ast) {
    std::ostringstream os;
    asm_ast->generateAsm(os);
    return os;
}