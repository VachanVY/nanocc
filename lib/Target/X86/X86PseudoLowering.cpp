#include "nanocc/Utils.hpp"
#include "nanocc/Codegen/ASM.hpp"


// Resolve Pseudo Registers -- Start
void AsmProgramNode::resolvePseudoRegisters(std::unordered_map<std::string, int>& pseudo_reg_map,
                                            std::vector<int>& stack_offset) {
    for (std::size_t i = 0; i < this->functions.size(); i++) {
        this->functions[i]->resolvePseudoRegisters(pseudo_reg_map, stack_offset[i]);
    }
}

void AsmFunctionNode::resolvePseudoRegisters(std::unordered_map<std::string, int>& pseudo_reg_map,
                                             int& stack_offset) {
    for (auto& instr : this->instructions) {
        instr->resolvePseudoRegisters(pseudo_reg_map, stack_offset);
    }
}

namespace { // no-name namespace for helper function
/// @brief Resolves a pseudo register to a stack node.
/// @param pseudo_src The pseudo register to resolve.
/// @param pseudo_reg_map The map of pseudo registers to stack offsets.
/// @param stack_offset The current stack offset.
/// @return A shared pointer to the resolved stack node.
std::shared_ptr<AsmStackNode>
resolvePseudoRegister(AsmPseudoNode* pseudo_src, // TODO(VachanVY): can we avoid using raw pointer?
                      std::unordered_map<std::string, int>& pseudo_reg_map, int& stack_offset) {
    // check if pseudo register already assigned in map
    bool assigned = pseudo_reg_map.contains(pseudo_src->identifier);

    if (assigned) {
        // if already assigned, replace it with AsmStackNode
        // local variables (negative offsets from %rbp)
        return std::make_shared<AsmStackNode>(-pseudo_reg_map[pseudo_src->identifier]);
    } else {
        // else if not assigned, bump the stack pointer first,
        // then assign that to new location
        stack_offset += 4;
        pseudo_reg_map[pseudo_src->identifier] = stack_offset;
        // local variables (negative offsets from %rbp)
        return std::make_shared<AsmStackNode>(-stack_offset);
    }
}
} // namespace

void AsmMovNode::resolvePseudoRegisters(std::unordered_map<std::string, int>& pseudo_reg_map,
                                        int& stack_offset) {
    if (auto pseudo_src = dyn_cast<AsmPseudoNode>(this->src.get())) {
        this->src = resolvePseudoRegister(pseudo_src, pseudo_reg_map, stack_offset);
    }
    if (auto pseudo_dest = dyn_cast<AsmPseudoNode>(this->dest.get())) {
        this->dest = resolvePseudoRegister(pseudo_dest, pseudo_reg_map, stack_offset);
    }
}

void AsmUnaryNode::resolvePseudoRegisters(std::unordered_map<std::string, int>& pseudo_reg_map,
                                          int& stack_offset) {
    if (auto pseudo_operand = dyn_cast<AsmPseudoNode>(this->operand.get())) {
        this->operand = resolvePseudoRegister(pseudo_operand, pseudo_reg_map, stack_offset);
    }
}

void AsmBinaryNode::resolvePseudoRegisters(std::unordered_map<std::string, int>& pseudo_reg_map,
                                           int& stack_offset) {
    if (auto pseudo_left = dyn_cast<AsmPseudoNode>(this->src.get())) {
        this->src = resolvePseudoRegister(pseudo_left, pseudo_reg_map, stack_offset);
    }
    if (auto pseudo_right = dyn_cast<AsmPseudoNode>(this->dest.get())) {
        this->dest = resolvePseudoRegister(pseudo_right, pseudo_reg_map, stack_offset);
    }
}

void AsmCmpNode::resolvePseudoRegisters(std::unordered_map<std::string, int>& pseudo_reg_map,
                                        int& stack_offset) {
    if (auto pseudo_src = dyn_cast<AsmPseudoNode>(this->src1.get())) {
        this->src1 = resolvePseudoRegister(pseudo_src, pseudo_reg_map, stack_offset);
    }
    if (auto pseudo_dest = dyn_cast<AsmPseudoNode>(this->src2.get())) {
        this->src2 = resolvePseudoRegister(pseudo_dest, pseudo_reg_map, stack_offset);
    }
}

void AsmIdivNode::resolvePseudoRegisters(std::unordered_map<std::string, int>& pseudo_reg_map,
                                         int& stack_offset) {
    if (auto pseudo_divisor = dyn_cast<AsmPseudoNode>(this->divisor.get())) {
        this->divisor = resolvePseudoRegister(pseudo_divisor, pseudo_reg_map, stack_offset);
    }
}

void AsmSetCCNode::resolvePseudoRegisters(std::unordered_map<std::string, int>& pseudo_reg_map,
                                          int& stack_offset) {
    if (auto pseudo_dest = dyn_cast<AsmPseudoNode>(this->dest.get())) {
        this->dest = resolvePseudoRegister(pseudo_dest, pseudo_reg_map, stack_offset);
    }
}

void AsmPushNode::resolvePseudoRegisters(std::unordered_map<std::string, int>& pseudo_reg_map,
                                         int& stack_offset) {
    if (auto pseudo_operand = dyn_cast<AsmPseudoNode>(this->operand.get())) {
        this->operand = resolvePseudoRegister(pseudo_operand, pseudo_reg_map, stack_offset);
    }
}
// Resolve Pseudo Registers -- End