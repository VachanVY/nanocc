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

    // Mov & Ret instructions
    if (val) {
        auto mov_instr = std::make_unique<AsmMovNode>();
        mov_instr->src = this->val->emit_asm();
        auto dest = std::make_unique<AsmRegisterNode>();
        dest->name = "%eax"; // return value in %eax
        mov_instr->dest = std::move(dest);
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

namespace { // anonymous namespace for helper function
std::shared_ptr<AsmStackNode>
_resolvePseudoRegister(AsmPseudoNode* pseudo_src,
                       std::unordered_map<std::string, int>& pseudo_reg_map, int& stack_offset) {
    auto stack_node = std::make_shared<AsmStackNode>();
    // check if pseudo register already assigned in map
    bool assigned = pseudo_reg_map.contains(pseudo_src->identifier);

    if (assigned) {
        // if already assigned, replace it with AsmStackNode
        stack_node->offset = pseudo_reg_map[pseudo_src->identifier];
    } else {
        // else if not assigned, bump the stack pointer first, then assign that
        // location
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
        if (isa<AsmMovNode>(instr.get())) {
            instr->fixUpInstructions(new_instructions);
        } else {
            new_instructions.push_back(std::move(instr));
        }
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

        auto tmp_reg = std::make_shared<AsmRegisterNode>();
        tmp_reg->name = "%r10d"; // using %r10d as temporary

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
*/