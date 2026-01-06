#include <print>
#include <string>
#include <cassert>
#include <memory>

#include "nanocc/IR/IR.hpp"
#include "nanocc/Utils.hpp"

#include "IRHelper.hpp"

void IRGen::irProgramNodeDump(std::unique_ptr<IRProgramNode>& ir_program, int indent) {
    for (auto& functions : ir_program->functions) {
        IRGen::irFunctionNodeDump(functions, indent);
    }
}

void IRGen::irFunctionNodeDump(std::unique_ptr<IRFunctionNode>& ir_function, int indent) {
    printIndent(indent);
    std::println("Function[");
    printIndent(indent + 1);
    std::println("name='{}'", ir_function->func_name);
    printIndent(indent + 1);
    if (ir_function->parameters.empty()) {
        std::println("parameters=[]");
    } else {
        std::println("parameters=[");
        for (auto& param : ir_function->parameters) {
            printIndent(indent + 2);
            std::println("'{}'", param);
        }
        printIndent(indent + 1);
        std::println("]");
    }
    printIndent(indent + 1);
    std::println("instructions=[");
    for (auto& instr : ir_function->ir_instructions) {
        irInstructionNodeDump(instr, indent + 2);
    }
    printIndent(indent + 1);
    std::println("]"); // end instructions
    printIndent(indent);
    std::println("]"); // end function
}

void IRGen::irRetNodeDump(IRRetNode* ret_node, int indent) {
    printIndent(indent);
    std::printf("return %s\n", ret_node->ret_val ? irValNodeDump(ret_node->ret_val).c_str() : "");
}

void IRGen::irUnaryNodeDump(IRUnaryNode* unary_node, int indent) {
    printIndent(indent);
    std::string dest = irValNodeDump(unary_node->val_dest);
    std::string src = irValNodeDump(unary_node->val_src);
    std::println("{} = {} {}", dest, unary_node->op_type, src);
}

void IRGen::irBinaryNodeDump(IRBinaryNode* binary_node, int indent) {
    printIndent(indent);
    std::string dest = irValNodeDump(binary_node->val_dest);
    std::string src1 = irValNodeDump(binary_node->val_src1);
    std::string src2 = irValNodeDump(binary_node->val_src2);
    std::println("{} = {} {} {}", dest, src1, binary_node->op_type, src2);
}

void IRGen::irCopyNodeDump(IRCopyNode* copy_node, int indent) {
    printIndent(indent);
    std::string dest = irValNodeDump(copy_node->val_dest);
    std::string src = irValNodeDump(copy_node->val_src);
    std::println("{} = {}", dest, src);
}

void IRGen::irJumpNodeDump(IRJumpNode* jump_node, int indent) {
    printIndent(indent);
    std::println("jump {}", jump_node->target_label);
}

void IRGen::irJumpIfZeroNodeDump(IRJumpIfZeroNode* jump_node, int indent) {
    printIndent(indent);
    std::string cond = irValNodeDump(jump_node->condition);
    std::println("jump_if_false {}, {}", cond, jump_node->target_label);
}

void IRGen::irJumpIfNotZeroNodeDump(IRJumpIfNotZeroNode* jump_node, int indent) {
    printIndent(indent);
    std::string cond = irValNodeDump(jump_node->condition);
    std::println("jump_if_true {}, {}", cond, jump_node->target_label);
}

void IRGen::irLabelNodeDump(IRLabelNode* label_node, int indent) {
    assert(indent > 0);
    printIndent(indent - 1);
    std::println("{}:", label_node->label_name);
}

void IRGen::irFunctionCallNodeDump(IRFunctionCallNode* func_call_node, int indent) {
    printIndent(indent);
    std::print("{} = {}(", irValNodeDump(func_call_node->return_dest), func_call_node->func_name);
    for (size_t i = 0; i < func_call_node->arguments.size(); ++i) {
        std::print("{}", irValNodeDump(func_call_node->arguments[i]));
        if (i < func_call_node->arguments.size() - 1) {
            std::print(", ");
        }
    }
    std::println(")");
}

std::string IRGen::irValNodeDump(std::shared_ptr<IRValNode>& val_node) {
    if (auto node = std::dynamic_pointer_cast<IRConstNode>(val_node)) {
        return node->val;
    } else if (auto var_node = std::dynamic_pointer_cast<IRVariableNode>(val_node)) {
        return var_node->var_name;
    }
    throw std::runtime_error("IR Dump Error: Unknown IRValNode type");
}

void IRGen::irInstructionNodeDump(std::unique_ptr<IRInstructionNode>& instr_node, int indent) {
    IRInstructionNode* ptr = instr_node.get();

    if (auto ret_node = dyn_cast<IRRetNode>(ptr)) {
        irRetNodeDump(ret_node, indent);
    } else if (auto unary_node = dyn_cast<IRUnaryNode>(ptr)) {
        irUnaryNodeDump(unary_node, indent);
    } else if (auto binary_node = dyn_cast<IRBinaryNode>(ptr)) {
        irBinaryNodeDump(binary_node, indent);
    } else if (auto copy_node = dyn_cast<IRCopyNode>(ptr)) {
        irCopyNodeDump(copy_node, indent);
    } else if (auto jump_node = dyn_cast<IRJumpNode>(ptr)) {
        irJumpNodeDump(jump_node, indent);
    } else if (auto jump_if_zero_node = dyn_cast<IRJumpIfZeroNode>(ptr)) {
        irJumpIfZeroNodeDump(jump_if_zero_node, indent);
    } else if (auto jump_if_not_zero_node = dyn_cast<IRJumpIfNotZeroNode>(ptr)) {
        irJumpIfNotZeroNodeDump(jump_if_not_zero_node, indent);
    } else if (auto label_node = dyn_cast<IRLabelNode>(ptr)) {
        irLabelNodeDump(label_node, indent);
    } else if (auto func_call_node = dyn_cast<IRFunctionCallNode>(ptr)) {
        irFunctionCallNodeDump(func_call_node, indent);
    } else {
        throw std::runtime_error("IR Dump Error: Unknown IRInstructionNode type");
    }
}