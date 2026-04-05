#pragma once

#include <deque>
#include <list>
#include <memory>
#include <print>
#include <string>
#include <unordered_map>
#include <vector>

#include "nanocc/IR/IR.hpp"
#include "nanocc/IR/IRDump.hpp"

class BasicBlock {
public:
  size_t blockId;
  std::deque<std::unique_ptr<IRInstructionNode>> IRInstructions;
  using Iter = std::list<std::shared_ptr<BasicBlock>>::iterator;

  static void printBasicBlocks(std::list<std::shared_ptr<BasicBlock>>& BBList) {
    std::println("--------- Basic Blocks --------");
    for (auto& BB : BBList) {
      std::println("Basic Block ID: {}", BB->blockId);
      for (auto& IRInstr : BB->IRInstructions) {
        IRGen::instructionNodeIRDump(*IRInstr, 2);
      }
      std::println();
    }
    std::println("-------------------------------");
  }

  static std::list<std::shared_ptr<BasicBlock>>
  getBasicBlocks(std::list<std::unique_ptr<IRInstructionNode>>& Instructions);

  static std::vector<BasicBlock::Iter>
  getSuccessors(BasicBlock::Iter BBIter,
                std::list<std::shared_ptr<BasicBlock>>& BBList);

  static std::vector<BasicBlock::Iter>
  getPredecessor(BasicBlock::Iter BBIter,
                 std::list<std::shared_ptr<BasicBlock>>& BBList);
};

class LabelBBMap {
private:
  std::unordered_map<std::string, BasicBlock::Iter> map;

public:
  static LabelBBMap& obj() {
    static LabelBBMap instance;
    return instance;
  }
  void insert(const std::string& labelName, BasicBlock::Iter BBIter) {
    map[labelName] = BBIter;
  }
  void erase(const std::string& labelName) { map.erase(labelName); }
  void clear() { map.clear(); }
  BasicBlock::Iter& at(const std::string& labelName) {
    return map.at(labelName);
  }
  BasicBlock::Iter* find(const std::string& labelName) {
    auto it = map.find(labelName);
    if (it == map.end())
      return nullptr;
    return &it->second;
  }
};
