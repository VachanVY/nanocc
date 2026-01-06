#ifndef NANOCC_TARGET_X86_X86TARGETEMITTER_HPP
#define NANOCC_TARGET_X86_X86TARGETEMITTER_HPP

#include <memory>
#include <sstream>

#include "nanocc/Codegen/ASM.hpp"

namespace nanocc {
    /// @brief Corrects the assembly instructions in the given ASM AST
    /// @param asm_ast The ASM AST to correct
    /// @param debug Whether to print debug information during correction
    void x86CorrectAssembly(std::unique_ptr<AsmProgramNode>& asm_ast, bool debug);

    /// @brief Generates the assembly code from the given ASM AST
    /// @param asm_ast The ASM AST to generate code from
    /// @return The generated assembly code as a string stream
    std::ostringstream x86EmitAssembly(const std::unique_ptr<AsmProgramNode>& asm_program);
} // namespace nanocc

#endif // NANOCC_TARGET_X86_X86TARGETEMITTER_HPP