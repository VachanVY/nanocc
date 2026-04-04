#pragma once

#include <deque>
#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "nanocc/IR/IR.hpp"
#include "nanocc/IR/IRDump.hpp"

class BasicBlock {
public:
  size_t blockId;
  std::deque<std::unique_ptr<IRInstructionNode>> IRInstructions;

  IRInstructionNode* getTerminator() {
    if (IRInstructions.empty())
      return nullptr;
    if (IRInstructions.back()->isTerminator())
      return IRInstructions.back().get();
    return nullptr;
  }

  static void printBasicBlocks(std::list<std::shared_ptr<BasicBlock>>& BBList) {
    for (auto& BB : BBList) {
      std::println("Basic Block ID: {}", BB->blockId);
      for (auto& IRInstr : BB->IRInstructions) {
        IRGen::instructionNodeIRDump(*IRInstr, 2);
      }
    }
  }

  static std::list<std::shared_ptr<BasicBlock>>
  getBasicBlocks(std::list<std::unique_ptr<IRInstructionNode>>& Instructions);

  static std::vector<BasicBlockIterator>
  getSuccessors(std::list<std::shared_ptr<BasicBlock>>::iterator BBIter,
                std::list<std::shared_ptr<BasicBlock>>& BBList);

  static std::vector<BasicBlockIterator>
  getPredecessor(std::list<std::shared_ptr<BasicBlock>>::iterator BBIter,
                 std::list<std::shared_ptr<BasicBlock>>& BBList);
};

using BasicBlockIterator = std::list<std::shared_ptr<BasicBlock>>::iterator;

class LabelBBMap {
private:
  std::unordered_map<std::string, BasicBlockIterator> map;

public:
  static LabelBBMap& obj() {
    static LabelBBMap instance;
    return instance;
  }
  void insert(const std::string& labelName, BasicBlockIterator& BBIter) {
    map[labelName] = BBIter;
  }
  void erase(const std::string& labelName) { map.erase(labelName); }
  BasicBlockIterator& at(const std::string& labelName) {
    return map.at(labelName);
  }
};
