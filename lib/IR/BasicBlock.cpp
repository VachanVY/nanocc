#include "unordered_map"

#include "nanocc/IR/BasicBlock.hpp"
#include "nanocc/IR/IR.hpp"
#include "nanocc/Utils/Utils.hpp"

namespace {
// TODO(VachanVY): Store raw pointer instead of unique pointer...?
// std::vector<std::deque<std::unique_ptr<IRInstructionNode>>> ==>
// std::vector<std::deque<IRInstructionNode*>>
std::vector<std::deque<std::unique_ptr<IRInstructionNode>>> groupIRInstructions(
    std::list<std::unique_ptr<IRInstructionNode>>& Instructions) {
  std::vector<std::deque<std::unique_ptr<IRInstructionNode>>> Blocks;
  std::deque<std::unique_ptr<IRInstructionNode>> currBlock;

  for (auto& IRInstr : Instructions) {
    // If a Label, it's the start of a new Basic BLock
    if (isa<IRLabelNode>(IRInstr.get())) {
      if (!currBlock.empty()) {
        Blocks.push_back(std::move(currBlock));
      }
      currBlock.push_back(std::move(IRInstr));
    } else if ( // If a Jump<Type>Node, it's the end of the Basic Block
        isa<IRJumpNode>(IRInstr.get()) ||
        isa<IRJumpIfZeroNode>(IRInstr.get()) ||
        isa<IRJumpIfNotZeroNode>(IRInstr.get()) ||
        isa<IRRetNode>(IRInstr.get())) {
      currBlock.push_back(std::move(IRInstr));
      Blocks.push_back(std::move(currBlock)); // currBlock is now empty
    } else {
      currBlock.push_back(std::move(IRInstr));
    }
  }
  if (!currBlock.empty())
    Blocks.push_back(std::move(currBlock));
  return Blocks;
}
} // namespace

std::list<std::shared_ptr<BasicBlock>> BasicBlock::getBasicBlocks(
    std::list<std::unique_ptr<IRInstructionNode>>& Instructions) {
  std::list<std::shared_ptr<BasicBlock>> BBList;
  size_t blockId = 0;
  LabelBBMap::obj().clear();
  std::vector<std::deque<std::unique_ptr<IRInstructionNode>>> Blocks =
      groupIRInstructions(Instructions);
  for (auto& IRInstrGroup : Blocks) {
    auto BB = std::make_shared<BasicBlock>();
    BB->IRInstructions = std::move(IRInstrGroup);
    BB->blockId = blockId++;
    auto BBIter = BBList.emplace(BBList.end(), BB);
    auto& FirstIRInstr = BB->IRInstructions.front();
    if (auto* LabelIRInstr = dyn_cast<IRLabelNode>(FirstIRInstr.get())) {
      LabelBBMap::obj().insert(LabelIRInstr->labelName, BBIter);
    }
  }
  return BBList;
}

std::vector<BasicBlock::Iter>
BasicBlock::getSuccessors(BasicBlock::Iter BBIter,
                          std::list<std::shared_ptr<BasicBlock>>& BBList) {
  std::vector<BasicBlock::Iter> successors;
  BasicBlock* BB = (*BBIter).get();

  if (BB->IRInstructions.empty()) {
    return successors;
  }

  IRInstructionNode* BBLastIRInstr = BB->IRInstructions.back().get();
  if (!BBLastIRInstr || isa<IRRetNode>(BBLastIRInstr)) {
    return successors;
  } else if (auto* JumpIRInstr = dyn_cast<IRJumpNode>(BBLastIRInstr)) {
    if (BasicBlock::Iter* branch =
            LabelBBMap::obj().find(JumpIRInstr->labelName)) {
      successors.push_back(*branch);
    }
    return successors;
  }
  std::string labelName;
  if (auto* JumpIfZeroIRInstr = dyn_cast<IRJumpIfZeroNode>(BBLastIRInstr)) {
    labelName = JumpIfZeroIRInstr->labelName;
  } else if (auto* JumpIfNotZeroIRInstr =
                 dyn_cast<IRJumpIfNotZeroNode>(BBLastIRInstr)) {
    labelName = JumpIfNotZeroIRInstr->labelName;
  }
  if (BasicBlock::Iter defaultBranch = std::next(BBIter);
      defaultBranch != BBList.end())
    successors.push_back(defaultBranch);
  if (labelName.size() > 0) {
    if (BasicBlock::Iter* trueBranch = LabelBBMap::obj().find(labelName)) {
      successors.push_back(*trueBranch);
    }
  }
  return successors;
}

std::vector<BasicBlock::Iter>
BasicBlock::getPredecessor(BasicBlock::Iter BBTarget,
                           std::list<std::shared_ptr<BasicBlock>>& BBList) {
  std::vector<BasicBlock::Iter> predecessors;
  for (auto BBIter = BBList.begin(); BBIter != BBList.end(); BBIter++) {
    for (BasicBlock::Iter BBChild : BasicBlock::getSuccessors(BBIter, BBList)) {
      if (BBChild == BBTarget) {
        predecessors.push_back(BBIter);
      }
    }
  }
  return predecessors;
}
