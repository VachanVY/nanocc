#pragma once

#include "nanocc/AST/AST.hpp"
#include "nanocc/Codegen/IRToPseudoAsmPass.hpp"
#include "nanocc/IR/IR.hpp"
#include "nanocc/Lexer/Lexer.hpp"
#include "nanocc/Parser/Parser.hpp"
#include "nanocc/Sema/Sema.hpp"
#include "nanocc/Target/X86/X86TargetEmitter.hpp"
#include "nanocc/Transforms/PassManager.hpp"
#include "nanocc/Utils/Utils.hpp"

std::string getAsmOutputFromCFile(const std::string& c_filename,
                                  const nanocc::OptFlags& optimize_flags,
                                  bool debug = false) {
  auto contents = nanocc::getFileContents(c_filename);
  auto tokens = nanocc::lexer(contents, debug);
  auto ast = nanocc::parse(tokens, debug);
  nanocc::semanticAnalysis(ast, debug);
  auto interm_repr = nanocc::generateIntermRepr(ast, debug);
  if (!optimize_flags.optPasses.empty()) {
    nanocc::runIROptimizationPipeline(*interm_repr, optimize_flags, debug);
  }
  auto pseudo_asm = nanocc::intermReprToPseudoAsm(interm_repr, debug);
  nanocc::x86CorrectAssembly(pseudo_asm, debug);
  auto output = nanocc::x86EmitAssembly(pseudo_asm);
  return output.str();
}

nanocc::OptFlags parseDevFlags(int argc, char* argv[], bool& debug) {
  nanocc::OptFlags optFlags;
  for (int i = 5; i < argc; i++) {
    std::string flag = argv[i];
    if (flag == "-fopt-constfold") {
      optFlags.optPasses.insert(nanocc::OptPass::ConstantFolding);
    } else if (flag == "-fopt-unreach") {
      optFlags.optPasses.insert(nanocc::OptPass::UnreachableCodeElim);
    } else if (flag == "-fopt-copyprop") {
      optFlags.optPasses.insert(nanocc::OptPass::CopyPropagation);
    } else if (flag == "-fopt-dse") {
      optFlags.optPasses.insert(nanocc::OptPass::DeadStoreElim);
    } else if (flag == "-fdump") {
      debug = true;
    } else {
      std::println("Warning: Unrecognized optimization flag '{}'", flag);
    }
  }
  return optFlags;
}
