#pragma once

#include <ostream>

#include "asmgen.hpp"

constexpr auto TAB4 = "    ";

// Emitters for asm AST nodes. Implementations are in ../codegen.cpp
void emit_operand(const AsmOperandNode *operand, std::ostream &os);
void emit_instruction(const AsmIntructionNode *instruction, std::ostream &os);
void emit_function(const AsmFunctionNode &func, std::ostream &os);
void emit_program(const AsmProgramNode &program, std::ostream &os);
