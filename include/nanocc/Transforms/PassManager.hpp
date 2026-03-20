#pragma once

#include <functional>
#include <unordered_set>
#include <vector>

#include "nanocc/IR/IR.hpp"

class PassManager {
private:
  std::vector<std::function<bool(IRProgramNode&)>> Passes;

public:
  PassManager() = default;
  ~PassManager() = default;

  template <typename PassType> void AddPass(PassType Pass);

  void run(IRProgramNode& IRProgram, bool debug = false);
};

namespace nanocc {
enum class OptPass {
  ConstantFolding,
  UnreachableCodeElim,
  CopyPropagation,
  DeadStoreElim
};
// dev flags, no to be used by users
struct OptFlags {
  std::unordered_set<OptPass> optPasses;
};

void runIROptimizationPipeline(IRProgramNode& IRProgram, const OptFlags& flags,
                               bool debug = false);
} // namespace nanocc