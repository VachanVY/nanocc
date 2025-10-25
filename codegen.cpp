#include <fstream>

#include "codegen.hpp"
#include "asmgen.hpp"
#include "utils.hpp"

void AsmProgramNode::emit_asm(std::ostream& os) {
    if (!func) {
        throw std::runtime_error("Program contains no functions to emit");
    }
    func->emit_asm(os);
    os << TAB4 << ".section .note.GNU-stack, \"\",@progbits\n";
}

void AsmFunctionNode::emit_asm(std::ostream& os) {
    os << TAB4 << ".globl " << name << "\n";
    os << name << ":\n";
    os << TAB4 << "pushq %rbp\n";
    os << TAB4 << "movq %rsp, %rbp\n";
    for (const auto& instruction : instructions) {
        instruction->emit_asm(os);
    }
}

// Instruction Nodes -- Start
void AsmMovNode::emit_asm(std::ostream& os) {
    if (!src || !dest) {
        throw std::runtime_error("AsmMovNode missing operands during emission");
    }
    os << TAB4 << "movl ";
    src->emit_asm(os);
    os << ", ";
    dest->emit_asm(os);
    os << '\n';
}

void AsmUnaryNode::emit_asm(std::ostream& os) {
    if (!operand) {
        throw std::runtime_error("AsmUnaryNode missing operand during emission");
    }
    std::string mnemonic;
    char ch = op_type.empty() ? '\0' : op_type[0];
    switch (ch) {
    case '-':
        mnemonic = "negl";
        break;
    case '~':
        mnemonic = "notl";
        break;
    default:
        throw std::runtime_error("Unsupported unary op during emission: " + op_type);
    }
    os << TAB4 << mnemonic << " ";
    operand->emit_asm(os);
    os << '\n';
}

void AsmBinaryNode::emit_asm(std::ostream& os) {
    if (this->op_type == "+") {
        os << TAB4 << "addl ";
    } else if (this->op_type == "-") {
        os << TAB4 << "subl ";
    } else if (this->op_type == "*") {
        os << TAB4 << "imull ";
    } else {
        throw std::runtime_error("Unsupported binary op during emission: " + op_type);
    }
    this->src->emit_asm(os);
    os << ", ";
    this->dest->emit_asm(os);
    os << '\n';
}

void AsmIdivNode::emit_asm(std::ostream& os) {
    if (!divisor) {
        throw std::runtime_error("AsmIdivNode missing divisor during emission");
    }
    os << TAB4 << "idivl ";
    divisor->emit_asm(os);
    os << '\n';
}

void AsmCdqNode::emit_asm(std::ostream& os) {
    os << TAB4 << "cdq\n";
}

void AsmAllocateStackNode::emit_asm(std::ostream& os) {
    os << TAB4 << "subq $" << stack_size << ", %rsp\n";
}

void AsmRetNode::emit_asm(std::ostream& os) {
    os << TAB4 << "movq %rbp, %rsp\n";
    os << TAB4 << "popq %rbp\n";
    os << TAB4 << "ret\n";
}
// Instruction Nodes -- end

// Operand Nodes -- Start
void AsmImmediateNode::emit_asm(std::ostream& os) { os << "$" << value; }

void AsmRegisterNode::emit_asm(std::ostream& os) { os << name; }

void AsmStackNode::emit_asm(std::ostream& os) { os << "-" << offset << "(%rbp)"; }
// Operand Nodes -- end
