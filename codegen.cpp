#include <fstream>

#include "codegen.hpp"
#include "asmgen.hpp"
#include "utils.hpp"
#include "checker.hpp" // access global_type_checker_map

void AsmProgramNode::generateAsm(std::ostream& os) {
    for (const auto& function : this->functions) {
        function->generateAsm(os);
    }
    os << TAB4 << ".section .note.GNU-stack, \"\",@progbits\n";
}

void AsmFunctionNode::generateAsm(std::ostream& os) {
    os << TAB4 << ".globl " << name << "\n";
    os << name << ":\n";
    os << TAB4 << "pushq %rbp\n";
    os << TAB4 << "movq %rsp, %rbp\n";
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
    std::string mnemonic;
    char ch = op_type.empty() ? '\0' : op_type[0];
    switch (op_type[0]) {
    case '-': {
        mnemonic = "negl";
        break;
    }
    case '~': {
        mnemonic = "notl";
        break;
    }
    default:
        throw std::runtime_error("Unsupported unary op during emission: " + op_type);
    }

    os << TAB4 << mnemonic << " ";
    operand->generateAsm(os);
    os << '\n';
}

void AsmBinaryNode::generateAsm(std::ostream& os) {
    if (this->op_type == "+") {
        os << TAB4 << "addl ";
    } else if (this->op_type == "-") {
        os << TAB4 << "subl ";
    } else if (this->op_type == "*") {
        os << TAB4 << "imull ";
    } else {
        throw std::runtime_error("Unsupported binary op during emission: " + op_type);
    }
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

void AsmIdivNode::generateAsm(std::ostream& os) {
    if (!divisor) {
        throw std::runtime_error("AsmIdivNode missing divisor during emission");
    }
    os << TAB4 << "idivl ";
    divisor->generateAsm(os);
    os << '\n';
}

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
    os << TAB4 << "subq $" << stack_size << ", %rsp\n";
}

void AsmDeallocateStackNode::generateAsm(std::ostream& os) {
    os << TAB4 << "addq $" << stack_size << ", %rsp\n";
}

void AsmPushNode::generateAsm(std::ostream& os) {
    assert(operand && "AsmPushNode missing operand during emission");
    os << TAB4 << "pushl ";
    operand->generateAsm(os);
    os << '\n';
}

void AsmCallNode::generateAsm(std::ostream& os) {
    os << TAB4 << "call " << func_name;
    // add @PLT suffix for functions without definations
    const Type& func_info = global_type_checker_map[func_name];
    assert(std::holds_alternative<FuncType>(func_info) &&
           "AsmCallNode::generateAsm: func_name not found in global_type_checker_map");
    const FuncType& func_type = std::get<FuncType>(func_info);
    if (!func_type.defined) {
        os << "@PLT";
    }
    os << '\n';
}

void AsmRetNode::generateAsm(std::ostream& os) {
    os << TAB4 << "movq %rbp, %rsp\n";
    os << TAB4 << "popq %rbp\n";
    os << TAB4 << "ret\n\n";
}
// Instruction Nodes -- end

// Operand Nodes -- Start
void AsmImmediateNode::generateAsm(std::ostream& os) { os << "$" << value; }

void AsmRegisterNode::generateAsm(std::ostream& os) { os << name; }

void AsmStackNode::generateAsm(std::ostream& os) { os << "-" << offset << "(%rbp)"; }
// Operand Nodes -- end
