#include "nanocc/Utils.hpp"
#include "nanocc/Sema/Sema.hpp"
#include "nanocc/Codegen/ASM.hpp"
#include "nanocc/Target/X86/X86TargetInfo.hpp"
#include "nanocc/Target/X86/X86TargetEmitter.hpp"

void AsmProgramNode::generateAsm(std::ostream& os) {
    for (const auto& function : this->functions) {
        function->generateAsm(os);
    }
    os << TAB4 << ".section .note.GNU-stack, \"\",@progbits\n";
}

void AsmFunctionNode::generateAsm(std::ostream& os) {
    os << TAB4 << ".globl " << name << "\n";
    os << name << ":\n";
    // pushq %rbp
    os << TAB4 << "pushq " << getRegString(Reg::rbp) << "\n";
    // movq %rsp, %rbp
    os << TAB4 << "movq " << getRegString(Reg::rsp) << ", " << getRegString(Reg::rbp) << "\n";
    for (const auto& instruction : instructions) {
        instruction->generateAsm(os);
    }
}

// Instruction Nodes -- Start
void AsmMovNode::generateAsm(std::ostream& os) {
    if (!src || !dest) {
        throw std::runtime_error("AsmMovNode missing operands during emission");
    }
    os << TAB4 << "movl ";
    src->generateAsm(os);
    os << ", ";
    dest->generateAsm(os);
    os << '\n';
}

void AsmUnaryNode::generateAsm(std::ostream& os) {
    if (!operand) {
        throw std::runtime_error("AsmUnaryNode missing operand during emission");
    }
    static auto unop_map = std::unordered_map<char, std::string>{
        {'-', "negl"},
        {'~', "notl"},
    };
    // index 0 since op_type is string // returns char
    std::string mnemonic = unop_map.at(op_type[0]);

    os << TAB4 << mnemonic << " ";
    operand->generateAsm(os);
    os << '\n';
}

void AsmBinaryNode::generateAsm(std::ostream& os) {
    static auto binops = std::unordered_map<std::string, std::string>{
        {"+", "addl"},
        {"-", "subl"},
        {"*", "imull"},
    };
    os << TAB4 << binops.at(this->op_type) << " ";
    this->src->generateAsm(os);
    os << ", ";
    this->dest->generateAsm(os);
    os << '\n';
}

void AsmCmpNode::generateAsm(std::ostream& os) {
    os << TAB4 << "cmpl ";
    this->src1->generateAsm(os);
    os << ", ";
    this->src2->generateAsm(os);
    os << '\n';
}

/*
`a/b` quotient stored in `EAX`, remainder stored in `EDX`

```asm
movl $a, %eax      # move dividend to EAX
movl $b, -4(%rbp)  # move divisor to stack
cdq                # sign extend EAX to EDX:EAX
idivl -4(%rbp)     # divide EDX:EAX by divisor at -4(%rbp)
```
*/
void AsmIdivNode::generateAsm(std::ostream& os) {
    if (!divisor) {
        throw std::runtime_error("AsmIdivNode missing divisor during emission");
    }
    os << TAB4 << "idivl ";
    divisor->generateAsm(os);
    os << '\n';
}

/*
`cdq` sign extends the value of the `EAX` register into the `EDX:EAX`.
if `%eax` is positive, all bits in edx are cleared to `0`.
if `%eax` is negative, all bits in edx are set to `1`.
*/
void AsmCdqNode::generateAsm(std::ostream& os) { os << TAB4 << "cdq\n"; }

void AsmJmpNode::generateAsm(std::ostream& os) { os << TAB4 << "jmp " << this->label << "\n"; }

void AsmJmpCCNode::generateAsm(std::ostream& os) {
    os << TAB4 << "j" << this->cond_code << " " << this->label << "\n";
}

void AsmSetCCNode::generateAsm(std::ostream& os) {
    os << TAB4 << "set" << this->cond_code << " ";
    this->dest->generateAsm(os);
    os << "\n";
}

void AsmLabelNode::generateAsm(std::ostream& os) {
    os << "  " << this->label << ":\n"; // no `TAB4` for labels
}

void AsmAllocateStackNode::generateAsm(std::ostream& os) {
    // subq $stack_size, %rsp
    os << TAB4 << "subq $" << stack_size << ", " << getRegString(Reg::rsp) << "\n";
}

void AsmDeallocateStackNode::generateAsm(std::ostream& os) {
    // addq $stack_size, %rsp
    os << TAB4 << "addq $" << stack_size << ", " << getRegString(Reg::rsp) << "\n";
}

void AsmPushNode::generateAsm(std::ostream& os) {
    assert(operand && "AsmPushNode missing operand during emission");
    os << TAB4 << "pushq ";
    operand->generateAsm(os);
    os << '\n';
}

void AsmCallNode::generateAsm(std::ostream& os) {
    os << TAB4 << "call " << func_name;
    // add @PLT suffix for functions without definations
    const Type& func_info = nanocc::global_type_checker_map[func_name];
    assert(std::holds_alternative<FuncType>(func_info) &&
           "AsmCallNode::generateAsm: func_name not found in global_type_checker_map");
    const FuncType& func_type = std::get<FuncType>(func_info);
    if (!func_type.defined) {
        os << "@PLT";
    }
    os << '\n';
}

void AsmRetNode::generateAsm(std::ostream& os) {
    os << TAB4 << "movq"
       << " " << getRegString(Reg::rbp) << ", " << getRegString(Reg::rsp) << "\n";
    os << TAB4 << "popq " << getRegString(Reg::rbp) << "\n";
    os << TAB4 << "ret\n\n";
}
// Instruction Nodes -- end

// Operand Nodes -- Start
void AsmImmediateNode::generateAsm(std::ostream& os) { os << "$" << value; }

void AsmRegisterNode::generateAsm(std::ostream& os) { os << name; }

// `0(%rbp)`: rbp
// `8(%rbp)`: return address of next instruction after `call func`
// pos `offset`: `16(%rbp)`, `24(%rbp)`, ... (stack args beyond 6th)
// neg `offset`: `-4(%rbp)`, `-8(%rbp)`, ... (local vars)
void AsmStackNode::generateAsm(std::ostream& os) {
    os << offset << "(" << getRegString(Reg::rbp) << ")";
}
// Operand Nodes -- end

namespace nanocc {
void x86CorrectAssembly(std::unique_ptr<AsmProgramNode>& asm_ast, bool debug) {
    // maps from `AsmPseudoNode::identifier` to assigned `AsmStackNode::offset`
    std::unordered_map<std::string, int> pseudo_reg_map;
    // for every function, stack offset starts at 0
    int num_functions = asm_ast->functions.size();
    // offsets grow in 4-byte (int) increments // increment by 4 then use
    std::vector<int> stack_offsets(num_functions, 0);
    asm_ast->resolvePseudoRegisters(pseudo_reg_map, stack_offsets);
    if (debug) {
        for (std::size_t i = 0; i < stack_offsets.size(); i++) {
            std::println("Function {}: Stack Size = {}", asm_ast->functions[i]->name,
                         stack_offsets[i]);
        }
    }
    asm_ast->fixUpInstructions(stack_offsets);
}

std::ostringstream x86EmitAssembly(const std::unique_ptr<AsmProgramNode>& asm_program) {
    std::ostringstream os;
    asm_program->generateAsm(os);
    return os;
}
} // namespace nanocc