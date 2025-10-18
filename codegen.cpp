#include "codegen.hpp"
#include "asmgen.hpp"
#include "parser.hpp"
#include "utils.hpp"
#include <cstdlib>
#include <fstream>

void emit_operand(const AsmOperandNode *operand, std::ostream &os) {
    if (const auto *imm = dynamic_cast<const AsmImmediateNode *>(operand)) {
        os << "$" << imm->value;
        return;
    }
    if (const auto *reg = dynamic_cast<const AsmRegisterNode *>(operand)) {
        os << reg->name;
        return;
    }
    throw std::runtime_error(
        "Unsupported operand node encountered during emission");
}

void emit_instruction(const AsmIntructionNode *instruction, std::ostream &os) {
    if (const auto *mov = dynamic_cast<const AsmMovNode *>(instruction)) {
        if (!mov->src || !mov->dest) {
            throw std::runtime_error(
                "AsmMovNode missing operands during emission");
        }
        os << TAB4 << "mov ";
        emit_operand(mov->src.get(), os);
        os << ", ";
        emit_operand(mov->dest.get(), os);
        os << '\n';
        return;
    }

    if (dynamic_cast<const AsmRetNode *>(instruction)) {
        os << TAB4 << "ret\n";
        return;
    }

    throw std::runtime_error(
        "Unsupported instruction node encountered during emission");
}

void emit_function(const AsmFunctionNode &func, std::ostream &os) {
    os << TAB4 << ".globl " << func.name << "\n";
    os << func.name << ":\n";
    for (const auto &instruction : func.instructions) {
        emit_instruction(instruction.get(), os);
    }
}

void emit_program(const AsmProgramNode &program, std::ostream &os) {
    if (!program.func) {
        throw std::runtime_error("Program contains no functions to emit");
    }
    emit_function(*program.func, os);
    os << TAB4 << ".section .note.GNU-stack, \"\",@progbits\n";
}
