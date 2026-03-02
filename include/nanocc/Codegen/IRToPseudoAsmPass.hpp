#pragma once

#include <memory>

#include "nanocc/Codegen/ASM.hpp"
#include "nanocc/IR/IR.hpp"

namespace nanocc {
std::unique_ptr<AsmProgramNode>
intermReprToPseudoAsm(std::unique_ptr<IRProgramNode> &ir_ast,
                      bool debug = false);
} // namespace nanocc