#include <fstream>
#include <string>

#include "CompilerPipeline.hpp"

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
