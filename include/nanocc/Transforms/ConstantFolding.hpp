#include "nanocc/IR/IR.hpp"
#include "Base.hpp"



class ConstantFolding : public Pass {
public:
    bool run(IRProgramNode& IRProgram) override;
};