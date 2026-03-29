#include <print>

#include "nanocc/IR/IRDump.hpp"

#include "nanocc/Transforms/PassManager.hpp"

#include "nanocc/Transforms/ConstantFolding.hpp" // ConstantFoldInstructions
#include "nanocc/Transforms/CopyPropagation.hpp" // CopyPropagate
#include "nanocc/Transforms/DeadStoreElimination.hpp" // DeadStoreElimination
#include "nanocc/Transforms/SimplifyCFG.hpp" // UnreachableCodeElimination

template <typename PassType> void PassManager::AddPass(PassType Pass) {
  Passes.push_back(Pass);
}

void PassManager::run(IRProgramNode& IRProgram, bool debug) {
  int iteration = 1;
  do {
    bool changed = false;
    for (auto& Pass : Passes) {
      changed |= Pass(IRProgram);
    }
    // stop if no changes were made by any pass
    if (!changed)
      break;
    if (debug) {
      std::println("Optimization Iteration: {}", iteration++);
      IRGen::programNodeIRDump(IRProgram, 0);
    }
  } while (true);
}

namespace nanocc {

void runIROptimizationPipeline(IRProgramNode& IRProgram, const OptFlags& flags,
                               bool debug) {
  PassManager PM;
  if (flags.optPasses.contains(OptPass::ConstantFolding)) {
    PM.AddPass(ConstantFoldInstructions);
  }
  if (flags.optPasses.contains(OptPass::UnreachableCodeElim)) {
    PM.AddPass(SimplifyCFG);
  }
  if (flags.optPasses.contains(OptPass::CopyPropagation)) {
    PM.AddPass(CopyPropagate);
  }
  if (flags.optPasses.contains(OptPass::DeadStoreElim)) {
    PM.AddPass(DeadStoreElimination);
  }
  PM.run(IRProgram, debug);
}
} // namespace nanocc
