#include <print>
#include <string>
#include <utility>

#include "irgen.hpp"
#include "parser.hpp"
#include "utils.hpp"

/* Dev Docs
what does emit_ir of class X do?
> return IR form of that class X i.e IRXNode

func XNode::emit_lr()
    make IRXNode
    IRXNode->ChildNode = ChildNode->emit_ir()
    return IRXNode

use shared_ptr for IRValNode (and its derived classes)
*/

std::unique_ptr<IRProgramNode> ProgramNode::emit_ir() {
    auto ir_program = std::make_unique<IRProgramNode>();
    if (func) {
        ir_program->func = func->emit_ir();
    }
    return ir_program;
}

void IRProgramNode::dump_ir(int indent) const {
    printIndent(indent);
    std::println("IRProgram(");
    if (func) {
        func->dump_ir(indent + 1);
    }
    printIndent(indent);
    std::println(")");
}

std::unique_ptr<IRFunctionNode> FunctionNode::emit_ir() {
    std::vector<std::unique_ptr<IRInstructionNode>> instructions;
    if (statement) {
        instructions = statement->emit_ir();
    }
    auto ir_function =
        std::make_unique<IRFunctionNode>(var_identifier->name, std::move(instructions));
    return ir_function;
}

void IRFunctionNode::dump_ir(int indent) const {
    printIndent(indent);
    std::println("Function(");
    printIndent(indent + 1);
    std::println("name='{}'", var_name);
    printIndent(indent + 1);
    std::println("instructions=[");
    for (const auto& instr : instructions) {
        if (instr) {
            instr->dump_ir(indent + 2);
        }
    }
    printIndent(indent + 1);
    std::println("]");
    printIndent(indent);
    std::println(")");
}

std::vector<std::unique_ptr<IRInstructionNode>> StatementNode::emit_ir() {
    std::vector<std::unique_ptr<IRInstructionNode>> ir_instructions;

    // get ir from Expr Node; `emit_tacky` func from the book;
    // returns the destination register of prev operation i.e source register
    // for this iter
    auto dest_var = expr->emit_ir(ir_instructions);
    // emit return of the computed value
    auto ret_instruction = std::make_unique<IRRetNode>(std::move(dest_var));
    ir_instructions.push_back(std::move(ret_instruction));
    return ir_instructions;
}

std::shared_ptr<IRValNode> // return the destination register
ExprNode::emit_ir(std::vector<std::unique_ptr<IRInstructionNode>>& instructions) {
    if (left_exprf) {
        return left_exprf->emit_ir(instructions);
    }
    throw std::runtime_error("IR Generation Error: Malformed Expression");
}

namespace { // helper functions to handle binary short-circuiting operators
// Generate unique temporary VARIABLE names
auto getTempVarName = []() {
    static size_t temp_var_counter = 0;
    return "tmp." + std::to_string(temp_var_counter++);
};

std::shared_ptr<IRValNode>
handleRelationalOps(BinaryNode* binop, // raw pointer be careful // TODO(VachanVY): anyway to NOT
                                       // use raw pointer here?
                    std::vector<std::unique_ptr<IRInstructionNode>>& instructions) {
    // left and right are ExprNode // can be ExprFactorNode too...
    // runtime polymorphism...
    auto left_val = binop->left->emit_ir(instructions);
    auto right_val = binop->right->emit_ir(instructions);

    std::string tmp = getTempVarName();
    auto val_dest = std::make_shared<IRVariableNode>(tmp);

    auto ir_binary = std::make_unique<IRBinaryNode>(binop->op_type, left_val, right_val, val_dest);
    instructions.push_back(std::move(ir_binary));
    return val_dest;
}

auto getLabelName = [](const std::string& prefix) {
    assert(!prefix.empty() && "Label prefix cannot be empty");
    static size_t label_counter = 0;
    return prefix + "." + std::to_string(label_counter++);
};

std::shared_ptr<IRValNode>
handleShortCircuitOps(BinaryNode* binop,
                      std::vector<std::unique_ptr<IRInstructionNode>>& instructions) {
    assert(binop->op_type == "&&" || binop->op_type == "||" && "Not a short-circuit operator");
    auto result = std::make_shared<IRVariableNode>(getTempVarName());

    std::string short_label = getLabelName("short");
    std::string end_label = getLabelName("end");

    bool is_and = (binop->op_type == "&&");

    auto left_val = binop->left->emit_ir(instructions);
    // jump to short-circuit if left determines the result
    if (is_and) {
        instructions.push_back(std::make_unique<IRJumpIfZeroNode>(left_val, short_label));
    } else {
        instructions.push_back(std::make_unique<IRJumpIfNotZeroNode>(left_val, short_label));
    }

    auto right_val = binop->right->emit_ir(instructions);
    // jump to short-circuit if right determines the result
    if (is_and) {
        instructions.push_back(std::make_unique<IRJumpIfZeroNode>(right_val, short_label));
    } else {
        instructions.push_back(std::make_unique<IRJumpIfNotZeroNode>(right_val, short_label));
    }

    // both conditions passed: AND is true, OR is false
    instructions.push_back(
        std::make_unique<IRCopyNode>(std::make_shared<IRConstNode>(is_and ? "1" : "0"), result));
    instructions.push_back(std::make_unique<IRJumpNode>(end_label));

    // short-circuit label: AND is false, OR is true
    instructions.push_back(std::make_unique<IRLabelNode>(short_label));
    instructions.push_back(
        std::make_unique<IRCopyNode>(std::make_shared<IRConstNode>(is_and ? "0" : "1"), result));

    instructions.push_back(std::make_unique<IRLabelNode>(end_label));
    return result;
}
} // namespace

// return the destination register
std::shared_ptr<IRValNode>
ExprFactorNode::emit_ir(std::vector<std::unique_ptr<IRInstructionNode>>& instructions) {
    if (auto binop = dyn_cast<BinaryNode>(this)) {
        if (binop->op_type == "&&" || binop->op_type == "||") {
            return handleShortCircuitOps(binop, instructions);
        } else {
            return handleRelationalOps(binop, instructions);
        }
    } else if (constant) {
        auto ir_const = std::make_shared<IRConstNode>(constant->val);
        return ir_const;
    } else if (unary && factor) {
        // get inner most expression's value
        auto src_val = factor->emit_ir(instructions);
        std::string tmp = getTempVarName();
        auto dest_var = std::make_shared<IRVariableNode>(tmp);

        auto ir_unary = std::make_unique<IRUnaryNode>(unary->op_type, src_val, dest_var);
        instructions.push_back(std::move(ir_unary));
        return dest_var;
    } else if (expr) { // will be null after it's parsed
        // will it ever reach here?
        // ig no because the program will get stuck in an infinite recursion
        // let's see...
        return expr->emit_ir(instructions);
    }
    throw std::runtime_error("IR Generation Error: Malformed Expression Factor");
}

void IRRetNode::dump_ir(int indent) const {
    printIndent(indent);
    if (val) {
        std::println("return {}", val->dump_ir());
    } else {
        std::println("return <void>");
    }
}

void IRUnaryNode::dump_ir(int indent) const {
    printIndent(indent);
    const std::string dest = val_dest->dump_ir();
    const std::string src = val_src->dump_ir();
    std::println("{} = {} {}", dest, op_type, src);
}

void IRBinaryNode::dump_ir(int indent) const {
    printIndent(indent);
    const std::string dest = val_dest->dump_ir();
    const std::string src1 = val_src1->dump_ir();
    const std::string src2 = val_src2->dump_ir();
    std::println("{} = {} {} {}", dest, src1, op_type, src2);
}

void IRCopyNode::dump_ir(int indent) const {
    printIndent(indent);
    const std::string dest = val_dest->dump_ir();
    const std::string src = val_src->dump_ir();
    std::println("{} = {}", dest, src);
}

void IRJumpNode::dump_ir(int indent) const {
    printIndent(indent);
    std::println("jump {}", target_label);
}

void IRJumpIfZeroNode::dump_ir(int indent) const {
    printIndent(indent);
    const std::string cond = condition->dump_ir();
    std::println("jump_if_zero {}, {}", cond, target_label);
}

void IRJumpIfNotZeroNode::dump_ir(int indent) const {
    printIndent(indent);
    const std::string cond = condition->dump_ir();
    std::println("jump_if_not_zero {}, {}", cond, target_label);
}

void IRLabelNode::dump_ir(int indent) const {
    assert(indent > 0);
    printIndent(indent - 1);
    std::println("{}:", label_name);
}

std::string IRConstNode::dump_ir() const { return val; }
std::string IRVariableNode::dump_ir() const { return var_name; }

/*
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
    for (const auto& [token_type, lexeme] : tokens) {
        std::println("{}, {}, {}", i++, tokenTypeToString(token_type), lexeme);
    }
    auto ast = parse(tokens);
    ast->dump();
    auto ir = ast->emit_ir();
    ir->dump_ir();

    return 0;
}
// */