#include "nanocc/Utils.hpp"
#include "nanocc/Lexer/Lexer.hpp"
#include "nanocc/AST/AST.hpp"
#include "nanocc/Parser/Parser.hpp"
#include "nanocc/Sema/Sema.hpp"
#include "nanocc/IR/IR.hpp"
#include "nanocc/Codegen/IRToPseudoAsmPass.hpp"
#include "nanocc/Target/X86/X86TargetEmitter.hpp"

std::string getAsmOutputFromCFile(const std::string& c_filename, bool debug = false) {
    auto contents = nanocc::getFileContents(c_filename);
    auto tokens = nanocc::lexer(contents, debug);
    auto ast = nanocc::parse(tokens, debug);
    nanocc::semanticAnalysis(ast, debug);
    auto interm_repr = nanocc::generateIntermRepr(ast, debug);
    auto pseudo_asm = nanocc::intermReprToPseudoAsm(interm_repr, debug);
    nanocc::x86CorrectAssembly(pseudo_asm, debug);
    auto output = nanocc::x86EmitAssembly(pseudo_asm);
    return output.str();
}

// takes in only .c files and produces .s files
// ./nanocc -S <filename>.c -o <asm_output_file>.s
int main(int argc, char* argv[]) {
    assert(argc == 5 && std::string(argv[1]) == "-S" && std::string(argv[3]) == "-o" &&
           "Usage: ./nanocc -S <source_file.c> -o <asm_output_file.s>");
    std::string filename = argv[2];
    std::string asm_output = getAsmOutputFromCFile(filename, false);
    std::string asm_filename = argv[4];
    std::ofstream asm_file(asm_filename);
    if (!asm_file) {
        std::println(stderr, "Error: Failed to write to {}", asm_filename);
        return 1;
    }
    asm_file << asm_output;
    asm_file.close();
    return 0;
}
