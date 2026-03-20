#include <cassert>
#include <memory>
#include <print>
#include <string>

#include "nanocc/IR/IR.hpp"
#include "nanocc/Utils/Utils.hpp"

#include "IRHelper.hpp"

namespace IRGen {

void programNodeIRDump(const IRProgramNode &ir_program, int indent) {
  for (const auto &decl : ir_program.top_level) {
    if (auto func = dyn_cast<IRFunctionNode>(decl.get())) {
      functionNodeIRDump(*func, indent);
    } else if (auto static_var = dyn_cast<IRStaticVarNode>(decl.get())) {
      staticVarNodeIRDump(*static_var, indent);
    } else {
      throw std::runtime_error("IR Dump Error: Unknown IRTopLevelNode type");
    }
  }
}

void functionNodeIRDump(const IRFunctionNode &ir_function, int indent) {
  printIndent(indent);
  std::println("Function[");
  printIndent(indent + 1);
  std::println("name='{}'", ir_function.func_name);
  printIndent(indent + 1);
  if (ir_function.parameters.empty()) {
    std::println("parameters=[]");
  } else {
    std::println("parameters=[");
    for (const auto &param : ir_function.parameters) {
      printIndent(indent + 2);
      std::println("'{}'", param);
    }
    printIndent(indent + 1);
    std::println("]");
  }
  printIndent(indent + 1);
  std::println("instructions=[");
  for (const auto &instr : ir_function.ir_instructions) {
    instructionNodeIRDump(*instr, indent + 2);
  }
  printIndent(indent + 1);
  std::println("]"); // end instructions
  printIndent(indent);
  std::println("]"); // end function
}

void staticVarNodeIRDump(const IRStaticVarNode &ir_static_var, int indent) {
  printIndent(indent);
  std::println("StaticVar[name='{}', global={}, init={}]",
               ir_static_var.var_name->name, ir_static_var.global,
               ir_static_var.init);
}

void retNodeIRDump(const IRRetNode &ret_node, int indent) {
  printIndent(indent);
  std::printf("return %s\n", ret_node.ret_val
                                 ? valNodeIRDump(*ret_node.ret_val).c_str()
                                 : "");
}

void unaryNodeIRDump(const IRUnaryNode &unary_node, int indent) {
  printIndent(indent);
  std::string dest = valNodeIRDump(*unary_node.val_dest);
  std::string src = valNodeIRDump(*unary_node.val_src);
  std::println("{} = {} {}", dest, unary_node.op_type, src);
}

void binaryNodeIRDump(const IRBinaryNode &binary_node, int indent) {
  printIndent(indent);
  std::string dest = valNodeIRDump(*binary_node.val_dest);
  std::string src1 = valNodeIRDump(*binary_node.val_src1);
  std::string src2 = valNodeIRDump(*binary_node.val_src2);
  std::println("{} = {} {} {}", dest, src1, binary_node.op_type, src2);
}

void copyNodeIRDump(const IRCopyNode &copy_node, int indent) {
  printIndent(indent);
  std::string dest = valNodeIRDump(*copy_node.val_dest);
  std::string src = valNodeIRDump(*copy_node.val_src);
  std::println("{} = {}", dest, src);
}

void jumpNodeIRDump(const IRJumpNode &jump_node, int indent) {
  printIndent(indent);
  std::println("jump {}", jump_node.target_label);
}

void jumpIfZeroNodeIRDump(const IRJumpIfZeroNode &jump_node, int indent) {
  printIndent(indent);
  std::string cond = valNodeIRDump(*jump_node.condition);
  std::println("jump_if_false {}, {}", cond, jump_node.target_label);
}

void jumpIfNotZeroNodeIRDump(const IRJumpIfNotZeroNode &jump_node, int indent) {
  printIndent(indent);
  std::string cond = valNodeIRDump(*jump_node.condition);
  std::println("jump_if_true {}, {}", cond, jump_node.target_label);
}

void labelNodeIRDump(const IRLabelNode &label_node, int indent) {
  assert(indent > 0);
  printIndent(indent - 1);
  std::println("{}:", label_node.label_name);
}

void functionCallNodeIRDump(const IRFunctionCallNode &func_call_node,
                            int indent) {
  printIndent(indent);
  std::print("{} = {}(", valNodeIRDump(*func_call_node.return_dest),
             func_call_node.func_name);
  for (size_t i = 0; i < func_call_node.arguments.size(); ++i) {
    std::print("{}", valNodeIRDump(*func_call_node.arguments[i]));
    if (i < func_call_node.arguments.size() - 1) {
      std::print(", ");
    }
  }
  std::println(")");
}

std::string valNodeIRDump(const IRValNode &val_node) {
  if (auto node = dyn_cast<IRConstNode>(&val_node)) {
    return std::to_string(node->IntVal);
  } else if (auto var_node = dyn_cast<IRVariableNode>(&val_node)) {
    return var_node->var_name;
  }
  throw std::runtime_error("IR Dump Error: Unknown IRValNode type");
}

void instructionNodeIRDump(const IRInstructionNode &instr_node, int indent) {
  if (auto ret_node = dyn_cast<IRRetNode>(&instr_node)) {
    retNodeIRDump(*ret_node, indent);
  } else if (auto unary_node = dyn_cast<IRUnaryNode>(&instr_node)) {
    unaryNodeIRDump(*unary_node, indent);
  } else if (auto binary_node = dyn_cast<IRBinaryNode>(&instr_node)) {
    binaryNodeIRDump(*binary_node, indent);
  } else if (auto copy_node = dyn_cast<IRCopyNode>(&instr_node)) {
    copyNodeIRDump(*copy_node, indent);
  } else if (auto jump_node = dyn_cast<IRJumpNode>(&instr_node)) {
    jumpNodeIRDump(*jump_node, indent);
  } else if (auto jump_if_zero_node = dyn_cast<IRJumpIfZeroNode>(&instr_node)) {
    jumpIfZeroNodeIRDump(*jump_if_zero_node, indent);
  } else if (auto jump_if_not_zero_node =
                 dyn_cast<IRJumpIfNotZeroNode>(&instr_node)) {
    jumpIfNotZeroNodeIRDump(*jump_if_not_zero_node, indent);
  } else if (auto label_node = dyn_cast<IRLabelNode>(&instr_node)) {
    labelNodeIRDump(*label_node, indent);
  } else if (auto func_call_node = dyn_cast<IRFunctionCallNode>(&instr_node)) {
    functionCallNodeIRDump(*func_call_node, indent);
  } else {
    throw std::runtime_error("IR Dump Error: Unknown IRInstructionNode type");
  }
}
} // namespace IRGen
