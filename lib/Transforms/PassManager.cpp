#include "nanocc/Transforms/PassManager.hpp"
#include "nanocc/Transforms/ConstantFolding.hpp"      // ConstantFoldInstructions
#include "nanocc/Transforms/SimplifyCFG.hpp"          // UnreachableCodeElimination
#include "nanocc/Transforms/CopyPropagation.hpp"      // CopyPropagate
#include "nanocc/Transforms/DeadStoreElimination.hpp" // DeadStoreElimination

template <typename PassType> 
void PassManager::AddPass(PassType Pass) {
  Passes.push_back(Pass);
}

void PassManager::run(IRProgramNode& IRProgram) {
  do {
    bool changed = false;
    for (auto& Pass : Passes) {
      changed |= Pass(IRProgram);
    }
    // stop if no changes were made by any pass
    if (!changed) break;
  } while (true);
}

namespace nanocc {
void runIROptimizationPipeline(IRProgramNode& IRProgram, const OptFlags& flags) {
  PassManager PM;
  if (flags.enable_constant_folding) {
    PM.AddPass(ConstantFoldInstructions);
  }
  if (flags.enable_unreachable_code_elimination) {
    PM.AddPass(SimplifyCFG);
  }
  if (flags.enable_copy_propagation) {
    PM.AddPass(CopyPropagate);
  }
  if (flags.enable_dead_store_elimination) {
    PM.AddPass(DeadStoreElimination);
  }
  PM.run(IRProgram);
}
} // namespace nanocc
