#include <vector>
#include <memory>
#include <algorithm>
#include <stdexcept>
#include <unordered_map>

#include "nanocc/Utils.hpp"
#include "nanocc/IR/IR.hpp"
#include "nanocc/Codegen/ASM.hpp"
#include "nanocc/Target/X86/X86TargetInfo.hpp"


// Emit Assembly Functions -- Start

/// @brief Lowers the given IR program to an ASM program.
/// @return The lowered ASM program.
std::unique_ptr<AsmProgramNode> IRProgramNode::lowerToAsm() {
    auto asm_program = std::make_unique<AsmProgramNode>();
    for (const auto& function : this->functions) {
        auto asm_function = function->lowerToAsm();
        asm_program->functions.push_back(std::move(asm_function));
    }
    return asm_program;
}

// function caller: calls other functions if present
std::unique_ptr<AsmFunctionNode> IRFunctionNode::lowerToAsm() {
    auto asm_func = std::make_unique<AsmFunctionNode>(this->func_name);

    // place all parameters currently in the registers on to the stack,
    // the registers might get used by any function call
    // so to not lose their values, we push them onto the stack
    // we will OPTIMIZE this later in REGISTER ALLOCATION phase
    /// MOVING PARAMS FROM REGISTERS TO PSEUDO REGISTERS, USE 32 BIT REGISTERS
    int num_args = this->parameters.size();
    Reg arg_resisters[6] = {Reg::edi, Reg::esi, Reg::edx, Reg::ecx, Reg::r8d, Reg::r9d};
    for (int i = 0; i < std::min(num_args, 6); i++) { // `0` to `5` `if num_args >=6`
        auto psedo_reg = std::make_shared<AsmPseudoNode>(this->parameters[i]);
        asm_func->instructions.push_back(std::make_unique<AsmMovNode>(
            std::make_shared<AsmRegisterNode>(getRegString(arg_resisters[i])), psedo_reg));
    }

    // how do we know the stack locations for the next arguments beyond 6 are placed in Stack(16),
    // Stack(24) and so on? => when this function was called (`call func`), the address of the next
    // instruction is pushed to the stack, then the control is transferred to the function.
    // Then in the function prologue, we push %rbp.
    // So 0(%rbp) = saved rbp, 8(%rbp) = return address,
    // 16(%rbp) = 7th arg, 24(%rbp) = 8th arg, and so on.
    // The first 6 args are in registers as per the calling convention.
    for (int i = 6; i < num_args; i++) { // `6` to `num_args-1`
        auto psedo_reg = std::make_shared<AsmPseudoNode>(this->parameters[i]);
        auto stack_loc = std::make_shared<AsmStackNode>(8 * (i - 4)); // +16, +24, +32, ...
        asm_func->instructions.push_back(std::make_unique<AsmMovNode>(stack_loc, psedo_reg));
    }

    for (const auto& instr : this->ir_instructions) {
        // every IR instruction can emit multiple ASM instructions
        auto asm_instrs = instr->lowerToAsm();
        for (auto& asm_instr : asm_instrs) {
            asm_func->instructions.push_back(std::move(asm_instr));
        }
    }
    return asm_func;
}

std::vector<std::unique_ptr<AsmInstructionNode>> IRRetNode::lowerToAsm() {
    auto instructions = std::vector<std::unique_ptr<AsmInstructionNode>>();

    // IR Ret = Asm Mov + Ret instructions
    if (ret_val) {
        auto src = this->ret_val->lowerToAsm();
        auto dest = std::make_shared<AsmRegisterNode>(getRegString(Reg::eax));
        auto mov_instr = std::make_unique<AsmMovNode>(src, dest);
        instructions.push_back(std::move(mov_instr));
    }
    auto ret_instr = std::make_unique<AsmRetNode>();
    instructions.push_back(std::move(ret_instr));
    return instructions;
}

std::vector<std::unique_ptr<AsmInstructionNode>> IRUnaryNode::lowerToAsm() {
    auto instructions = std::vector<std::unique_ptr<AsmInstructionNode>>();
    if (this->op_type == "!") { // relational op
        auto src = this->val_src->lowerToAsm();
        auto dest = this->val_dest->lowerToAsm();

        auto cmp_instr = std::make_unique<AsmCmpNode>(std::make_shared<AsmImmediateNode>("0"), src);
        instructions.push_back(std::move(cmp_instr));

        auto mov_instr =
            std::make_unique<AsmMovNode>(std::make_shared<AsmImmediateNode>("0"), dest);
        instructions.push_back(std::move(mov_instr));

        auto setcc_instr = std::make_unique<AsmSetCCNode>("e", dest);
        instructions.push_back(std::move(setcc_instr));
        return instructions;
    } else if (this->op_type == "~" || this->op_type == "-") {
        auto dest = this->val_dest->lowerToAsm();
        auto src = this->val_src->lowerToAsm();

        auto mov_instr = std::make_unique<AsmMovNode>(src, dest);
        instructions.push_back(std::move(mov_instr));

        auto unary_instr = std::make_unique<AsmUnaryNode>(this->op_type, dest);
        instructions.push_back(std::move(unary_instr));
        return instructions;
    } else {
        throw std::runtime_error("IRUnaryNode::lowerToAsm: Unsupported op_type " + this->op_type);
    }
}

namespace {
static const std::unordered_map<std::string, std::string> RELATIONAL_OPS = {
    {"==", "e"}, {"!=", "ne"}, {"<", "l"}, {"<=", "le"}, {">", "g"}, {">=", "ge"}};

constexpr std::string getCondCode(const std::string& op_type) {
    auto it = RELATIONAL_OPS.find(op_type);
    if (it == RELATIONAL_OPS.end()) {
        throw std::runtime_error("getCondCode: Unsupported relational operator " + op_type);
    }
    return it->second;
}
} // namespace

std::vector<std::unique_ptr<AsmInstructionNode>> IRBinaryNode::lowerToAsm() {
    auto instructions = std::vector<std::unique_ptr<AsmInstructionNode>>();
    // AsmOperandNode::lowerToAsm returns imm/pseudo if const/variable
    auto dest = this->val_dest->lowerToAsm();

    // separate handling for (+, -, *), (/, %) and (==, !=, <, >, <=, >= {relational ops})
    if (this->op_type == "/" || this->op_type == "%") {
        auto eax_reg = std::make_shared<AsmRegisterNode>(getRegString(Reg::eax));
        auto src1 = this->val_src1->lowerToAsm();
        auto mov_instr = std::make_unique<AsmMovNode>(src1, eax_reg);
        instructions.push_back(std::move(mov_instr));

        auto cdq_instr = std::make_unique<AsmCdqNode>();
        instructions.push_back(std::move(cdq_instr));

        auto divisor = this->val_src2->lowerToAsm();
        auto idiv_instr = std::make_unique<AsmIdivNode>(divisor);
        instructions.push_back(std::move(idiv_instr));

        auto result_reg = (this->op_type == "/")
                              ? eax_reg
                              : std::make_shared<AsmRegisterNode>(getRegString(Reg::edx));
        auto mov_res_instr = std::make_unique<AsmMovNode>(result_reg, dest);
        instructions.push_back(std::move(mov_res_instr));
    } else if (this->op_type == "+" || this->op_type == "-" || this->op_type == "*") {
        auto src1 = this->val_src1->lowerToAsm();
        auto mov_instr = std::make_unique<AsmMovNode>(src1, dest);
        instructions.push_back(std::move(mov_instr));

        auto src2 = this->val_src2->lowerToAsm();
        auto binary_instr = std::make_unique<AsmBinaryNode>(this->op_type, src2, dest);
        instructions.push_back(std::move(binary_instr));
    } else if (RELATIONAL_OPS.contains(this->op_type)) {
        auto src1 = this->val_src1->lowerToAsm();
        auto src2 = this->val_src2->lowerToAsm();
        auto dest = this->val_dest->lowerToAsm();

        auto cmp_instr = std::make_unique<AsmCmpNode>(src2, src1);
        instructions.push_back(std::move(cmp_instr));

        auto move_instr =
            std::make_unique<AsmMovNode>(std::make_shared<AsmImmediateNode>("0"), dest);
        instructions.push_back(std::move(move_instr));

        auto setcc_instr = std::make_unique<AsmSetCCNode>(getCondCode(this->op_type), dest);
        instructions.push_back(std::move(setcc_instr));
    } else {
        throw std::runtime_error("IRBinaryNode::lowerToAsm: Unsupported op_type " + this->op_type);
    }
    return instructions;
}

std::shared_ptr<AsmOperandNode> IRConstNode::lowerToAsm() {
    return std::make_shared<AsmImmediateNode>(this->val);
}

std::shared_ptr<AsmOperandNode> IRVariableNode::lowerToAsm() {
    return std::make_shared<AsmPseudoNode>(this->var_name);
}

std::vector<std::unique_ptr<AsmInstructionNode>> IRCopyNode::lowerToAsm() {
    auto instructions = std::vector<std::unique_ptr<AsmInstructionNode>>();
    auto src = this->val_src->lowerToAsm();
    auto dest = this->val_dest->lowerToAsm();

    auto mov_instr = std::make_unique<AsmMovNode>(src, dest);
    instructions.push_back(std::move(mov_instr));
    return instructions;
}

std::vector<std::unique_ptr<AsmInstructionNode>> IRJumpNode::lowerToAsm() {
    auto instructions = std::vector<std::unique_ptr<AsmInstructionNode>>();
    auto jmp_instr = std::make_unique<AsmJmpNode>(this->target_label);
    instructions.push_back(std::move(jmp_instr));
    return instructions;
}

namespace {
// helper function for conditional jumps; `IRJumpIfZeroNode` and `IRJumpIfNotZeroNode`
std::vector<std::unique_ptr<AsmInstructionNode>>
emit_jump_if(const std::string& cc, // condition code
             const std::shared_ptr<IRValNode>& condition, const std::string& target_label) {
    std::vector<std::unique_ptr<AsmInstructionNode>> instructions;
    auto cond_asm = condition->lowerToAsm();

    // compare condition with 0
    instructions.push_back(
        std::make_unique<AsmCmpNode>(std::make_shared<AsmImmediateNode>("0"), cond_asm));

    // conditional jump
    instructions.push_back(std::make_unique<AsmJmpCCNode>(cc, target_label));

    return instructions;
}
} // namespace

std::vector<std::unique_ptr<AsmInstructionNode>> IRJumpIfZeroNode::lowerToAsm() {
    auto instructions = emit_jump_if("e", this->condition, this->target_label);
    return instructions;
}

std::vector<std::unique_ptr<AsmInstructionNode>> IRJumpIfNotZeroNode::lowerToAsm() {
    auto instructions = emit_jump_if("ne", this->condition, this->target_label);
    return instructions;
}

std::vector<std::unique_ptr<AsmInstructionNode>> IRLabelNode::lowerToAsm() {
    auto instructions = std::vector<std::unique_ptr<AsmInstructionNode>>();
    auto label_instr = std::make_unique<AsmLabelNode>(this->label_name);
    instructions.push_back(std::move(label_instr));
    return instructions;
}

/// @brief Things/Instructions to execute just before a function call.
/// Not to be confused with Function defination part.
///
/// function callee: is called by other functions
/// @return instructions
std::vector<std::unique_ptr<AsmInstructionNode>> IRFunctionCallNode::lowerToAsm() {
    auto instructions = std::vector<std::unique_ptr<AsmInstructionNode>>();
    Reg arg_resisters[6] = {Reg::edi, Reg::esi, Reg::edx, Reg::ecx, Reg::r8d, Reg::r9d};
    int num_args = this->arguments.size();

    // num of stack args
    int num_stack_args = (num_args > 6) ? (num_args - 6) : 0;

    // stack must be 16-byte aligned before call
    // each stack arg takes 8 bytes, so if odd number of stack args, add 8 bytes padding
    // even number of stack args means stack is already 16-byte aligned
    int stack_padding = 0;
    if (num_stack_args % 2 == 1) {
        stack_padding = 8;
        instructions.push_back(std::make_unique<AsmAllocateStackNode>(stack_padding));
    }

    // pass first 6 arguments in registers
    for (int i = 0; i < std::min(num_args, 6); i++) {
        auto arg_asm = this->arguments[i]->lowerToAsm();
        auto reg_node = std::make_shared<AsmRegisterNode>(getRegString(arg_resisters[i]));
        instructions.push_back(std::make_unique<AsmMovNode>(arg_asm, reg_node));
    }

    // pass remaining args on stack in reverse order
    for (int i = num_args - 1; i >= 6; i--) {
        auto arg_asm = this->arguments[i]->lowerToAsm();
        // push (to stack) can only take immediate values or registers
        // pushing a Register pushes the entire 8 bytes (64 bits)
        if (isa<AsmImmediateNode>(arg_asm.get()) || isa<AsmRegisterNode>(arg_asm.get())) {
            instructions.push_back(std::make_unique<AsmPushNode>(arg_asm));
        } else { // move to temp register first then push to stack
            // use 32-bit register for movl, then push the 64-bit register
            auto temp_reg_32 = std::make_shared<AsmRegisterNode>(getRegString(Reg::eax));
            auto temp_reg_64 = std::make_shared<AsmRegisterNode>(getRegString(Reg::rax));
            instructions.push_back(std::make_unique<AsmMovNode>(arg_asm, temp_reg_32));
            instructions.push_back(std::make_unique<AsmPushNode>(temp_reg_64));
        }
    }

    // call function
    instructions.push_back(std::make_unique<AsmCallNode>(this->func_name));

    // adjust stack: remove stack-passed arguments + padding.
    // NOTE: any arguments passed on the stack use 8-byte slots per the System V ABI,
    // even if the argument type is only 4 bytes.
    int total_stack_adjust = (num_stack_args * 8 /*bytes*/) + stack_padding;
    if (total_stack_adjust > 0) {
        instructions.push_back(std::make_unique<AsmDeallocateStackNode>(total_stack_adjust));
    }

    // return value
    auto ret_dest = this->return_dest->lowerToAsm();
    auto eax_reg = std::make_shared<AsmRegisterNode>(getRegString(Reg::eax));
    instructions.push_back(std::make_unique<AsmMovNode>(eax_reg, ret_dest));

    return instructions;
}
// Emit Assembly Functions -- End

namespace nanocc {
std::unique_ptr<AsmProgramNode> intermReprToPseudoAsm(std::unique_ptr<IRProgramNode>& ir_ast,
                        bool debug) {
    auto asm_ast = ir_ast->lowerToAsm();
    return asm_ast;
}
} // namespace nanocc
