/* Dev Docs
what does generateIR of class X do?
> return IR form of that class X i.e IRXNode
> Not all classes have thier own IRXNode counterpart, they just
  emit IR instructions to be used in parent nodes

func XNode::emit_lr()
    make IRXNode
    IRXNode->ChildNode = ChildNode->generateIR()
    return IRXNode

use shared_ptr for IRValNode (and its derived classes)
*/

#include <print>
#include <string>
#include <utility>
#include <vector>
#include <memory>

#include "nanocc/AST/AST.hpp"
#include "nanocc/IR/IR.hpp"
#include "nanocc/Utils.hpp"

#include "IRHelper.hpp"

namespace { // helper function
void extendInstrFromVector(std::vector<std::unique_ptr<IRInstructionNode>>&& src,
                           std::vector<std::unique_ptr<IRInstructionNode>>& dest) {
    for (auto& instr : src) {
        dest.push_back(std::move(instr));
    }
}
} // namespace

namespace irgen {

std::unique_ptr<IRProgramNode> programNodeIRGen(std::unique_ptr<ProgramNode>& program_node) {
    auto ir_program = std::make_unique<IRProgramNode>();
    for (auto& function : program_node->functions) {
        if (function->body) {
            auto ir_function = functionDeclNodeIRGen(function);
            ir_program->functions.push_back(std::move(ir_function));
        }
    }
    return ir_program;
}

std::vector<std::unique_ptr<IRInstructionNode>>
declarationNodeIRGen(std::unique_ptr<DeclarationNode>& declaration) {
    if (declaration->func) {
        // ignore function's with no body, functions with
        // definitions are handled in ProgramNode
        return {};
    } else if (declaration->var) {
        return variableDeclNodeIRGen(declaration->var);
    } else {
        throw std::runtime_error("IR Generation Error: Empty DeclarationNode");
    }
}

std::unique_ptr<IRFunctionNode> functionDeclNodeIRGen(std::unique_ptr<FunctionDeclNode>& function) {
    // a function has many blocks => many instructions
    std::vector<std::unique_ptr<IRInstructionNode>> instructions;
    extendInstrFromVector(blockNodeIRGen(function->body), instructions);
    // handle edge case: ensure function ends with a return; always return 0 no matter what
    // if the func already ends with a return, this is redundant but okay for now
    auto ret0 = std::make_unique<IRRetNode>(std::make_shared<IRConstNode>("0"));
    instructions.push_back(std::move(ret0));

    auto ir_function =
        std::make_unique<IRFunctionNode>(function->func_name->name, std::move(instructions));

    for (const auto& param : function->parameters) {
        ir_function->parameters.push_back(param->name);
    }

    return ir_function;
}

std::vector<std::unique_ptr<IRInstructionNode>> blockNodeIRGen(std::unique_ptr<BlockNode>& block) {
    std::vector<std::unique_ptr<IRInstructionNode>> ir_instructions;
    for (auto& block_item : block->block_items) {
        extendInstrFromVector(blockItemNodeIRGen(block_item), ir_instructions);
    }
    return ir_instructions;
}

std::vector<std::unique_ptr<IRInstructionNode>>
blockItemNodeIRGen(std::unique_ptr<BlockItemNode>& block_item) {
    std::vector<std::unique_ptr<IRInstructionNode>> ir_instructions;
    if (block_item->declaration) {
        extendInstrFromVector(declarationNodeIRGen(block_item->declaration), ir_instructions);
    } else if (block_item->statement) {
        extendInstrFromVector(statementNodeIRGen(block_item->statement), ir_instructions);
    } else {
        throw std::runtime_error("IR Generation Error: Empty BlockItemNode");
    }
    return ir_instructions;
}

std::vector<std::unique_ptr<IRInstructionNode>>
variableDeclNodeIRGen(std::unique_ptr<VariableDeclNode>& var) {
    std::vector<std::unique_ptr<IRInstructionNode>> ir_instructions;
    if (var->init_expr) {
        // get the value of the initialization expression
        auto dest_var = exprNodeIRGen(var->init_expr, ir_instructions);
        // emit copy instruction to assign the value to the variable
        auto ir_var = std::make_shared<IRVariableNode>(var->var_identifier->name);
        auto ir_copy = std::make_unique<IRCopyNode>(std::move(dest_var), std::move(ir_var));
        ir_instructions.push_back(std::move(ir_copy));
    }
    // else, no initialization => no IR needed (default to 0)
    return ir_instructions;
}

std::vector<std::unique_ptr<IRInstructionNode>>
statementNodeIRGen(std::unique_ptr<StatementNode>& statement) {
    std::vector<std::unique_ptr<IRInstructionNode>> ir_instructions;

    // get ir from `ExprNode`; `emit_tacky` func from the book;
    // returns the destination register of prev operation i.e source register
    // for this iter
    if (statement->return_stmt) {
        extendInstrFromVector(returnNodeIRGen(statement->return_stmt), ir_instructions);
    } else if (statement->expression_stmt) {
        extendInstrFromVector(expressionNodeIRGen(statement->expression_stmt), ir_instructions);
    } else if (statement->ifelse_stmt) {
        extendInstrFromVector(ifElseNodeIRGen(statement->ifelse_stmt), ir_instructions);
    } else if (statement->compound_stmt) {
        extendInstrFromVector(compoundNodeIRGen(statement->compound_stmt), ir_instructions);
    } else if (statement->break_stmt) {
        extendInstrFromVector(breakNodeIRGen(statement->break_stmt), ir_instructions);
    } else if (statement->continue_stmt) {
        extendInstrFromVector(continueNodeIRGen(statement->continue_stmt), ir_instructions);
    } else if (statement->while_stmt) {
        extendInstrFromVector(whileNodeIRGen(statement->while_stmt), ir_instructions);
    } else if (statement->dowhile_stmt) {
        extendInstrFromVector(doWhileNodeIRGen(statement->dowhile_stmt), ir_instructions);
    } else if (statement->for_stmt) {
        extendInstrFromVector(forNodeIRGen(statement->for_stmt), ir_instructions);
    } else {
        if (!statement->null_stmt) { // do nothing for null statement
            throw std::runtime_error("IR Generation Error: Malformed StatementNode");
        }
    }
    return ir_instructions;
}

std::vector<std::unique_ptr<IRInstructionNode>>
returnNodeIRGen(std::unique_ptr<ReturnNode>& return_stmt) {
    std::vector<std::unique_ptr<IRInstructionNode>> ir_instructions;
    auto dest_var = exprNodeIRGen(return_stmt->ret_expr, ir_instructions);
    // emit return of the computed value
    auto ret_instruction = std::make_unique<IRRetNode>(std::move(dest_var));
    ir_instructions.push_back(std::move(ret_instruction));
    return ir_instructions;
}

std::vector<std::unique_ptr<IRInstructionNode>>
expressionNodeIRGen(std::unique_ptr<ExpressionNode>& expression_stmt) {
    std::vector<std::unique_ptr<IRInstructionNode>> ir_instructions;
    // this will return a new temporary variable holding the expression result
    // but we won't use it that again in the IR Generation
    auto dest_var = exprNodeIRGen(expression_stmt->expr, ir_instructions);
    return ir_instructions;
}

std::vector<std::unique_ptr<IRInstructionNode>>
ifElseNodeIRGen(std::unique_ptr<IfElseNode>& ifelse_stmt) {
    std::vector<std::unique_ptr<IRInstructionNode>> ir_instructions;

    // no else block => `end` label
    // else block present => `else` label
    std::string end_else_label = getLabelName(!ifelse_stmt->else_block ? "end" : "else");

    auto cond_var = exprNodeIRGen(ifelse_stmt->condition, ir_instructions);
    // if condition is false, jump to else / end
    auto jumpifzero = std::make_unique<IRJumpIfZeroNode>(cond_var, end_else_label);
    ir_instructions.push_back(std::move(jumpifzero));

    // emit if block instructions
    extendInstrFromVector(statementNodeIRGen(ifelse_stmt->if_block), ir_instructions);

    if (!ifelse_stmt->else_block) { // if condition only; else is absent
        // no else block; just place end label
        auto end_label = std::make_unique<IRLabelNode>(end_else_label);
        ir_instructions.push_back(std::move(end_label));
    } else { // if-else condition
        std::string end_label_str = getLabelName("end");
        ir_instructions.push_back(std::make_unique<IRJumpNode>(end_label_str));

        ir_instructions.push_back(std::make_unique<IRLabelNode>(end_else_label));

        extendInstrFromVector(statementNodeIRGen(ifelse_stmt->else_block), ir_instructions);

        ir_instructions.push_back(std::make_unique<IRLabelNode>(end_label_str));
    }
    return ir_instructions;
}

std::vector<std::unique_ptr<IRInstructionNode>>
compoundNodeIRGen(std::unique_ptr<CompoundNode>& compound_stmt) {
    return blockNodeIRGen(compound_stmt->block);
}

std::vector<std::unique_ptr<IRInstructionNode>>
breakNodeIRGen(std::unique_ptr<BreakNode>& break_stmt) {
    std::vector<std::unique_ptr<IRInstructionNode>> ir_instructions;

    std::string break_str = "break_" + break_stmt->label->name;
    auto jump_to_break = std::make_unique<IRJumpNode>(break_str);
    ir_instructions.push_back(std::move(jump_to_break));

    return ir_instructions;
}

std::vector<std::unique_ptr<IRInstructionNode>>
continueNodeIRGen(std::unique_ptr<ContinueNode>& continue_stmt) {
    std::vector<std::unique_ptr<IRInstructionNode>> ir_instructions;

    std::string continue_str = "continue_" + continue_stmt->label->name;
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
std::vector<std::unique_ptr<IRInstructionNode>>
whileNodeIRGen(std::unique_ptr<WhileNode>& while_stmt) {
    std::vector<std::unique_ptr<IRInstructionNode>> ir_instructions;

    // start/continue Label
    std::string start_cont_str = "continue_" + while_stmt->label->name;
    auto continue_label = std::make_unique<IRLabelNode>(start_cont_str);
    ir_instructions.push_back(std::move(continue_label));

    // condition instructions // jump to break if condition false
    auto cond_var = exprNodeIRGen(while_stmt->condition, ir_instructions);
    std::string break_str = "break_" + while_stmt->label->name;
    auto jump_if_zero = std::make_unique<IRJumpIfZeroNode>(cond_var, break_str);
    ir_instructions.push_back(std::move(jump_if_zero));

    // body instructions
    extendInstrFromVector(statementNodeIRGen(while_stmt->body), ir_instructions);

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
std::vector<std::unique_ptr<IRInstructionNode>>
doWhileNodeIRGen(std::unique_ptr<DoWhileNode>& dowhile_stmt) {
    std::vector<std::unique_ptr<IRInstructionNode>> ir_instructions;

    // start Label
    std::string start_str = "start_" + dowhile_stmt->label->name;
    auto start_label = std::make_unique<IRLabelNode>(start_str);
    ir_instructions.push_back(std::move(start_label));

    // body instructions
    extendInstrFromVector(statementNodeIRGen(dowhile_stmt->body), ir_instructions);

    // continue label
    std::string continue_str = "continue_" + dowhile_stmt->label->name;
    auto continue_label = std::make_unique<IRLabelNode>(continue_str);
    ir_instructions.push_back(std::move(continue_label));

    // condition instructions // jump to start if condition true
    auto cond_var = exprNodeIRGen(dowhile_stmt->condition, ir_instructions);
    auto jump_if_not_zero = std::make_unique<IRJumpIfNotZeroNode>(cond_var, start_str);
    ir_instructions.push_back(std::move(jump_if_not_zero));

    // break label
    std::string break_str = "break_" + dowhile_stmt->label->name;
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
std::vector<std::unique_ptr<IRInstructionNode>> forNodeIRGen(std::unique_ptr<ForNode>& for_stmt) {
    std::vector<std::unique_ptr<IRInstructionNode>> ir_instructions;

    // init
    extendInstrFromVector(forInitNodeIRGen(for_stmt->init), ir_instructions);

    // start
    std::string start_str = "start_" + for_stmt->label->name;
    auto start_label = std::make_unique<IRLabelNode>(start_str);
    ir_instructions.push_back(std::move(start_label));

    // condition instructions // jump to break if condition false
    // else if no condition => always true (use a non-zero constant 69; no jump needed)
    std::string break_str = "break_" + for_stmt->label->name;
    if (for_stmt->condition) {
        auto cond_var = exprNodeIRGen(for_stmt->condition, ir_instructions);
        auto jump_if_zero = std::make_unique<IRJumpIfZeroNode>(cond_var, break_str);
        ir_instructions.push_back(std::move(jump_if_zero));
    }

    // body instructions // can have break/continue
    // if continue is there here, will jump just before post instructions
    extendInstrFromVector(statementNodeIRGen(for_stmt->body), ir_instructions);

    // continue label
    std::string continue_str = "continue_" + for_stmt->label->name;
    auto continue_label = std::make_unique<IRLabelNode>(continue_str);
    ir_instructions.push_back(std::move(continue_label));

    // post instructions
    if (for_stmt->post) {
        auto post_var = exprNodeIRGen(for_stmt->post, ir_instructions);
    }

    // jump
    auto jump_to_start = std::make_unique<IRJumpNode>(start_str);
    ir_instructions.push_back(std::move(jump_to_start));

    // break label
    auto break_label = std::make_unique<IRLabelNode>(break_str);
    ir_instructions.push_back(std::move(break_label));

    return ir_instructions;
}

std::vector<std::unique_ptr<IRInstructionNode>>
nullNodeIRGen(std::unique_ptr<NullNode>& null_stmt) {
    return {}; // no-op for null statement
}

std::vector<std::unique_ptr<IRInstructionNode>>
forInitNodeIRGen(std::unique_ptr<ForInitNode>& init) {
    std::vector<std::unique_ptr<IRInstructionNode>> ir_instructions;
    if (init->declaration) {
        extendInstrFromVector(variableDeclNodeIRGen(init->declaration), ir_instructions);
    } else if (init->init_expr) {
        // we only need the instructions; the result is not used
        exprNodeIRGen(init->init_expr, ir_instructions);
    }
    return ir_instructions;
}

std::shared_ptr<IRValNode>
exprNodeIRGen(std::unique_ptr<ExprNode>& expr,
              std::vector<std::unique_ptr<IRInstructionNode>>& instructions) {
    assert(expr->left_exprf && "IR Generation Error: Malformed Expression");
    return exprFactorNodeIRGen(expr->left_exprf, instructions);
}

namespace { // helper functions to handle binary short-circuiting operators
/// @brief handle non-short-circuiting binary operations
std::shared_ptr<IRValNode>
handleOtherBinOps(BinaryNode* binop, // TODO(VachanVY): anyway to NOT use raw pointer here?
                  std::vector<std::unique_ptr<IRInstructionNode>>& instructions) {
    auto left_val = exprNodeIRGen(binop->left_expr, instructions);
    auto right_val = exprNodeIRGen(binop->right_expr, instructions);

    std::string tmp = getUniqueName("tmp");
    auto val_dest = std::make_shared<IRVariableNode>(tmp);

    auto ir_binary = std::make_unique<IRBinaryNode>(binop->op_type, left_val, right_val, val_dest);
    instructions.push_back(std::move(ir_binary));
    return val_dest;
}

/// @brief handle short-circuiting binary operations (&&, ||)
/// @param binop
/// @param instructions
/// @return result variable holding the final value of the operation
std::shared_ptr<IRValNode>
handleShortCircuitOps(BinaryNode* binop,
                      std::vector<std::unique_ptr<IRInstructionNode>>& instructions) {
    assert(binop->op_type == "&&" || binop->op_type == "||" && "Not a short-circuit operator");
    auto result = std::make_shared<IRVariableNode>(getUniqueName("tmp"));

    std::string short_label = getLabelName("short");
    std::string end_label = getLabelName("end");

    bool is_and = (binop->op_type == "&&");

    auto left_val = exprNodeIRGen(binop->left_expr, instructions);
    // jump to short-circuit if left determines the result
    if (is_and) {
        instructions.push_back(std::make_unique<IRJumpIfZeroNode>(left_val, short_label));
    } else {
        instructions.push_back(std::make_unique<IRJumpIfNotZeroNode>(left_val, short_label));
    }

    auto right_val = exprNodeIRGen(binop->right_expr, instructions);
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

std::shared_ptr<IRValNode>
exprFactorNodeIRGen(std::unique_ptr<ExprFactorNode>& exprf,
                    std::vector<std::unique_ptr<IRInstructionNode>>& instructions) {
    if (auto binop = dyn_cast<BinaryNode>(exprf.get())) {
        return binaryNodeIRGen(binop, instructions);
    } else if (auto assignop = dyn_cast<AssignmentNode>(exprf.get())) {
        return assignmentNodeIRGen(assignop, instructions);
    } else if (auto condop = dyn_cast<ConditionalNode>(exprf.get())) {
        return conditionalNodeIRGen(condop, instructions);
    } else if (exprf->var_identifier) {
        return varNodeIRGen(exprf->var_identifier, instructions);
    } else if (exprf->constant) {
        return constantNodeIRGen(exprf->constant, instructions);
    } else if (exprf->unary) {
        return unaryNodeIRGen(exprf->unary, instructions);
    } else if (exprf->expr) {
        return exprNodeIRGen(exprf->expr, instructions);
    } else if (exprf->func_call) {
        return functionCallNodeIRGen(exprf->func_call, instructions);
    }
    throw std::runtime_error("IR Generation Error: Malformed Expression Factor");
}

std::shared_ptr<IRValNode>
constantNodeIRGen(std::unique_ptr<ConstantNode>& constant,
                  std::vector<std::unique_ptr<IRInstructionNode>>& instructions) {
    return std::make_shared<IRConstNode>(constant->val);
}

std::shared_ptr<IRValNode>
varNodeIRGen(std::unique_ptr<VarNode>& var,
             std::vector<std::unique_ptr<IRInstructionNode>>& instructions) {
    return std::make_shared<IRVariableNode>(var->var_name->name);
}

std::shared_ptr<IRValNode>
unaryNodeIRGen(std::unique_ptr<UnaryNode>& unary,
               std::vector<std::unique_ptr<IRInstructionNode>>& instructions) {
    // get inner most expression's value
    auto src_val = exprFactorNodeIRGen(unary->operand, instructions);
    std::string tmp = getUniqueName("tmp");
    auto dest_var = std::make_shared<IRVariableNode>(tmp);

    auto ir_unary = std::make_unique<IRUnaryNode>(unary->op_type, src_val, dest_var);
    instructions.push_back(std::move(ir_unary));
    return dest_var;
}

std::shared_ptr<IRValNode>
binaryNodeIRGen(BinaryNode* binop, std::vector<std::unique_ptr<IRInstructionNode>>& instructions) {
    if (binop->op_type == "&&" || binop->op_type == "||") {
        return handleShortCircuitOps(binop, instructions);
    } else {
        return handleOtherBinOps(binop, instructions);
    }
}

std::shared_ptr<IRValNode>
assignmentNodeIRGen(AssignmentNode* assignop,
                    std::vector<std::unique_ptr<IRInstructionNode>>& instructions) {
    // evaluate rhs expr, add it's instructions to the list, then
    // evaluate lhs expr to get the variable to assign to result of rhs. add instructions to
    // list on the way...
    auto right_val = exprNodeIRGen(assignop->right_expr, instructions); // result of the rhs expr
    auto left_val = exprNodeIRGen(assignop->left_expr, instructions);   // VarNode->generateIR()

    auto ir_copy = std::make_unique<IRCopyNode>(right_val, left_val); // left_val <= right_val
    instructions.push_back(std::move(ir_copy));

    return left_val;
}

std::shared_ptr<IRValNode>
conditionalNodeIRGen(ConditionalNode* condop,
                     std::vector<std::unique_ptr<IRInstructionNode>>& instructions) {
    // result = condition ? true_expr : false_expr
    auto result = std::make_shared<IRVariableNode>(getUniqueName("tmp"));

    // eval condition
    auto cond_val = exprNodeIRGen(condop->condition, instructions);
    std::string else_label = getLabelName("else_branch");
    instructions.push_back(std::make_unique<IRJumpIfZeroNode>(cond_val, else_label));

    // if true
    auto true_val = exprNodeIRGen(condop->true_expr, instructions);
    instructions.push_back(std::make_unique<IRCopyNode>(true_val, result));

    std::string end_label = getLabelName("end");
    instructions.push_back(std::make_unique<IRJumpNode>(end_label));

    // else branch
    instructions.push_back(std::make_unique<IRLabelNode>(else_label));
    auto false_val = exprNodeIRGen(condop->false_expr, instructions);
    instructions.push_back(std::make_unique<IRCopyNode>(false_val, result));

    // end
    instructions.push_back(std::make_unique<IRLabelNode>(end_label));
    return result;
}

std::shared_ptr<IRValNode>
functionCallNodeIRGen(std::unique_ptr<FunctionCallNode>& func_call,
                      std::vector<std::unique_ptr<IRInstructionNode>>& instructions) {
    auto func_name = func_call->func_identifier->name;
    std::vector<std::shared_ptr<IRValNode>> args_vals;
    for (auto& arg : func_call->arguments) {
        // `ExprNode::generateIR` returns the destination variable of the expression
        // we need that to pass as argument to the function call
        args_vals.push_back(exprNodeIRGen(arg, instructions));
    }
    auto result = std::make_shared<IRVariableNode>(getUniqueName("tmp"));
    auto ir_func_call = std::make_unique<IRFunctionCallNode>(func_name, args_vals, result);
    instructions.push_back(std::move(ir_func_call));
    return result;
}
} // namespace irgen
