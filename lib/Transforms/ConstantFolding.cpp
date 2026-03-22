#include "nanocc/Transforms/ConstantFolding.hpp"
#include "nanocc/Utils/Utils.hpp"

/*
Replace with Copy:
    Unary Instruction with constant operand
    Binary Instruction with constant operands

Replace with Jump or remove:
    JumpIfZero with constant condition
    JumpIfNotZero with constant condition
*/

namespace {
// if operand is a constant evaluate it at compile time.
// unary ops (as of now): ~, -, !
static bool
handleUnaryConstantFolding(IRUnaryNode* IRUnaryOp,
                           std::unique_ptr<IRInstructionNode>& IRInstr) {
  bool changed = false;
  if (auto* IRSrcConst = dyn_cast<IRConstNode>(IRUnaryOp->val_src.get())) {
    std::shared_ptr<IRConstNode> constEval;
    switch (IRUnaryOp->op_type) {
    case TokenType::TILDE: // bitwise not
      constEval = std::make_shared<IRConstNode>(~IRSrcConst->IntVal);
      break;
    case TokenType::MINUS: // negation
      constEval = std::make_shared<IRConstNode>(-IRSrcConst->IntVal);
      break;
    case TokenType::NOT: // logical not
      constEval = std::make_shared<IRConstNode>(!IRSrcConst->IntVal);
      break;
    }
    auto folded = std::make_unique<IRCopyNode>(constEval, IRUnaryOp->val_dest);
    IRInstr = std::move(folded);
    changed = true;
  }
  return changed;
}

// if both operands are constant evaluate at compile time
// binary ops in IR: *, /, %, +, -, <, <=, >, >=, ==, !=, &&, ||
static bool
handleBinaryConstantFolding(IRBinaryNode* IRBinaryOp,
                            std::unique_ptr<IRInstructionNode>& IRInstr) {
  bool changed = false;
  if (auto* IRSrc1Const = dyn_cast<IRConstNode>(IRBinaryOp->val_src1.get()))
    if (auto* IRSrc2Const = dyn_cast<IRConstNode>(IRBinaryOp->val_src2.get())) {
      std::shared_ptr<IRConstNode> constEval;
      int val1 = IRSrc1Const->IntVal;
      int val2 = IRSrc2Const->IntVal;
      switch (IRBinaryOp->op_type) {
      case TokenType::STAR:
        constEval = std::make_shared<IRConstNode>(val1 * val2);
        break;
      case TokenType::SLASH:
        if (val2 == 0)
          return changed;
        constEval = std::make_shared<IRConstNode>(val1 / val2);
        break;
      case TokenType::PERCENT:
        if (val2 == 0)
          return changed;
        constEval = std::make_shared<IRConstNode>(val1 % val2);
        break;
      case TokenType::PLUS:
        constEval = std::make_shared<IRConstNode>(val1 + val2);
        break;
      case TokenType::MINUS:
        constEval = std::make_shared<IRConstNode>(val1 - val2);
        break;
      case TokenType::LESSTHAN:
        constEval = std::make_shared<IRConstNode>(val1 < val2);
        break;
      case TokenType::LESS_EQUAL:
        constEval = std::make_shared<IRConstNode>(val1 <= val2);
        break;
      case TokenType::GREATERTHAN:
        constEval = std::make_shared<IRConstNode>(val1 > val2);
        break;
      case TokenType::GREATER_EQUAL:
        constEval = std::make_shared<IRConstNode>(val1 >= val2);
        break;
      case TokenType::EQUAL:
        constEval = std::make_shared<IRConstNode>(val1 == val2);
        break;
      case TokenType::NOT_EQUAL:
        constEval = std::make_shared<IRConstNode>(val1 != val2);
        break;
      case TokenType::AND:
        constEval = std::make_shared<IRConstNode>(val1 && val2);
        break;
      case TokenType::OR:
        constEval = std::make_shared<IRConstNode>(val1 || val2);
        break;
      }
      auto folded =
          std::make_unique<IRCopyNode>(constEval, IRBinaryOp->val_dest);
      IRInstr = std::move(folded);
      changed = true;
    }
  return changed;
}

// if jump condition is a constant evaluate it at compile time,
// replace it with just Jump or remove it depending on the condition
static bool
handleJumpIfZeroConstantFolding(IRJumpIfZeroNode* IRJumpIfZero,
                                std::unique_ptr<IRInstructionNode>& IRInstr) {
  bool changed = false;
  if (auto* IRSrcConst = dyn_cast<IRConstNode>(IRJumpIfZero->condition.get())) {
    changed = true;
    if (IRSrcConst->IntVal == 0) {
      // condition is always true, replace with unconditional jump
      auto folded = std::make_unique<IRJumpNode>(IRJumpIfZero->target_label);
      IRInstr = std::move(folded);
      return changed;
    }
    // condition is always false, remove the instruction
    IRInstr.reset();
  }
  return changed;
}

// if jump condition is a constant evaluate it at compile time,
// replace it with just Jump or remove it depending on the condition
static bool handleJumpIfNotZeroConstantFolding(
    IRJumpIfNotZeroNode* IRJumpIfNotZero,
    std::unique_ptr<IRInstructionNode>& IRInstr) {
  bool changed = false;
  if (auto* IRSrcConst =
          dyn_cast<IRConstNode>(IRJumpIfNotZero->condition.get())) {
    changed = true;
    if (IRSrcConst->IntVal != 0) {
      // condition is always true, replace with unconditional jump
      auto folded = std::make_unique<IRJumpNode>(IRJumpIfNotZero->target_label);
      IRInstr = std::move(folded);
      return changed;
    }
    // condition is always false, remove the instruction
    IRInstr.reset();
  }
  return changed;
}
} // namespace

namespace nanocc {

bool ConstantFoldInstructions(IRProgramNode& IRProgram) {
  bool changed = false;
  for (auto& TopLvl : IRProgram.top_level) {
    if (auto* IRFunc = dyn_cast<IRFunctionNode>(TopLvl.get())) {
      auto& IRVecInstr = IRFunc->ir_instructions;
      for (int i = 0; i < IRVecInstr.size(); i++) {
        auto& IRInstr = IRVecInstr[i];
        if (auto* IRUnaryOp = dyn_cast<IRUnaryNode>(IRInstr.get())) {
          changed |= handleUnaryConstantFolding(IRUnaryOp, IRInstr);
        } else if (auto* IRBinaryOp = dyn_cast<IRBinaryNode>(IRInstr.get())) {
          changed |= handleBinaryConstantFolding(IRBinaryOp, IRInstr);
        } else if (auto* IRJumpIfFalse =
                       dyn_cast<IRJumpIfZeroNode>(IRInstr.get())) {
          changed |= handleJumpIfZeroConstantFolding(IRJumpIfFalse, IRInstr);
        } else if (auto* IRJumpIfTrue =
                       dyn_cast<IRJumpIfNotZeroNode>(IRInstr.get())) {
          changed |= handleJumpIfNotZeroConstantFolding(IRJumpIfTrue, IRInstr);
        }
        // TODO(VachanVY): use a better data structure to avoid O(n) erases; this is a temp solution
        if (!IRInstr) {
          IRVecInstr.erase(IRVecInstr.begin() + i);
          --i;
        }
      }
    }
  }
  return changed;
}
} // namespace nanocc
