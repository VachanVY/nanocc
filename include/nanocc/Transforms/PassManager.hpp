#pragma once

#include <functional>
#include <vector>

#include "nanocc/IR/IR.hpp"

class PassManager {
private:
  std::vector<std::function<bool(IRProgramNode&)>> Passes;

public:
  PassManager() = default;
  ~PassManager() = default;

  template <typename PassType> void AddPass(PassType Pass);

  void run(IRProgramNode& IRProgram);
};

namespace nanocc {
// dev flags, no to be used by users
struct OptFlags {
  bool enable_constant_folding = false;
  bool enable_unreachable_code_elimination = false;
  bool enable_copy_propagation = false;
  bool enable_dead_store_elimination = false;
};

void runIROptimizationPipeline(IRProgramNode& IRProgram, const OptFlags& flags);
} // namespace nanocc