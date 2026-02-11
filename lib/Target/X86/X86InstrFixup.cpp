#include <nanocc/Utils.hpp>
#include "nanocc/Codegen/ASM.hpp"
#include "nanocc/Target/X86/X86TargetInfo.hpp"

// Fix Instructions -- Start
void AsmProgramNode::fixUpInstructions(const std::vector<int>& stack_sizes) {
    for (std::size_t i = 0; i < this->functions.size(); i++) {
        this->functions[i]->fixUpInstructions(stack_sizes[i]);
    }
}

void AsmFunctionNode::fixUpInstructions(const int& stack_size) {
    auto allocate_stack = std::make_unique<AsmAllocateStackNode>(stack_size);
    allocate_stack->fixUpInstructions(this->instructions);

    std::vector<std::unique_ptr<AsmInstructionNode>> new_instructions;
    new_instructions.push_back(std::move(allocate_stack));
    for (auto& instr : this->instructions) {
        if (isa<AsmMovNode>(instr.get()) || isa<AsmBinaryNode>(instr.get()) ||
            isa<AsmIdivNode>(instr.get()) || isa<AsmCmpNode>(instr.get())) {
            instr->fixUpInstructions(new_instructions);
        } else {
            new_instructions.push_back(std::move(instr));
        }
    }
    this->instructions = std::move(new_instructions);
}

/// @brief Round up the Stack Size up to the next multiple of 16 bytes.
/// Make it easier to maintain correct stack alignment during function calls.
/// https://math.stackexchange.com/questions/291468/how-to-find-the-nearest-multiple-of-16-to-my-given-number-n
///
/// TODO(VachanVY): WHATS THE POINT OF THIS, SINCE WE ARE ALREADY PADDING DURING FUNCTION CALLS?
/// Func Defination:
///     > already 16 bit alligned at start

///     > before Func Call what if the stack is not 16 bit alligned? because of of odd number of stack args?
///     > thats why we add padding of 8 bytes to make it 16 bit alligned before the call, and then remove 
///       the padding + stack args after the call. 
/// Func Call:
/// @param instructions
void AsmAllocateStackNode::fixUpInstructions(
    std::vector<std::unique_ptr<AsmInstructionNode>>& instructions) {
    this->stack_size = (this->stack_size > 0) ? ((this->stack_size + 15) & ~15) : 0;
}

void AsmMovNode::fixUpInstructions(std::vector<std::unique_ptr<AsmInstructionNode>>& instructions) {
    // if both src and dest are stack nodes,
    // we need to use a temporary dest register (%r10d)
    auto src_stack = dyn_cast<AsmStackNode>(this->src.get());
    auto dest_stack = dyn_cast<AsmStackNode>(this->dest.get());
    // both are stack nodes, use temp registers, add to instructions
    if (src_stack && dest_stack) {
        // Eg:
        // Move(Stack(-4), Stack(-8)) ==Convert=to==>
        // Move(Stack(-4), TmpReg); Move(TmpReg, Stack(-8));

        auto tmp_reg =
            std::make_shared<AsmRegisterNode>(getRegString(Reg::r10d)); // using %r10d as temporary

        auto first_move = std::make_unique<AsmMovNode>(this->src, tmp_reg);
        instructions.push_back(std::move(first_move));

        auto second_move = std::make_unique<AsmMovNode>(tmp_reg, this->dest);
        instructions.push_back(std::move(second_move));
    } else {
        // else push a copy of this instruction into the new list
        auto copy = std::make_unique<AsmMovNode>(this->src, this->dest);
        instructions.push_back(std::move(copy));
    }
}

/// @brief `add` and `sub`, instructions if both operands are stack nodes.\n
/// `imul` can't take memory address as it's destination. FIXES ALL THESE THINGS.
void AsmBinaryNode::fixUpInstructions(
    std::vector<std::unique_ptr<AsmInstructionNode>>& instructions) {
    const bool src_is_stack = isa<AsmStackNode>(this->src.get());
    const bool dest_is_stack = isa<AsmStackNode>(this->dest.get());

    if ((this->op_type == "+" || this->op_type == "-") && src_is_stack && dest_is_stack) {
        // Eg:
        // Binary(Op, Stack(-4), Stack(-8)) ==Convert=to==>
        // Move(Stack(-4), TmpReg1); Binary(Op, TmpReg1, Stack(-8));

        auto tmp_reg = std::make_shared<AsmRegisterNode>(getRegString(Reg::r10d));
        auto move_instr = std::make_unique<AsmMovNode>(this->src, tmp_reg);
        instructions.push_back(std::move(move_instr));

        auto binary_instr = std::make_unique<AsmBinaryNode>(this->op_type, tmp_reg, this->dest);
        instructions.push_back(std::move(binary_instr));
    } else if (this->op_type == "*" && dest_is_stack) {
        // Eg:
        // Imul(Const(3), Stack(-4)) ==Convert=to==>
        // Move(Stack(-4), TmpReg); Imul(Const(3), TmpReg); Move(TmpReg, Stack(-4));

        auto tmp_reg = std::make_shared<AsmRegisterNode>(getRegString(Reg::r11d));
        auto first_move = std::make_unique<AsmMovNode>(this->dest, tmp_reg);
        instructions.push_back(std::move(first_move));

        auto imul_instr = std::make_unique<AsmBinaryNode>(this->op_type, this->src, tmp_reg);
        instructions.push_back(std::move(imul_instr));

        auto final_move = std::make_unique<AsmMovNode>(tmp_reg, this->dest);
        instructions.push_back(std::move(final_move));
    } else {
        auto copy = std::make_unique<AsmBinaryNode>(this->op_type, this->src, this->dest);
        instructions.push_back(std::move(copy));
    }
}

/// @brief handles the case when divisor of `idiv` is an immediate value.
/// @param instructions
void AsmIdivNode::fixUpInstructions(
    std::vector<std::unique_ptr<AsmInstructionNode>>& instructions) {
    if (isa<AsmImmediateNode>(this->divisor.get())) {
        auto tmp_reg = std::make_shared<AsmRegisterNode>(getRegString(Reg::r10d));
        auto mov_instr = std::make_unique<AsmMovNode>(this->divisor, tmp_reg);
        instructions.push_back(std::move(mov_instr));

        auto idiv_instr = std::make_unique<AsmIdivNode>(tmp_reg);
        instructions.push_back(std::move(idiv_instr));
        return;
    }

    auto copy = std::make_unique<AsmIdivNode>(this->divisor);
    instructions.push_back(std::move(copy));
}

/// @brief `cmp` can't take both operands as memory addresses.
/// Also can't take immediate value as destination.
/// @param instructions
void AsmCmpNode::fixUpInstructions(std::vector<std::unique_ptr<AsmInstructionNode>>& instructions) {
    if (isa<AsmStackNode>(this->src1.get()) && isa<AsmStackNode>(this->src2.get())) {
        auto tmp_reg = std::make_shared<AsmRegisterNode>(getRegString(Reg::r10d));
        auto move_instr = std::make_unique<AsmMovNode>(this->src1, tmp_reg);
        instructions.push_back(std::move(move_instr));

        auto cmp_instr = std::make_unique<AsmCmpNode>(tmp_reg, this->src2);
        instructions.push_back(std::move(cmp_instr));
        return;
    } else if (isa<AsmImmediateNode>(this->src2.get())) {
        // Eg:
        // Cmp(src, Imm(5)) ==Convert=to==>
        // Move(Imm(5), TmpReg); Cmp(src, TmpReg)
        auto tmp_reg = std::make_shared<AsmRegisterNode>(getRegString(Reg::r10d));
        auto move_instr = std::make_unique<AsmMovNode>(this->src2, tmp_reg);
        instructions.push_back(std::move(move_instr));

        auto cmp_instr = std::make_unique<AsmCmpNode>(this->src1, tmp_reg);
        instructions.push_back(std::move(cmp_instr));
        return;
    }
    auto copy = std::make_unique<AsmCmpNode>(this->src1, this->src2);
    instructions.push_back(std::move(copy));
}
// Fix Instructions -- End
