#pragma once

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
