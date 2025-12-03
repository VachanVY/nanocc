/*
contains implementations of:
* lowerToAsm methods for IR nodes to generate corresponding ASM nodes
* resolvePseudoRegisters methods for ASM nodes to resolve pseudo registers
* fixUpInstructions methods for ASM nodes to fix instructions needing multiple steps
*/

/*
int foo(void) {return 0;}
int bar(void) { // `bar` is the calleR
    int a = foo(); // `foo` is the calleE
    return a;
}

`IRFunctionCallNode::lowerToAsm()`
handles what happens just before a assembly func call is made:
adjust stack alignment before calling function
pass first 6 arguments in registers
pass remaining arguments on stack in reverse order
call function
adjust stack back, since call is over, remove stack args and padding
return value
That's why `AsmCallNode` doesn't need any args inside it.

`IRFunctionNode::lowerToAsm()`
place all parameters currently in the registers on to the stack,
the registers might get used by any other func call made in the function body
so to not lose their values, we push them onto the stack
WE WILL OPTIMIZE THIS LATER in REGISTER ALLOCATION PHASE
*/

/*
first 6 args are in registers:
1st: %edi
2nd: %esi
3rd: %edx
4th: %ecx
5th: %r8d
6th: %r9d
*/

/*
`0(%rbp)`: rbp
`8(%rbp)`: return address of next instruction after `call func`
pos `offset`: `16(%rbp)`, `24(%rbp)`, ... (stack args beyond 6th)
neg `offset`: `-4(%rbp)`, `-8(%rbp)`, ... (local vars)
*/

/*TODO: add a print pseudo assembly i.e before the
fixups, will be easy to understand/debug asm code*/

#include <memory>
#include <print>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "asmgen.hpp"
#include "irgen.hpp"
#include "parser.hpp"
#include "utils.hpp"
#include "codegen.hpp"

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
