#include <print>
#include <string>
#include <utility>

#include "irgen.hpp"
#include "parser.hpp"
#include "asmgen.hpp"
#include "utils.hpp"

/* Dev Docs
what does generateIR of class X do?
> return IR form of that class X i.e IRXNode

func XNode::emit_lr()
    make IRXNode
    IRXNode->ChildNode = ChildNode->generateIR()
    return IRXNode

use shared_ptr for IRValNode (and its derived classes)
*/

std::unique_ptr<IRProgramNode> ProgramNode::generateIR() {
    auto ir_program = std::make_unique<IRProgramNode>();
    ir_program->func = func->generateIR();
    return ir_program;
}

void IRProgramNode::dump(int indent) const {
    printIndent(indent);
    std::println("IRProgram(");
    func->dump(indent + 1);
    printIndent(indent);
    std::println(")");
}

std::unique_ptr<IRFunctionNode> FunctionNode::generateIR() {
    // a function has many blocks => many instructions
    std::vector<std::unique_ptr<IRInstructionNode>> instructions;
    auto body_instructions = body->generateIR();
    for (auto& instr : body_instructions) {
        instructions.push_back(std::move(instr));
    }
    // edge case: ensure function ends with a return; always return 0 no matter what
    // if the func already ends with a return, this is redundant but okay for now
    auto ret0 = std::make_unique<IRRetNode>(std::make_shared<IRConstNode>("0"));
    instructions.push_back(std::move(ret0));

    auto ir_function =
        std::make_unique<IRFunctionNode>(var_identifier->name, std::move(instructions));
    return ir_function;
}

void IRFunctionNode::dump(int indent) const {
    printIndent(indent);
    std::println("Function(");
    printIndent(indent + 1);
    std::println("name='{}'", var_name);
    printIndent(indent + 1);
    std::println("instructions=[");
    for (const auto& instr : instructions) {
        if (instr) {
            instr->dump(indent + 2);
        }
    }
    printIndent(indent + 1);
    std::println("]");
    printIndent(indent);
    std::println(")");
}

std::vector<std::unique_ptr<IRInstructionNode>> BlockNode::generateIR() {
    std::vector<std::unique_ptr<IRInstructionNode>> ir_instructions;
    for (const auto& block_item : this->block_items) {
        auto item_instructions = block_item->generateIR();
        for (auto& instr : item_instructions) {
            ir_instructions.push_back(std::move(instr));
        }
    }
    return ir_instructions;
}

std::vector<std::unique_ptr<IRInstructionNode>> BlockItemNode::generateIR() {
    std::vector<std::unique_ptr<IRInstructionNode>> ir_instructions;
    if (declaration) {
        auto decl_instructions = declaration->generateIR();
        for (auto& instr : decl_instructions) {
            ir_instructions.push_back(std::move(instr));
        }
    } else if (statement) {
        auto stmt_instructions = statement->generateIR();
        for (auto& instr : stmt_instructions) {
            ir_instructions.push_back(std::move(instr));
        }
    } else {
        throw std::runtime_error("IR Generation Error: Empty BlockItemNode");
    }
    return ir_instructions;
}

std::vector<std::unique_ptr<IRInstructionNode>> DeclarationNode::generateIR() {
    std::vector<std::unique_ptr<IRInstructionNode>> ir_instructions;
    if (init_expr) {
        // get the value of the initialization expression
        auto dest_var = init_expr->generateIR(ir_instructions);
        // emit copy instruction to assign the value to the variable
        auto ir_var = std::make_shared<IRVariableNode>(var_identifier->name);
        auto ir_copy = std::make_unique<IRCopyNode>(std::move(dest_var), std::move(ir_var));
        ir_instructions.push_back(std::move(ir_copy));
    }
    // else, no initialization => no IR needed (default to 0)
    return ir_instructions;
}

std::vector<std::unique_ptr<IRInstructionNode>> StatementNode::generateIR() {
    std::vector<std::unique_ptr<IRInstructionNode>> ir_instructions;

    // get ir from `ExprNode`; `emit_tacky` func from the book;
    // returns the destination register of prev operation i.e source register
    // for this iter
    if (return_stmt) {
        auto ret_instructions = return_stmt->generateIR();
        for (auto& instr : ret_instructions) {
            ir_instructions.push_back(std::move(instr));
        }
    } else if (expression_stmt) {
        auto expr_instructions = expression_stmt->generateIR();
        for (auto& instr : expr_instructions) {
            ir_instructions.push_back(std::move(instr));
        }
    } else if (ifelse_stmt) {
        auto ifelse_instructions = ifelse_stmt->generateIR();
        for (auto& instr : ifelse_instructions) {
            ir_instructions.push_back(std::move(instr));
        }
    } else if (compound_stmt) {
        auto compound_instructions = compound_stmt->generateIR();
        for (auto& instr : compound_instructions) {
            ir_instructions.push_back(std::move(instr));
        }
    } else if (break_stmt) {
        auto break_instructions = break_stmt->generateIR();
        for (auto& instr : break_instructions) {
            ir_instructions.push_back(std::move(instr));
        }
    } else if (continue_stmt) {
        auto continue_instructions = continue_stmt->generateIR();
        for (auto& instr : continue_instructions) {
            ir_instructions.push_back(std::move(instr));
        }
    } else if (while_stmt) {
        auto while_instructions = while_stmt->generateIR();
        for (auto& instr : while_instructions) {
            ir_instructions.push_back(std::move(instr));
        }
    } else if (dowhile_stmt) {
        auto dowhile_instructions = dowhile_stmt->generateIR();
        for (auto& instr : dowhile_instructions) {
            ir_instructions.push_back(std::move(instr));
        }
    } else if (for_stmt) {
        auto for_instructions = for_stmt->generateIR();
        for (auto& instr : for_instructions) {
            ir_instructions.push_back(std::move(instr));
        }
    } else {
        if (!null_stmt) { // do nothing for null statement
            throw std::runtime_error("IR Generation Error: Malformed StatementNode");
        }
    }
    return ir_instructions;
}

std::vector<std::unique_ptr<IRInstructionNode>> ReturnNode::generateIR() {
    std::vector<std::unique_ptr<IRInstructionNode>> ir_instructions;
    auto dest_var = ret_expr->generateIR(ir_instructions);
    // emit return of the computed value
    auto ret_instruction = std::make_unique<IRRetNode>(std::move(dest_var));
    ir_instructions.push_back(std::move(ret_instruction));
    return ir_instructions;
}

std::vector<std::unique_ptr<IRInstructionNode>> ExpressionNode::generateIR() {
    std::vector<std::unique_ptr<IRInstructionNode>> ir_instructions;
    // this will return a new temporary variable holding the expression result
    // but we won't use it that again in the IR Generation
    auto dest_var = expr->generateIR(ir_instructions);
    return ir_instructions;
}

std::vector<std::unique_ptr<IRInstructionNode>> IfElseNode::generateIR() {
    std::vector<std::unique_ptr<IRInstructionNode>> ir_instructions;

    // no else block => `end` label
    // else block present => `else` label
    std::string end_else_label = getLabelName(!else_block ? "end" : "else");

    auto cond_var = condition->generateIR(ir_instructions);
    // if condition is false, jump to else / end
    auto jumpifzero = std::make_unique<IRJumpIfZeroNode>(cond_var, end_else_label);
    ir_instructions.push_back(std::move(jumpifzero));

    // emit if block instructions
    auto if_instr = if_block->generateIR();
    for (auto& instr : if_instr) {
        ir_instructions.push_back(std::move(instr));
    }

    if (!else_block) { // if condition only; else is absent
        // no else block; just place end label
        auto end_label = std::make_unique<IRLabelNode>(end_else_label);
        ir_instructions.push_back(std::move(end_label));
    } else { // if-else condition
        std::string end_label_str = getLabelName("end");
        ir_instructions.push_back(std::make_unique<IRJumpNode>(end_label_str));

        ir_instructions.push_back(std::make_unique<IRLabelNode>(end_else_label));

        auto else_instr = else_block->generateIR();
        for (auto& instr : else_instr) {
            ir_instructions.push_back(std::move(instr));
        }

        ir_instructions.push_back(std::make_unique<IRLabelNode>(end_label_str));
    }
    return ir_instructions;
}

std::vector<std::unique_ptr<IRInstructionNode>> CompoundNode::generateIR() { return block->generateIR(); }

std::vector<std::unique_ptr<IRInstructionNode>> BreakNode::generateIR() {
    std::vector<std::unique_ptr<IRInstructionNode>> ir_instructions;

    std::string break_str = "break_" + label->name;
    auto jump_to_break = std::make_unique<IRJumpNode>(break_str);
    ir_instructions.push_back(std::move(jump_to_break));

    return ir_instructions;
}

std::vector<std::unique_ptr<IRInstructionNode>> ContinueNode::generateIR() {
    std::vector<std::unique_ptr<IRInstructionNode>> ir_instructions;

    std::string continue_str = "continue_" + label->name;
    auto jump_to_continue = std::make_unique<IRJumpNode>(continue_str);
    ir_instructions.push_back(std::move(jump_to_continue));

    return ir_instructions;
}

/* ```
while (<condition>) {
    <body>; // can have break/continue
    // continue will jump here; will go to <condition> check anyway
    // so we will use start and continue as same label
}
// break will jump here
``` */
std::vector<std::unique_ptr<IRInstructionNode>> WhileNode::generateIR() {
    std::vector<std::unique_ptr<IRInstructionNode>> ir_instructions;

    // start/continue Label
    std::string start_cont_str = "continue_" + label->name;
    auto continue_label = std::make_unique<IRLabelNode>(start_cont_str);
    ir_instructions.push_back(std::move(continue_label));

    // condition instructions // jump to break if condition false
    auto cond_var = condition->generateIR(ir_instructions);
    std::string break_str = "break_" + label->name;
    auto jump_if_zero = std::make_unique<IRJumpIfZeroNode>(cond_var, break_str);
    ir_instructions.push_back(std::move(jump_if_zero));

    // body instructions
    auto body_instr = body->generateIR();
    for (auto& instr : body_instr) {
        ir_instructions.push_back(std::move(instr));
    }

    // jump back to start/continue
    auto jump_to_continue = std::make_unique<IRJumpNode>(start_cont_str);
    ir_instructions.push_back(std::move(jump_to_continue));

    // break label
    auto break_label = std::make_unique<IRLabelNode>(break_str);
    ir_instructions.push_back(std::move(break_label));

    return ir_instructions;
}

/* ```
do {
    <body>; // can have break/continue
    // continue will jump here
} while (<condition>);
// break will jump here
```*/
std::vector<std::unique_ptr<IRInstructionNode>> DoWhileNode::generateIR() {
    std::vector<std::unique_ptr<IRInstructionNode>> ir_instructions;

    // start Label
    std::string start_str = "start_" + label->name;
    auto start_label = std::make_unique<IRLabelNode>(start_str);
    ir_instructions.push_back(std::move(start_label));

    // body instructions
    auto body_instr = body->generateIR();
    for (auto& instr : body_instr) {
        ir_instructions.push_back(std::move(instr));
    }

    // continue label
    std::string continue_str = "continue_" + label->name;
    auto continue_label = std::make_unique<IRLabelNode>(continue_str);
    ir_instructions.push_back(std::move(continue_label));

    // condition instructions // jump to start if condition true
    auto cond_var = condition->generateIR(ir_instructions);
    auto jump_if_not_zero = std::make_unique<IRJumpIfNotZeroNode>(cond_var, start_str);
    ir_instructions.push_back(std::move(jump_if_not_zero));

    // break label
    std::string break_str = "break_" + label->name;
    auto break_label = std::make_unique<IRLabelNode>(break_str);
    ir_instructions.push_back(std::move(break_label));

    return ir_instructions;
}

/* ```
for(<init>; <condition>; <post>) {
    <body>; // can have break/continue
    // continue will jump here; will go to <post> anyway
}
<init>
<condition> <--, // jump to break_label if false
<body>         | continue_label after <body>
<post> --------' break_label    after <post>
``` */
std::vector<std::unique_ptr<IRInstructionNode>> ForNode::generateIR() {
    std::vector<std::unique_ptr<IRInstructionNode>> ir_instructions;

    // init
    auto init_instructions = init->generateIR();
    for (auto& instr : init_instructions) {
        ir_instructions.push_back(std::move(instr));
    }

    // start
    std::string start_str = "start_" + label->name;
    auto start_label = std::make_unique<IRLabelNode>(start_str);
    ir_instructions.push_back(std::move(start_label));

    // condition instructions // jump to break if condition false
    // else if no condition => always true (use a non-zero constant 69; no jump needed)
    std::string break_str = "break_" + label->name;
    if (condition) {
        auto cond_var = condition->generateIR(ir_instructions);
        auto jump_if_zero = std::make_unique<IRJumpIfZeroNode>(cond_var, break_str);
        ir_instructions.push_back(std::move(jump_if_zero));
    }

    // body instructions // can have break/continue
    // if continue is there here, will jump just before post instructions
    auto body_instr = body->generateIR();
    for (auto& instr : body_instr) {
        ir_instructions.push_back(std::move(instr));
    }

    // continue label
    std::string continue_str = "continue_" + label->name;
    auto continue_label = std::make_unique<IRLabelNode>(continue_str);
    ir_instructions.push_back(std::move(continue_label));

    // post instructions
    if (post) {
        auto post_var = post->generateIR(ir_instructions);
    }

    // jump
    auto jump_to_start = std::make_unique<IRJumpNode>(start_str);
    ir_instructions.push_back(std::move(jump_to_start));

    // break label
    auto break_label = std::make_unique<IRLabelNode>(break_str);
    ir_instructions.push_back(std::move(break_label));

    return ir_instructions;
}

std::vector<std::unique_ptr<IRInstructionNode>> NullNode::generateIR() {
    return {}; // no-op for null statement
}

std::vector<std::unique_ptr<IRInstructionNode>> ForInitNode::generateIR() {
    std::vector<std::unique_ptr<IRInstructionNode>> ir_instructions;
    if (declaration) {
        auto decl_instructions = declaration->generateIR();
        for (auto& instr : decl_instructions) {
            ir_instructions.push_back(std::move(instr));
        }
    } else if (init_expr) {
        // we only need the instructions; the result is not used
        init_expr->generateIR(ir_instructions);
    }
    return ir_instructions;
}

std::shared_ptr<IRValNode> // return the destination register
ExprNode::generateIR(std::vector<std::unique_ptr<IRInstructionNode>>& instructions) {
    assert(left_exprf && "IR Generation Error: Malformed Expression");
    return left_exprf->generateIR(instructions);
}

namespace { // helper functions to handle binary short-circuiting operators
/// @brief handle non-short-circuiting binary operations
std::shared_ptr<IRValNode>
handleOtherBinOps(BinaryNode* binop, // TODO(VachanVY): anyway to NOT use raw pointer here?
                  std::vector<std::unique_ptr<IRInstructionNode>>& instructions) {
    auto left_val = binop->left_expr->generateIR(instructions);
    auto right_val = binop->right_expr->generateIR(instructions);

    std::string tmp = getUniqueName("tmp");
    auto val_dest = std::make_shared<IRVariableNode>(tmp);

    auto ir_binary = std::make_unique<IRBinaryNode>(binop->op_type, left_val, right_val, val_dest);
    instructions.push_back(std::move(ir_binary));
    return val_dest;
}

std::shared_ptr<IRValNode>
handleShortCircuitOps(BinaryNode* binop,
                      std::vector<std::unique_ptr<IRInstructionNode>>& instructions) {
    assert(binop->op_type == "&&" || binop->op_type == "||" && "Not a short-circuit operator");
    auto result = std::make_shared<IRVariableNode>(getUniqueName("tmp"));

    std::string short_label = getLabelName("short");
    std::string end_label = getLabelName("end");

    bool is_and = (binop->op_type == "&&");

    auto left_val = binop->left_expr->generateIR(instructions);
    // jump to short-circuit if left determines the result
    if (is_and) {
        instructions.push_back(std::make_unique<IRJumpIfZeroNode>(left_val, short_label));
    } else {
        instructions.push_back(std::make_unique<IRJumpIfNotZeroNode>(left_val, short_label));
    }

    auto right_val = binop->right_expr->generateIR(instructions);
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
ExprFactorNode::generateIR(std::vector<std::unique_ptr<IRInstructionNode>>& instructions) {
    if (auto binop = dyn_cast<BinaryNode>(this)) {
        if (binop->op_type == "&&" || binop->op_type == "||") {
            return handleShortCircuitOps(binop, instructions);
        } else {
            return handleOtherBinOps(binop, instructions);
        }
    } else if (auto assignop = dyn_cast<AssignmentNode>(this)) {
        // evaluate rhs expr, add it's instructions to the list, then
        // evaluate lhs expr to get the variable to assign to result of rhs. add instructions to
        // list on the way...
        auto right_val = assignop->right_expr->generateIR(instructions); // result of the rhs expr
        auto left_val = assignop->left_expr->generateIR(instructions);   // VarNode->generateIR()

        auto ir_copy = std::make_unique<IRCopyNode>(right_val, left_val); // left_val <= right_val
        instructions.push_back(std::move(ir_copy));
        return left_val;
    } else if (auto condop = dyn_cast<ConditionalNode>(this)) {
        // result = condition ? true_expr : false_expr
        auto result = std::make_shared<IRVariableNode>(getUniqueName("tmp"));

        // eval condition
        auto cond_val = condop->condition->generateIR(instructions);
        std::string else_label = getLabelName("else_branch");
        instructions.push_back(std::make_unique<IRJumpIfZeroNode>(cond_val, else_label));

        // if true
        auto true_val = condop->true_expr->generateIR(instructions);
        instructions.push_back(std::make_unique<IRCopyNode>(true_val, result));

        std::string end_label = getLabelName("end");
        instructions.push_back(std::make_unique<IRJumpNode>(end_label));

        // else branch
        instructions.push_back(std::make_unique<IRLabelNode>(else_label));
        auto false_val = condop->false_expr->generateIR(instructions);
        instructions.push_back(std::make_unique<IRCopyNode>(false_val, result));

        // end
        instructions.push_back(std::make_unique<IRLabelNode>(end_label));
        return result;
    } else if (var_identifier) {
        auto ir_var = std::make_shared<IRVariableNode>(var_identifier->var_name->name);
        return ir_var;
    } else if (constant) {
        auto ir_const = std::make_shared<IRConstNode>(constant->val);
        return ir_const;
    } else if (unary) {
        // get inner most expression's value
        auto src_val = unary->operand->generateIR(instructions);
        std::string tmp = getUniqueName("tmp");
        auto dest_var = std::make_shared<IRVariableNode>(tmp);

        auto ir_unary = std::make_unique<IRUnaryNode>(unary->op_type, src_val, dest_var);
        instructions.push_back(std::move(ir_unary));
        return dest_var;
    } else if (expr) {
        return expr->generateIR(instructions);
    }
    throw std::runtime_error("IR Generation Error: Malformed Expression Factor");
}

void IRRetNode::dump(int indent) const {
    printIndent(indent);
    if (val) {
        std::println("return {}", val->dump());
    } else {
        std::println("return <void>");
    }
}

void IRUnaryNode::dump(int indent) const {
    printIndent(indent);
    const std::string dest = val_dest->dump();
    const std::string src = val_src->dump();
    std::println("{} = {} {}", dest, op_type, src);
}

void IRBinaryNode::dump(int indent) const {
    printIndent(indent);
    const std::string dest = val_dest->dump();
    const std::string src1 = val_src1->dump();
    const std::string src2 = val_src2->dump();
    std::println("{} = {} {} {}", dest, src1, op_type, src2);
}

void IRCopyNode::dump(int indent) const {
    printIndent(indent);
    const std::string dest = val_dest->dump();
    const std::string src = val_src->dump();
    std::println("{} = {}", dest, src);
}

void IRJumpNode::dump(int indent) const {
    printIndent(indent);
    std::println("jump {}", target_label);
}

void IRJumpIfZeroNode::dump(int indent) const {
    printIndent(indent);
    const std::string cond = condition->dump();
    std::println("jump_if_zero {}, {}", cond, target_label);
}

void IRJumpIfNotZeroNode::dump(int indent) const {
    printIndent(indent);
    const std::string cond = condition->dump();
    std::println("jump_if_not_zero {}, {}", cond, target_label);
}

void IRLabelNode::dump(int indent) const {
    assert(indent > 0);
    printIndent(indent - 1);
    std::println("{}:", label_name);
}

std::string IRConstNode::dump() const { return val; }
std::string IRVariableNode::dump() const { return var_name; }



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
    auto tokens = lexer(contents, true);
    auto ast = parse(tokens, true);

    SymbolTable global_sym_table;
    ast->resolveTypes(global_sym_table);
    if (true) {
        std::println("----- Identifier Resolution -----");
        ast->dump();
        std::println("---------------------------------");
    }

    std::string loop_label = "";
    ast->loopLabelling(loop_label);
    if (true) {
        std::println("----- Loop Labelling -----");
        ast->dump();
        std::println("--------------------------");
    }

    auto ir_program = ast->generateIR();
    if (true) {
        std::println("----------- IR Generation -----------");
        ir_program->dump();
        std::println("-------------------------------------");
    }
    return 0;
}
// */