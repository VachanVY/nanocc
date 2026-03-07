#pragma once

#include "nanocc/IR/IR.hpp"

namespace nanocc {
    bool DeadStoreElimination(IRProgramNode& IRProgram);
} // namespace nanocc
