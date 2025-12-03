#include <print>
#include <cassert>

#include "helper.hpp"

// takes in only .c files and produces .s files
// ./nanocc -S <filename>.c -o <asm_output_file>.s
int main(int argc, char* argv[]) {
    assert(argc == 5 && std::string(argv[1]) == "-S" && std::string(argv[3]) == "-o" &&
           "Usage: ./nanocc -S <source_file.c> -o <asm_output_file.s>");
    std::string filename = argv[2];
    std::string asm_output = getAsmOutputFromCFile(filename, false);
    std::string asm_filename = argv[4];
    std::ofstream asm_file(asm_filename);
    if (!asm_file) {
        std::println(stderr, "Error: Failed to write to {}", asm_filename);
        return 1;
    }
    asm_file << asm_output;
    asm_file.close();
    return 0;
}