#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <print>

#include "asmgen.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "utils.hpp"

std::unique_ptr<AsmProgramNode> ProgramNode::parse_asm_ast() {
    auto asm_program = std::make_unique<AsmProgramNode>();
    if (this->func) {
        asm_program->func = std::move(this->func->parse_asm_ast());
    }
    return asm_program;
}

std::unique_ptr<AsmFunctionNode> FunctionNode::parse_asm_ast() {
    auto asm_func = std::make_unique<AsmFunctionNode>();
    asm_func->name = this->var_identifier->name;

    if (this->statement) {
        asm_func->instructions = this->statement->parse_asm_ast();
        // AsmMovNode(AsmOperandNode src_reg, AsmOperandNode dest_reg),
        // AsmRetNode()
    }

    return asm_func;
}

std::vector<std::unique_ptr<AsmIntructionNode>> StatementNode::parse_asm_ast() {
    std::vector<std::unique_ptr<AsmIntructionNode>> instructions;

    if (!this->expr) {
        throw std::runtime_error(
            "StatementNode::parse_asm_ast called without expression");
    }

    instructions.push_back(this->expr->parse_asm_ast());
    instructions.push_back(std::make_unique<AsmRetNode>());

    return instructions;
}

std::unique_ptr<AsmIntructionNode> ExprNode::parse_asm_ast() {
    if (!this->integer) {
        throw std::runtime_error(
            "ExprNode::parse_asm_ast currently supports only integer literals");
    }

    auto mov = std::make_unique<AsmMovNode>();

    auto src = std::make_unique<AsmImmediateNode>();
    src->value = this->integer->val;
    mov->src = std::move(src);

    auto dest = std::make_unique<AsmRegisterNode>();
    dest->name = "%eax";
    mov->dest = std::move(dest);

    return mov;
}
