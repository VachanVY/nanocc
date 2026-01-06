#ifndef NANOCC_CODEGEN_IRTOPSEUDOASMPASS_HPP
#define NANOCC_CODEGEN_IRTOPSEUDOASMPASS_HPP

#include <memory>

#include "nanocc/IR/IR.hpp"
#include "nanocc/Codegen/ASM.hpp"

namespace nanocc {
std::unique_ptr<AsmProgramNode> intermReprToPseudoAsm(std::unique_ptr<IRProgramNode>& ir_ast,
                        bool debug = false);
} // namespace nanocc

#endif // NANOCC_CODEGEN_IRTOPSEUDOASMPASS_HPP