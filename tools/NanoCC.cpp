#include <fstream>
#include <string>

#include "nanocc/AST/AST.hpp"
#include "nanocc/Codegen/IRToPseudoAsmPass.hpp"
#include "nanocc/IR/IR.hpp"
#include "nanocc/Lexer/Lexer.hpp"
#include "nanocc/Parser/Parser.hpp"
#include "nanocc/Sema/Sema.hpp"
#include "nanocc/Target/X86/X86TargetEmitter.hpp"
#include "nanocc/Transforms/PassManager.hpp"
#include "nanocc/Utils/Utils.hpp"

inline std::string getAsmOutputFromCFile(const std::string& c_filename,
                                         const nanocc::OptFlags& optimize_flags,
                                         bool debug = false) {
  std::string contents = nanocc::getFileContents(c_filename);
  std::deque<Token> tokens = nanocc::lexer(contents, debug);
  std::unique_ptr<ProgramNode> ast = nanocc::parse(tokens, debug);
  nanocc::semanticAnalysis(*ast, debug);
  std::unique_ptr<IRProgramNode> interm_repr =
      nanocc::generateIntermRepr(*ast, debug);
  if (!optimize_flags.optPasses.empty()) {
    nanocc::runIROptimizationPipeline(*interm_repr, optimize_flags, debug);
  }
  auto pseudo_asm = nanocc::intermReprToPseudoAsm(interm_repr, debug);
  nanocc::x86CorrectAssembly(pseudo_asm, debug);
  auto output = nanocc::x86EmitAssembly(pseudo_asm);
  return output.str();
}

inline nanocc::OptFlags parseDevFlags(int argc, char* argv[], bool& debug) {
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

// takes in only .c files and produces .s files
// ./nanocc -S <filename>.c -o <asm_output_file>.s
// ./nanocc -S <filename>.c -o <asm_output_file>.s -fopt-constfold
// -fopt-copyprop -fopt-dse -fopt-unreach -fdump
int main(int argc, char* argv[]) {
  assert(std::string(argv[1]) == "-S" && std::string(argv[3]) == "-o" &&
         "Usage: ./nanocc -S <source_file.c> -o <asm_output_file.s> "
         "<dev-only-flags>");
  std::string filename = argv[2];
  std::string asm_filename = argv[4];
  std::ofstream asm_file(asm_filename);
  if (!asm_file) {
    std::println(stderr, "Error: Failed to write to {}", asm_filename);
    return 1;
  }
  bool debug = false;
  nanocc::OptFlags optFlags = parseDevFlags(argc, argv, debug);
  std::string asm_output = getAsmOutputFromCFile(filename, optFlags, debug);

  asm_file << asm_output;
  asm_file.close();
  return 0;
}
