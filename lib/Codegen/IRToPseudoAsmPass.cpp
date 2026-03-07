#include <algorithm>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "IRToPseudoAsmHelper.hpp"
#include "nanocc/Codegen/ASM.hpp"
#include "nanocc/IR/IR.hpp"
#include "nanocc/Target/X86/X86TargetInfo.hpp"
#include "nanocc/Utils/Utils.hpp"

// Emit Assembly Functions -- Start

namespace {
static const std::unordered_map<std::string, std::string> RELATIONAL_OPS = {
    {"==", "e"},  {"!=", "ne"}, {"<", "l"},
    {"<=", "le"}, {">", "g"},   {">=", "ge"}};

constexpr std::string getCondCode(const std::string &op_type) {
  auto it = RELATIONAL_OPS.find(op_type);
  if (it == RELATIONAL_OPS.end()) {
    throw std::runtime_error("getCondCode: Unsupported relational operator " +
                             op_type);
  }
  return it->second;
}
} // namespace

namespace AsmGen {
/// @brief Lowers the given IR program to an ASM program.
/// @return The lowered ASM program.
std::unique_ptr<AsmProgramNode> programLowerIRToAsm(IRProgramNode &program) {
  auto asm_program = std::make_unique<AsmProgramNode>();
  for (auto &top_level : program.top_level) {
    if (auto *func = dyn_cast<IRFunctionNode>(top_level.get())) {
      asm_program->top_level.push_back(functionLowerIRToAsm(*func));
    } else if (auto *static_var = dyn_cast<IRStaticVarNode>(top_level.get())) {
      asm_program->top_level.push_back(staticVarLowerIRToAsm(*static_var));
    } else {
      throw std::runtime_error(
          "programLowerIRToAsm: unknown IR top level node type");
    }
  }
  return asm_program;
}

// function caller: calls other functions if present
std::unique_ptr<AsmFunctionNode> functionLowerIRToAsm(IRFunctionNode &func) {
  auto asm_func =
      std::make_unique<AsmFunctionNode>(func.func_name, func.global);

  // place all parameters currently in the registers on to the stack,
  // the registers might get used by any function call
  // so to not lose their values, we push them onto the stack
  // we will OPTIMIZE this later in REGISTER ALLOCATION phase
  /// MOVING PARAMS FROM REGISTERS TO PSEUDO REGISTERS, USE 32 BIT REGISTERS
  int num_args = func.parameters.size();
  Reg arg_resisters[6] = {Reg::edi, Reg::esi, Reg::edx,
                          Reg::ecx, Reg::r8d, Reg::r9d};
  for (int i = 0; i < std::min(num_args, 6);
       i++) { // `0` to `5` `if num_args >=6`
    auto psedo_reg = std::make_shared<AsmPseudoNode>(func.parameters[i]);
    asm_func->instructions.push_back(std::make_unique<AsmMovNode>(
        std::make_shared<AsmRegisterNode>(getRegString(arg_resisters[i])),
        psedo_reg));
  }

  // how do we know the stack locations for the next arguments beyond 6 are
  // placed in Stack(16), Stack(24) and so on? => when this function was called
  // (`call func`), the address of the next instruction is pushed to the stack,
  // then the control is transferred to the function. Then in the function
  // prologue, we push %rbp. So 0(%rbp) = saved rbp, 8(%rbp) = return address,
  // 16(%rbp) = 7th arg, 24(%rbp) = 8th arg, and so on.
  // The first 6 args are in registers as per the calling convention.
  for (int i = 6; i < num_args; i++) { // `6` to `num_args-1`
    auto psedo_reg = std::make_shared<AsmPseudoNode>(func.parameters[i]);
    auto stack_loc =
        std::make_shared<AsmStackNode>(8 * (i - 4)); // +16, +24, +32, ...
    asm_func->instructions.push_back(
        std::make_unique<AsmMovNode>(stack_loc, psedo_reg));
  }

  for (const auto &instr : func.ir_instructions) {
    // every IR instruction can emit multiple ASM instructions
    auto asm_instrs = instructionLowerIRToAsm(instr);
    for (auto &asm_instr : asm_instrs) {
      asm_func->instructions.push_back(std::move(asm_instr));
    }
  }
  return asm_func;
}

std::unique_ptr<AsmStaticVariableNode>
staticVarLowerIRToAsm(IRStaticVarNode &static_var) {
  return std::make_unique<AsmStaticVariableNode>(
      static_var.var_name->name, static_var.global, static_var.init);
}

std::vector<std::unique_ptr<AsmInstructionNode>>
instructionLowerIRToAsm(const std::unique_ptr<IRInstructionNode> &instr) {
  if (auto *node = dyn_cast<IRRetNode>(instr.get()))
    return retLowerIRToAsm(*node);
  if (auto *node = dyn_cast<IRUnaryNode>(instr.get()))
    return unaryLowerIRToAsm(*node);
  if (auto *node = dyn_cast<IRBinaryNode>(instr.get()))
    return binaryLowerIRToAsm(*node);
  if (auto *node = dyn_cast<IRCopyNode>(instr.get()))
    return copyLowerIRToAsm(*node);
  if (auto *node = dyn_cast<IRJumpNode>(instr.get()))
    return jumpLowerIRToAsm(*node);
  if (auto *node = dyn_cast<IRJumpIfZeroNode>(instr.get()))
    return jumpIfZeroLowerIRToAsm(*node);
  if (auto *node = dyn_cast<IRJumpIfNotZeroNode>(instr.get()))
    return jumpIfNotZeroLowerIRToAsm(*node);
  if (auto *node = dyn_cast<IRLabelNode>(instr.get()))
    return labelLowerIRToAsm(*node);
  if (auto *node = dyn_cast<IRFunctionCallNode>(instr.get()))
    return functionCallLowerIRToAsm(*node);

  throw std::runtime_error(
      "instructionLowerIRToAsm: unknown IR instruction type");
}

std::shared_ptr<AsmOperandNode>
operandLowerIRToAsm(const std::shared_ptr<IRValNode> &val) {
  if (auto *node = dyn_cast<IRConstNode>(val.get()))
    return std::make_shared<AsmImmediateNode>(node->IntVal);
  if (auto *node = dyn_cast<IRVariableNode>(val.get()))
    return std::make_shared<AsmPseudoNode>(node->var_name);

  throw std::runtime_error("operandLowerIRToAsm: unknown IR value type");
}

std::vector<std::unique_ptr<AsmInstructionNode>>
retLowerIRToAsm(IRRetNode &node) {
  std::vector<std::unique_ptr<AsmInstructionNode>> instructions;

  // IR Ret = Asm Mov + Ret instructions
  if (node.ret_val) {
    auto src = operandLowerIRToAsm(node.ret_val);
    auto dest = std::make_shared<AsmRegisterNode>(getRegString(Reg::eax));
    instructions.push_back(std::make_unique<AsmMovNode>(src, dest));
  }
  instructions.push_back(std::make_unique<AsmRetNode>());
  return instructions;
}

std::vector<std::unique_ptr<AsmInstructionNode>>
unaryLowerIRToAsm(IRUnaryNode &node) {
  std::vector<std::unique_ptr<AsmInstructionNode>> instructions;

  if (node.op_type == "!") { // relational op
    auto src = operandLowerIRToAsm(node.val_src);
    auto dest = operandLowerIRToAsm(node.val_dest);

    instructions.push_back(std::make_unique<AsmCmpNode>(
        std::make_shared<AsmImmediateNode>(0), src));
    instructions.push_back(std::make_unique<AsmMovNode>(
        std::make_shared<AsmImmediateNode>(0), dest));
    instructions.push_back(std::make_unique<AsmSetCCNode>("e", dest));
  } else if (node.op_type == "~" || node.op_type == "-") {
    auto dest = operandLowerIRToAsm(node.val_dest);
    auto src = operandLowerIRToAsm(node.val_src);

    instructions.push_back(std::make_unique<AsmMovNode>(src, dest));
    instructions.push_back(std::make_unique<AsmUnaryNode>(node.op_type, dest));
  } else {
    throw std::runtime_error("unaryLowerIRToAsm: Unsupported op_type " +
                             node.op_type);
  }
  return instructions;
}

std::vector<std::unique_ptr<AsmInstructionNode>>
binaryLowerIRToAsm(IRBinaryNode &node) {
  std::vector<std::unique_ptr<AsmInstructionNode>> instructions;
  // operandLowerIRToAsm returns imm/pseudo if const/variable
  auto dest = operandLowerIRToAsm(node.val_dest);

  // separate handling for (+, -, *), (/, %) and (==, !=, <, >, <=, >=
  // {relational ops})
  if (node.op_type == "/" || node.op_type == "%") {
    auto eax_reg = std::make_shared<AsmRegisterNode>(getRegString(Reg::eax));
    auto src1 = operandLowerIRToAsm(node.val_src1);
    instructions.push_back(std::make_unique<AsmMovNode>(src1, eax_reg));
    instructions.push_back(std::make_unique<AsmCdqNode>());

    auto divisor = operandLowerIRToAsm(node.val_src2);
    instructions.push_back(std::make_unique<AsmIdivNode>(divisor));

    auto result_reg =
        (node.op_type == "/")
            ? eax_reg
            : std::make_shared<AsmRegisterNode>(getRegString(Reg::edx));
    instructions.push_back(std::make_unique<AsmMovNode>(result_reg, dest));
  } else if (node.op_type == "+" || node.op_type == "-" ||
             node.op_type == "*") {
    auto src1 = operandLowerIRToAsm(node.val_src1);
    instructions.push_back(std::make_unique<AsmMovNode>(src1, dest));

    auto src2 = operandLowerIRToAsm(node.val_src2);
    instructions.push_back(
        std::make_unique<AsmBinaryNode>(node.op_type, src2, dest));
  } else if (RELATIONAL_OPS.contains(node.op_type)) {
    auto src1 = operandLowerIRToAsm(node.val_src1);
    auto src2 = operandLowerIRToAsm(node.val_src2);
    auto dest = operandLowerIRToAsm(node.val_dest);

    instructions.push_back(std::make_unique<AsmCmpNode>(src2, src1));
    instructions.push_back(std::make_unique<AsmMovNode>(
        std::make_shared<AsmImmediateNode>(0), dest));
    instructions.push_back(
        std::make_unique<AsmSetCCNode>(getCondCode(node.op_type), dest));
  } else {
    throw std::runtime_error("binaryLowerIRToAsm: Unsupported op_type " +
                             node.op_type);
  }
  return instructions;
}

std::vector<std::unique_ptr<AsmInstructionNode>>
copyLowerIRToAsm(IRCopyNode &node) {
  std::vector<std::unique_ptr<AsmInstructionNode>> instructions;
  auto src = operandLowerIRToAsm(node.val_src);
  auto dest = operandLowerIRToAsm(node.val_dest);
  instructions.push_back(std::make_unique<AsmMovNode>(src, dest));
  return instructions;
}

std::vector<std::unique_ptr<AsmInstructionNode>>
jumpLowerIRToAsm(IRJumpNode &node) {
  std::vector<std::unique_ptr<AsmInstructionNode>> instructions;
  instructions.push_back(std::make_unique<AsmJmpNode>(node.target_label));
  return instructions;
}

namespace {
// helper function for conditional jumps; `IRJumpIfZeroNode` and
// `IRJumpIfNotZeroNode`
std::vector<std::unique_ptr<AsmInstructionNode>>
emitConditionalJump(const std::string &cc, // condition code
                    const std::shared_ptr<IRValNode> &condition,
                    const std::string &target_label) {
  std::vector<std::unique_ptr<AsmInstructionNode>> instructions;
  auto cond_asm = AsmGen::operandLowerIRToAsm(condition);

  // compare condition with 0
  instructions.push_back(std::make_unique<AsmCmpNode>(
      std::make_shared<AsmImmediateNode>(0), cond_asm));

  // conditional jump
  instructions.push_back(std::make_unique<AsmJmpCCNode>(cc, target_label));

  return instructions;
}
} // namespace

std::vector<std::unique_ptr<AsmInstructionNode>>
jumpIfZeroLowerIRToAsm(IRJumpIfZeroNode &node) {
  return emitConditionalJump("e", node.condition, node.target_label);
}

std::vector<std::unique_ptr<AsmInstructionNode>>
jumpIfNotZeroLowerIRToAsm(IRJumpIfNotZeroNode &node) {
  return emitConditionalJump("ne", node.condition, node.target_label);
}

std::vector<std::unique_ptr<AsmInstructionNode>>
labelLowerIRToAsm(IRLabelNode &node) {
  std::vector<std::unique_ptr<AsmInstructionNode>> instructions;
  instructions.push_back(std::make_unique<AsmLabelNode>(node.label_name));
  return instructions;
}

/// @brief Things/Instructions to execute just before a function call.
/// Not to be confused with Function defination part.
///
/// function callee: is called by other functions
/// @return instructions
std::vector<std::unique_ptr<AsmInstructionNode>>
functionCallLowerIRToAsm(IRFunctionCallNode &node) {
  std::vector<std::unique_ptr<AsmInstructionNode>> instructions;
  Reg arg_resisters[6] = {Reg::edi, Reg::esi, Reg::edx,
                          Reg::ecx, Reg::r8d, Reg::r9d};
  int num_args = node.arguments.size();

  // num of stack args
  int num_stack_args = (num_args > 6) ? (num_args - 6) : 0;

  // stack must be 16-byte aligned before call
  // each stack arg takes 8 bytes, so if odd number of stack args, add 8 bytes
  // padding even number of stack args means stack is already 16-byte aligned
  int stack_padding = 0;
  if (num_stack_args % 2 == 1) {
    stack_padding = 8;
    instructions.push_back(
        std::make_unique<AsmAllocateStackNode>(stack_padding));
  }

  // pass first 6 arguments in registers
  for (int i = 0; i < std::min(num_args, 6); i++) {
    auto arg_asm = operandLowerIRToAsm(node.arguments[i]);
    auto reg_node =
        std::make_shared<AsmRegisterNode>(getRegString(arg_resisters[i]));
    instructions.push_back(std::make_unique<AsmMovNode>(arg_asm, reg_node));
  }

  // pass remaining args on stack in reverse order
  for (int i = num_args - 1; i >= 6; i--) {
    auto arg_asm = operandLowerIRToAsm(node.arguments[i]);
    // push (to stack) can only take immediate values or registers
    // pushing a Register pushes the entire 8 bytes (64 bits)
    if (isa<AsmImmediateNode>(arg_asm.get()) ||
        isa<AsmRegisterNode>(arg_asm.get())) {
      instructions.push_back(std::make_unique<AsmPushNode>(arg_asm));
    } else { // move to temp register first then push to stack
      // use 32-bit register for movl, then push the 64-bit register
      auto temp_reg_32 =
          std::make_shared<AsmRegisterNode>(getRegString(Reg::eax));
      auto temp_reg_64 =
          std::make_shared<AsmRegisterNode>(getRegString(Reg::rax));
      instructions.push_back(
          std::make_unique<AsmMovNode>(arg_asm, temp_reg_32));
      instructions.push_back(std::make_unique<AsmPushNode>(temp_reg_64));
    }
  }

  // call function
  instructions.push_back(std::make_unique<AsmCallNode>(node.func_name));

  // adjust stack: remove stack-passed arguments + padding.
  // NOTE: any arguments passed on the stack use 8-byte slots per the System V
  // ABI, even if the argument type is only 4 bytes.
  int total_stack_adjust = (num_stack_args * 8 /*bytes*/) + stack_padding;
  if (total_stack_adjust > 0) {
    instructions.push_back(
        std::make_unique<AsmDeallocateStackNode>(total_stack_adjust));
  }

  // return value
  auto ret_dest = operandLowerIRToAsm(node.return_dest);
  auto eax_reg = std::make_shared<AsmRegisterNode>(getRegString(Reg::eax));
  instructions.push_back(std::make_unique<AsmMovNode>(eax_reg, ret_dest));

  return instructions;
}
} // namespace AsmGen
// Emit Assembly Functions -- End

namespace nanocc {
std::unique_ptr<AsmProgramNode>
intermReprToPseudoAsm(std::unique_ptr<IRProgramNode> &interm_repr, bool debug) {
  return AsmGen::programLowerIRToAsm(*interm_repr);
}
} // namespace nanocc
