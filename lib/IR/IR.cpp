#include "print"

#include "nanocc/IR/IR.hpp"

#include "IRHelper.hpp"

namespace nanocc {
std::unique_ptr<IRProgramNode> generateIntermRepr(std::unique_ptr<ProgramNode>& ast, bool debug) {
    auto interm_repr = IRGen::programNodeIRGen(ast);
    if (debug) {
        std::println("----------- IR Generation -----------");
        IRGen::programNodeIRDump(interm_repr, 0);
        std::println("-------------------------------------");
    }
    return interm_repr;
}
} // namespace nanocc
