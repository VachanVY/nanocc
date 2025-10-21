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
*/

std::unique_ptr<IRProgramNode> ProgramNode::emit_ir() {
    auto ir_program = std::make_unique<IRProgramNode>();
    if (func) {
        ir_program->func = func->emit_ir();
    }
    return ir_program;
}

void IRProgramNode::dump_ir(int indent) const {
    print_indent(indent);
    std::println("IRProgram(");
    if (func) {
        func->dump_ir(indent + 1);
    }
    print_indent(indent);
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
    print_indent(indent);
    std::println("Function(");
    print_indent(indent + 1);
    std::println("name='{}'", var_name);
    print_indent(indent + 1);
    std::println("instructions=[");
    for (const auto& instr : instructions) {
        if (instr) {
            instr->dump_ir(indent + 2);
        }
    }
    print_indent(indent + 1);
    std::println("]");
    print_indent(indent);
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

std::unique_ptr<IRValNode> // return the destination register
ExprNode::emit_ir(std::vector<std::unique_ptr<IRInstructionNode>>& instructions) {
    static size_t temp_counter = 0;
    auto get_tempvar = [&]() { return std::string("tmp.") + std::to_string(temp_counter++); };

    // case 1: constant literal
    if (constant) {
        auto ir_const = std::make_unique<IRConstNode>();
        ir_const->val = constant->val;
        return ir_const;
    }
    // case 2 & 3: expression with or without unary operator
    else if (expr) {
        // case 2: parenthesized or nested expression without operator
        if (!unary) {
            return expr->emit_ir(instructions);
        }

        // case 3: unary operation: op expr
        // get inner most expression's value
        auto src_val = expr->emit_ir(instructions);

        std::string tmp = get_tempvar();
        auto dest_return = std::make_unique<IRVariableNode>();
        dest_return->var_name = tmp;

        auto ir_unary = std::make_unique<IRUnaryNode>();
        ir_unary->op_type = unary->op_type; // "~" or "-"
        ir_unary->val_src = std::move(src_val);

        auto val_dest = std::make_unique<IRVariableNode>();
        val_dest->var_name = tmp;
        ir_unary->val_dest = std::move(val_dest);

        instructions.push_back(std::move(ir_unary));
        return dest_return;
    }

    throw std::runtime_error("Compiler Error: Shouldn't pass here raise issue");
}

void IRRetNode::dump_ir(int indent) const {
    print_indent(indent);
    if (val) {
        std::println("return {}", val->repr());
    } else {
        std::println("return <void>");
    }
}

void IRUnaryNode::dump_ir(int indent) const {
    print_indent(indent);
    const std::string dest = val_dest ? val_dest->repr() : "<tmp>";
    const std::string src = val_src ? val_src->repr() : "<src>";
    std::println("{} = {} {}", dest, op_type, src);
}

std::string IRConstNode::repr() const { return std::to_string(val); }
std::string IRVariableNode::repr() const { return var_name; }

/*
int main(int argc, char *argv[]) {
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
    for (const auto &[token_class, lexemes] : tokens) {
        std::println("{}, {}, {}", i++, token_class, lexemes);
    }
    auto ast = parse(tokens);
    ast->dump();
    auto ir = ast->emit_ir();
    ir->dump_ir();

    return 0;
}
// */