#include "unordered_map"

#include "nanocc/IR/BasicBlock.hpp"
#include "nanocc/IR/IR.hpp"
#include "nanocc/Utils/Utils.hpp"

namespace {
/*
1. Group the IR Instructions
2. Convert the grouped IR Instructions into Basic Blocks
3. Convert the Basic Blocks to a Graph Data Structure by adding edges
*/

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
  std::vector<std::deque<std::unique_ptr<IRInstructionNode>>> Blocks =
      groupIRInstructions(Instructions);
  for (auto& IRInstrGroup : Blocks) {
    auto BB = std::make_shared<BasicBlock>();
    BB->IRInstructions = std::move(IRInstrGroup);
    BB->blockId = blockId++;
    auto BBIter = BBList.emplace(BBList.end(), BB);
    auto& FirstIRInstr = BB->IRInstructions.front();
    if (auto* IRLabelInstr = dyn_cast<IRLabelNode>(FirstIRInstr.get())) {
      LabelBBMap::obj().insert(IRLabelInstr->labelName, BBIter);
    }
  }
  return BBList;
}

std::vector<BasicBlockIterator>
BasicBlock::getSuccessors(BasicBlockIterator BBIter,
                          std::list<std::shared_ptr<BasicBlock>>& BBList) {
  std::vector<BasicBlockIterator> successors;
  BasicBlock* BB = (*BBIter).get();

  IRInstructionNode* BBLastIRInstr = BB->IRInstructions.back().get();
  if (!BBLastIRInstr || isa<IRRetNode>(BBLastIRInstr)) {
    return successors;
  } else if (auto* JumpIRInstr = dyn_cast<IRJumpNode>(BBLastIRInstr)) {
    BasicBlockIterator branch = LabelBBMap::obj().at(JumpIRInstr->labelName);
    successors.push_back(branch);
    return successors;
  }
  std::string labelName;
  if (auto* JumpIfZeroIRInstr = dyn_cast<IRJumpIfZeroNode>(BBLastIRInstr)) {
    labelName = JumpIfZeroIRInstr->labelName;
  } else if (auto* JumpIfNotZeroIRInstr =
                 dyn_cast<IRJumpIfNotZeroNode>(BBLastIRInstr)) {
    labelName = JumpIfNotZeroIRInstr->labelName;
  }
  assert(labelName.size() != 0);

  if (BasicBlockIterator falseBranch = std::next(BBIter);
      falseBranch != BBList.end())
    successors.push_back(falseBranch);
  BasicBlockIterator trueBranch = LabelBBMap::obj().at(labelName);
  successors.push_back(trueBranch);
  return successors;
}

std::vector<BasicBlockIterator> BasicBlock::getPredecessor(
    std::list<std::shared_ptr<BasicBlock>>::iterator BBTarget,
    std::list<std::shared_ptr<BasicBlock>>& BBList) {
  std::vector<BasicBlockIterator> predecessors;

  for (BasicBlockIterator BBIter = BBList.begin(); BBIter != BBList.end();
       BBIter++) {
    for (BasicBlockIterator BBChild :
         BasicBlock::getSuccessors(BBIter, BBList)) {
      if (BBChild == BBTarget) {
        predecessors.push_back(BBIter);
      }
    }
  }
  return predecessors;
}

/*
void addEdge(std::shared_ptr<BasicBlock> BBparent,
             std::shared_ptr<BasicBlock> BBchild) {
  if (!BBparent || !BBchild)
    return;
  BBparent->childrenBB.push_back(BBchild);
  BBchild->parentsBB.push_back(BBparent);
}

void addEdgesToBasicBlocks(std::list<std::shared_ptr<BasicBlock>> BBList) {
  for (auto BBIter = BBList.begin(); BBIter != BBList.end(); BBIter++) {
    std::shared_ptr<BasicBlock>& BB = *BBIter;

    std::shared_ptr<BasicBlock> BBNext = nullptr;
    if (std::next(BBIter) != BBList.end()) {
      BBNext = *std::next(BBIter);
    };

    IRInstructionNode* BBLastInstr = BB->getTerminator();
    if (!BBLastInstr)
      continue;
    if (auto* RetInstr = dyn_cast<IRRetNode>(BBLastInstr)) {
      continue;
    } else if (auto* JumpInstr = dyn_cast<IRJumpNode>(BBLastInstr)) {
      addEdge(BB, LabelToBB.at(JumpInstr->labelName));
    } else if (auto* JumpIfZeroInstr =
                   dyn_cast<IRJumpIfZeroNode>(BBLastInstr)) {
      addEdge(BB, BBNext);
      addEdge(BB, LabelToBB.at(JumpIfZeroInstr->labelName));
    } else if (auto* JumpIfNotZeroInstr =
                   dyn_cast<IRJumpIfNotZeroNode>(BBLastInstr)) {
      addEdge(BB, BBNext);
      addEdge(BB, LabelToBB.at(JumpIfNotZeroInstr->labelName));
    } else {
      addEdge(BB, BBNext);
    }
  }
}
*/