#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "nanocc/IR/BasicBlock.hpp"
#include "nanocc/IR/IR.hpp"
#include "nanocc/Transforms/SimplifyCFG.hpp"
#include "nanocc/Utils/Utils.hpp"

namespace {
bool removeUnreachableBlocks(std::list<std::shared_ptr<BasicBlock>>& BBList) {
  if (BBList.empty()) {
    return false;
  }

  // perform BFS and mark reachable blocks as visited
  std::unordered_set<size_t> visited;

  std::queue<BasicBlock::Iter> que;
  BasicBlock::Iter EntryBlock = BBList.begin();
  que.push(EntryBlock);
  visited.insert((*EntryBlock)->blockId);

  while (!que.empty()) {
    BasicBlock::Iter BBIter = que.front();
    que.pop();

    // go to adjacent nodes and add to que if they are not yet visited
    for (auto childBBIter : BasicBlock::getSuccessors(BBIter, BBList)) {
      if (!visited.contains((*childBBIter)->blockId)) {
        visited.insert((*childBBIter)->blockId);
        que.push(childBBIter);
      }
    }
  }

  // remove unvisited blocks from BBList
  bool changed = false;
  for (auto BBIter = BBList.begin(); BBIter != BBList.end();) {
    if (!visited.contains((*BBIter)->blockId)) {
      changed = true;
      // if first IR Instruction is Label, remove that from `LabelToBBMap`
      if (!(*BBIter)->IRInstructions.empty()) {
        auto& FirstIRInstr = (*BBIter)->IRInstructions.front();
        if (auto* IRLabelInstr = dyn_cast<IRLabelNode>(FirstIRInstr.get())) {
          LabelBBMap::obj().erase(IRLabelInstr->labelName);
        }
      }
      BBIter = BBList.erase(BBIter);
    } else {
      BBIter++;
    }
  }
  return changed;
}

bool removeRedundantJumpsLabelsEmptyBlocks(
    std::list<std::shared_ptr<BasicBlock>>& BBList) {
  bool changed = false;
  
  // returns true if BB was erased, due to being empty else false
  auto eraseBBIfEmpty = [&](BasicBlock::Iter& BBIter) -> bool {
    if ((*BBIter)->IRInstructions.empty()) {
      BBIter = BBList.erase(BBIter);
      changed = true;
      return true;
    }
    return false;
  };

  for (auto BBIter = BBList.begin(); BBIter != BBList.end();) {
    BasicBlock* BB = (*BBIter).get();

    if (eraseBBIfEmpty(BBIter)) continue;

    // remove redundant Jumps:
    // If the default next block is the only child, then
    // remove the Jump instruction, it's not useful
    IRInstructionNode* BBLastIRInstr = BB->IRInstructions.back().get();
    if (std::next(BBIter) != BBList.end() && (isa<IRJumpNode>(BBLastIRInstr) ||
        isa<IRJumpIfZeroNode>(BBLastIRInstr) ||
        isa<IRJumpIfNotZeroNode>(BBLastIRInstr))) {
      bool keepJump = false;
      auto BBDefaultNextIter = std::next(BBIter);
      // if BBDefaultNextIter is the only child, then remove that Instruction
      for (auto BBChildIter : BasicBlock::getSuccessors(BBIter, BBList)) {
        if (BBChildIter != BBDefaultNextIter) {
          keepJump = true;
          break;
        }
      }
      if (!keepJump) {
        BB->IRInstructions.pop_back();
        changed = true;
        if (eraseBBIfEmpty(BBIter)) continue;
      }
    }

    // remove redundant Label:
    // If the only parent BasicBlock is the one before BB
    // in the BBList also then remove it, anyway it will
    // reach the BasicBlock BB without the label
    auto& BBFirstIRInstr = BB->IRInstructions.front();
    if (auto* IRLabelInstr = dyn_cast<IRLabelNode>(BBFirstIRInstr.get());
        IRLabelInstr) {
      bool keepLabel = false;
      if (BBIter == BBList.begin()) {
        // entry block, but has predecessors => keep label
        //            , has no predecessors  => remove label
        keepLabel = !BasicBlock::getPredecessor(BBIter, BBList).empty();
      } else {
        auto BBDefaultPrevIter = std::prev(BBIter);
        for (auto BBParentIter : BasicBlock::getPredecessor(BBIter, BBList)) {
          if (BBParentIter != BBDefaultPrevIter) {
            keepLabel = true;
            break;
          }
        }
      }
      if (!keepLabel) {
        LabelBBMap::obj().erase(IRLabelInstr->labelName);
        BB->IRInstructions.pop_front();
        if (eraseBBIfEmpty(BBIter)) continue;
      }
    }

    // remove BasicBlock if it has no Instructions left
    if (!eraseBBIfEmpty(BBIter)) {
      ++BBIter;
    }
  }
  return changed;
}
} // namespace

namespace nanocc {
/// @brief Unreachable code elimination
/// @param IRProgram
/// @return
bool SimplifyCFG(IRProgramNode& IRProgram) {
  bool changed = false;
  for (auto& topLevel : IRProgram.topLevel) {
    if (auto* funcNode = dyn_cast<IRFunctionNode>(topLevel.get())) {
      auto& IRInstructions = funcNode->IRInstructions;
      auto BBList = BasicBlock::getBasicBlocks(IRInstructions);
      changed |= removeUnreachableBlocks(BBList);
      changed |= removeRedundantJumpsLabelsEmptyBlocks(BBList);

      IRInstructions.clear();
      for (auto& BB : BBList) {
        for (auto& IRInstr : BB->IRInstructions) {
          IRInstructions.push_back(std::move(IRInstr));
        }
      }
    }
  }
  return changed;
}
} // namespace nanocc
