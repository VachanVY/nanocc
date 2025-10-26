#include <print>
#include <string>

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
    auto ir_func = std::make_unique<IRFunctionNode>();
    ir_func->var_name = var_identifier->name;
    if (statement) {
        ir_func->instructions = statement->emit_ir();
    }
    return ir_func;
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
    auto ret = std::make_unique<IRRetNode>();
    ret->val = std::move(dest_var);
    ir_instructions.push_back(std::move(ret));
    return ir_instructions;
}

std::shared_ptr<IRValNode> // return the destination register
ExprNode::emit_ir(std::vector<std::unique_ptr<IRInstructionNode>>& instructions) {
    if (left_exprf) {
        return left_exprf->emit_ir(instructions);
    }
    throw std::runtime_error("IR Generation Error: Malformed Expression");
}

// return the destination register
std::shared_ptr<IRValNode>
ExprFactorNode::emit_ir(std::vector<std::unique_ptr<IRInstructionNode>>& instructions) {
    static size_t temp_counter = 0;
    auto getTempVarName = [&]() { return "tmp." + std::to_string(temp_counter++); };
    if (auto binop = dyn_cast<BinaryNode>(this)) {
        // left and right are ExprNode // can be ExprFactorNode too...
        // runtime polymorphism...
        auto left_val = binop->left->emit_ir(instructions);
        auto right_val = binop->right->emit_ir(instructions);

        std::string tmp = getTempVarName();
        auto val_dest = std::make_shared<IRVariableNode>();
        val_dest->var_name = tmp;

        auto ir_binary = std::make_unique<IRBinaryNode>();
        ir_binary->op_type = binop->op_type;
        ir_binary->val_src1 = left_val;
        ir_binary->val_src2 = right_val;
        ir_binary->val_dest = val_dest;

        instructions.push_back(std::move(ir_binary));
        return val_dest;
    } else if (constant) {
        auto ir_const = std::make_shared<IRConstNode>();
        ir_const->val = constant->val;
        return ir_const;
    } else if (unary && factor) {
        // get inner most expression's value
        auto src_val = factor->emit_ir(instructions);
        std::string tmp = getTempVarName();
        auto dest_var = std::make_shared<IRVariableNode>();
        dest_var->var_name = tmp;

        auto val_dest = std::make_shared<IRVariableNode>();
        val_dest->var_name = tmp;

        auto ir_unary = std::make_unique<IRUnaryNode>();
        ir_unary->op_type = unary->op_type;
        ir_unary->val_src = src_val;
        ir_unary->val_dest = val_dest;

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