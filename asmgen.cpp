#include <memory>
#include <print>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "asmgen.hpp"
#include "irgen.hpp"
#include "utils.hpp"

namespace { // helper functions
// ik there is something called a constructor... but will use this for now
std::shared_ptr<AsmRegisterNode> makeRegister(const char* name) {
    auto reg = std::make_shared<AsmRegisterNode>();
    reg->name = name;
    return reg;
}

} // namespace

// Emit Assembly Functions -- Start
std::unique_ptr<AsmProgramNode> IRProgramNode::emit_asm() {
    auto asm_program = std::make_unique<AsmProgramNode>();
    if (this->func) {
        asm_program->func = std::move(this->func->emit_asm());
    }
    return asm_program;
}

std::unique_ptr<AsmFunctionNode> IRFunctionNode::emit_asm() {
    auto asm_func = std::make_unique<AsmFunctionNode>();
    asm_func->name = this->var_name;

    if (!this->instructions.empty()) {
        for (const auto& instr : this->instructions) {
            // every IR instruction can emit multiple ASM instructions
            auto asm_instrs = instr->emit_asm();
            for (auto& asm_instr : asm_instrs) {
                asm_func->instructions.push_back(std::move(asm_instr));
            }
        }
    }
    return asm_func;
}

std::vector<std::unique_ptr<AsmInstructionNode>> IRRetNode::emit_asm() {
    auto instructions = std::vector<std::unique_ptr<AsmInstructionNode>>();

    // IR Ret = Asm Mov + Ret instructions
    if (val) {
        auto mov_instr = std::make_unique<AsmMovNode>();
        mov_instr->src = this->val->emit_asm();
        mov_instr->dest = makeRegister("%eax"); // return value in %eax
        instructions.push_back(std::move(mov_instr));
    }
    auto ret_instr = std::make_unique<AsmRetNode>();
    instructions.push_back(std::move(ret_instr));
    return instructions;
}

std::vector<std::unique_ptr<AsmInstructionNode>> IRUnaryNode::emit_asm() {
    auto instructions = std::vector<std::unique_ptr<AsmInstructionNode>>();

    auto dest = this->val_dest->emit_asm();

    auto mov_instr = std::make_unique<AsmMovNode>();
    mov_instr->src = this->val_src->emit_asm();
    mov_instr->dest = dest;
    instructions.push_back(std::move(mov_instr));

    auto unary_instr = std::make_unique<AsmUnaryNode>();
    unary_instr->op_type = this->op_type;
    unary_instr->operand = dest;
    instructions.push_back(std::move(unary_instr));
    return instructions;
}

std::vector<std::unique_ptr<AsmInstructionNode>> IRBinaryNode::emit_asm() {
    auto instructions = std::vector<std::unique_ptr<AsmInstructionNode>>();
    // AsmOperandNode::emit_asm returns imm/pseudo if const/variable
    auto dest = this->val_dest->emit_asm();

    // separate handling for (+, -, *) and (/, %)
    if (this->op_type == "/" || this->op_type == "%") {
        auto mov_instr = std::make_unique<AsmMovNode>();
        mov_instr->src = this->val_src1->emit_asm();
        auto eax_reg = makeRegister("%eax");
        mov_instr->dest = eax_reg;
        instructions.push_back(std::move(mov_instr));

        auto cdq_instr = std::make_unique<AsmCdqNode>();
        instructions.push_back(std::move(cdq_instr));

        auto idiv_instr = std::make_unique<AsmIdivNode>();
        idiv_instr->divisor = this->val_src2->emit_asm();
        instructions.push_back(std::move(idiv_instr));

        auto mov_res_instr = std::make_unique<AsmMovNode>();
        auto result_reg = (this->op_type == "/") ? eax_reg : makeRegister("%edx");
        mov_res_instr->src = result_reg;
        mov_res_instr->dest = dest;
        instructions.push_back(std::move(mov_res_instr));
    } else if (this->op_type == "+" || this->op_type == "-" || this->op_type == "*") {
        auto mov_instr = std::make_unique<AsmMovNode>();
        mov_instr->src = this->val_src1->emit_asm();
        mov_instr->dest = dest;
        instructions.push_back(std::move(mov_instr));

        auto binary_instr = std::make_unique<AsmBinaryNode>();
        binary_instr->op_type = this->op_type;
        binary_instr->src = this->val_src2->emit_asm();
        binary_instr->dest = dest;
        instructions.push_back(std::move(binary_instr));
    } else {
        throw std::runtime_error("IRBinaryNode::emit_asm: Unsupported op_type " + this->op_type);
    }
    return instructions;
}

std::shared_ptr<AsmOperandNode> IRConstNode::emit_asm() {
    auto asm_imm = std::make_shared<AsmImmediateNode>();
    asm_imm->value = this->val;
    return asm_imm;
}

std::shared_ptr<AsmOperandNode> IRVariableNode::emit_asm() {
    auto asm_pseudo = std::make_shared<AsmPseudoNode>();
    asm_pseudo->identifier = this->var_name;
    return asm_pseudo;
}
// Emit Assembly Functions -- End

// Resolve Pseudo Registers -- Start
void AsmProgramNode::resolvePseudoRegisters(std::unordered_map<std::string, int>& pseudo_reg_map,
                                            int& stack_offset) {
    if (this->func) {
        func->resolvePseudoRegisters(pseudo_reg_map, stack_offset);
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
_resolvePseudoRegister(AsmPseudoNode* pseudo_src, // careful: raw pointer
                       std::unordered_map<std::string, int>& pseudo_reg_map, int& stack_offset) {
    auto stack_node = std::make_shared<AsmStackNode>();
    // check if pseudo register already assigned in map
    bool assigned = pseudo_reg_map.contains(pseudo_src->identifier);

    if (assigned) {
        // if already assigned, replace it with AsmStackNode
        stack_node->offset = pseudo_reg_map[pseudo_src->identifier];
    } else {
        // else if not assigned, bump the stack pointer first,
        // then assign that new location
        stack_offset += 4;
        pseudo_reg_map[pseudo_src->identifier] = stack_offset;
        stack_node->offset = stack_offset;
    }
    return stack_node;
}
} // namespace

void AsmMovNode::resolvePseudoRegisters(std::unordered_map<std::string, int>& pseudo_reg_map,
                                        int& stack_offset) {
    if (auto pseudo_src = dyn_cast<AsmPseudoNode>(this->src.get())) {
        auto stack_node = _resolvePseudoRegister(pseudo_src, pseudo_reg_map, stack_offset);
        this->src = stack_node;
    }
    if (auto pseudo_dest = dyn_cast<AsmPseudoNode>(this->dest.get())) {
        auto stack_node = _resolvePseudoRegister(pseudo_dest, pseudo_reg_map, stack_offset);
        this->dest = stack_node;
    }
}

void AsmUnaryNode::resolvePseudoRegisters(std::unordered_map<std::string, int>& pseudo_reg_map,
                                          int& stack_offset) {
    if (auto pseudo_operand = dyn_cast<AsmPseudoNode>(this->operand.get())) {
        auto stack_node = _resolvePseudoRegister(pseudo_operand, pseudo_reg_map, stack_offset);
        this->operand = stack_node;
    }
}

void AsmBinaryNode::resolvePseudoRegisters(std::unordered_map<std::string, int>& pseudo_reg_map,
                                           int& stack_offset) {
    if (auto pseudo_left = dyn_cast<AsmPseudoNode>(this->src.get())) {
        auto stack_node = _resolvePseudoRegister(pseudo_left, pseudo_reg_map, stack_offset);
        this->src = stack_node;
    }
    if (auto pseudo_right = dyn_cast<AsmPseudoNode>(this->dest.get())) {
        auto stack_node = _resolvePseudoRegister(pseudo_right, pseudo_reg_map, stack_offset);
        this->dest = stack_node;
    }
}

void AsmIdivNode::resolvePseudoRegisters(std::unordered_map<std::string, int>& pseudo_reg_map,
                                         int& stack_offset) {
    if (auto pseudo_divisor = dyn_cast<AsmPseudoNode>(this->divisor.get())) {
        auto stack_node = _resolvePseudoRegister(pseudo_divisor, pseudo_reg_map, stack_offset);
        this->divisor = stack_node;
    }
}
// Resolve Pseudo Registers -- End

// Fix Instructions -- Start
void AsmProgramNode::fixUpInstructions(const int& stack_size) {
    if (this->func) {
        this->func->fixUpInstructions(stack_size);
    }
}

void AsmFunctionNode::fixUpInstructions(const int& stack_size) {
    auto allocate_stack = std::make_unique<AsmAllocateStackNode>();
    allocate_stack->stack_size = stack_size;

    std::vector<std::unique_ptr<AsmInstructionNode>> new_instructions;
    new_instructions.push_back(std::move(allocate_stack));
    for (auto& instr : this->instructions) {
        if (auto* mov = dyn_cast<AsmMovNode>(instr.get())) {
            mov->fixUpInstructions(new_instructions);
            continue;
        }

        if (auto* binary = dyn_cast<AsmBinaryNode>(instr.get())) {
            binary->fixUpInstructions(new_instructions);
            continue;
        }

        if (auto* idiv = dyn_cast<AsmIdivNode>(instr.get())) {
            idiv->fixUpInstructions(new_instructions);
            continue;
        }

        new_instructions.push_back(std::move(instr));
    }
    this->instructions = std::move(new_instructions);
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
        // Move(Stack(-4), TmpReg); Move(TmpReg, Stack(-8))

        auto tmp_reg = makeRegister("%r10d"); // using %r10d as temporary

        auto first_move = std::make_unique<AsmMovNode>();
        first_move->src = this->src;
        first_move->dest = tmp_reg;
        instructions.push_back(std::move(first_move));

        auto second_move = std::make_unique<AsmMovNode>();
        second_move->src = tmp_reg;
        second_move->dest = this->dest;
        instructions.push_back(std::move(second_move));
    } else {
        // else push a copy of this instruction into the new list
        // TODO(VachanVY): Can I prevent this extra copy?
        auto copy = std::make_unique<AsmMovNode>();
        copy->src = this->src;
        copy->dest = this->dest;
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

        auto tmp_reg = makeRegister("%r10d");
        auto move_instr = std::make_unique<AsmMovNode>();
        move_instr->src = this->src;
        move_instr->dest = tmp_reg;
        instructions.push_back(std::move(move_instr));

        auto binary_instr = std::make_unique<AsmBinaryNode>();
        binary_instr->op_type = this->op_type;
        binary_instr->src = tmp_reg;
        binary_instr->dest = this->dest;
        instructions.push_back(std::move(binary_instr));
    } else if (this->op_type == "*" && dest_is_stack) {
        // Eg:
        // Imul(Const(3), Stack(-4)) ==Convert=to==>
        // Move(Stack(-4), TmpReg); Imul(Const(3), TmpReg); Move(TmpReg, Stack(-4));

        auto tmp_reg = makeRegister("%r11d");
        auto move_instr = std::make_unique<AsmMovNode>();
        move_instr->src = this->dest;
        move_instr->dest = tmp_reg;
        instructions.push_back(std::move(move_instr));

        auto imul_instr = std::make_unique<AsmBinaryNode>();
        imul_instr->op_type = this->op_type;
        imul_instr->src = this->src;
        imul_instr->dest = tmp_reg;
        instructions.push_back(std::move(imul_instr));

        auto final_move = std::make_unique<AsmMovNode>();
        final_move->src = tmp_reg;
        final_move->dest = this->dest;
        instructions.push_back(std::move(final_move));
    } else {
        auto copy = std::make_unique<AsmBinaryNode>();
        copy->op_type = this->op_type;
        copy->src = this->src;
        copy->dest = this->dest;
        instructions.push_back(std::move(copy));
    }
}

/// @brief handles the case when divisor of `idiv` is an immediate value.
void AsmIdivNode::fixUpInstructions(
    std::vector<std::unique_ptr<AsmInstructionNode>>& instructions) {
    if (isa<AsmImmediateNode>(this->divisor.get())) {
        auto tmp_reg = makeRegister("%r10d");
        auto mov_instr = std::make_unique<AsmMovNode>();
        mov_instr->src = this->divisor;
        mov_instr->dest = tmp_reg;
        instructions.push_back(std::move(mov_instr));

        auto idiv_instr = std::make_unique<AsmIdivNode>();
        idiv_instr->divisor = tmp_reg;
        instructions.push_back(std::move(idiv_instr));
        return;
    }

    auto copy = std::make_unique<AsmIdivNode>();
    copy->divisor = this->divisor;
    instructions.push_back(std::move(copy));
}
// Fix Instructions -- End

/*
#include "lexer.hpp"
#include "parser.hpp"
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::println(stderr,
                        "Usage: {} [--lex|--parse|--validate|--tacky|--codegen]"
                        "<source_file>",
                        argv[0]);
        return 1;
    }

    // Find the source file (non-flag argument)
    std::string filename;
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (!arg.starts_with("--")) {
            filename = arg;
            break;
        }
    }

    if (filename.empty()) {
        std::println(stderr, "Error: No source file specified");
        return 1;
    }
    auto contents = getFileContents(filename);
    auto tokens = lexer(contents);
    size_t i = 0;
    for (const auto& [token_class, lexemes] : tokens) {
        std::println("{}, {}, {}", i++, token_class, lexemes);
    }
    auto ast = parse(tokens);
    ast->dump();
    auto ir_ast = ast->emit_ir();
    ir_ast->dump_ir();
    auto asm_ast = ir_ast->emit_asm();

    // maps from `AsmPseudoNode::identifier` to assigned `AsmStackNode::offset`
    std::unordered_map<std::string, int> pseudo_reg_map;
    int stack_offset = 0; // offsets grow in 4-byte (int) increments // increment by 4 then use
    asm_ast->resolvePseudoRegisters(pseudo_reg_map, stack_offset);
    std::println("After resolving pseudo registers: {}", stack_offset);

    asm_ast->fixUpInstructions(stack_offset);
    return 0;
}
// */