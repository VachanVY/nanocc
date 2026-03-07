#include "nanocc/IR/IR.hpp"

class Pass {
public:
    virtual bool run(IRProgramNode &M) = 0;
    virtual ~Pass() = default;
};